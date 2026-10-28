#include "ExtendedCsvWorker.h"
#include "CsvParser.h"
#include <sstream>
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>

#include <exception>
// Возвращает уникальное имя обработчика для SADer
std::string ExtendedCsvWorker::name() const
{
    return "extended_csv";
}

// Возвращает описание возможностей обработчика
std::string ExtendedCsvWorker::description() const
{
    return "Extended CSV file processing";
}

// Описывает входные аргументы обработчика
Schema ExtendedCsvWorker::schema() const
{
    Schema schema;

    // Обязательный аргумент: путь к CSV-файлу 
    // Объект schema использует std::vector и std::string, которые автоматически управляют своей памятью.
    schema.args.push_back({"path", "string", true, "Path to CSV file", ""});

    // Обязательный аргумент: операция с CSV-файлом
    schema.args.push_back({"operation", "string", true,
                           "CSV operation", "read|count|sum|mean|min|max"});


     // Необязательный аргумент: название столбца CSV-файла
    schema.args.push_back({"column", "string", false,
                           "CSV column name", ""});  

    // Описание результата выполнения операции с CSV-файлом
    schema.result_description = "Result of CSV processing operation";


    return schema;
}

// Выполняет операцию с CSV-файлом
Result ExtendedCsvWorker::execute(const Arguments& args)
{
    // Проверяем наличие обязательных аргументов
    auto pathIt = args.find("path");
    auto operationIt = args.find("operation");

    if (pathIt == args.end() || pathIt->second.empty())
    {
        return {false, "", "Missing or empty argument: path"};
    }

    if (operationIt == args.end() || operationIt->second.empty())
    {
        return {false, "", "Missing or empty argument: operation"};
    }

    // Получаем значения аргументов
    const std::string& path = pathIt->second;
    const std::string& operation = operationIt->second;

    // Проверяем поддерживаемые операции
    if (operation != "read" && operation != "count" &&
        operation != "sum" && operation != "mean" &&
        operation != "min" && operation != "max")
    {
        return {false, "", "Unsupported CSV operation"};
    }

    // Читаем CSV-файл и обрабатываем возможные ошибки
    try
    {
        CsvParser parser;
        CsvParser::Table table = parser.read(path);

        // Проверяем наличие заголовка CSV
        if (table.empty())
        {
            return {false, "", "CSV file is empty"};
        }
        // Проверяем наличие столбцов в заголовке
        const std::size_t columnCount = table[0].size();

        if (columnCount == 0 || (columnCount == 1 && table[0][0].empty()))
        {
            return {false, "", "CSV header is empty"};
        }

        // Проверяем одинаковое количество столбцов во всех строках
        for (std::size_t i = 1; i < table.size(); ++i)
        {
            if (table[i].size() != columnCount)
            {
                return {false, "", "Invalid CSV row: column count mismatch"};
            }
        }

        
        // Возвращаем содержимое CSV-таблицы
        if (operation == "read")
        {
            std::ostringstream output;

            // Перебираем строки таблицы
            for (const auto& row : table)
            {
                // Перебираем поля текущей строки
                for (std::size_t i = 0; i < row.size(); ++i)
                {
                    if (i > 0)
                    {
                        output << ",";
                    }

                    output << row[i];
                }

                output << "\n";
            }

            return {true, output.str(), ""};
        }


        // Подсчитываем строки данных без заголовка
        if (operation == "count")
        {
            return {true, std::to_string(table.size() - 1), ""};
        }

        // Проверяем наличие аргумента column
        auto columnIt = args.find("column");

        if (columnIt == args.end() || columnIt->second.empty())
        {
            return {false, "", "Missing or empty argument: column"};
        }

        // Ищем указанный столбец в заголовке CSV
        const std::string& column = columnIt->second;
        const auto& header = table[0];

        auto it = std::find(header.begin(), header.end(), column);

        if (it == header.end())
        {
            return {false, "", "CSV column not found: " + column};
        }

        // Определяем индекс найденного столбца
        const std::size_t columnIndex =
            static_cast<std::size_t>(std::distance(header.begin(), it));

         // Проверяем числовые значения выбранного столбца
        std::vector<double> values;

        for (std::size_t i = 1; i < table.size(); ++i)
        {
            const std::string& cell = table[i][columnIndex];

            try
            {
                std::size_t processed = 0;
                double value = std::stod(cell, &processed);

                if (processed != cell.size() || !std::isfinite(value))
                {
                    return {false, "", "Invalid numeric value in CSV column"};
                }

                values.push_back(value);
            }
            catch (const std::invalid_argument&)
            {
                return {false, "", "Invalid numeric value in CSV column"};
            }
            catch (const std::out_of_range&)
            {
                return {false, "", "Numeric value out of range"};
            }
        }

        // Проверяем наличие строк данных
        if (values.empty())
        {
            return {false, "", "No data rows for numeric operation"};
        }

        // Вычисляем сумму, минимум и максимум
        double sum = 0.0;
        double minimum = values[0];
        double maximum = values[0];

        for (double value : values)
        {
            sum += value;

            if (value < minimum)
            {
                minimum = value;
            }

            if (value > maximum)
            {
                maximum = value;
            }
        }

        // Проверяем переполнение при суммировании
        if (!std::isfinite(sum))
        {
            return {false, "", "Numeric result out of range"};
        }

        // Возвращаем результат выбранной операции
        if (operation == "sum")
        {
            return {true, std::to_string(sum), ""};
        }

        if (operation == "mean")
        {
            return {true, std::to_string(sum / values.size()), ""};
        }

        if (operation == "min")
        {
            return {true, std::to_string(minimum), ""};
        }

        if (operation == "max")
        {
            return {true, std::to_string(maximum), ""};
        }

        return {false, "", "Unsupported CSV operation"};
        
    }
    catch (const std::exception& e)
    {
        return {false, "", e.what()};
    }
}