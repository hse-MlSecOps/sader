#include <cassert>
#include <memory>

#include <sader/WorkerRegistry.h>
#include <workers/hash/HashWorker.h>

void testDuplicateRegistration()
{
    WorkerRegistry registry;

    registry.registerWorker(
        std::make_unique<HashWorker>()
    );

    
    // Попробуй зарегистрировать второй HashWorker.
    // Проверь, что возникло std::invalid_argument.
}

int main()
{
    WorkerRegistry registry;

    registry.registerWorker(
        std::make_unique<HashWorker>()
    );

    Worker* worker = registry.findByName("hash");

    assert(worker != nullptr);
    assert(worker->name() == "hash");

    return 0;
}