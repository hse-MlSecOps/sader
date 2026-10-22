#pragma once

#include <string>

#include "FileWorker.h"

class PostgresFileWorker final : public FileWorker
{
public:
    explicit PostgresFileWorker(std::string dsn);

    Result execute(const Arguments& args) override;

private:
    void writeAudit(
        const std::string& path,
        const Result& result
    ) const;

    std::string dsn_;
};
