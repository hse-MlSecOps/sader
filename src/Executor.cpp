#include <sader/Executor.h>


void Executor::addWorker(std::unique_ptr<Worker> worker)
{
    workers_.push_back(std::move(worker));
}

void Executor::execute(const Command& command) const {
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

void Executor::discover(const std::string& query) const
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

void Executor::call(const std::string& workerName, const Arguments& args) const
{
    for (const auto& worker : workers_)
    {
        if (worker->name() == workerName)
        {
            Result result = worker->execute(args);

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