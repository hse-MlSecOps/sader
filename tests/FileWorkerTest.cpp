#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

#include <workers/file/FileWorker.h>

int main()
{
    namespace fs = std::filesystem;

    FileWorker worker;

    // --- Проверка схемы ---
    const auto schema = worker.schema();
    assert(schema.args.size() == 1);
    assert(schema.args[0].name == "path");
    assert(schema.args[0].type == "string");
    assert(schema.args[0].required);

    // --- Успешный вызов ---
    const fs::path testFile =
        fs::temp_directory_path() / "sader_file_worker_test.txt";

    {
        std::ofstream output(testFile);
        output << "hello from FileWorker\n";
    }

    const Arguments validArgs{{"path", testFile.string()}};
    const Result validResult = worker.execute(validArgs);

    assert(validResult.success);
    assert(validResult.error.empty());
    assert(validResult.output.find("hello from FileWorker") != std::string::npos);
    assert(validResult.output.find("Name: sader_file_worker_test.txt") != std::string::npos);
    assert(validResult.output.find("Extension: .txt") != std::string::npos);
    assert(validResult.output.find("Size: 22 bytes") != std::string::npos);

    // --- Ошибочный вызов №1: пустой путь ---
    const Arguments emptyArgs{{"path", ""}};
    const Result emptyPathResult = worker.execute(emptyArgs);
    assert(!emptyPathResult.success);
    assert(emptyPathResult.error == "path is required");

    // --- Ошибочный вызов №2: файла не существует ---
    const Arguments missingArgs{{"path", testFile.string() + ".missing"}};
    const Result missingFileResult = worker.execute(missingArgs);
    assert(!missingFileResult.success);
    assert(missingFileResult.error == "File does not exist");

    // --- Ошибочный вызов №3: неизвестный аргумент ---
    const Arguments unknownArgs{{"unknown", "value"}};
    const Result unknownResult = worker.execute(unknownArgs);
    assert(!unknownResult.success);
    assert(unknownResult.error == "Unknown argument: unknown");

    std::error_code ec;
    fs::remove(testFile, ec);
    return 0;
}
