#include <iostream>
#include <string>
#include <Eigen/Dense>
#include "Gauss_solve.h"

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Ошибка: Не указан путь к файлу данных СЛАУ.\n";
        std::cerr << "Использование: " << argv[0] << " <input_matrix.csv>\n";
        return 1;
    }

    try
    {
        // Чтение расширенной матрицы из CSV
        GaussMatrix ab = load_csv_to_matrix(argv[1]);

        // Решение системы методом Гаусса
        GaussVector x = Gauss_solve(ab);

        // Преобразуем вектор в матрицу Nx1
        GaussMatrix x_matrix(x.size(), 1);
        x_matrix.col(0) = x;

        // Вывод результата
        print_matrix_as_csv(std::cout, x_matrix);

    }
    catch (const std::exception& e)
    {
        std::cerr << "Произошла ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}