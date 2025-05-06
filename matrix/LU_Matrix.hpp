#pragma once
#include <memory>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "RealMatrixBase.hpp" // 使用基类存储 L 和 U
template <typename MatrixBase>
class LU_Matrix {
public:
    using value_type = typename MatrixBase::value_type;
    // 构造函数，接受一个矩阵并进行 LU 分解
    LU_Matrix(const MatrixBase& mat) : size(mat.get_cols()), lu_cached(false),L(mat.get_rows(),mat.get_cols()), U(mat)  {
        // 初始化行置换数组
        for (size_t i = 0; i < size; ++i) {
            row_permutation.reserve(size);
            row_permutation[i] = i;
        }
    }
    // 计算行列式
    value_type determinant() const {
        try {
            lu_decompose();
        } catch (const std::runtime_error& e) {
            return 0; // 如果矩阵是奇异的，行列式为 0
        }
        value_type det = 1;
        for (size_t i = 0; i < size; ++i) {
            det *= U(i, i);
        }
        if (swap_count % 2 != 0) {
            det = -det;
        }
        return det;
    }
    // 求解线性方程组 Ax = b
    std::vector<value_type> solve(const std::vector<value_type>& b) const {
        lu_decompose();
        if (b.size() != size) {
            throw std::invalid_argument("Size of vector b must match the size of the matrix.");
        }
        // 前向替代求解 Ly = b
        std::vector<value_type> y(size, 0);
        for (size_t i = 0; i < size; ++i) {
            y[i] = b[i];
            for (size_t j = 0; j < i; ++j) {
                y[i] -= L(i, j) * y[j];
            }
        }
        // 后向替代求解 Ux = y
        std::vector<value_type> x(size, 0);
        for (int i = size - 1; i >= 0; --i) {
            x[i] = y[i];
            for (size_t j = i + 1; j < size; ++j) {
                x[i] -= U(i, j) * x[j];
            }
            x[i] /= U(i, i);
        }
        return x;
    }
    // 计算逆矩阵
    std::vector<std::vector<value_type>> inverse() const {
        lu_decompose();
        std::vector<std::vector<value_type>> inverse_matrix(size, std::vector<value_type>(size, 0));
        for (size_t i = 0; i < size; ++i) {
            std::vector<value_type> e(size, 0);
            e[i] = 1;
            // 前向替代求解 PLy = e
            std::vector<value_type> y(size, 0);
            for (size_t j = 0; j < size; ++j) {
                y[j] = e[row_permutation[j]]; // 使用行置换数组
                for (size_t k = 0; k < j; ++k) {
                    y[j] -= L(j, k) * y[k];
                }
            }
            // 后向替代求解 Ux = y
            std::vector<value_type> x(size, 0);
            for (int j = size - 1; j >= 0; --j) {
                if (U(j, j) == value_type()) {
                    throw std::runtime_error("Matrix is singular, cannot compute inverse");
                }
                x[j] = y[j];
                for (size_t k = j + 1; k < size; ++k) {
                    x[j] -= U(j, k) * x[k];
                }
                x[j] /= U(j, j);
            }
            // 将结果写入逆矩阵
            for (size_t j = 0; j < size; ++j) {
                inverse_matrix[j][i] = x[j];
            }
        }
        return inverse_matrix;
    }
private:
    size_t size;
    mutable bool lu_cached; // 是否已缓存 LU 分解
    mutable MatrixBase L;   // L 矩阵
    mutable MatrixBase U;   // U 矩阵
    mutable int swap_count = 0; // 行交换次数
    mutable std::vector<size_t> row_permutation; // 行置换数组
    // LU 分解
    void lu_decompose() const {
        if (lu_cached) return;
        for (size_t i = 0; i < size; ++i) {
            // 部分选主元
            size_t pivot_row = i;
            for (size_t j = i + 1; j < size; ++j) {
                if (std::abs(U(j, i)) > std::abs(U(pivot_row, i))) {
                    pivot_row = j;
                }
            }
            // 如果主元为零，矩阵是奇异的
            if (U(pivot_row, i) == value_type()) {
                throw std::runtime_error("Matrix is singular");
            }
            // 行交换
            if (pivot_row != i) {
                std::swap(row_permutation[i], row_permutation[pivot_row]);
                U=U.swap(i, pivot_row);
                L=L.swap(i, pivot_row); // 同时交换 L 矩阵的行
                ++swap_count;
            }
            // 设置 L 的对角线元素为 1
            L(i, i) = 1.0;
            // 消元
            for (size_t j = i + 1; j < size; ++j) {
                value_type factor = U(j, i) / U(i, i);
                L(j, i) = factor;
                for (size_t k = i; k < size; ++k) {
                    U(j, k) -= factor * U(i, k);
                }
            }
        }
        lu_cached = true;
    }
};