#pragma once
/**
 * @file vigenere.h
 * @brief Реализация классического шифра Виженера (Unicode + Latin/Cyrillic).
 */
#include <string>

namespace pd::crypto {

/**
 * @class Vigenere
 * @brief Шифр Виженера. Сохраняет регистр и пропускает не-буквенные символы.
 *
 * Поддерживаются латинский (A–Z) и кириллический (А–Я) алфавиты независимо.
 * Ключ должен содержать хотя бы один буквенный символ.
 */
class Vigenere {
public:
    /// Шифрование открытого текста.
    static std::string encrypt(const std::string& plain, const std::string& key);
    /// Расшифрование шифротекста.
    static std::string decrypt(const std::string& cipher, const std::string& key);

private:
    static std::string transform(const std::string& text, const std::string& key, bool encrypt);
};

} // namespace pd::crypto
