#pragma once

#include <sader/Worker.h>

class FileWorker : public Worker
{
public:
    FileWorker();
    ~FileWorker() override;

    std::string name() const override;
    std::string description() const override;
};