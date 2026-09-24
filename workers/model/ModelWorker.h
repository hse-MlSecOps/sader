#pragma once

#include <sader/Worker.h>

class ModelWorker : public Worker
{
public:
    ModelWorker();
    ~ModelWorker() override;

    std::string name() const override;
    std::string description() const override;
};
