#pragma once

#include <string>
#include <vector>

// Отвечает за чтение и разбор CSV-файлов
class CsvParser
{
public:
    // Таблица: каждая строка содержит набор полей
    using Row = std::vector<std::string>;
    using Table = std::vector<Row>;

    // Читает CSV-файл и возвращает таблицу
    Table read(const std::string& path) const;

private:
    // Разделяет строку CSV на отдельные поля
    Row parseLine(const std::string& line) const;
};