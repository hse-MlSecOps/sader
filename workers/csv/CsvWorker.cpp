#include <iostream>

#include "CsvWorker.h"

CsvWorker::CsvWorker()
{
    std::cout << "CsvWorker constructed\n";
}

CsvWorker::~CsvWorker()
{
    std::cout << "CsvWorker destroyed\n";
}

std::string CsvWorker::name() const
{
    return "csv";
}

std::string CsvWorker::description() const
{
    return "Read csv table";
}