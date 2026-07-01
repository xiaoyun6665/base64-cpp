//
// Created by yun on 2026/6/30.
//
module;
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#define LIKELY(x) x
#define UNLIKELY(x) x
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif

export module base64;

static constexpr  std::array<char, 64> kEncodeTable = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/'
};

static constexpr std::array<char, 64> kEncodeTableURL = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','-','_'
};

class DecodeTable {
    std::array<std::uint8_t, 256> table_;

public:
    constexpr DecodeTable() : table_{} {
        table_.fill(0xFF);
        for (int i = 0; i < 64; ++i) {
            table_[static_cast<std::uint8_t>(kEncodeTable[i])] = i;
        }
        // URLSafe
        table_[static_cast<std::uint8_t>('-')] = 62;
        table_[static_cast<std::uint8_t>('_')] = 63;
        table_[static_cast<std::uint8_t>('=')] = 0;
    }

    [[nodiscard]] std::uint8_t at(char c) const noexcept {
        return table_[static_cast<std::uint8_t>(c)];
    }
};

static constexpr DecodeTable kDecodeTable;

FORCE_INLINE std::string encode_impl(std::string_view data, const std::array<char, 64>& table) {
    const std::size_t len = data.size();
    if (len == 0) return {};

    const size_t out_len = 4 * ((len + 2) / 3);
    std::string result;
    result.resize(out_len, '=');

    const std::uint8_t* src = reinterpret_cast<const std::uint8_t*>(data.data());
    char* dst = result.data();

    std::size_t i = 0;
    std::size_t j = 0;

    const std::size_t full_group = len / 3;
    for (std::size_t g = 0; g < full_group; ++g) {
        std::uint32_t triple = (src[i] << 16) | (src[i + 1] << 8) | src[i + 2];
        dst[j] = table[(triple >> 18) & 0x3F];
        dst[j + 1] = table[(triple >> 12) & 0x3F];
        dst[j + 2] = table[(triple >> 6) & 0x3F];
        dst[j + 3] = table[triple & 0x3F];
        i += 3;
        j += 4;
    }

    const size_t remaining = len % 3;
    if (remaining == 1) {
        std::uint32_t triple = src[i] << 16;
        dst[j]   = table[(triple >> 18) & 0x3F];
        dst[j+1] = table[(triple >> 12) & 0x3F];
        // dst[j+2] and dst[j+3] = '='
    } else if (remaining == 2) {
        std::uint32_t triple = (src[i] << 16) | (src[i+1] << 8);
        dst[j]   = table[(triple >> 18) & 0x3F];
        dst[j+1] = table[(triple >> 12) & 0x3F];
        dst[j+2] = table[(triple >> 6)  & 0x3F];
        // dst[j+3] = '='
    }

    return result;
}

FORCE_INLINE std::string decode_impl(std::string_view data) {
    const size_t len = data.size();
    if (len == 0) return "";
    if (len % 4 != 0) {
        throw std::invalid_argument("Invalid base64 string length");
    }

    // 计算填充
    size_t padding = 0;
    if (len >= 1 && data[len-1] == '=') padding++;
    if (len >= 2 && data[len-2] == '=') padding++;

    // 预分配
    const size_t out_len = (len / 4) * 3 - padding;
    std::string result(out_len, '\0');

    const std::uint8_t* src = reinterpret_cast<const std::uint8_t*>(data.data());
    std::uint8_t* dst = reinterpret_cast<std::uint8_t*>(result.data());

    size_t i = 0;
    size_t j = 0;

    // 跳过末尾填充，主循环处理完整4字节组
    const size_t valid_len = len - padding;
    for (; i < valid_len; i += 4, j += 3) {
        std::uint8_t a = kDecodeTable.at(static_cast<char>(src[i]));
        std::uint8_t b = kDecodeTable.at(static_cast<char>(src[i+1]));
        std::uint8_t c = kDecodeTable.at(static_cast<char>(src[i+2]));
        std::uint8_t d = kDecodeTable.at(static_cast<char>(src[i+3]));

        // 检查非法字符（0xFF表示无效）
        if ((a | b | c | d) == 0xFF) [[unlikely]] {
            // 精确找出哪个字符非法
            if (a == 0xFF && src[i] != '=')
                throw std::invalid_argument("Invalid character at position " + std::to_string(i));
            if (b == 0xFF && src[i+1] != '=')
                throw std::invalid_argument("Invalid character at position " + std::to_string(i+1));
            if (c == 0xFF && src[i+2] != '=')
                throw std::invalid_argument("Invalid character at position " + std::to_string(i+2));
            if (d == 0xFF && src[i+3] != '=')
                throw std::invalid_argument("Invalid character at position " + std::to_string(i+3));
        }

        std::uint32_t quad = (a << 18) | (b << 12) | (c << 6) | d;
        dst[j]   = (quad >> 16) & 0xFF;
        if (j + 1 < out_len) dst[j+1] = (quad >> 8) & 0xFF;
        if (j + 2 < out_len) dst[j+2] = quad & 0xFF;
    }

    return result;
}


// ============================================================
// 公开API
// ============================================================
export namespace base64 {
    std::string encode(std::string_view input) {
        return encode_impl(input, kEncodeTable);
    }

    std::string decode(std::string_view input) {
        return decode_impl(input);
    }

    std::string encodeURI(std::string_view input) {
        return encode_impl(input, kEncodeTableURL);
    }

    // JavaScript通常用Uint8Array处理二进制，额外提供字节版本
    std::string encodeFromBytes(const std::string& bytes) {
        return encode_impl(bytes, kEncodeTable);
    }

    std::string decodeToBytes(std::string_view input) {
        return decode_impl(input);
    }
}
