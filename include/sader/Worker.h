#pragma once

#include <string>

class Worker {
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual ~Worker() = default;
};