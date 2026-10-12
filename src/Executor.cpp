#include <sader/Executor.h>

Executor::Executor(WorkerRegistry& registry): registry_(registry){}

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
    auto matches = registry_.discover(query);
    if (matches.empty())
    {
        std::cout << "No capabilities found\n";
        return;
    }

    for (const Worker* worker : matches)
    {
        std::cout
            << worker->name()
            << " - "
            << worker->description()
            << '\n';
    }
}

void Executor::describe(const std::string& workerName) const
{
    const Worker* worker = registry_.findByName(workerName);
    if (!worker)
    {
        std::cout << "Воркер не найден: " << workerName << "\n";
    }
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

void Executor::call(const std::string& workerName, const Arguments& args) const
{
    Worker* worker = registry_.findByName(workerName);

    if (!worker)
    {
        std::cout << "Воркер не найден: " << workerName << "\n";
    }

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
}