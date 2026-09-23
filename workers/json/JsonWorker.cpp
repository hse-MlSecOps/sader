#include "JsonWorker.h"
#include <iostream>
#include <nlohmann/json.hpp>

JsonWorker::JsonWorker() {
    std::cout << "JsonWorker constructed\n";
}

JsonWorker::~JsonWorker() {
    std::cout << "JsonWorker destructed\n";
}

std::string JsonWorker::name() const {
    return "json";
}

std::string JsonWorker::description() const {
    return "Parse JSON and get a value by path";
}

std::vector<ArgumentInfo> JsonWorker::schema() const {
    return {
        {"json", "string", true},
        {"path", "string", true}
    };
}

Result JsonWorker::execute(const std::string& jsonText, const std::string& key) const {
    nlohmann::json parsed;

    try {
        parsed = nlohmann::json::parse(jsonText);
    } catch (const std::exception& e) {
        Result failure;
        failure.ok = false;
        failure.value = "";
        failure.error = std::string("invalid json: ") + e.what();
        return failure;
    }

    if (parsed.contains(key)) {
        Result success;
        success.ok = true;
        success.value = parsed[key].dump();
        success.error = "";
        return success;
    }

    Result failure;
    failure.ok = false;
    failure.value = "";
    failure.error = "path not found: " + key;
    return failure;
}