#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <cstring>
#include "base85ed.h"

namespace base85
{
    constexpr uint32_t POW85[] = { 52200625, 614125, 7225, 85, 1 };

    std::vector<uint8_t> encode(std::vector<uint8_t> const& bytes)
    {
        std::vector<uint8_t> out;
        size_t i = 0;
        size_t len = bytes.size();

        while (i < len)
        {
            size_t chunk_size = len - i;
            if (chunk_size > 4) chunk_size = 4;

            uint32_t value = 0;
            for (size_t j = 0; j < 4; ++j)
            {
                value <<= 8;
                if (j < chunk_size)
                {
                    value |= bytes[i + j];
                }
            }

            if (chunk_size == 4 && value == 0)
            {
                out.push_back('z');
                i += 4;
                continue;
            }

            uint8_t encoded_chunk[5];
            uint32_t temp = value;
            for (int j = 0; j < 5; ++j)
            {
                encoded_chunk[j] = static_cast<uint8_t>((temp / POW85[j]) % 85 + '!');
                temp %= POW85[j];
            }

            size_t chars_to_output = (chunk_size == 4) ? 5 : (chunk_size + 1);

            for (size_t j = 0; j < chars_to_output; ++j)
            {
                out.push_back(encoded_chunk[j]);
            }

            i += chunk_size;
        }
        return out;
    }

    std::vector<uint8_t> decode(std::vector<uint8_t> const& b85str)
    {
        std::vector<uint8_t> out;
        size_t i = 0;
        size_t len = b85str.size();

        while (i < len)
        {
            if (b85str[i] == 'z')
            {
                out.push_back(0);
                out.push_back(0);
                out.push_back(0);
                out.push_back(0);
                i++;
                continue;
            }

            size_t group_size = 0;
            uint32_t value = 0;

            for (size_t j = 0; j < 5; ++j)
            {
                if (i + j >= len) break;

                uint8_t c = b85str[i + j];

                if (c < '!' || c > 'u')
                {
                    throw std::runtime_error("Invalid character in Base85 string.");
                }

                value = value * 85 + (c - '!');
                group_size++;
            }

            if (group_size < 2)
            {
                throw std::runtime_error("Invalid Base85 string length or padding.");
            }

            size_t bytes_to_write = group_size - 1;

            for (int k = 3; k >= 0; --k)
            {
                if (bytes_to_write > 0)
                {
                    out.push_back(static_cast<uint8_t>((value >> (k * 8)) & 0xFF));
                    bytes_to_write--;
                }
            }

            i += group_size;
        }
        return out;
    }
}