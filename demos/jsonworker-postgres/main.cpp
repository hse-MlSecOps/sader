#include "JsonWorker.h"
#include "WebServer.h"

#include <iostream>
#include <memory>
#include <string>

namespace {

const char* kDsn = "host=127.0.0.1 port=5432 dbname=sader_demo user=sader password=sader";

void report(const std::string& title, const Result& result) {
    std::cout << title << ": ok = " << (result.ok ? "true" : "false");
    if (result.ok) {
        std::cout << ", value = " << result.value << "\n";
    } else {
        std::cout << ", error = " << result.error << "\n";
    }
}

int runDemo() {
    JsonWorker worker(kDsn);

    std::cout << "\n=== " << worker.name() << " - " << worker.description() << " ===\n";
    for (const auto& arg : worker.schema()) {
        std::cout << "  arg '" << arg.name << "': type=" << arg.type
                  << ", required=" << (arg.required ? "true" : "false")
                  << ", constraints=" << arg.constraints << "\n";
    }

    std::cout << "\n--- JsonWorker (PostgreSQL-backed store) ---\n";
    report("Call 1 (put)",
           worker.execute({{"op", "put"},
                           {"key", "user:ivan"},
                           {"json", R"({"name":"Ivan","age":27,"user":{"city":"Moscow","roles":["admin","dev"]}})"}}));
    report("Call 2 (get whole document)",
           worker.execute({{"op", "get"}, {"key", "user:ivan"}}));
    report("Call 3 (get by path)",
           worker.execute({{"op", "get"}, {"key", "user:ivan"}, {"path", "user.city"}}));
    report("Call 4 (list)",
           worker.execute({{"op", "list"}}));
    report("Call 5 (missing key)",
           worker.execute({{"op", "get"}}));
    report("Call 6 (invalid json)",
           worker.execute({{"op", "put"}, {"key", "bad"}, {"json", R"({"name": )"}}));
    report("Call 7 (path not found)",
           worker.execute({{"op", "get"}, {"key", "user:ivan"}, {"path", "missing.field"}}));
    report("Call 8 (delete)",
           worker.execute({{"op", "delete"}, {"key", "user:ivan"}}));
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        return runDemo();
    }

    JsonWorker worker(kDsn);
    WebServer server(worker, "127.0.0.1", 8080);
    return server.run();
}
