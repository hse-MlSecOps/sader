#include <iostream>
#include <algorithm>

#include "HttpWorker.h"

HttpWorker::HttpWorker()
{
    std::cout << "HTTPWorker constructed\n";
}

HttpWorker::~HttpWorker()
{
    std::cout << "HTTPWorker destroyed\n";
}

std::string HttpWorker::name() const
{
    return "http";
}

std::string HttpWorker::description() const
{
    return "http GET and DELETE for current URL" 
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
    
    std::string method = args.method;
    std::transform(method.begin(), method.end(), method.begin(), ::toupper);

    if (method != "GET" && method != "DELETE") // Сюда нужно будет докинуть остальные методы
    {
        error = "Unsupported HTTP method: " + args.method + ". Only GET and DELETE are allowed.";
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
    
    if (method == "DELETE")
    {
        return
        {
            true,
            200,
            "HTTP Delete is not implemented yet",
            ""
        };
    }
    else if (method == "GET")
    {
        return
        {
            true,
            200,
            "HTTP Get is not implemented yet",
            ""
        };
    }

}
