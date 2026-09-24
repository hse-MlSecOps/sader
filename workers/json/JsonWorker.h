#pragma once

#include <sader/Worker.h>

class JsonWorker final : public Worker
{
public:
    JsonWorker();
    ~JsonWorker() override;

    std::string name() const override;
    std::string description() const override;
};