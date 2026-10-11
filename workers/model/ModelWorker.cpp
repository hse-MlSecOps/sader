#include "ModelWorker.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace
{
    // Порог классификации по умолчанию (в схеме threshold необязателен).
    constexpr double kDefaultThreshold = 0.5;

    // Проверка контракта входных векторов. Вызывается до любых вычислений.
    void validateVectors(
        const std::vector<double>& left,
        const std::vector<double>& right)
    {
        if (left.empty() || right.empty())
        {
            throw std::invalid_argument("Vectors must not be empty");
        }
        if (left.size() != right.size())
        {
            throw std::invalid_argument("Vectors must have the same size");
        }
        for (const std::vector<double>* v : {&left, &right})
        {
            for (double value : *v)
            {
                if (!std::isfinite(value))
                {
                    throw std::invalid_argument(
                        "Vector components must be finite numbers");
                }
            }
        }
    }

    // Cosine similarity: dot / (||left|| * ||right||), диапазон [-1, 1].
    // Контракт: векторы уже проверены validateVectors; нулевой вектор запрещён.
    double cosineSimilarity(
        const std::vector<double>& left,
        const std::vector<double>& right)
    {
        const double dot = std::transform_reduce(
            left.begin(), left.end(), right.begin(), 0.0);
        const double leftNorm = std::sqrt(std::transform_reduce(
            left.begin(), left.end(), left.begin(), 0.0));
        const double rightNorm = std::sqrt(std::transform_reduce(
            right.begin(), right.end(), right.begin(), 0.0));

        if (leftNorm == 0.0 || rightNorm == 0.0)
        {
            throw std::invalid_argument(
                "Cosine similarity is undefined for zero vector");
        }

        // Округление при делении может дать значение чуть за пределами
        // [-1, 1] (например, 1.0000000000000002 для совпадающих векторов) —
        // зажимаем в допустимый диапазон cosine similarity.
        return std::clamp(dot / (leftNorm * rightNorm), -1.0, 1.0);
    }

    // Парсинг "1,2.5,-3" в вектор. Строгий: каждый токен должен быть
    // корректным конечным числом целиком.
    std::vector<double> parseVector(const std::string& text)
    {
        std::vector<double> values;
        std::size_t start = 0;
        while (true)
        {
            const std::size_t comma = text.find(',', start);
            const std::string token = text.substr(
                start, comma == std::string::npos ? comma : comma - start);

            std::size_t parsed = 0;
            const double value = std::stod(token, &parsed);
            if (parsed != token.size() || !std::isfinite(value))
            {
                throw std::invalid_argument("Invalid number: " + token);
            }
            values.push_back(value);

            if (comma == std::string::npos)
            {
                break;
            }
            start = comma + 1;
        }
        return values;
    }
}

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

Result ModelWorker::execute(const Arguments& args)
{
    // execute() не бросает исключений: любое нарушение контракта
    // превращается в Result{success=false, error=...}, состояние воркера
    // при этом не меняется (воркер не имеет mutable-состояния).
    try
    {
        const auto operationIt = args.find("operation");
        if (operationIt == args.end())
        {
            return {false, "", "Missing required argument: operation"};
        }
        if (operationIt->second != "classify")
        {
            return {
                false, "",
                "Unsupported operation: " + operationIt->second};
        }

        const auto vectorIt = args.find("vector");
        if (vectorIt == args.end())
        {
            return {false, "", "Missing required argument: vector"};
        }
        const auto otherIt = args.find("other");
        if (otherIt == args.end())
        {
            return {false, "", "Missing required argument: other"};
        }

        double threshold = kDefaultThreshold;
        if (const auto thresholdIt = args.find("threshold");
            thresholdIt != args.end())
        {
            std::size_t parsed = 0;
            threshold = std::stod(thresholdIt->second, &parsed);
            if (parsed != thresholdIt->second.size())
            {
                return {
                    false, "",
                    "Invalid threshold: " + thresholdIt->second};
            }
        }

        const std::vector<double> left = parseVector(vectorIt->second);
        const std::vector<double> right = parseVector(otherIt->second);

        return {true, classify(left, right, threshold), ""};
    }
    catch (const std::exception& e)
    {
        return {false, "", e.what()};
    }
}

std::string ModelWorker::classify(
    const std::vector<double>& left,
    const std::vector<double>& right,
    double threshold) const
{
    if (std::isnan(threshold))
    {
        throw std::invalid_argument("Threshold must not be NaN");
    }
    if (threshold < -1.0 || threshold > 1.0)
    {
        throw std::invalid_argument("Threshold must be within [-1, 1]");
    }

    validateVectors(left, right);

    const double similarity = cosineSimilarity(left, right);
    return similarity >= threshold ? "similar" : "dissimilar";
}
