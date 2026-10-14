#pragma once

#include <curl/curl.h>
#include <stdexcept>

class CurlGlobal
{
public:
    CurlGlobal()
    {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
        {
            throw std::runtime_error("Failed to initialize libcurl");
        }
    }
    ~CurlGlobal()
    {
        curl_global_cleanup();
    }
    
    CurlGlobal(const CurlGlobal&) = delete;
    CurlGlobal& operator=(const CurlGlobal&) = delete;

    CurlGlobal(CurlGlobal&&) = delete;
    CurlGlobal& operator=(CurlGlobal&&) = delete;
};
