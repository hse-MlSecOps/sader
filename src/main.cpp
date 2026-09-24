#include "stdafx.h"

#include <sader/Command.h>
#include <sader/Executor.h>
#include <workers/hash/HashWorker.h>
#include <workers/file/FileWorker.h>
#include <workers/csv/CsvWorker.h>
#include <workers/http/HttpWorker.h>
#include <workers/model/ModelWorker.h>

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
    Executor executor;

    executor.addWorker(std::make_unique<HashWorker>());

    executor.addWorker(std::make_unique<FileWorker>());

    executor.addWorker(std::make_unique<CsvWorker>());

    executor.addWorker(std::make_unique<HttpWorker>());

    executor.addWorker(std::make_unique<ModelWorker>());

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