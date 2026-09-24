#include "stdafx.h"

#include <sader/Command.h>
#include <sader/Executor.h>
#include <workers/hash/HashWorker.h>
#include <workers/file/FileWorker.h>
#include <workers/csv/CsvWorker.h>
#include <workers/http/HttpWorker.h>
#include <workers/text/TextWorker.h>
#include <workers/process/ProcessWorker.h>
#include <workers/model/ModelWorker.h>


Command parseCommand(const std::string& line)
{
    if (line.starts_with("DISCOVER "))
    {
        std::string query = line.substr(9);

        if (query.empty())
        {
            throw std::invalid_argument("DISCOVER требует аргумент");
        }

        return {CommandType::Discover, std::move(query), {}};
    }

    if (line.starts_with("DESCRIBE "))
    {
        std::string workerName = line.substr(9);

        if (workerName.empty())
        {
            throw std::invalid_argument("DESCRIBE требует имя воркера");
        }

        return {CommandType::Describe, std::move(workerName), {}};
    }

    if (line.starts_with("CALL "))
    {
        std::string rest = line.substr(5);

        // Первое слово — имя воркера, остальное — аргументы key=value
        std::string workerName;
        std::unordered_map<std::string, std::string> args;

        std::istringstream stream(rest);
        stream >> workerName;

        if (workerName.empty())
        {
            throw std::invalid_argument("CALL требует имя воркера");
        }

        std::string token;
        while (stream >> token)
        {
            auto eqPos = token.find('=');
            if (eqPos == std::string::npos)
            {
                throw std::invalid_argument(
                    "Неверный формат аргумента: " + token + " (ожидается key=value)");
            }

            std::string key = token.substr(0, eqPos);
            std::string value = token.substr(eqPos + 1);

            args[key] = value;
        }

        return {CommandType::Call, std::move(workerName), std::move(args)};
    }

    throw std::invalid_argument(
        "Неизвестная команда. Доступные: DISCOVER, DESCRIBE, CALL, EXIT"
    );
}

int main()
{
    Executor executor;

    executor.addWorker(std::make_unique<HashWorker>());

    executor.addWorker(std::make_unique<FileWorker>());

    executor.addWorker(std::make_unique<CsvWorker>());

    executor.addWorker(std::make_unique<HttpWorker>());

    executor.addWorker(std::make_unique<TextWorker>());
    executor.addWorker(std::make_unique<ProcessWorker>());

    executor.addWorker(std::make_unique<ModelWorker>());

    std::string line;

    while (true)
    {
        std::cout << "SADER> ";

        if (!std::getline(std::cin, line))
        {
            break;
        }

        if (line == "EXIT" || line == "exit" || line == "QUIT" || line == "quit")
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