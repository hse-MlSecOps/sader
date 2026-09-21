#pragma once

#include <sader/Worker.h>

class CsvWorker : public Worker
{
public:
    CsvWorker();
    ~CsvWorker() override;

    std::string name() const override;
    std::string description() const override;
};