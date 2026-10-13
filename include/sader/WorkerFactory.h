#pragma once

#include <memory>
#include <string>
#include <sader/Worker.h>

enum class WorkersName
{
    Hash,
    File,
    Csv,
    Http,
    Text,
    Process,
    Model
};

class WorkerFactory 
{
public:
    static std::unique_ptr<Worker> create(const WorkersName type);
};
