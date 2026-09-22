#include <iostream>

#include "JsonWorker.h"

JsonWorker::JsonWorker()
{
    std::cout << "CFileWorker constructed\n";
}

JsonWorker::~JsonWorker()
{
    std::cout << "CFileWorker destroyed\n";
}

std::string JsonWorker::name() const
{
    return "file";
}

std::string JsonWorker::description() const
{
    return "Parsing JSON and extracting the value using the specified path.";
}