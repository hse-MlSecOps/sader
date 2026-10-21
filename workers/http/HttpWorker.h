#pragma once

#include <sader/Worker.h>

struct HttpArguments
{
    std::string url;
    std::string method = "GET"; // добаил метод по умолчанию
};

struct HttpResult
{
    bool success = false;
    int statusCode = 0;
    std::string body;
    std::string error;
};

class HttpWorker : public Worker
{
public:
    HttpWorker();
    ~HttpWorker() override;

    std::string name() const override;
    std::string description() const override;
    HttpResult execute(const HttpArguments& args) const ; 
    Result execute(const Arguments& args) override; // добавил чтобы не менять базовый класс Worker
     
private:
    bool validate(const HttpArguments& args, std::string& error) const;
};