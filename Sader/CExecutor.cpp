#include "CExecutor.h"

#include <iostream>
#include <utility>

void CExecutor::addWorker(std::unique_ptr<CWorker> worker)
{
    workers_.push_back(std::move(worker));
}

void CExecutor::execute(const Command& command) const {
    switch (command.type) 
    {
    case CommandType::Discover:
        discover(command.argument);
        break;
    case CommandType::Describe:
        break;
    case CommandType::Call:
        throw std::runtime_error("Command is not implemented yet");
    }
}

void CExecutor::discover(const std::string& query) const
{
    bool found = false;

    for (const auto& worker : workers_)
    {
        const std::string description = worker->description();

        if (description.find(query) != std::string::npos)
        {
            std::cout
                << worker->name()
                << " - "
                << description
                << '\n';

            found = true;
        }
    }

    if (!found)
    {
        std::cout << "No capabilities found\n";
    }
}