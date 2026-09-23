#pragma once
#include <string>

struct ArgumentInfo {
    std::string argName;
    std::string type;
    bool required;
};

struct Result {
    bool ok;
    std::string value;
    std::string error;
};

class Worker {
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual ~Worker() = default;
};