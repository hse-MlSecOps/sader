#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "ProcessWorker.h"

namespace {

class PipeGuard
{
public:
    explicit PipeGuard(const std::string& cmd)
        : pipe_(popen(cmd.c_str(), "r"))
    {
        if (!pipe_)
        {
            throw std::runtime_error("Не удалось открыть канал для команды: " + cmd);
        }
    }

    ~PipeGuard()
    {
        if (pipe_)
        {
            pclose(pipe_);
        }
    }

    PipeGuard(const PipeGuard&) = delete;
    PipeGuard& operator=(const PipeGuard&) = delete;

    FILE* get() const { return pipe_; }

    int close()
    {
        int status = pclose(pipe_);
        pipe_ = nullptr;
        return status;
    }

private:
    FILE* pipe_;
};

std::string captureOutput(FILE* pipe)
{
    std::string result;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        result += buffer;
    }

    return result;
}

}

ProcessWorker::ProcessWorker()
    : whitelist_{"ls", "echo", "date", "whoami", "uname", "cat", "wc", "pwd", "hostname"}
{
    std::cout << "ProcessWorker создан\n";
}

ProcessWorker::~ProcessWorker()
{
    std::cout << "ProcessWorker уничтожен\n";
}

std::string ProcessWorker::name() const
{
    return "process";
}

std::string ProcessWorker::description() const
{
    return "Выполнение команды из белого списка с захватом stdout/exit code";
}

Schema ProcessWorker::schema() const
{
    return Schema{
        {
            {"command", "string", true,
             "Команда для выполнения (должна быть в WhiteList",
             "Должна быть одной из: ls, echo, date, whoami, uname, cat, wc, pwd, hostname"},
            {"args", "string", false,
             "Аргументы для передачи команде",
             "Не должны содержать метасимволы оболочки: ; | & ` $ ( ) { }"}
        },
        "JSON с полями stdout (string) и exit_code (int)"
    };
}

bool ProcessWorker::isInWhitelist(const std::string& command) const
{
    return whitelist_.count(command) > 0;
}

bool ProcessWorker::hasUnsafeCharacters(const std::string& input) const
{
    const std::string unsafe = ";|&`$(){}><";

    for (char c : input)
    {
        if (unsafe.find(c) != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

Result ProcessWorker::execute(const Arguments& args)
{
    auto commandIt = args.find("command");
    if (commandIt == args.end() || commandIt->second.empty())
    {
        return Result{false, "", "Отсутствует обязательный аргумент: command"};
    }

    const std::string& command = commandIt->second;

    if (!isInWhitelist(command))
    {
        return Result{false, "", "Команда не в WhiteList: " + command};
    }

    std::string cmdArgs;
    auto argsIt = args.find("args");
    if (argsIt != args.end())
    {
        cmdArgs = argsIt->second;
    }

    if (hasUnsafeCharacters(cmdArgs))
    {
        return Result{false, "", "Недопустимые символы в аргументах (метасимволы оболочки запрещены)"};
    }

    if (cmdArgs.size() > 1024)
    {
        return Result{false, "", "Аргументы слишком длинные (максимум 1024 символа)"};
    }

    std::string fullCommand = command;
    if (!cmdArgs.empty())
    {
        fullCommand += " " + cmdArgs;
    }

    try
    {
        PipeGuard pipe(fullCommand);

        std::string output = captureOutput(pipe.get());
        int rawStatus = pipe.close();

        int exitCode = WEXITSTATUS(rawStatus);

        std::string resultJson =
            "{\"stdout\": \"" + output + "\", \"exit_code\": " + std::to_string(exitCode) + "}";

        return Result{true, resultJson, ""};
    }
    catch (const std::exception& e)
    {
        return Result{false, "", std::string("Ошибка выполнения: ") + e.what()};
    }
}
