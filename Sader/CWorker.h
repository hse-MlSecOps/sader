#pragma once

#include <string>

class CWorker {
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual ~CWorker() = default;
};