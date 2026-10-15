#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>

#include "CsvWorker.h"

// Разбирает одну строку CSV на отдельные поля.
// Учитывает запятые и экранированные кавычки внутри полей.
static bool parseCsvLine(
    const std::string& line,
    std::vector<std::string>& fields)
{
    fields.clear();

    std::string field;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i)
    {
        char ch = line[i];

        if (ch == '"')
        {
            if (inQuotes &&
                i + 1 < line.size() &&
                line[i + 1] == '"')
            {
                // Две кавычки внутри quoted-поля означают
                // одну обычную кавычку.
                field += '"';
                ++i;
            }
            else
            {
                // Открываем или закрываем quoted-поле.
                inQuotes = !inQuotes;
            }
        }
        else if (ch == ',' && !inQuotes)
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += ch;
        }
    }

    // Незакрытые кавычки означают некорректную строку.
    if (inQuotes)
    {
        return false;
    }

    // Добавляем последнее поле.
    fields.push_back(field);

    return true;
}

// Преобразует строку в число и проверяет,
// что вся строка содержит корректное числовое значение.
static bool parseNumber(const std::string& text, double& value)
{
    try
    {
        std::size_t parsedChars = 0;

        value = std::stod(text, &parsedChars);

        // stod может успешно преобразовать только начало строки.
        // Поэтому проверяем, что после числа не осталось текста.
        if (parsedChars != text.size())
        {
            return false;
        }

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

CsvWorker::CsvWorker()
{
    std::cout << "CsvWorker constructed\n";
}

CsvWorker::~CsvWorker()
{
    std::cout << "CsvWorker destroyed\n";
}

std::string CsvWorker::name() const
{
    return "csv";
}

std::string CsvWorker::description() const
{
    return "Read csv table";
}

Schema CsvWorker::schema() const
{
    Schema schema;

    schema.result_description = "CSV table operation result";

    schema.args.push_back({
        "path",
        "string",
        true,
        "Path to CSV file",
        "existing regular file"
    });

    schema.args.push_back({
        "operation",
        "string",
        true,
        "CSV operation: read, count, sum, mean, min, max",
        "read|count|sum|mean|min|max"
    });

    schema.args.push_back({
        "column",
        "string",
        false,
        "Name of the CSV column",
        ""
    });

    return schema;
}

Result CsvWorker::execute(const Arguments& args)
{
    // Получаем путь к файлу.
    auto pathIt = args.find("path");

    if (pathIt == args.end() || pathIt->second.empty())
    {
        return {
            false,
            "",
            "path is required"
        };
    }

    // Получаем операцию.
    auto operationIt = args.find("operation");

    if (operationIt == args.end())
    {
        return {
            false,
            "",
            "operation is required"
        };
    }

    const std::string& operation = operationIt->second;

    // Проверяем допустимость операции.
    if (operation != "read" &&
        operation != "count" &&
        operation != "sum" &&
        operation != "mean" &&
        operation != "min" &&
        operation != "max")
    {
        return {
            false,
            "",
            "Unknown operation: " + operation
        };
    }

    // Открываем файл.
    std::ifstream input(pathIt->second);

    if (!input.is_open())
    {
        return {
            false,
            "",
            "Failed to open CSV file"
        };
    }

    std::string line;

    // Первая строка — заголовок.
    if (!std::getline(input, line))
    {
        return {
            false,
            "",
            "CSV file is empty"
        };
    }

    // Убираем CR в конце строки для файлов с Windows-окончаниями строк.
    if (!line.empty() && line.back() == '\r')
    {
        line.pop_back();
    }

    const std::string header = line;

    std::vector<std::string> columns;

    if (!parseCsvLine(header, columns))
    {
        return {
            false,
            "",
            "Invalid CSV header: unmatched quotes"
        };
    }

    if (columns.empty())
    {
        return {
            false,
            "",
            "CSV header contains no columns"
        };
    }

    // Получаем имя столбца, если оно передано.
    auto columnIt = args.find("column");

    // Для read и count столбец не требуется.
    // Для остальных операций он обязателен.
    if (operation != "read" &&
        operation != "count" &&
        (columnIt == args.end() || columnIt->second.empty()))
    {
        return {
            false,
            "",
            "column is required for this operation"
        };
    }

    // Находим индекс нужного столбца.
    std::size_t columnIndex = 0;

    if (columnIt != args.end())
    {
        auto columnPos = std::find(
            columns.begin(),
            columns.end(),
            columnIt->second
        );

        if (columnPos == columns.end())
        {
            if (operation != "read" && operation != "count")
            {
                return {
                    false,
                    "",
                    "Unknown column: " + columnIt->second
                };
            }
        }
        else
        {
            columnIndex = static_cast<std::size_t>(
                columnPos - columns.begin()
            );
        }
    }

    // Подготавливаем результат read.
    std::ostringstream tableOutput;
    tableOutput << header << '\n';

    // Переменные для статистических операций.
    double sum = 0.0;
    double minValue = 0.0;
    double maxValue = 0.0;

    std::size_t count = 0;
    std::size_t rowCount = 0;

    // Читаем строки данных.
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        // Сохраняем исходную строку для операции read.
        tableOutput << line << '\n';

        // Пустые строки не считаем строками данных.
        if (line.empty())
        {
            continue;
        }

        std::vector<std::string> row;

        if (!parseCsvLine(line, row))
        {
            return {
                false,
                "",
                "Invalid CSV row: unmatched quotes"
            };
        }

        // Количество полей должно соответствовать заголовку.
        if (row.size() != columns.size())
        {
            return {
                false,
                "",
                "Invalid CSV row: number of fields does not match header"
            };
        }

        ++rowCount;

        // read и count не требуют числового преобразования.
        if (operation == "read" || operation == "count")
        {
            continue;
        }

        double value = 0.0;

        if (!parseNumber(row[columnIndex], value))
        {
            return {
                false,
                "",
                "Non-numeric value in column: " + columnIt->second
            };
        }

        // Первое число задаёт начальные минимум и максимум.
        if (count == 0)
        {
            minValue = value;
            maxValue = value;
        }
        else
        {
            if (value < minValue)
            {
                minValue = value;
            }

            if (value > maxValue)
            {
                maxValue = value;
            }
        }

        sum += value;
        ++count;
    }

    // Возвращаем всю таблицу.
    if (operation == "read")
    {
        return {
            true,
            tableOutput.str(),
            ""
        };
    }

    // Возвращаем количество строк данных.
    if (operation == "count")
    {
        std::ostringstream output;
        output << "Count: " << rowCount;

        return {
            true,
            output.str(),
            ""
        };
    }

    // Для статистических операций нужно хотя бы одно число.
    if (count == 0)
    {
        return {
            false,
            "",
            "No numeric values to process"
        };
    }

    std::ostringstream output;

    if (operation == "sum")
    {
        output << "Sum: " << sum;
    }
    else if (operation == "mean")
    {
        output << "Mean: " << sum / static_cast<double>(count);
    }
    else if (operation == "min")
    {
        output << "Min: " << minValue;
    }
    else if (operation == "max")
    {
        output << "Max: " << maxValue;
    }

    return {
        true,
        output.str(),
        ""
    };
}