#pragma once

#include <sader/Worker.h>

#include <string>
#include <vector>

class ModelWorker : public Worker
{
public:
    ModelWorker();
    ~ModelWorker() override;

    std::string name() const override;
    std::string description() const override;
    Schema schema() const override;

    // L2-нормализация вектора (unit vector).
    // Бросает std::invalid_argument для пустого или нулевого вектора.
    std::vector<double> normalize(const std::vector<double>& values) const;

    double cosine(
        const std::vector<double>& left,
        const std::vector<double>& right
    ) const;

    std::string classify(
        const std::vector<double>& left,
        const std::vector<double>& right,
        double threshold
    ) const;
};