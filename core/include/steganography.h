#pragma once
/**
 * @file steganography.h
 * @brief LSB-стеганография: внедрение и извлечение текстовых сообщений в RGB-байты.
 *
 * Алгоритм:
 *  - первые 32 младших бита младших байт пикселей хранят длину сообщения (в байтах);
 *  - далее по одному биту младшего разряда каждого байта пикселя записывается
 *    последовательность бит сообщения (UTF-8).
 *
 * Работает на любом сыром буфере байтов (например, плоский RGB(A) из QImage).
 */
#include <string>
#include <vector>
#include <cstdint>

namespace pd::stego {

/**
 * @brief Встроить сообщение в массив байтов изображения.
 * @param pixels  Изменяемый буфер байт пикселей.
 * @param msg     Сообщение в UTF-8.
 * @return true, если ёмкости хватило.
 */
bool embed(std::vector<uint8_t>& pixels, const std::string& msg);

/**
 * @brief Извлечь сообщение, ранее встроенное embed().
 * @param pixels  Буфер байтов изображения.
 * @return извлечённое сообщение или пустую строку при ошибке.
 */
std::string extract(const std::vector<uint8_t>& pixels);

/// Максимальная длина сообщения для буфера @p capacityBytes (в байтах).
size_t capacity(size_t pixelsBytes);

} // namespace pd::stego
