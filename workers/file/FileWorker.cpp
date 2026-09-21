#include <iostream>

#include "FileWorker.h"

FileWorker::FileWorker()
{
    std::cout << "CFileWorker constructed\n";
}

FileWorker::~FileWorker()
{
    std::cout << "CFileWorker destroyed\n";
}

std::string FileWorker::name() const
{
    return "file";
}

std::string FileWorker::description() const
{
    return "Read text files and return file content and metadata";
}