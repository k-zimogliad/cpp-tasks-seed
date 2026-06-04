#include <vector>
#include <cstdint>
#include <stdexcept>
#include <array>

#include "base85ed.h"

namespace
{
constexpr char BASE85_ALPHABET[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    "!#$%&()*+-;<=>?@^_`{|}~";

std::array<int, 256> make_decode_table()
{
    std::array<int, 256> table{};
    table.fill(-1);

    for (int i = 0; i < 85; ++i)
    {
        table[static_cast<unsigned char>(BASE85_ALPHABET[i])] = i;
    }

    return table;
}

const std::array<int, 256> DECODE_TABLE = make_decode_table();
}

namespace base85
{
std::vector<uint8_t> encode(std::vector<uint8_t> const& bytes)
{
    std::vector<uint8_t> out;

    for (size_t i = 0; i < bytes.size(); i += 4)
    {
        size_t chunk_size = std::min<size_t>(4, bytes.size() - i);

        uint32_t value = 0;

        for (size_t j = 0; j < 4; ++j)
        {
            value <<= 8;

            if (j < chunk_size)
            {
                value |= bytes[i + j];
            }
        }

        char encoded[5];

        for (int k = 4; k >= 0; --k)
        {
            encoded[k] = BASE85_ALPHABET[value % 85];
            value /= 85;
        }

        size_t chars_to_write = (chunk_size == 4)
                                ? 5
                                : chunk_size + 1;

        for (size_t j = 0; j < chars_to_write; ++j)
        {
            out.push_back(static_cast<uint8_t>(encoded[j]));
        }
    }

    return out;
}

std::vector<uint8_t> decode(std::vector<uint8_t> const& b85str)
{
    std::vector<uint8_t> out;

    size_t i = 0;

    while (i < b85str.size())
    {
        size_t remaining = b85str.size() - i;
        size_t group_size = std::min<size_t>(5, remaining);

        if (group_size == 1)
        {
            throw std::runtime_error("Invalid Base85 string length");
        }

        uint32_t value = 0;

        for (size_t j = 0; j < 5; ++j)
        {
            int digit;

            if (j < group_size)
            {
                uint8_t c = b85str[i + j];

                digit = DECODE_TABLE[c];

                if (digit < 0)
                {
                    throw std::runtime_error("Invalid character in Base85 string");
                }
            }
            else
            {
                digit = 84;
            }

            value = value * 85 + static_cast<uint32_t>(digit);
        }

        uint8_t decoded[4];

        decoded[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
        decoded[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
        decoded[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
        decoded[3] = static_cast<uint8_t>(value & 0xFF);

        size_t bytes_to_write = group_size - 1;

        for (size_t j = 0; j < bytes_to_write; ++j)
        {
            out.push_back(decoded[j]);
        }

        i += group_size;
    }

    return out;
}
}