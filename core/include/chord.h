#pragma once
/**
 * @file chord.h
 * @brief Метод хорд (regula falsi) для нахождения корня нелинейного уравнения.
 */
#include <functional>
#include <stdexcept>

namespace pd::math {

/**
 * @struct ChordResult
 * @brief Результат работы метода хорд.
 */
struct ChordResult {
    double root;        ///< Найденный корень
    int    iterations;  ///< Количество итераций
    double residual;    ///< |f(root)| в финале
    bool   converged;   ///< Достигнута ли точность
};

/**
 * @brief Метод хорд для f на отрезке [a, b].
 * @param f       Непрерывная функция.
 * @param a,b     Концы отрезка, причём f(a)*f(b) < 0.
 * @param eps     Требуемая точность по f.
 * @param maxIter Лимит итераций.
 * @throws std::invalid_argument если f(a)*f(b) >= 0.
 */
ChordResult chord(const std::function<double(double)>& f,
                  double a, double b,
                  double eps = 1e-9, int maxIter = 1000);

/**
 * @brief Простейший интерпретатор полинома вида "a_n*x^n + ... + a_0".
 * Поддерживает: целые/дробные коэффициенты, x, x^k, +, -, *, /, скобки, sin/cos/exp/log.
 * Реализован рекурсивный спуск.
 */
double evalExpr(const std::string& expr, double x);

} // namespace pd::math
