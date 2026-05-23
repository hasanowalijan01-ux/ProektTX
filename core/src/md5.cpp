#include "md5.h"
#include <cstring>
#include <cstdio>
#include <algorithm>

namespace pd::crypto {

static const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static const uint8_t S[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

static inline uint32_t ROTL(uint32_t x, uint32_t n) { return (x << n) | (x >> (32 - n)); }

static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (~x & z); }
static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (x & z) | (y & ~z); }
static inline uint32_t Hfn(uint32_t x, uint32_t y, uint32_t z) { return x ^ y ^ z; }
static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return y ^ (x | ~z); }

Md5::Md5() {
    H[0] = 0x67452301;
    H[1] = 0xefcdab89;
    H[2] = 0x98badcfe;
    H[3] = 0x10325476;
    bufLen = 0;
    totalBits = 0;
    finalized = false;
}

void Md5::transform(const uint8_t block[64]) {
    uint32_t M[16];
    for (int i = 0; i < 16; ++i)
        M[i] = (uint32_t)block[i * 4]
             | ((uint32_t)block[i * 4 + 1] << 8)
             | ((uint32_t)block[i * 4 + 2] << 16)
             | ((uint32_t)block[i * 4 + 3] << 24);

    uint32_t a = H[0], b = H[1], c = H[2], d = H[3];

    for (int i = 0; i < 64; ++i) {
        uint32_t f, g;
        if (i < 16) {
            f = F(b, c, d);
            g = (uint32_t)i;
        } else if (i < 32) {
            f = G(b, c, d);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = Hfn(b, c, d);
            g = (3 * i + 5) % 16;
        } else {
            f = I(b, c, d);
            g = (7 * i) % 16;
        }
        uint32_t tmp = d;
        d = c;
        c = b;
        b = b + ROTL(a + f + K[i] + M[g], S[i]);
        a = tmp;
    }

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
}

void Md5::update(const void* data, size_t len) {
    if (finalized) return;
    const uint8_t* p = static_cast<const uint8_t*>(data);
    totalBits += static_cast<uint64_t>(len) * 8;
    while (len > 0) {
        size_t take = std::min<size_t>(64 - bufLen, len);
        std::memcpy(buf + bufLen, p, take);
        bufLen += take;
        p += take;
        len -= take;
        if (bufLen == 64) {
            transform(buf);
            bufLen = 0;
        }
    }
}

std::array<uint8_t, 16> Md5::digest() {
    if (!finalized) {
        uint64_t bitsLen = totalBits;
        uint8_t pad[128] = {0};
        pad[0] = 0x80;
        size_t padLen = (bufLen < 56) ? (56 - bufLen) : (120 - bufLen);
        update(pad, padLen);

        uint8_t lenBlock[8];
        for (int i = 0; i < 8; ++i)
            lenBlock[i] = static_cast<uint8_t>(bitsLen >> (i * 8));
        std::memcpy(buf + bufLen, lenBlock, 8);
        bufLen += 8;
        transform(buf);
        bufLen = 0;
        finalized = true;
    }

    std::array<uint8_t, 16> out{};
    for (int i = 0; i < 4; ++i) {
        out[i * 4 + 0] = static_cast<uint8_t>(H[i]);
        out[i * 4 + 1] = static_cast<uint8_t>(H[i] >> 8);
        out[i * 4 + 2] = static_cast<uint8_t>(H[i] >> 16);
        out[i * 4 + 3] = static_cast<uint8_t>(H[i] >> 24);
    }
    return out;
}

std::string Md5::hex() {
    auto d = digest();
    std::string s;
    s.reserve(32);
    char b[3];
    for (auto v : d) {
        std::snprintf(b, sizeof b, "%02x", v);
        s.append(b, 2);
    }
    return s;
}

std::string Md5::hash(const std::string& s) {
    Md5 h;
    h.update(s);
    return h.hex();
}

} // namespace pd::crypto
