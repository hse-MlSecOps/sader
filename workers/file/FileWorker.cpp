#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <system_error>

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
    return {
        {
            {
                "path",
                "string",
                true,
                "Path to the text file",
                "existing regular file, maximum size 1 MiB"
            }
        },
        "File content and metadata: path, name, extension and size"
    };
}

bool FileWorker::validatePath(
    const std::string& filePath,
    std::uintmax_t& fileSize,
    std::string& error
) const
{
    if (filePath.empty())
    {
        error = "path is required";
        return false;
    }

    const fs::path path(filePath);
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

Result FileWorker::execute(const Arguments& args)
{
    for (const auto& entry : args)
    {
        if (entry.first != "path")
        {
            return {
                false,
                "",
                "Unknown argument: " + entry.first
            };
        }
    }

    const auto pathIt = args.find("path");

    if (pathIt == args.end())
    {
        return {
            false,
            "",
            "path is required"
        };
    }

    std::uintmax_t fileSize = 0;
    std::string error;

    if (!validatePath(pathIt->second, fileSize, error))
    {
        return {
            false,
            "",
            error
        };
    }

    const fs::path path(pathIt->second);
    std::ifstream input(path, std::ios::binary);

    if (!input.is_open())
    {
        return {
            false,
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
            "",
            "Failed to read file"
        };
    }

    if (content.size() > MaxFileSizeBytes)
    {
        return {
            false,
            "",
            "File is too large: maximum size is 1 MiB"
        };
    }

    std::ostringstream output;

    output
        << "Path: " << path.lexically_normal().string() << '\n'
        << "Name: " << path.filename().string() << '\n'
        << "Extension: " << path.extension().string() << '\n'
        << "Size: " << content.size() << " bytes\n"
        << "Content:\n"
        << content;

    return {
        true,
        output.str(),
        ""
    };
}
