#pragma once

#include "CWorker.h"

class CFileWorker : public CWorker
{
public:
    CFileWorker();
    ~CFileWorker() override;

    std::string name() const override;
    std::string description() const override;
};