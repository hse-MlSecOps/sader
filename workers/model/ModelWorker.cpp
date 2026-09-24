#include "ModelWorker.h"

#include <iostream>

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
            {"operation", "string", true, "normalize, cosine or classify", ""},
            {"vector", "number[]", true, "input vector", ""},
            {"other", "number[]", false, "second vector for cosine/classify", ""},
            {"threshold", "number", false, "threshold for classification", ""}
        },
        "Result of the operation"
    };
}

std::vector<double> ModelWorker::normalize(const std::vector<double>&) const
{
    return {};
}

double ModelWorker::cosine(const std::vector<double>&, const std::vector<double>&) const
{
    return 0.0;
}

std::string ModelWorker::classify(const std::vector<double>&, const std::vector<double>&, double) const
{
    return {};
}
