#include "JsonWorker.h"
#include <iostream>

int main() {
    JsonWorker worker;

    std::cout << worker.name() << " - " << worker.description() << "\n\n";

    Result r1 = worker.execute(R"({"name": "Ivan", "age": 27})", "name");
    std::cout << "Call 1 (ok case): ok = " << r1.ok << ", value = " << r1.value << "\n";

    Result r2 = worker.execute(R"({"name": "Ivan", "age": 27})", "city");
    std::cout << "Call 2 (missing key): ok = " << r2.ok << ", error = " << r2.error << "\n";

    Result r3 = worker.execute(R"({"name": )", "name");
    std::cout << "Call 3 (invalid json): ok = " << r3.ok << ", error = " << r3.error << "\n";
}