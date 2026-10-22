#pragma once

#include "Worker.h"

#include <string>

// Persistent JSON document store backed by PostgreSQL.
// Capability: save, load, extract by dotted path, list and delete documents.
class JsonWorker : public Worker {
public:
    explicit JsonWorker(std::string dsn);

    std::string name() const override;
    std::string description() const override;
    Schema schema() const override;
    Result execute(const Arguments& args) override;

private:
    void ensureSchema();

    std::string dsn_;
};
