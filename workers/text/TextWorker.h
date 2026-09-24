#pragma once

#include <map>
#include <string>
#include <vector>

#include <sader/Worker.h>

class TextWorker : public Worker
{
public:
    TextWorker();
    ~TextWorker() override;

    std::string name() const override;
    std::string description() const override;

    Schema schema() const override;
    Result execute(const Arguments& args) override;
};
