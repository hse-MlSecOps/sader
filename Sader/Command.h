#pragma once

#include <string>

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
};