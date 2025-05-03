#pragma once
#include <memory>
#include "Matrix.hpp"
template <typename Matrix>
class LU_Matrix{
    public:
        LU_Matrix(Matrix&& mat) {
            this->size = mat.get_cols();
            lu_cached=false;
            // 初始化 L 和 U 矩阵
            L = Matrix(0.0);
            U = Matrix(mat);
        }
        // LU_Matrix(const LU_Matrix& mat) {
        //     this->size = size;
        //     lu_cached = false;
        //     // 初始化 L 和 U 矩阵
        //     L = MatrixBase<Derived<Size,Size>,Size,Size>(0.0);
        //     U = MatrixBase<Derived<Size,Size>,Size,Size>(mat);
        // };
        //获取L矩阵
        const Matrix& get_L() const {
            lu_decompose();
            return *L;
        }
        //获取U矩阵
        const Matrix& get_U() const {
            lu_decompose();
            return *U;
        }
        //获取行交换次数
        int get_swap_count() const {
            lu_decompose();
            return swap_count;
        }
    private:
        size_t size;
        mutable bool lu_cached; // 是否已缓存LU分解
        mutable Matrix L; // L矩阵
        mutable Matrix U; // U矩阵
        mutable int swap_count = 0; // 行交换次数
        // LU 分解
        void lu_decompose() const {
            if (lu_cached) return; // 如果已缓存，直接返回
            int swap_count = 0;
            for (size_t i = 0; i < size; ++i) {
                // 部分选主元
                size_t pivot_row = i;
                for (size_t j = i + 1; j < size; ++j) {
                    if (std::abs(U(j, i)) > std::abs(U(pivot_row, i))) {
                        pivot_row = j;
                    }
                }
                // 如果主元为零，矩阵是奇异的
                if (U(pivot_row, i) == 0.0) throw std::runtime_error("Matrix is singular");
                // 行交换
                if (pivot_row != i) {
                    U.swap(i,pivot_row);
                    ++swap_count;
                }
                // 设置 L 的对角线元素为 1
                L(i, i) = 1.0;
                // 消元
                for (size_t j = i + 1; j < size; ++j) {
                    double factor = U(j, i) / U(i, i);
                    L(j, i) = factor;
                    for (size_t k = i; k < size; ++k) {
                        U(j, k) -= factor * U(i, k);
                    }
                }
            }
            swap_count = swap_count;
            lu_cached=true;
        }
};