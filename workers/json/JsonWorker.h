#pragma once
#include "Worker.h"
#include <vector>
#include <string>

class JsonWorker : public Worker {
public:
    JsonWorker();
    ~JsonWorker();

    std::string name() const override;
    std::string description() const override;
    std::vector<ArgumentInfo> schema() const;
    Result execute(const std::string& jsonText, const std::string& key) const;
};