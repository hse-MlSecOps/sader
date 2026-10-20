#include "HashWorker.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>
#include <system_error>

namespace
{
constexpr const char* WorkerName = "hash";
constexpr const char* WorkerDescription =
    "Calculate MD5 and SHA-256 checksums for text values and files";

std::uint32_t rotateLeft(std::uint32_t value, std::uint32_t shift)
{
    return (value << shift) | (value >> (32U - shift));
}

std::uint32_t rotateRight(std::uint32_t value, std::uint32_t shift)
{
    return (value >> shift) | (value << (32U - shift));
}

std::string toHex(const std::vector<std::uint8_t>& bytes)
{
    std::ostringstream output;
    output << std::hex << std::setfill('0');

    for (const std::uint8_t byte : bytes)
    {
        output << std::setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

void appendLittleEndian64(std::vector<std::uint8_t>& data, std::uint64_t value)
{
    for (int i = 0; i < 8; ++i)
    {
        data.push_back(static_cast<std::uint8_t>((value >> (8 * i)) & 0xffU));
    }
}

void appendBigEndian64(std::vector<std::uint8_t>& data, std::uint64_t value)
{
    for (int i = 7; i >= 0; --i)
    {
        data.push_back(static_cast<std::uint8_t>((value >> (8 * i)) & 0xffU));
    }
}

std::string md5(const std::vector<std::uint8_t>& input)
{
    static constexpr std::array<std::uint32_t, 64> shifts = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };

    static constexpr std::array<std::uint32_t, 64> constants = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };

    std::vector<std::uint8_t> data = input;
    const std::uint64_t bitLength = static_cast<std::uint64_t>(data.size()) * 8U;

    data.push_back(0x80);
    while ((data.size() % 64) != 56)
    {
        data.push_back(0);
    }
    appendLittleEndian64(data, bitLength);

    std::uint32_t a0 = 0x67452301;
    std::uint32_t b0 = 0xefcdab89;
    std::uint32_t c0 = 0x98badcfe;
    std::uint32_t d0 = 0x10325476;

    for (std::size_t offset = 0; offset < data.size(); offset += 64)
    {
        std::array<std::uint32_t, 16> words{};

        for (std::size_t i = 0; i < 16; ++i)
        {
            const std::size_t index = offset + i * 4;
            words[i] =
                static_cast<std::uint32_t>(data[index]) |
                (static_cast<std::uint32_t>(data[index + 1]) << 8) |
                (static_cast<std::uint32_t>(data[index + 2]) << 16) |
                (static_cast<std::uint32_t>(data[index + 3]) << 24);
        }

        std::uint32_t a = a0;
        std::uint32_t b = b0;
        std::uint32_t c = c0;
        std::uint32_t d = d0;

        for (std::uint32_t i = 0; i < 64; ++i)
        {
            std::uint32_t f = 0;
            std::uint32_t g = 0;

            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            const std::uint32_t nextD = d;
            d = c;
            c = b;
            b += rotateLeft(a + f + constants[i] + words[g], shifts[i]);
            a = nextD;
        }

        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    }

    std::vector<std::uint8_t> digest;
    digest.reserve(16);

    for (const std::uint32_t word : {a0, b0, c0, d0})
    {
        digest.push_back(static_cast<std::uint8_t>(word & 0xffU));
        digest.push_back(static_cast<std::uint8_t>((word >> 8) & 0xffU));
        digest.push_back(static_cast<std::uint8_t>((word >> 16) & 0xffU));
        digest.push_back(static_cast<std::uint8_t>((word >> 24) & 0xffU));
    }

    return toHex(digest);
}

std::string sha256(const std::vector<std::uint8_t>& input)
{
    static constexpr std::array<std::uint32_t, 64> constants = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    std::vector<std::uint8_t> data = input;
    const std::uint64_t bitLength = static_cast<std::uint64_t>(data.size()) * 8U;

    data.push_back(0x80);
    while ((data.size() % 64) != 56)
    {
        data.push_back(0);
    }
    appendBigEndian64(data, bitLength);

    std::array<std::uint32_t, 8> hash = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    for (std::size_t offset = 0; offset < data.size(); offset += 64)
    {
        std::array<std::uint32_t, 64> words{};

        for (std::size_t i = 0; i < 16; ++i)
        {
            const std::size_t index = offset + i * 4;
            words[i] =
                (static_cast<std::uint32_t>(data[index]) << 24) |
                (static_cast<std::uint32_t>(data[index + 1]) << 16) |
                (static_cast<std::uint32_t>(data[index + 2]) << 8) |
                static_cast<std::uint32_t>(data[index + 3]);
        }

        for (std::size_t i = 16; i < 64; ++i)
        {
            const std::uint32_t s0 =
                rotateRight(words[i - 15], 7) ^
                rotateRight(words[i - 15], 18) ^
                (words[i - 15] >> 3);
            const std::uint32_t s1 =
                rotateRight(words[i - 2], 17) ^
                rotateRight(words[i - 2], 19) ^
                (words[i - 2] >> 10);

            words[i] = words[i - 16] + s0 + words[i - 7] + s1;
        }

        std::uint32_t a = hash[0];
        std::uint32_t b = hash[1];
        std::uint32_t c = hash[2];
        std::uint32_t d = hash[3];
        std::uint32_t e = hash[4];
        std::uint32_t f = hash[5];
        std::uint32_t g = hash[6];
        std::uint32_t h = hash[7];

        for (std::size_t i = 0; i < 64; ++i)
        {
            const std::uint32_t s1 =
                rotateRight(e, 6) ^ rotateRight(e, 11) ^ rotateRight(e, 25);
            const std::uint32_t choose = (e & f) ^ ((~e) & g);
            const std::uint32_t temp1 = h + s1 + choose + constants[i] + words[i];
            const std::uint32_t s0 =
                rotateRight(a, 2) ^ rotateRight(a, 13) ^ rotateRight(a, 22);
            const std::uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
            const std::uint32_t temp2 = s0 + majority;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    std::vector<std::uint8_t> digest;
    digest.reserve(32);

    for (const std::uint32_t word : hash)
    {
        digest.push_back(static_cast<std::uint8_t>((word >> 24) & 0xffU));
        digest.push_back(static_cast<std::uint8_t>((word >> 16) & 0xffU));
        digest.push_back(static_cast<std::uint8_t>((word >> 8) & 0xffU));
        digest.push_back(static_cast<std::uint8_t>(word & 0xffU));
    }

    return toHex(digest);
}
}

HashWorker::HashWorker() = default;

HashWorker::~HashWorker() = default;

std::string HashWorker::name() const
{
    return WorkerName;
}

std::string HashWorker::description() const
{
    return WorkerDescription;
}

Schema HashWorker::schema() const
{
    return {
        {
            {
                "algorithm",
                "string",
                true,
                "Hash algorithm",
                "md5 or sha256"
            },
            {
                "text",
                "string",
                false,
                "Text value to hash",
                "required when path is not provided"
            },
            {
                "path",
                "string",
                false,
                "Path to file to hash",
                "existing regular file, maximum size 10 MiB"
            }
        },
        "Algorithm name, input type and hexadecimal digest"
    };
}

Result HashWorker::makeError(const std::string& message) const
{
    return {false, "", message};
}

Result HashWorker::hashBytes(
    const std::string& algorithm,
    const std::vector<std::uint8_t>& data,
    const std::string& inputType
) const
{
    std::string digest;

    if (algorithm == "md5")
    {
        digest = md5(data);
    }
    else if (algorithm == "sha256")
    {
        digest = sha256(data);
    }
    else
    {
        return makeError("unsupported algorithm: " + algorithm);
    }

    std::ostringstream output;
    output
        << "Algorithm: " << algorithm << '\n'
        << "Input: " << inputType << '\n'
        << "Digest: " << digest;

    return {true, output.str(), ""};
}

Result HashWorker::execute(const Arguments& args)
{
    for (const auto& entry : args)
    {
        if (entry.first != "algorithm" &&
            entry.first != "text" &&
            entry.first != "path")
        {
            return makeError("Unknown argument: " + entry.first);
        }
    }

    const auto algorithmIt = args.find("algorithm");
    if (algorithmIt == args.end() || algorithmIt->second.empty())
    {
        return makeError("algorithm is required");
    }

    const bool hasText = args.find("text") != args.end();
    const bool hasPath = args.find("path") != args.end();

    if (hasText == hasPath)
    {
        return makeError("Provide exactly one input argument: text or path");
    }

    if (hasText)
    {
        const std::string& text = args.at("text");
        const std::vector<std::uint8_t> data(text.begin(), text.end());

        return hashBytes(algorithmIt->second, data, "text");
    }

    const std::string& filePath = args.at("path");
    if (filePath.empty())
    {
        return makeError("path must not be empty");
    }

    const std::filesystem::path path(filePath);
    std::error_code ec;

    if (!std::filesystem::exists(path, ec))
    {
        return makeError(ec ? "Failed to inspect file path: " + ec.message()
                            : "file does not exist");
    }

    if (!std::filesystem::is_regular_file(path, ec))
    {
        return makeError(ec ? "Failed to inspect file type: " + ec.message()
                            : "path must point to a regular file");
    }

    const std::uintmax_t fileSize = std::filesystem::file_size(path, ec);
    if (ec)
    {
        return makeError("Failed to get file size: " + ec.message());
    }

    if (fileSize > MaxFileSizeBytes)
    {
        return makeError("file is too large: maximum size is 10 MiB");
    }

    std::ifstream input(path, std::ios::binary);
    if (!input.is_open())
    {
        return makeError("Failed to open file for reading");
    }

    std::vector<std::uint8_t> data{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    };

    if (input.bad())
    {
        return makeError("Failed to read file");
    }

    if (data.size() > MaxFileSizeBytes)
    {
        return makeError("file is too large: maximum size is 10 MiB");
    }

    return hashBytes(algorithmIt->second, data, "file");
}
