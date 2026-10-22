#pragma once

#include <map>
#include <string>
#include <vector>

struct ArgumentInfo {
    std::string name;
    std::string type;
    bool required = true;
    std::string constraints;
};

using Schema = std::vector<ArgumentInfo>;
using Arguments = std::map<std::string, std::string>;

struct Result {
    bool ok = false;
    std::string value;
    std::string error;
};

class Worker {
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual Schema schema() const = 0;
    virtual Result execute(const Arguments& args) = 0;

    virtual ~Worker() = default;
};
