#include "ModelWorker.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int fail(const char* message)
{
    std::cerr << "FAIL: " << message << '\n';
    return 1;
}

bool approxEqual(double a, double b, double eps = 1e-9)
{
    return std::fabs(a - b) < eps;
}

// Проверяет, что вызов бросает std::invalid_argument.
template <typename F>
bool throwsInvalidArgument(F&& f)
{
    try
    {
        f();
    }
    catch (const std::invalid_argument&)
    {
        return true;
    }
    return false;
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

    // ---------- normalize ----------

    // normalize([3, 4]) = [0.6, 0.8]
    {
        const std::vector<double> result = worker.normalize({3, 4});
        if (result.size() != 2
            || !approxEqual(result[0], 0.6)
            || !approxEqual(result[1], 0.8))
        {
            return fail("normalize [3,4]");
        }
    }

    // normalize([1, 0]) = [1, 0]
    {
        const std::vector<double> result = worker.normalize({1, 0});
        if (result.size() != 2
            || !approxEqual(result[0], 1.0)
            || !approxEqual(result[1], 0.0))
        {
            return fail("normalize [1,0]");
        }
    }

    // normalize([-3, -4]) = [-0.6, -0.8]
    {
        const std::vector<double> result = worker.normalize({-3, -4});
        if (result.size() != 2
            || !approxEqual(result[0], -0.6)
            || !approxEqual(result[1], -0.8))
        {
            return fail("normalize negative");
        }
    }

    // normalize([5]) = [1]
    {
        const std::vector<double> result = worker.normalize({5});
        if (result.size() != 1 || !approxEqual(result[0], 1.0))
        {
            return fail("normalize single");
        }
    }

    // Результат — unit vector (длина 1)
    {
        const std::vector<double> result = worker.normalize({1, 2, 3});
        double length = 0.0;
        for (double value : result)
        {
            length += value * value;
        }
        if (!approxEqual(std::sqrt(length), 1.0))
        {
            return fail("normalize unit length");
        }
    }

    // Нулевой вектор -> invalid_argument
    if (!throwsInvalidArgument([&] { (void)worker.normalize({0, 0}); }))
    {
        return fail("normalize zero vector");
    }

    // Пустой вектор -> invalid_argument
    if (!throwsInvalidArgument([&] { (void)worker.normalize({}); }))
    {
        return fail("normalize empty vector");
    }

    // ---------- cosine ----------

    // Ортогональные векторы: cos = 0
    if (!approxEqual(worker.cosine({1, 0}, {0, 1}), 0.0))
    {
        return fail("cosine orthogonal");
    }

    // Совпадающие векторы: cos = 1
    if (!approxEqual(worker.cosine({1, 2, 3}, {1, 2, 3}), 1.0))
    {
        return fail("cosine identical");
    }

    // Противоположные векторы: cos = -1
    if (!approxEqual(worker.cosine({1, 0}, {-1, 0}), -1.0))
    {
        return fail("cosine opposite");
    }

    // Разные размерности -> invalid_argument
    if (!throwsInvalidArgument([&] { (void)worker.cosine({1, 2}, {1}); }))
    {
        return fail("cosine size mismatch");
    }

    // Нулевой вектор -> invalid_argument
    if (!throwsInvalidArgument([&] { (void)worker.cosine({0, 0}, {1, 2}); }))
    {
        return fail("cosine zero vector");
    }

    // Пустой вектор -> invalid_argument
    if (!throwsInvalidArgument([&] { (void)worker.cosine({}, {1}); }))
    {
        return fail("cosine empty vector");
    }

    if (!worker.classify({1, 0}, {1, 0}, 0.5).empty())
    {
        return fail("classify");
    }

    std::cout << "PASS\n";
    return 0;
}
