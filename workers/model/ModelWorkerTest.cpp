#include "ModelWorker.h"

#include <iostream>
#include <string>

int fail(const char* message)
{
    std::cerr << "FAIL: " << message << '\n';
    return 1;
}

int main()
{
    ModelWorker worker;

    if (worker.name() != "model")
    {
        return fail("name");
    }

    if (worker.description().find("cosine similarity") == std::string::npos)
    {
        return fail("description");
    }

    const Schema schema = worker.schema();
    if (schema.args.size() != 4)
    {
        return fail("schema size");
    }

    if (schema.args[0].name != "operation" || !schema.args[0].required)
    {
        return fail("operation");
    }

    if (schema.args[1].name != "vector" || !schema.args[1].required)
    {
        return fail("vector");
    }

    if (schema.args[2].name != "other" || schema.args[2].required)
    {
        return fail("other");
    }

    if (schema.args[3].name != "threshold" || schema.args[3].required)
    {
        return fail("threshold");
    }

    if (!worker.normalize({3, 4}).empty())
    {
        return fail("normalize");
    }

    if (worker.cosine({1, 0}, {0, 1}) != 0.0)
    {
        return fail("cosine");
    }

    if (!worker.classify({1, 0}, {1, 0}, 0.5).empty())
    {
        return fail("classify");
    }

    std::cout << "PASS\n";
    return 0;
}
