#pragma once
/**
 * @file sha384.h
 * @brief Реализация хеш-функции SHA-384 (FIPS 180-4) с нуля.
 */
#include <string>
#include <cstdint>
#include <array>

namespace pd::crypto {

/**
 * @class Sha384
 * @brief Потоковая реализация SHA-384.
 *
 * Использование:
 * @code
 *   Sha384 h;
 *   h.update("hello");
 *   std::string hex = h.hex();
 * @endcode
 */
class Sha384 {
public:
    Sha384();
    void update(const void* data, size_t len);
    void update(const std::string& s) { update(s.data(), s.size()); }
    /// Возвращает 48-байтовый хеш и завершает поток.
    std::array<uint8_t, 48> digest();
    /// Возвращает hex-представление.
    std::string hex();

    /// Однократное вычисление.
    static std::string hash(const std::string& s);

private:
    void transform(const uint8_t block[128]);
    uint64_t H[8];
    uint8_t  buf[128];
    uint64_t bufLen;
    uint64_t totalBits;
    bool finalized;
};

} // namespace pd::crypto
