#pragma once
/**
 * @file md5.h
 * @brief Реализация хеш-функции MD5 (RFC 1321) с нуля.
 */
#include <string>
#include <cstdint>
#include <array>

namespace pd::crypto {

/**
 * @class Md5
 * @brief Потоковая реализация MD5.
 *
 * Использование:
 * @code
 *   Md5 h;
 *   h.update("hello");
 *   std::string hex = h.hex();
 * @endcode
 */
class Md5 {
public:
    Md5();
    void update(const void* data, size_t len);
    void update(const std::string& s) { update(s.data(), s.size()); }
    /// Возвращает 16-байтовый хеш и завершает поток.
    std::array<uint8_t, 16> digest();
    /// Возвращает hex-представление (32 символа).
    std::string hex();

    /// Однократное вычисление.
    static std::string hash(const std::string& s);

private:
    void transform(const uint8_t block[64]);
    uint32_t H[4];
    uint8_t  buf[64];
    size_t   bufLen;
    uint64_t totalBits;
    bool     finalized;
};

} // namespace pd::crypto
