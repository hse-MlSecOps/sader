#include <iostream>

#include "HttpWorker.h"

HttpWorker::HttpWorker()
{
    std::cout << "CHashWorker constructed\n";
}

HttpWorker::~HttpWorker()
{
    std::cout << "CHashWorker destroyed\n";
}

std::string HttpWorker::name() const
{
    return "http";
}

std::string HttpWorker::description() const
{
    return "http GET for current URL";
}