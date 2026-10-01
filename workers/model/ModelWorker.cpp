#include "ModelWorker.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

ModelWorker::ModelWorker()
{
    std::cout << "CModelWorker constructed\n";
}

ModelWorker::~ModelWorker()
{
    std::cout << "CModelWorker destroyed\n";
}

std::string ModelWorker::name() const
{
    return "model";
}

std::string ModelWorker::description() const
{
    return "Worker: vector normalization / cosine similarity / threshold classification.";
}

Schema ModelWorker::schema() const
{
    return Schema{
        {
            { "operation", "string", true, "normalize, cosine or classify", "" },
            { "vector", "number[]", true, "comma-separated numbers", "" },
            { "other", "number[]", false, "second vector for cosine and classify", "" },
            { "threshold", "number", false, "classification threshold", "" }
        },
        "Operation result or error"
    };
}

// L2-нормализация: v / ||v||, где ||v|| = sqrt(sum(v_i^2)).
// Возвращает unit vector той же направленности.
std::vector<double> ModelWorker::normalize(const std::vector<double>& values) const
{
    if (values.empty())
    {
        throw std::invalid_argument("Vector must not be empty");
    }

    double norm = 0.0;
    for (double value : values)
    {
        norm += value * value;
    }
    norm = std::sqrt(norm);

    if (norm == 0.0)
    {
        throw std::invalid_argument("Cannot normalize zero vector");
    }

    std::vector<double> result(values.size());
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        result[i] = values[i] / norm;
    }

    return result;
}

double ModelWorker::cosine(
    const std::vector<double>& left,
    const std::vector<double>& right) const
{
    if (left.empty() || right.empty())
    {
        throw std::invalid_argument("Vectors must not be empty");
    }

    if (left.size() != right.size())
    {
        throw std::invalid_argument("Vectors must have the same size");
    }

    double dotProduct = 0.0;
    double leftNorm = 0.0;
    double rightNorm = 0.0;

    for (std::size_t i = 0; i < left.size(); ++i)
    {
        dotProduct += left[i] * right[i];
        leftNorm += left[i] * left[i];
        rightNorm += right[i] * right[i];
    }

    leftNorm = std::sqrt(leftNorm);
    rightNorm = std::sqrt(rightNorm);

    if (leftNorm == 0.0 || rightNorm == 0.0)
    {
        throw std::invalid_argument(
            "Cosine similarity is undefined for zero vector"
        );
    }

    return dotProduct / (leftNorm * rightNorm);
}

std::string ModelWorker::classify(
    const std::vector<double>&,
    const std::vector<double>&,
    double) const
{
    return {};
}