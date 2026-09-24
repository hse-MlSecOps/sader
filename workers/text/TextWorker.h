#pragma once

#include <map>
#include <string>
#include <vector>

#include <sader/Worker.h>

class TextWorker : public Worker
{
public:
    struct Argument
    {
        std::string name;
        std::string type;
        bool required;
        std::string limits;
    };

    struct Result
    {
        bool ok;
        std::string value;
        std::string error;
    };

    using Schema = std::vector<Argument>;
    using Arguments = std::map<std::string, std::string>;

    TextWorker();
    ~TextWorker() override;

    std::string name() const override;
    std::string description() const override;

    Schema schema() const;
    Result execute(const Arguments& args) const;
};
