#include "JsonWorker.h"

#include <iostream>

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
    return "Parse JSON and extract data by path";
}