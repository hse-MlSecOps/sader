#include <stdexcept>

#include <sader/Worker.h>
#include <sader/WorkerFactory.h>
#include <workers/hash/HashWorker.h>
#include <workers/file/FileWorker.h>
#include <workers/csv/CsvWorker.h>
#include <workers/http/HttpWorker.h>
#include <workers/text/TextWorker.h>
#include <workers/process/ProcessWorker.h>
#include <workers/model/ModelWorker.h>

std::unique_ptr<Worker> WorkerFactory::create(const WorkersName type)
{
    switch (type)
    {
        case WorkersName::File: return std::make_unique<FileWorker>();
        case WorkersName::Hash: return std::make_unique<HashWorker>();
        case WorkersName::Csv: return std::make_unique<CsvWorker>();
        case WorkersName::Http: return std::make_unique<HttpWorker>();
        case WorkersName::Process: return std::make_unique<ProcessWorker>();
        case WorkersName::Text: return std::make_unique<TextWorker>();
        case WorkersName::Model: return std::make_unique<ModelWorker>();
        default: throw std::invalid_argument("No suitable Worker was found.");
    }
}