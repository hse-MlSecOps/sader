#include <cassert>
#include <iostream>
#include <string>

#include "../workers/process/ProcessWorker.h"

int main()
{
    ProcessWorker worker;

    std::cout << "\nТесты ProcessWorker\n\n";

    {
        Arguments args;
        args["command"] = "echo";
        args["args"] = "hello world";

        Result result = worker.execute(args);

        std::cout << "[Тест 1] Успешный вызов: echo hello world\n";
        std::cout << "  success: " << (result.success ? "true" : "false") << "\n";
        std::cout << "  output:  " << result.output << "\n";

        assert(result.success == true);
        assert(result.error.empty());
        assert(result.output.find("hello world") != std::string::npos);
        assert(result.output.find("exit_code") != std::string::npos);

        std::cout << "ПРОЙДЕН\n\n";
    }

    {
        Arguments args;
        args["command"] = "rm";

        Result result = worker.execute(args);

        std::cout << "[Тест 2] Ошибка: команда rm не в белом списке\n";
        std::cout << "  success: " << (result.success ? "true" : "false") << "\n";
        std::cout << "  error:   " << result.error << "\n";

        assert(result.success == false);
        assert(result.output.empty());
        assert(result.error.find("WhiteList") != std::string::npos);

        std::cout << "ПРОЙДЕН\n\n";
    }

    {
        Arguments args;

        Result result = worker.execute(args);

        std::cout << "[Тест 3] Ошибка: отсутствует аргумент command\n";
        std::cout << "  success: " << (result.success ? "true" : "false") << "\n";
        std::cout << "  error:   " << result.error << "\n";

        assert(result.success == false);
        assert(result.output.empty());
        assert(result.error.find("Отсутствует обязательный аргумент") != std::string::npos);

        std::cout << "ПРОЙДЕН\n\n";
    }

    {
        Arguments args;
        args["command"] = "echo";
        args["args"] = "hello; rm -rf /";

        Result result = worker.execute(args);

        std::cout << "[Тест 4] Ошибка: метасимволы в аргументах (shell injection)\n";
        std::cout << "  success: " << (result.success ? "true" : "false") << "\n";
        std::cout << "  error:   " << result.error << "\n";

        assert(result.success == false);
        assert(result.output.empty());
        assert(result.error.find("метасимволы") != std::string::npos);

        std::cout << " ПРОЙДЕН\n\n";
    }

    {
        Arguments args;
        args["command"] = "pwd";

        Result result = worker.execute(args);

        std::cout << "[Тест 5] Успешный вызов: pwd (без аргументов)\n";
        std::cout << "  success: " << (result.success ? "true" : "false") << "\n";
        std::cout << "  output:  " << result.output << "\n";

        assert(result.success == true);
        assert(result.error.empty());
        assert(result.output.find("exit_code") != std::string::npos);

        std::cout << "ПРОЙДЕН\n\n";
    }

    std::cout << "Все 5 тестов пройдены успешно!\n\n";

    return 0;
}
