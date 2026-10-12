#include <sader/WorkerRegistry.h>

void WorkerRegistry::registerWorker(std::unique_ptr<Worker> worker)
{
    // Проверить на nullptr
    if (!worker)
    {
        throw std::invalid_argument("Worker cannot be nullptr");
    }
    // проверить на повторяющейся элемент
    auto it = std::find_if(
        workers_.begin(),
        workers_.end(), 
        [&worker](const std::unique_ptr<Worker>& item)
        {
            return item->name() == worker->name();
        });
    if (it != workers_.end())
    {
        throw std::invalid_argument("Dublicate Workers");
    } 
    workers_.push_back(std::move(worker));
}

Worker* WorkerRegistry::findByName(const std::string& query)
{
    bool found = false;
    for (const auto& worker : workers_)
    {
        if (worker->name() == query)
        {
            return worker.get();
        }
    }
    return nullptr;
}

std::vector<const Worker*> WorkerRegistry::discover(const std::string& query) const
{
    std::vector<const Worker*> matches;

    for (const auto& worker: workers_)
    {
        const std::string description = worker->description();
        
        if (description.find(query) != std::string::npos)
        {
           matches.push_back(worker.get());
        }
    }
    return matches;
}