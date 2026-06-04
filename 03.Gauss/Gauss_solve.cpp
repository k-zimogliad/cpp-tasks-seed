#include "Gauss_solve.h"
#include <cmath>
#include <stdexcept>

GaussVector Gauss_solve(GaussMatrix& ab)
{
    int n = ab.rows();

    // Прямой ход метода Гаусса
    for (int i = 0; i < n; ++i)
    {
        // Выбор главного элемента по столбцу
        int max_row = i;
        for (int k = i + 1; k < n; ++k)
        {
            if (std::abs(ab(k, i)) > std::abs(ab(max_row, i)))
            {
                max_row = k;
            }
        }

        // Если матрица вырожденная
        if (std::abs(ab(max_row, i)) < 1e-12)
        {
            throw std::runtime_error("Система не имеет единственного решения.");
        }

        // Меняем строки местами, если максимальный элемент не на диагонали
        if (max_row != i)
        {
            ab.row(i).swap(ab.row(max_row));
        }

        // Исключение переменных ниже текущей строки
        for (int k = i + 1; k < n; ++k)
        {
            double factor = ab(k, i) / ab(i, i);
            ab.row(k) -= factor * ab.row(i);
        }
    }

    // Обратный ход метода Гаусса
    GaussVector x = GaussVector::Zero(n);
    for (int i = n - 1; i >= 0; --i)
    {
        double rhs = ab(i, n);

        if (i < n - 1)
        {
            int count = n - 1 - i;
            rhs -= ab.row(i).segment(i + 1, count).dot(x.segment(i + 1, count));
        }

        x(i) = rhs / ab(i, i);
    }

    return x;
}