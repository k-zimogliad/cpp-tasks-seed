#include <gtest/gtest.h>
#include <random>
#include <stdexcept>
#include <Eigen/Dense>
#include "Gauss_solve.h"

// Базовая проверка 
TEST(GaussSolve, SmallDeterministicSolve)
{
    GaussMatrix ab(3, 4);
    ab << 2, 1, -1, 8,
        -3, -1, 2, -11,
        -2, 1, 2, -3;

    GaussVector expected(3);
    expected << 2, 3, -1;

    GaussVector result = Gauss_solve(ab);

    EXPECT_NEAR((result - expected).norm(), 0.0, 1e-9);
}

// Проверка выбора главного элемента
TEST(GaussSolve, RequiresPivoting)
{
    GaussMatrix ab(2, 3);
    ab << 0, 2, 4,
        3, 2, 7;

    GaussVector expected(2);
    expected << 1, 2;

    GaussVector result = Gauss_solve(ab);

    EXPECT_NEAR((result - expected).norm(), 0.0, 1e-9);
}

// Вырожденная матрица
TEST(GaussSolve, SingularMatrixThrowsException)
{
    GaussMatrix ab(2, 3);
    ab << 1, 2, 3,
        2, 4, 6; 

    EXPECT_THROW(Gauss_solve(ab), std::runtime_error);
}

// Eдиничная матрица коэффициентов
TEST(GaussSolve, IdentityMatrix)
{
    GaussMatrix ab(4, 5);
    ab << 1, 0, 0, 0, 5.5,
        0, 1, 0, 0, -2.1,
        0, 0, 1, 0, 0.0,
        0, 0, 0, 1, 9.9;

    GaussVector expected(4);
    expected << 5.5, -2.1, 0.0, 9.9;

    GaussVector result = Gauss_solve(ab);

    EXPECT_NEAR((result - expected).norm(), 0.0, 1e-9);
}

// Проверка генератора псевдослучайных чисел
TEST(GaussSolve, RandomGeneratorReproducibility)
{
    const int N = 20;
    const unsigned int seed = 12345;

    // Сборка первой системы
    std::mt19937 gen1(seed);
    std::uniform_real_distribution<double> dis(-5.0, 5.0);
    GaussMatrix ab1(N, N + 1);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= N; ++j) ab1(i, j) = dis(gen1);
    }

    // Сборка второй системы
    std::mt19937 gen2(seed);
    GaussMatrix ab2(N, N + 1);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= N; ++j) ab2(i, j) = dis(gen2);
    }

    ASSERT_TRUE(ab1.isApprox(ab2, 1e-15));

    GaussVector res1 = Gauss_solve(ab1);
    GaussVector res2 = Gauss_solve(ab2);

    EXPECT_EQ(res1, res2); // Результаты должны совпадать 
}

// Стресс-тест на большой случайной матрице
TEST(GaussSolve, LargeRandomSolve)
{
    const int N = 150;

    std::mt19937 gen(777);
    std::uniform_real_distribution<double> dis(-50.0, 50.0);

    GaussMatrix ab(N, N + 1);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= N; ++j) {
            ab(i, j) = dis(gen);
        }
    }

    GaussMatrix ab_copy = ab;
    GaussVector our_result = Gauss_solve(ab);

    // Эталонное решение 
    Eigen::MatrixXd A = ab_copy.leftCols(N);
    Eigen::VectorXd b = ab_copy.col(N);
    Eigen::VectorXd reference_result = A.partialPivLu().solve(b);

    double error = (our_result - reference_result).norm();
    EXPECT_NEAR(error, 0.0, 1e-5);
}