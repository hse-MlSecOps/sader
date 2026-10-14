#include <iostream>
#include <string>
#include <memory>
#include <curl/curl.h>
#include "HttpWorker.h"

namespace
{
    struct CurlDeleter {
        void operator()(CURL* pCurl) const noexcept {
            if (pCurl) {
                curl_easy_cleanup(pCurl);
            }
        }
    };
    using curlPtr = std::unique_ptr<CURL, CurlDeleter>;
    
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        const size_t totalSize = size * nmemb;
        try
        {
            auto* buffer = static_cast<std::string*>(userp);
            buffer->append(static_cast<char*>(contents), totalSize);
            return totalSize;
        }
        catch(...)
        {
            return 0;
        }
    }
}

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
    HttpResult result;

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

    curlPtr curl(curl_easy_init());

    if (!curl)
    {
        result.success = false;
        result.error = "Не удалось инициализировать дескриптор curl";
        return result;
    }

    char errbuf[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(curl.get(), CURLOPT_URL, args.url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &result.body);
    curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, errbuf);

    CURLcode res = curl_easy_perform(curl.get());
    if (res != CURLE_OK) 
    {
        result.success = false;
        
        if (errbuf[0] != '\0') 
        {
            result.error = errbuf;
        } 
        else 
        {
            result.error = curl_easy_strerror(res);
        }
        return result;
    }
    long responseCode = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &responseCode);
    result.statusCode = static_cast<int>(responseCode);
    if (result.statusCode >= 200 && result.statusCode < 300) 
    {
        result.success = true;
    } 
    else 
    {
        result.success = false;
        result.error = "HTTP Error " + std::to_string(result.statusCode);
    }
    return result;
}

Result HttpWorker::execute(const Arguments& args)
{
    auto it = args.find("url");
    if (it == args.end())
    {
        return
        {
            false,
            "",
            "Missing required argument: url"
        };
    }
    HttpArguments httpArgs{it->second};
    HttpResult httpResult = execute(httpArgs);
    if (!httpResult.success)
    {
        return
        {
            false,
            "",
            httpResult.error
        };
    }
    return
    {
        true,
        httpResult.body,
        ""
    };
}