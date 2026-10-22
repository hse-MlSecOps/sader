#include "PostgresFileWorker.h"

#include <libpq-fe.h>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

using PgConnection = std::unique_ptr<PGconn, decltype(&PQfinish)>;
using PgResult = std::unique_ptr<PGresult, decltype(&PQclear)>;

PostgresFileWorker::PostgresFileWorker(std::string dsn)
    : dsn_(std::move(dsn))
{
}

Result PostgresFileWorker::execute(const Arguments& args)
{
    Result result = FileWorker::execute(args);

    std::string path;

    const auto it = args.find("path");
    if (it != args.end())
    {
        path = it->second;
    }

    try
    {
        writeAudit(path, result);
    }
    catch (const std::exception& e)
    {
        const std::string auditError =
            std::string("PostgreSQL audit failed: ") + e.what();

        if (!result.success)
        {
            return {false, "", result.error + "; " + auditError};
        }

        return {false, "", auditError};
    }

    return result;
}

void PostgresFileWorker::writeAudit(
    const std::string& path,
    const Result& result
) const
{
    PgConnection connection(
        PQconnectdb(dsn_.c_str()),
        &PQfinish
    );

    if (!connection)
    {
        throw std::runtime_error("Cannot allocate PostgreSQL connection");
    }

    if (PQstatus(connection.get()) != CONNECTION_OK)
    {
        throw std::runtime_error(PQerrorMessage(connection.get()));
    }

    const std::string fileName = fs::path(path).filename().string();

    std::string sizeText;
    const char* sizeValue = nullptr;

    if (result.success)
    {
        std::error_code ec;
        const auto size = fs::file_size(path, ec);

        if (!ec)
        {
            sizeText = std::to_string(size);
            sizeValue = sizeText.c_str();
        }
    }

    const char* values[5] = {
        path.c_str(),
        fileName.c_str(),
        sizeValue,
        result.success ? "true" : "false",
        result.error.empty() ? nullptr : result.error.c_str()
    };

    PgResult query(
        PQexecParams(
            connection.get(),
            "INSERT INTO file_worker_calls "
            "(file_path, file_name, file_size_bytes, success, error_message) "
            "VALUES ($1, $2, $3::bigint, $4::boolean, $5)",
            5,
            nullptr,
            values,
            nullptr,
            nullptr,
            0
        ),
        &PQclear
    );

    if (!query)
    {
        throw std::runtime_error(PQerrorMessage(connection.get()));
    }

    if (PQresultStatus(query.get()) != PGRES_COMMAND_OK)
    {
        throw std::runtime_error(PQresultErrorMessage(query.get()));
    }
}
