#include <iostream>
#include <algorithm>
#include <curl/curl.h>
#include <string>

#include "HttpWorker.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* responseString = static_cast<std::string*>(userp);
    responseString->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


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
    return "http GET and DELETE for current URL"; // добавил DELETE
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
    
    std::string method = args.method;
    std::transform(method.begin(), method.end(), method.begin(), ::toupper);

    if (method == "DELETE")
    {
        // Добавил: Инициализация сессии cURL
        CURL* curl = curl_easy_init();
        if (!curl)
        {
            return { false, 0, "", "Failed to initialize cURL" };
        }

        // Добавил: Переменные для хранения ответа и статус-кода
        std::string responseBuffer;
        long responseCode = 0;

        // ДОБАВЛЕНО: Настройка URL назначения
        curl_easy_setopt(curl, CURLOPT_URL, args.url.c_str());
        // ДОБАВЛЕНО: Установка HTTP-метода в DELETE
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        // ДОБАВЛЕНО: Привязка колбэка для записи входящих данных от сервера
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
        
        // ДОБАВЛЕНО: Ограничение времени ожидания запроса (10 секунд)
        //curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        // ДОБАВЛЕНО: Выполнение сетевого запроса
        CURLcode res = curl_easy_perform(curl);
        
        // ДОБАВЛЕНО: Проверка на сетевые ошибки cURL (например, нет интернета или DNS умер)
        if (res != CURLE_OK)
        {
            std::string curlError = curl_easy_strerror(res);
            curl_easy_cleanup(curl);
            return { false, 0, "", "cURL error: " + curlError };
        }

        // Оценка ответа от сервера
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        bool success = (responseCode >= 200 && responseCode < 300);

        curl_easy_cleanup(curl);
        
        return
        {
            success,
            static_cast<int>(responseCode),
            responseBuffer,
            success ? "" : "HTTP request failed with status code " + std::to_string(responseCode)
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

    return { false, 0, "", "Unknown error" };

}

Result HttpWorker::execute(const Arguments& args)
{
    HttpArguments httpArgs;

    // Извлекаем "url" из std::unordered_map
    auto urlIt = args.find("url");
    if (urlIt != args.end())
    {
        httpArgs.url = urlIt->second;
    }

    // Извлекаем "method" из std::unordered_map (если не передан, останется дефолтный "GET")
    auto methodIt = args.find("method");
    if (methodIt != args.end())
    {
        httpArgs.method = methodIt->second;
    }

    // Вызываем вашу внутреннюю строго типизированную логику
    HttpResult httpResult = execute(httpArgs);

    // Мапим внутренний HttpResult в базовую структуру Result проекта Sader
    Result baseResult;
    baseResult.success = httpResult.success;
    
    if (httpResult.success)
    {
        // В output пишем тело ответа (body)
        baseResult.output = httpResult.body;
        baseResult.error = "";
    }
    else
    {
        baseResult.output = "";
        baseResult.error = httpResult.error;
    }

    return baseResult;
}
