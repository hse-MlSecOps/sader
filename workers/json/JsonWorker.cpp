#include <iostream>

#include "JsonWorker.h"

JsonWorker::JsonWorker()
{
    std::cout << "JsonWorker constructed\n";
}

JsonWorker::~JsonWorker()
{
    std::cout << "JsonWorker destroyed\n";
}

std::string JsonWorker::name() const
{
    return "json";
}

std::string JsonWorker::description() const
{
    return "Parse JSON text (json object or array) and extract a value "
           "by key or path";
}
