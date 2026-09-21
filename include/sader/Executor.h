#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <sader/Worker.h>
#include <sader/Command.h>

class Executor
{
public:
    void addWorker(std::unique_ptr<Worker> worker);

    void execute(const Command& command) const;

private:
    void discover(const std::string& query) const;

    std::vector<std::unique_ptr<Worker>> workers_;
};