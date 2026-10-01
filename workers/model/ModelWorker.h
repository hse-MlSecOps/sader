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

    // Точка входа по контракту Worker: парсит аргументы, выполняет операцию
    // и возвращает структурированный Result. Не бросает исключений.
    Result execute(const Arguments& args) override;

    std::vector<double> normalize(const std::vector<double>& values) const;
    double cosine(const std::vector<double>& left, const std::vector<double>& right) const;

    // Бинарная классификация по cosine similarity: "similar", если
    // cos(left, right) >= threshold, иначе "dissimilar".
    // threshold обязан лежать в [-1, 1]. При нарушении контракта
    // (пустые/разнородные/нулевые векторы, некорректный порог)
    // бросает std::invalid_argument.
    [[nodiscard]] std::string classify(
        const std::vector<double>& left,
        const std::vector<double>& right,
        double threshold) const;
};
