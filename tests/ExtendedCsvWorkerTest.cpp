#include <cassert>
#include <string>

#include <workers/extended_csv/ExtendedCsvWorker.h>

int main()
{
    ExtendedCsvWorker worker;

    // Проверяем имя и схему обработчика
    assert(worker.name() == "extended_csv");

    const auto schema = worker.schema();
    assert(schema.args.size() == 3);
    assert(schema.args[0].name == "path");
    assert(schema.args[0].required);
    assert(schema.args[1].name == "operation");
    assert(schema.args[1].required);
    assert(schema.args[2].name == "column");
    assert(!schema.args[2].required);

    // Проверяем успешные операции
    const std::string path = "tests/data/extended_csv_valid.csv";

    const Result count = worker.execute({
        {"path", path}, {"operation", "count"}
    });
    assert(count.success);
    assert(count.output == "3");

    const Result sum = worker.execute({
        {"path", path}, {"operation", "sum"}, {"column", "price"}
    });
    assert(sum.success);
    assert(sum.output == "600.000000");

    const Result mean = worker.execute({
        {"path", path}, {"operation", "mean"}, {"column", "price"}
    });
    assert(mean.success);
    assert(mean.output == "200.000000");

    const Result min = worker.execute({
        {"path", path}, {"operation", "min"}, {"column", "price"}
    });
    assert(min.success);
    assert(min.output == "100.000000");

    const Result max = worker.execute({
        {"path", path}, {"operation", "max"}, {"column", "price"}
    });
    assert(max.success);
    assert(max.output == "300.000000");

    const Result read = worker.execute({
        {"path", path}, {"operation", "read"}
    });
    assert(read.success);
    assert(read.output.find("Keyboard,100,2") != std::string::npos);

    // Проверяем ошибку отсутствующего файла
    const Result missingFile = worker.execute({
        {"path", "tests/data/missing.csv"}, {"operation", "count"}
    });
    assert(!missingFile.success);
    assert(!missingFile.error.empty());

    // Проверяем ошибку неизвестного столбца
    const Result unknownColumn = worker.execute({
        {"path", path}, {"operation", "sum"}, {"column", "unknown"}
    });
    assert(!unknownColumn.success);
    assert(unknownColumn.error == "CSV column not found: unknown");

    // Проверяем ошибку неизвестной операции
    const Result invalidOperation = worker.execute({
        {"path", path}, {"operation", "invalid"}
    });
    assert(!invalidOperation.success);
    assert(invalidOperation.error == "Unsupported CSV operation");

    // Проверяем некорректное числовое значение
    const Result invalidNumber = worker.execute({
        {"path", "tests/data/extended_csv_invalid.csv"},
        {"operation", "sum"},
        {"column", "price"}
    });
    assert(!invalidNumber.success);
    assert(invalidNumber.error == "Invalid numeric value in CSV column");

    return 0;
}