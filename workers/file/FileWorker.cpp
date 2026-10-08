#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <system_error>
#include <utility>

#include "FileWorker.h"

namespace fs = std::filesystem;

FileWorker::FileWorker()
{
    std::cout << "FileWorker constructed\n";
}

FileWorker::~FileWorker()
{
    std::cout << "FileWorker destroyed\n";
}

std::string FileWorker::name() const
{
    return "file";
}

std::string FileWorker::description() const
{
    return "Safely read text files and return content and file metadata";
}

Schema FileWorker::schema() const
{
    return Schema{
        {
            {"path", "string", true, "Path to the file to read", "existing regular file, maximum size 1 MiB"}
        },
        "File content and metadata or error"
    };
}

bool FileWorker::validate(
    const FileArguments& args,
    std::uintmax_t& fileSize,
    std::string& error
) const
{
    if (args.path.empty())
    {
        error = "File path is required";
        return false;
    }

    const fs::path path(args.path);
    std::error_code ec;

    const bool exists = fs::exists(path, ec);

    if (ec)
    {
        error = "Failed to inspect file path: " + ec.message();
        return false;
    }

    if (!exists)
    {
        error = "File does not exist";
        return false;
    }

    const bool regularFile = fs::is_regular_file(path, ec);

    if (ec)
    {
        error = "Failed to inspect file type: " + ec.message();
        return false;
    }

    if (!regularFile)
    {
        error = "Path must point to a regular file";
        return false;
    }

    fileSize = fs::file_size(path, ec);

    if (ec)
    {
        error = "Failed to get file size: " + ec.message();
        return false;
    }

    if (fileSize > MaxFileSizeBytes)
    {
        error = "File is too large: maximum size is 1 MiB";
        return false;
    }

    return true;
}

FileResult FileWorker::execute(const FileArguments& args) const
{
    std::uintmax_t fileSize = 0;
    std::string error;

    if (!validate(args, fileSize, error))
    {
        return {
            false,
            {},
            "",
            std::move(error)
        };
    }

    const fs::path path(args.path);
    std::ifstream input(path, std::ios::binary);

    if (!input.is_open())
    {
        return {
            false,
            {},
            "",
            "Failed to open file for reading"
        };
    }

    std::string content{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    };

    if (input.bad())
    {
        return {
            false,
            {},
            "",
            "Failed to read file"
        };
    }

    if (content.size() > MaxFileSizeBytes)
    {
        return {
            false,
            {},
            "",
            "File is too large: maximum size is 1 MiB"
        };
    }

    FileMetadata metadata{
        path.lexically_normal().string(),
        path.filename().string(),
        path.extension().string(),
        static_cast<std::uintmax_t>(content.size())
    };

    return {
        true,
        std::move(metadata),
        std::move(content),
        ""
    };
}
