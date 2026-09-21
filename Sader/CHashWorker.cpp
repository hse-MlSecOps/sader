#include "CHashWorker.h"

#include <iostream>

CHashWorker::CHashWorker()
{
    std::cout << "CHashWorker constructed\n";
}

CHashWorker::~CHashWorker()
{
    std::cout << "CHashWorker destroyed\n";
}

std::string CHashWorker::name() const
{
    return "hash";
}

std::string CHashWorker::description() const
{
    return "Calculate md5 and sha256 hashes for text and files";
}