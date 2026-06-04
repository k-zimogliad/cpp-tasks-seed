#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <cstring>

#include "base85ed.h"

namespace base85
{

    constexpr uint32_t POW85[] = { 85 * 85 * 85 * 85, 85 * 85 * 85, 85 * 85, 85, 1 };

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
            for (size_t j = 0; j < chunk_size; ++j)
            {
                value |= static_cast<uint32_t>(bytes[i + j]) << (24 - j * 8);
            }

            uint8_t encoded_chunk[5];
            uint32_t temp = value;
            for (int j = 4; j >= 0; --j)
            {
                encoded_chunk[j] = static_cast<uint8_t>((temp % 85) + '!');
                temp /= 85;
            }

            for (size_t j = 0; j < chunk_size + 1; ++j)
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
            size_t chunk_size = len - i;
            if (chunk_size > 5) chunk_size = 5;

            if (chunk_size < 2)
            {
                throw std::runtime_error("Invalid Base85 string length.");
            }

            uint32_t value = 0;
            for (size_t j = 0; j < chunk_size; ++j)
            {
                uint32_t c = b85str[i + j] & 0xFF;
                if (c < '!' || c > 'u')
                {
                    throw std::runtime_error("Invalid character in Base85 string.");
                }

                value += (c - '!') * POW85[j + (5 - chunk_size)];
            }

            if (chunk_size < 5)
            {
                for (size_t j = chunk_size; j < 5; ++j)
                {
                    value += static_cast<uint32_t>('u' - '!') * POW85[j + (5 - chunk_size)];
                }
            }

            size_t bytes_to_write = chunk_size - 1;
            for (size_t j = 0; j < bytes_to_write; ++j)
            {
                out.push_back(static_cast<uint8_t>((value >> (24 - j * 8)) & 0xFF));
            }

            i += chunk_size;
        }

        return out;
    }

}