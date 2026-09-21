#pragma once

#include <memory>
#include <string>
#include <vector>

#include "CWorker.h"
#include "Command.h"

class CExecutor
{
public:
    void addWorker(std::unique_ptr<CWorker> worker);

    void execute(const Command& command) const;

private:
    void discover(const std::string& query) const;

    std::vector<std::unique_ptr<CWorker>> workers_;
};