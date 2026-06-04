#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <random>
#include <chrono>

#include "base85ed.h"


static std::vector<uint8_t> cast_to_u8(std::vector<uint8_t> const& v)
{
    return v;
}

static std::vector<uint8_t> make_random_sequence(size_t length, uint32_t seed_value)
{
    std::vector<uint8_t> result(length);
    std::mt19937 engine(seed_value);
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < length; ++i)
    {
        result[i] = static_cast<uint8_t>(dist(engine));
    }
    return result;
}

TEST(Base85RoundTrip, ShortAndMixedStrings)
{
    const std::vector<std::string> sample_strings = 
    {
        "A", "AB", "ABC", "ABCD", "ABCDE",
        "Modern C++", "Base85 Specification",
        "!@#$%^&*()_+{}|:<>?[];',./",
        "9876543210", "XYZxyz",
        "\x01\x02\x03\x04", "\x00\xFF\x00\xFF",
        "\x7F\x80\x00\x01\xFF"
    };

    for (const auto& str : sample_strings)
    {
        std::vector<uint8_t> original(str.begin(), str.end());
        auto enc = cast_to_u8(base85::encode(original));
        auto dec = cast_to_u8(base85::decode(enc));
        EXPECT_EQ(original, dec);
    }
}

TEST(Base85RoundTrip, VariousVariableLengths)
{
    std::vector<size_t> chunk_sizes = { 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 32, 64, 128, 512, 2048 };
    uint32_t seed = 42;

    for (size_t size : chunk_sizes)
    {
        auto original_data = make_random_sequence(size, seed++);
        auto encoded_data = cast_to_u8(base85::encode(original_data));
        auto decoded_data = cast_to_u8(base85::decode(encoded_data));
        EXPECT_EQ(original_data, decoded_data);
    }
}

TEST(Base85SpecificPatterns, ZeroFilledBlocks)
{
    std::vector<uint8_t> empty_block(4, 0);

    auto enc = base85::encode(empty_block);

    EXPECT_EQ(enc.size(), 5);

    auto dec = base85::decode(enc);
    EXPECT_EQ(dec, empty_block);
}

TEST(Base85SpecificPatterns, MultipleZeroBlocks)
{
    std::vector<uint8_t> blocks(16, 0);
    auto enc = cast_to_u8(base85::encode(blocks));
    auto dec = cast_to_u8(base85::decode(enc));
    EXPECT_EQ(dec, blocks);
}

TEST(Base85SpecificPatterns, ConsecutiveZeroLengths)
{
    for (size_t len = 1; len <= 24; ++len)
    {
        std::vector<uint8_t> zeros(len, 0);
        auto enc = cast_to_u8(base85::encode(zeros));
        auto dec = cast_to_u8(base85::decode(enc));
        EXPECT_EQ(dec, zeros);
    }
}

TEST(Base85SpecificPatterns, MaximumByteValues)
{
    for (size_t len = 1; len <= 12; ++len)
    {
        std::vector<uint8_t> max_bytes(len, 0xFF);
        auto enc = cast_to_u8(base85::encode(max_bytes));
        auto dec = cast_to_u8(base85::decode(enc));
        EXPECT_EQ(dec, max_bytes);
    }
}

TEST(Base85SpecificPatterns, AlternatingBitMasks)
{
    for (size_t len = 1; len <= 20; ++len)
    {
        std::vector<uint8_t> pattern(len);
        for (size_t i = 0; i < len; ++i)
        {
            pattern[i] = (i % 2 == 0) ? 0x55 : 0xAA;
        }
        auto enc = cast_to_u8(base85::encode(pattern));
        auto dec = cast_to_u8(base85::decode(enc));
        EXPECT_EQ(dec, pattern);
    }
}

TEST(Base85EdgeCases, SingleByteInputs)
{
    std::vector<uint8_t> values = { 0x00, 0x01, 0x10, 0x7F, 0x80, 0xFE, 0xFF };
    for (uint8_t val : values)
    {
        std::vector<uint8_t> input = { val };
        auto enc = cast_to_u8(base85::encode(input));
        auto dec = cast_to_u8(base85::decode(enc));
        EXPECT_EQ(dec, input);
    }
}

TEST(Base85EdgeCases, ExhaustiveByteRange)
{
    std::vector<uint8_t> all_bytes(256);
    for (int i = 0; i < 256; ++i)
    {
        all_bytes[i] = static_cast<uint8_t>(i);
    }
    auto enc = cast_to_u8(base85::encode(all_bytes));
    auto dec = cast_to_u8(base85::decode(enc));
    EXPECT_EQ(dec, all_bytes);
}

TEST(Base85Validation, MalformedCharacters)
{
    std::vector<uint8_t> invalid_chars =
    {
        0x00,
        0x1F,
        0x7F
    };

    for (uint8_t bad_char : invalid_chars)
    {
        std::vector<uint8_t> invalid_input = 
        {
            'c', 'e', '`', 'E', bad_char
        };

        EXPECT_THROW(
            base85::decode(invalid_input),
            std::runtime_error
        );
    }
}

TEST(Base85Validation, IncompleteSymbolGroups)
{
    std::vector<uint8_t> single_char = { 'F' };
    EXPECT_THROW(base85::decode(single_char), std::runtime_error);

    std::vector<uint8_t> bad_trailing = { 'c', 'e', '`', 'E', '_', 'F' };
    EXPECT_THROW(base85::decode(bad_trailing), std::runtime_error);
}

TEST(Base85Stress, MassiveDataBuffers)
{
    std::vector<size_t> large_sizes = { 5000, 15000, 30000, 60000 };
    uint32_t current_seed = 101;

    for (size_t size : large_sizes)
    {
        auto original = make_random_sequence(size, current_seed++);

        auto start = std::chrono::high_resolution_clock::now();
        auto enc = cast_to_u8(base85::encode(original));
        auto dec = cast_to_u8(base85::decode(enc));
        auto end = std::chrono::high_resolution_clock::now();

        EXPECT_EQ(original, dec);

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        EXPECT_GE(ms, 0);
    }
}

TEST(Base85Stress, ContinuousExecutionStability)
{
    auto dataset = make_random_sequence(75000, 999);
    for (int iteration = 0; iteration < 5; ++iteration)
    {
        auto enc = cast_to_u8(base85::encode(dataset));
        auto dec = cast_to_u8(base85::decode(enc));
        EXPECT_EQ(dataset, dec);
    }
}