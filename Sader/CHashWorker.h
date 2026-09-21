#pragma once

#include "CWorker.h"

class CHashWorker : public CWorker
{
public:
    CHashWorker();
    ~CHashWorker() override;

    std::string name() const override;
    std::string description() const override;
};