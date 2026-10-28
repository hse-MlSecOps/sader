#pragma once
#include <sader/Worker.h>
// Дополнительный обработчик CSV-файлов для SADer.
// Наследуется от базового интерфейса Worker.
class ExtendedCsvWorker : public Worker
{
public:
 // Возвращает имя обработчика
    std::string name() const override;
// Возвращает описание обработчика
    std::string description() const override;

// Описывает аргументы, поддерживаемые обработчиком
    Schema schema() const override;
    // Выполняет операцию с CSV-файлом
    Result execute(const Arguments& args) override;
};
