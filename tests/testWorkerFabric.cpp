#include <cassert>
#include <memory>

#include <sader/WorkerFactory.h>
#include <workers/hash/HashWorker.h>
#include <workers/file/FileWorker.h> 
#include <workers/csv/CsvWorker.h>
#include <workers/http/HttpWorker.h>
#include <workers/text/TextWorker.h>
#include <workers/process/ProcessWorker.h>
#include <workers/model/ModelWorker.h>

void testCreateHashWorker()
{
    auto worker = WorkerFactory::create(WorkersName::Hash);

    assert(worker != nullptr);
    assert(worker->name() == "hash");
}

int main()
{
    testCreateHashWorker();
    return 0;
}