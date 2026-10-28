#include "CsvParser.h"

#include <fstream>
#include <stdexcept>

// Читает CSV-файл и возвращает таблицу
CsvParser::Table CsvParser::read(const std::string& path) const
{
    // Открываем файл с автоматическим управлением ресурсом (RAII)
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open CSV file: " + path);
    }

    Table table;
    std::string line;

    // Читаем файл построчно
    while (std::getline(file, line))
    {
        // Удаляем символ возврата каретки для Windows-файлов
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        // Добавляем разобранную строку в таблицу
        table.push_back(parseLine(line));
    }

    // Проверяем ошибки чтения файла
    if (file.bad())
    {
        throw std::runtime_error("Error reading CSV file");
    }

    return table;
}

// Разделяет CSV-строку на поля с учётом двойных кавычек
CsvParser::Row CsvParser::parseLine(const std::string& line) const
{
    Row row;
    std::string field;
    bool inQuotes = false;

    // Обрабатываем каждый символ строки
    for (std::size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];

        if (c == '"')
        {
            // Две кавычки подряд означают одну кавычку внутри поля
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                field += '"';
                ++i;
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes)
        {
            // Запятая вне кавычек завершает поле
            row.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }

    // Проверяем незакрытые кавычки
    if (inQuotes)
    {
        throw std::runtime_error("Unclosed quotes in CSV line");
    }

    // Добавляем последнее поле
    row.push_back(field);

    return row;
}
