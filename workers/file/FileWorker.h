#pragma once

#include <cstdint>
#include <string>

#include <sader/Worker.h>

class FileWorker : public Worker
{
public:
    static constexpr std::uintmax_t MaxFileSizeBytes = 1024 * 1024;

    FileWorker();
    ~FileWorker() override;

    std::string name() const override;
    std::string description() const override;

    Schema schema() const override;
    Result execute(const Arguments& args) override;

private:
    bool validatePath(
        const std::string& path,
        std::uintmax_t& fileSize,
        std::string& error
    ) const;
};
