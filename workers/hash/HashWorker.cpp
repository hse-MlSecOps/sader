#include <iostream>

#include "HashWorker.h"

namespace
{
    constexpr const char* WorkerName = "hash";
    constexpr const char* WorkerDescription =
        "Calculate MD5 and SHA-256 checksums for text values and files";
}

HashWorker::HashWorker()
{
    std::cout << "HashWorker ready for checksum discovery\n";
}

HashWorker::~HashWorker()
{
    std::cout << "HashWorker finished\n";
}

std::string HashWorker::name() const
{
    return WorkerName;
}

std::string HashWorker::description() const
{
    return WorkerDescription;
}
