#pragma once

#include <sader/Worker.h>

class HttpWorker : public Worker
{
public:
    HttpWorker();
    ~HttpWorker() override;

    std::string name() const override;
    std::string description() const override;
};