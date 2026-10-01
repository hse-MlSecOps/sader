#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

#include <workers/file/FileWorker.h>

int main()
{
    namespace fs = std::filesystem;

    FileWorker fileWorker;
    Worker& worker = fileWorker;

    const Schema schema = worker.schema();

    assert(schema.args.size() == 1);
    assert(schema.args[0].name == "path");
    assert(schema.args[0].type == "string");
    assert(schema.args[0].required);
    assert(!schema.result_description.empty());

    const fs::path testFile =
        fs::temp_directory_path() / "sader_file_worker_test.txt";

    {
        std::ofstream output(testFile);
        output << "hello from FileWorker\n";
    }

    const Arguments validArgs{
        {"path", testFile.string()}
    };

    const Result validResult = worker.execute(validArgs);

    assert(validResult.success);
    assert(validResult.error.empty());
    assert(
        validResult.output.find("Name: sader_file_worker_test.txt")
        != std::string::npos
    );
    assert(
        validResult.output.find("Extension: .txt")
        != std::string::npos
    );
    assert(
        validResult.output.find("hello from FileWorker")
        != std::string::npos
    );

    const Arguments missingPathArgs{};

    const Result missingPathResult =
        worker.execute(missingPathArgs);

    assert(!missingPathResult.success);
    assert(missingPathResult.error == "path is required");

    const Arguments missingFileArgs{
        {"path", testFile.string() + ".missing"}
    };

    const Result missingFileResult =
        worker.execute(missingFileArgs);

    assert(!missingFileResult.success);
    assert(missingFileResult.error == "File does not exist");

    std::error_code ec;
    fs::remove(testFile, ec);

    return 0;
}
