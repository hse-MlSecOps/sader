#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using Arguments = std::unordered_map<std::string, std::string>;

struct Result {
    bool success;
    std::string output;
    std::string error;
};

struct ArgDescriptor {
    std::string name;
    std::string type;
    bool required;
    std::string description;
    std::string constraint;
};

struct Schema {
    std::vector<ArgDescriptor> args;
    std::string result_description;
};

class Worker {
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual Schema schema() const
    {
        throw std::runtime_error("schema() not implemented for this worker");
    }

    virtual Result execute(const Arguments& /*args*/)
    {
        throw std::runtime_error("execute() not implemented for this worker");
    }

    virtual ~Worker() = default;
};