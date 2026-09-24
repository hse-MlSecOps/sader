#pragma once

#include <string>
#include <unordered_map>

enum class CommandType
{
    Discover,
    Describe,
    Call
};

struct Command
{
    CommandType type;
    std::string argument;
    std::unordered_map<std::string, std::string> arguments;
};