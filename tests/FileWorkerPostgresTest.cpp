#include <workers/file/PostgresFileWorker.h>

#include <libpq-fe.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

using PgConnection = std::unique_ptr<PGconn, decltype(&PQfinish)>;
using PgResult = std::unique_ptr<PGresult, decltype(&PQclear)>;

bool checkAudit(
    PGconn* conn,
    const std::string& path,
    bool expectedSuccess
)
{
    const char* values[] = {path.c_str()};

    PgResult result(
        PQexecParams(
            conn,
            "SELECT success FROM file_worker_calls "
            "WHERE file_path = $1 "
            "ORDER BY id DESC LIMIT 1",
            1,
            nullptr,
            values,
            nullptr,
            nullptr,
            0
        ),
        &PQclear
    );

    if (!result ||
        PQresultStatus(result.get()) != PGRES_TUPLES_OK ||
        PQntuples(result.get()) != 1)
    {
        return false;
    }

    const std::string value = PQgetvalue(result.get(), 0, 0);

    return value == (expectedSuccess ? "t" : "f");
}

int main()
{
    const char* dsn = std::getenv("SADER_FILE_DB_DSN");

    if (!dsn || !*dsn)
    {
        std::cout << "SKIP: SADER_FILE_DB_DSN is not set\n";
        return 77;
    }

    const auto unique =
        std::chrono::steady_clock::now().time_since_epoch().count();

    const std::string path =
        (fs::temp_directory_path() /
         ("sader_pg_test_" + std::to_string(unique) + ".txt")).string();

    {
        std::ofstream file(path);
        file << "PostgreSQL integration test\n";
    }

    const std::string missingPath = path + ".missing";

    PostgresFileWorker worker(dsn);

    const Result valid = worker.execute({{"path", path}});
    const Result invalid = worker.execute({{"path", missingPath}});

    PgConnection connection(PQconnectdb(dsn), &PQfinish);

    const bool connected =
        connection && PQstatus(connection.get()) == CONNECTION_OK;

    const bool passed =
        valid.success &&
        !invalid.success &&
        connected &&
        checkAudit(connection.get(), path, true) &&
        checkAudit(connection.get(), missingPath, false);

    std::error_code ec;
    fs::remove(path, ec);

    if (!passed)
    {
        std::cerr << "FileWorker PostgreSQL integration: FAIL\n";
        std::cerr << "valid error: " << valid.error << "\n";
        std::cerr << "invalid error: " << invalid.error << "\n";
        return 1;
    }

    std::cout << "FileWorker PostgreSQL integration: PASS\n";
    return 0;
}
