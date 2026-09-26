#include <iostream>

#include "HttpWorker.h"

HttpWorker::HttpWorker()
{
    std::cout << "CHashWorker constructed\n";
}

HttpWorker::~HttpWorker()
{
    std::cout << "CHashWorker destroyed\n";
}

std::string HttpWorker::name() const
{
    return "http";
}

std::string HttpWorker::description() const
{
    return "http GET for current URL";
}

bool HttpWorker::validate(const HttpArguments& args, std::string& error) const
{
    if (args.url.empty())
    {
        error = "URL is required";
        return false;
    }

    if (!args.url.starts_with("http://") &&
        !args.url.starts_with("https://"))
    {
        error = "URL must use http or https scheme";
        return false;
    }

    return true;
}

HttpResult HttpWorker::execute(const HttpArguments& args) const
{
    std::string error;

    if (!validate(args, error))
    {
        return
        {
            false,
            0,
            "",
            error
        };
    }
    return
    {
        true,
        200,
        "HTTP Get is not implemented yet",
        ""
    };
}
