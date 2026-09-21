#include "CFileWorker.h"

#include <iostream>

CFileWorker::CFileWorker()
{
    std::cout << "CFileWorker constructed\n";
}

CFileWorker::~CFileWorker()
{
    std::cout << "CFileWorker destroyed\n";
}

std::string CFileWorker::name() const
{
    return "file";
}

std::string CFileWorker::description() const
{
    return "Read text files and return file content and metadata";
}