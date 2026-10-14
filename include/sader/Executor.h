#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <sader/Command.h>
#include <sader/WorkerRegistry.h>

class Executor
{
public:
    explicit Executor(WorkerRegistry& registry);
    void execute(const Command& command) const;
private:
    void discover(const std::string& query) const;
    void describe(const std::string& workerName) const;
    void call(const std::string& workerName, const Arguments& args) const;
    
    WorkerRegistry& registry_;
};