#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

#include <workers/file/FileWorker.h>

int main()
{
    namespace fs = std::filesystem;

    FileWorker worker;

    // Проверяем схему FileWorker.
    const auto schema = worker.schema();

    assert(schema.args.size() == 1);
    assert(schema.args[0].name == "path");
    assert(schema.args[0].type == "string");
    assert(schema.args[0].required);

    // Успешный вызов.
    const fs::path testFile =
        fs::temp_directory_path() / "sader_file_worker_test.txt";

    {
        std::ofstream output(testFile);
        output << "hello from FileWorker\n";
    }

    const auto validResult =
        worker.readFile({testFile.string()});

    assert(validResult.success);
    assert(validResult.content == "hello from FileWorker\n");
    assert(validResult.metadata.name == "sader_file_worker_test.txt");
    assert(validResult.metadata.extension == ".txt");
    assert(validResult.metadata.sizeBytes == validResult.content.size());

    // Ошибочный вызов №1: пустой путь.
    const auto emptyPathResult =
        worker.readFile({""});

    assert(!emptyPathResult.success);
    assert(emptyPathResult.error == "File path is required");

    // Ошибочный вызов №2: файла не существует.
    const auto missingFileResult =
        worker.readFile({testFile.string() + ".missing"});

    assert(!missingFileResult.success);
    assert(missingFileResult.error == "File does not exist");

    std::error_code ec;
    fs::remove(testFile, ec);

    return 0;
}
