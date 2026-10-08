#include <iostream>
#include <pqxx/pqxx>

#include <sader/PostgresRegistry.h>

PostgresRegistry::PostgresRegistry(std::string dsn)
    : dsn_(std::move(dsn))
{
    pqxx::connection conn{dsn_};
    std::cout << "PostgresRegistry: подключение к БД установлено\n";
}

std::vector<Capability> PostgresRegistry::discover(std::string_view query)
{
    pqxx::connection conn{dsn_};
    pqxx::work tx{conn};

    auto r = tx.exec(
        "SELECT name, description FROM capability "
        "WHERE enabled AND description ILIKE '%' || $1 || '%' "
        "ORDER BY name",
        pqxx::params{std::string{query}}
    );

    std::vector<Capability> out;
    for (const auto& row : r)
    {
        out.push_back({row[0].as<std::string>(),
                       row[1].as<std::string>()});
    }

    tx.commit();
    return out;
}

void PostgresRegistry::logCallStart(std::uint64_t requestId, const std::string& command)
{
    try
    {
        pqxx::connection conn{dsn_};
        pqxx::work tx{conn};

        tx.exec(
            "INSERT INTO call_log(request_id, command, status) "
            "VALUES ($1, $2, 'started')",
            pqxx::params{static_cast<int64_t>(requestId), command}
        );

        tx.commit();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка логирования (start): " << e.what() << "\n";
    }
}

void PostgresRegistry::logCallFinish(std::uint64_t requestId, bool success)
{
    try
    {
        pqxx::connection conn{dsn_};
        pqxx::work tx{conn};

        tx.exec(
            "UPDATE call_log SET finished_at = NOW(), status = $1 "
            "WHERE request_id = $2",
            pqxx::params{success ? "completed" : "failed",
                         static_cast<int64_t>(requestId)}
        );

        tx.commit();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка логирования (finish): " << e.what() << "\n";
    }
}

std::uint64_t PostgresRegistry::nextRequestId()
{
    return ++requestCounter_;
}
