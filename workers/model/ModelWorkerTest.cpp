#include "ModelWorker.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

int main()
{
    ModelWorker worker;

    // Успешный вызов
    std::vector<double> a = { 1.0, 2.0, 3.0 };
    std::vector<double> b = { 1.0, 2.0, 3.0 };

    double result = worker.cosine(a, b);

    std::cout << "Cosine similarity: "
        << result
        << '\n';

    // Ошибка 1: разные размерности
    try
    {
        worker.cosine(
            { 1.0, 2.0 },
            { 1.0 }
        );
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Error: "
            << e.what()
            << '\n';
    }

    // Ошибка 2: нулевой вектор
    try
    {
        worker.cosine(
            { 0.0, 0.0 },
            { 1.0, 2.0 }
        );
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Error: "
            << e.what()
            << '\n';
    }

    return 0;
}