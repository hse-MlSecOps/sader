#pragma once

#include <set>
#include <string>

#include <sader/Worker.h>

class ProcessWorker : public Worker
{
public:
    ProcessWorker();
    ~ProcessWorker() override;

    std::string name() const override;
    std::string description() const override;
    Schema schema() const override;
    Result execute(const Arguments& args) override;

private:
    bool isInWhitelist(const std::string& command) const;
    bool hasUnsafeCharacters(const std::string& input) const;

    std::set<std::string> whitelist_;
};
