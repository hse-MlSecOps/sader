#pragma once

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sader/Worker.h>

class WorkerRegistry
{
public:
    void registerWorker(std::unique_ptr<Worker> worker);
    Worker* findByName(const std::string& query);
    const Worker* findByName(const std::string& query) const;
    std::vector<const Worker*> discover(
        const std::string& query
    ) const;

private:
    std::vector<std::unique_ptr<Worker>> workers_;
};