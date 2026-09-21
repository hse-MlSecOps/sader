#pragma once

#include <sader/Worker.h>

class HashWorker : public Worker
{
public:
    HashWorker();
    ~HashWorker() override;

    std::string name() const override;
    std::string description() const override;
};