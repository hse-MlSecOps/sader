#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <sader/Worker.h>

struct FileArguments
{
    std::string path;
};

struct FileMetadata
{
    std::string path;
    std::string name;
    std::string extension;
    std::uintmax_t sizeBytes = 0;
};

struct FileResult
{
    bool success = false;
    FileMetadata metadata;
    std::string content;
    std::string error;
};

class FileWorker : public Worker
{
public:
    static constexpr std::uintmax_t MaxFileSizeBytes = 1024 * 1024;

    FileWorker();
    ~FileWorker() override;

    std::string name() const override;
    std::string description() const override;

    Schema schema() const override;
    FileResult execute(const FileArguments& args) const;

private:
    bool validate(
        const FileArguments& args,
        std::uintmax_t& fileSize,
        std::string& error
    ) const;
};
