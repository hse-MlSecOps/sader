#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <sader/Worker.h>
#include <sader/Command.h>
#include <sader/PostgresRegistry.h>

class Executor
{
public:
    explicit Executor(PostgresRegistry& registry);

    void addWorker(std::unique_ptr<Worker> worker);

    void execute(const Command& command);

private:
    void discover(const std::string& query);
    void describe(const std::string& workerName) const;
    void call(const std::string& workerName, const Arguments& args);

    PostgresRegistry& registry_;
    std::vector<std::unique_ptr<Worker>> workers_;
};