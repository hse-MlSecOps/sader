#include "ModelWorker.h"

#include <cmath>
#include <iostream>
#include <limits>
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

    // ---------- classify ----------

    // Идентичные векторы: cos = 1 >= 0.5
    if (worker.classify({1, 0}, {1, 0}, 0.5) != "similar")
    {
        return fail("classify identical");
    }

    // Ортогональные векторы: cos = 0 < 0.5
    if (worker.classify({1, 0}, {0, 1}, 0.5) != "dissimilar")
    {
        return fail("classify orthogonal");
    }

    // Граница включающая: cos = 0 >= threshold = 0
    if (worker.classify({1, 0}, {0, 1}, 0.0) != "similar")
    {
        return fail("classify threshold inclusive");
    }

    // Противоположные векторы: cos = -1 < 0.5
    if (worker.classify({1, 0}, {-1, 0}, 0.5) != "dissimilar")
    {
        return fail("classify opposite");
    }

    // Отрицательный порог: cos = -1 < -0.5
    if (worker.classify({1, 0}, {-1, 0}, -0.5) != "dissimilar")
    {
        return fail("classify negative threshold");
    }

    // Порог ровно 1.0, совпадающие векторы: cos = 1 >= 1.
    // Значения выбраны точно представимыми в double, чтобы проверять
    // включающую границу без влияния ошибок округления.
    if (worker.classify({1, 0}, {1, 0}, 1.0) != "similar")
    {
        return fail("classify threshold 1.0");
    }

    // Порог вне [-1, 1]
    if (!throwsInvalidArgument([&] { (void)worker.classify({1, 0}, {1, 0}, 1.5); }))
    {
        return fail("classify threshold > 1");
    }
    if (!throwsInvalidArgument([&] { (void)worker.classify({1, 0}, {1, 0}, -1.5); }))
    {
        return fail("classify threshold < -1");
    }

    // NaN-порог
    if (!throwsInvalidArgument([&] {
            (void)worker.classify(
                {1, 0}, {1, 0},
                std::numeric_limits<double>::quiet_NaN());
        }))
    {
        return fail("classify NaN threshold");
    }

    // Пустой вектор
    if (!throwsInvalidArgument([&] { (void)worker.classify({}, {1, 0}, 0.5); }))
    {
        return fail("classify empty vector");
    }

    // Разные размерности
    if (!throwsInvalidArgument([&] { (void)worker.classify({1, 0}, {1, 0, 0}, 0.5); }))
    {
        return fail("classify size mismatch");
    }

    // Нулевой вектор
    if (!throwsInvalidArgument([&] { (void)worker.classify({0, 0}, {1, 0}, 0.5); }))
    {
        return fail("classify zero vector");
    }

    // NaN-компонента вектора
    if (!throwsInvalidArgument([&] {
            (void)worker.classify(
                {std::numeric_limits<double>::quiet_NaN()}, {1.0}, 0.5);
        }))
    {
        return fail("classify NaN component");
    }

    // ---------- execute: контракт Worker ----------

    // Успешный вызов classify через execute
    {
        const Result r = worker.execute({
            {"operation", "classify"},
            {"vector", "1,0"},
            {"other", "1,0"},
            {"threshold", "0.5"}
        });
        if (!r.success || r.output != "similar" || !r.error.empty())
        {
            return fail("execute classify ok");
        }
    }

    // threshold опционален: default 0.5, cos([1,0],[0,1]) = 0 < 0.5
    {
        const Result r = worker.execute({
            {"operation", "classify"},
            {"vector", "1,0"},
            {"other", "0,1"}
        });
        if (!r.success || r.output != "dissimilar")
        {
            return fail("execute default threshold");
        }
    }

    // Нет operation
    if (worker.execute({{"vector", "1,0"}}).success)
    {
        return fail("execute missing operation");
    }

    // Неподдерживаемая операция
    if (worker.execute({{"operation", "normalize"}, {"vector", "1,0"}}).success)
    {
        return fail("execute unsupported operation");
    }

    // Нет vector / other
    if (worker.execute({{"operation", "classify"}, {"other", "1,0"}}).success)
    {
        return fail("execute missing vector");
    }
    if (worker.execute({{"operation", "classify"}, {"vector", "1,0"}}).success)
    {
        return fail("execute missing other");
    }

    // Невалидное число в векторе
    {
        const Result r = worker.execute({
            {"operation", "classify"},
            {"vector", "1,abc"},
            {"other", "1,0"}
        });
        if (r.success || r.error.empty())
        {
            return fail("execute invalid number");
        }
    }

    // Число с «хвостом»: stod парсит префикс, но токен должен
    // быть корректным числом целиком
    {
        const Result r = worker.execute({
            {"operation", "classify"},
            {"vector", "1,2x"},
            {"other", "1,0"}
        });
        if (r.success || r.error.empty())
        {
            return fail("execute trailing garbage in number");
        }
    }

    // Невалидный threshold
    if (worker.execute({
            {"operation", "classify"},
            {"vector", "1,0"},
            {"other", "1,0"},
            {"threshold", "abc"}
        }).success)
    {
        return fail("execute invalid threshold");
    }

    // threshold с «хвостом» после числа
    if (worker.execute({
            {"operation", "classify"},
            {"vector", "1,0"},
            {"other", "1,0"},
            {"threshold", "0.5abc"}
        }).success)
    {
        return fail("execute trailing garbage in threshold");
    }

    // Порог вне диапазона: ошибка в Result, а не исключение наружу
    {
        const Result r = worker.execute({
            {"operation", "classify"},
            {"vector", "1,0"},
            {"other", "1,0"},
            {"threshold", "2"}
        });
        if (r.success || r.error.empty())
        {
            return fail("execute threshold out of range");
        }
    }

    // После ошибочного вызова воркер остаётся в рабочем состоянии
    {
        worker.execute({
            {"operation", "classify"},
            {"vector", "bad"},
            {"other", "1,0"}
        });
        const Result r = worker.execute({
            {"operation", "classify"},
            {"vector", "1,0"},
            {"other", "1,0"}
        });
        if (!r.success || r.output != "similar")
        {
            return fail("execute state after error");
        }
    }

    std::cout << "PASS\n";
    return 0;
}
