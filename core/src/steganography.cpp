#include "steganography.h"

namespace pd::stego {

size_t capacity(size_t bytes) {
    if (bytes <= 32) return 0;
    return (bytes - 32) / 8;
}

static void writeBit(std::vector<uint8_t>& p, size_t idx, int bit) {
    p[idx] = (p[idx] & 0xFE) | (bit & 1);
}
static int readBit(const std::vector<uint8_t>& p, size_t idx) {
    return p[idx] & 1;
}

bool embed(std::vector<uint8_t>& pixels, const std::string& msg) {
    uint32_t len = (uint32_t)msg.size();
    if (pixels.size() < 32 + (size_t)len * 8) return false;
    // length 32 bits LSB-first
    for (int i = 0; i < 32; ++i)
        writeBit(pixels, i, (len >> i) & 1);
    for (size_t i = 0; i < msg.size(); ++i) {
        uint8_t b = (uint8_t)msg[i];
        for (int j = 0; j < 8; ++j)
            writeBit(pixels, 32 + i*8 + j, (b >> j) & 1);
    }
    return true;
}

std::string extract(const std::vector<uint8_t>& pixels) {
    if (pixels.size() < 32) return {};
    uint32_t len = 0;
    for (int i = 0; i < 32; ++i)
        len |= ((uint32_t)readBit(pixels, i)) << i;
    if (pixels.size() < 32 + (size_t)len * 8) return {};
    if (len > capacity(pixels.size())) return {};
    std::string s; s.reserve(len);
    for (uint32_t i = 0; i < len; ++i) {
        uint8_t b = 0;
        for (int j = 0; j < 8; ++j)
            b |= (uint8_t)(readBit(pixels, 32 + i*8 + j) << j);
        s.push_back((char)b);
    }
    return s;
}

} // namespace pd::stego
