#pragma once

#include <cstdint>
#include <sader/Worker.h>
#include <string>
#include <vector>

class HashWorker : public Worker
{
public:
    static constexpr std::uintmax_t MaxFileSizeBytes = 10 * 1024 * 1024;

    HashWorker();
    ~HashWorker() override;

    std::string name() const override;
    std::string description() const override;

    Schema schema() const override;
    Result execute(const Arguments& args) override;

private:
    Result makeError(const std::string& message) const;
    Result hashBytes(
        const std::string& algorithm,
        const std::vector<std::uint8_t>& data,
        const std::string& inputType
    ) const;
};
