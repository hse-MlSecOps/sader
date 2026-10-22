#pragma once

#include "JsonWorker.h"

#include <string>

class WebServer {
public:
    WebServer(JsonWorker& worker, std::string host, int port);

    // Serves until SIGINT. Returns process exit code.
    int run();

private:
    JsonWorker& worker_;
    std::string host_;
    int port_;
};
