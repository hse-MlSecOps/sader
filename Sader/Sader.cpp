#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "Command.h"
#include "CExecutor.h"
#include "CHashWorker.h"
#include "CFileWorker.h"

Command parseCommand(const std::string& line)
{
    const std::string prefix = "DISCOVER ";

    if (line.starts_with(prefix))
    {
        std::string query = line.substr(prefix.size());

        if (query.empty())
        {
            throw std::invalid_argument(
                "DISCOVER requires a query"
            );
        }

        return {
            CommandType::Discover,
            std::move(query)
        };
    }

    throw std::invalid_argument(
        "Unknown or unsupported command"
    );
}

int main()
{
    CExecutor executor;

    executor.addWorker(
        std::make_unique<CHashWorker>()
    );

    executor.addWorker(
        std::make_unique<CFileWorker>()
    );

    std::string line;

    while (true)
    {
        std::cout << "SADER> ";

        if (!std::getline(std::cin, line))
        {
            break;
        }

        try
        {
            Command command = parseCommand(line);

            executor.execute(command);
        }
        catch (const std::exception& e)
        {
            std::cerr
                << "ERROR: "
                << e.what()
                << '\n';
        }
    }

    return 0;
}