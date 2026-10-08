#pragma once

#include <string>
#include <string_view>
#include <vector>

struct Capability {
    std::string name;
    std::string description;
};

class PostgresRegistry {
public:
    explicit PostgresRegistry(std::string dsn);

    std::vector<Capability> discover(std::string_view query);

    void logCallStart(std::uint64_t requestId, const std::string& command);
    void logCallFinish(std::uint64_t requestId, bool success);

    std::uint64_t nextRequestId();

private:
    std::string dsn_;
    std::uint64_t requestCounter_{0};
};
