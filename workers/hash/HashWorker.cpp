#include <iostream>

#include "HashWorker.h"

HashWorker::HashWorker()
{
    std::cout << "CHashWorker constructed\n";
}

HashWorker::~HashWorker()
{
    std::cout << "CHashWorker destroyed\n";
}

std::string HashWorker::name() const
{
    return "hash";
}

std::string HashWorker::description() const
{
    return "Calculate md5 and sha256 hashes for text and files";
}