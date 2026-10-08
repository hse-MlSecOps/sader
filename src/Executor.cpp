#include <sader/Executor.h>

Executor::Executor(PostgresRegistry& registry)
    : registry_(registry)
{
}

void Executor::addWorker(std::unique_ptr<Worker> worker)
{
    workers_.push_back(std::move(worker));
}

void Executor::execute(const Command& command) {
    switch (command.type) 
    {
    case CommandType::Discover:
        discover(command.argument);
        break;
    case CommandType::Describe:
        describe(command.argument);
        break;
    case CommandType::Call:
        call(command.argument, command.arguments);
        break;
    }
}

void Executor::discover(const std::string& query)
{
    auto capabilities = registry_.discover(query);

    if (capabilities.empty())
    {
        std::cout << "No capabilities found\n";
        return;
    }

    for (const auto& cap : capabilities)
    {
        std::cout << cap.name << " - " << cap.description << "\n";
    }
}

void Executor::describe(const std::string& workerName) const
{
    for (const auto& worker : workers_)
    {
        if (worker->name() == workerName)
        {
            Schema s = worker->schema();

            std::cout << "=== " << worker->name() << " ===\n";
            std::cout << worker->description() << "\n\n";
            std::cout << "Аргументы:\n";

            for (const auto& arg : s.args)
            {
                std::cout << "  " << arg.name
                          << " (" << arg.type << ")"
                          << (arg.required ? " [обязательный]" : " [необязательный]")
                          << " - " << arg.description << "\n";

                if (!arg.constraint.empty())
                {
                    std::cout << "    Ограничение: " << arg.constraint << "\n";
                }
            }

            std::cout << "\nРезультат: " << s.result_description << "\n";
            return;
        }
    }

    std::cout << "Воркер не найден: " << workerName << "\n";
}

void Executor::call(const std::string& workerName, const Arguments& args)
{
    for (const auto& worker : workers_)
    {
        if (worker->name() == workerName)
        {
            auto requestId = registry_.nextRequestId();
            registry_.logCallStart(requestId, workerName);

            Result result = worker->execute(args);

            registry_.logCallFinish(requestId, result.success);

            if (result.success)
            {
                std::cout << "Успех\n";
                std::cout << result.output << "\n";
            }
            else
            {
                std::cout << "Ошибка: " << result.error << "\n";
            }

            return;
        }
    }

    std::cout << "Воркер не найден: " << workerName << "\n";
}