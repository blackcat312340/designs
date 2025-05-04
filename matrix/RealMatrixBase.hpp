#pragma once
#include <vector>
#include <memory>
#include <iomanip>
#include <optional>
#include "MatrixBase.hpp"
#include "LU_Matrix.hpp"
template <typename T>
concept Arithmetic = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
    { a - b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
    { a / b } -> std::same_as<T>;
    { a += b } ->std::same_as<T&>;
    { a -= b } ->std::same_as<T&>;
    { a *= b } ->std::same_as<T&>;
    { a /= b } ->std::same_as<T&>;
};
template <Arithmetic T, typename Derived, size_t Rows, size_t Cols>
class RealMatrixBase :public MatrixBase<T,Derived,Rows,Cols> {
private:
    // 使用 std::optional 存储 LU_Matrix
    mutable std::optional<LU_Matrix<MatrixBase<T, Derived, Rows, Cols>>> lu;
    // 计算 LU 分解
    void compute_lu() const {
        if constexpr (is_square) {
            if (!lu.has_value()) {
                lu.emplace(static_cast<const Derived&>(*this));
            }
        } else {
            throw std::logic_error("LU decomposition is only available for square matrices.");
        }
    }
    static constexpr bool is_square = (Rows == Cols);
public:
    using MatrixBase<T,Derived,Rows,Cols>::MatrixBase;
    using MatrixBase<T,Derived,Rows,Cols>::get_cols;
    using MatrixBase<T,Derived,Rows,Cols>::get_rows;
    using MatrixBase<T,Derived,Rows,Cols>::swap;
    using MatrixBase<T,Derived,Rows,Cols>::slice;
    using MatrixBase<T,Derived,Rows,Cols>::print;
    using MatrixBase<T,Derived,Rows,Cols>::operator();
    using MatrixBase<T,Derived,Rows,Cols>::transpose;
    using MatrixBase<T,Derived,Rows,Cols>::operator=;
    //获取余子式
    auto get_minor(size_t row, size_t col) const {
        if (row >= Rows || col >= Cols) {
            throw std::out_of_range("Matrix indices out of range");
        }
        std::vector<std::vector<T>> sub_data;
        sub_data.reserve(Rows - 1);
        #pragma omp parallel for
        for (size_t i = 0; i < Rows; ++i) {
            if (i == row) continue;
            std::vector<T> new_row;
            new_row.reserve(Cols - 1);
            for (size_t j = 0; j < Cols; ++j) {
                if (j != col) new_row.push_back(this->data[i][j]);
            }
            sub_data.push_back(std::move(new_row));
        }
        return Derived(std::move(sub_data));
    }
    //矩阵加法（函数内并行优化）
    template <typename input, size_t OtherRows, size_t OtherCols>
    auto operator+(const MatrixBase<T,input,OtherRows,OtherCols>& rhs) const {
        if(Rows != OtherRows || Cols != OtherCols) throw std::invalid_argument("Matrix addition dimension mismatch");
        input result;
        #pragma omp parallel for
        for(size_t i = 0; i < Rows; ++i) {
            for(size_t j = 0; j < Cols; ++j)
                result(i,j) = this->data[i][j] + rhs(i,j);
        }
        return result;
    }
    //矩阵减法（函数内并行优化）
    template <typename input,size_t OtherRows, size_t OtherCols>
    auto operator-(const MatrixBase<T,input,OtherRows,OtherCols>& rhs) const {
        if(Rows != OtherRows || Cols != OtherCols) throw std::invalid_argument("Matrix subtraction dimension mismatch");
        input result;
        #pragma omp parallel for
        for(size_t i = 0; i < Rows; ++i) {
            for(size_t j = 0; j < Cols; ++j)
                result(i,j) = this->data[i][j] - rhs(i,j);
        }
        return result;
    }
    //矩阵乘法(函数内并行优化)
    template <typename input,size_t OtherRows, size_t OtherCols>
    auto operator*(const MatrixBase<T,input,OtherRows,OtherCols>& rhs) const {
        if(Cols != OtherRows) throw std::invalid_argument("Matrix multiplication dimension mismatch");
        input result;
        #pragma omp parallel for
        for (size_t i = 0; i < Rows; ++i) {
            for (size_t j = 0; j < OtherCols; ++j) {
                for (size_t k = 0; k < Cols; ++k) {
                    result(i, j) += this->data[i][k] * rhs(k, j);
                }
            }
        }
        return result;
    }
    //标量乘法（右乘）
    auto operator*(T scalar) const {
        Derived result(*static_cast<const Derived*>(this));
        #pragma omp parallel for
        for(size_t i = 0; i < Rows; ++i) {
            for(size_t j = 0; j < Cols; ++j)
                result(i,j) *= scalar;
        }
        return result;
    }
    //标量乘法（左乘）
    template <typename U, typename D, size_t R, size_t C>
    friend D operator*(U scalar, const RealMatrixBase<U, D, R, C>& mat);
    //解行列式
    T det() const noexcept {
        if constexpr (is_square) {
            compute_lu();
            return lu->determinant();
        } else {
            throw std::logic_error("Determinant is only available for square matrices.");
        }
    }
    //矩阵求逆(函数内并行)
    Derived inv() const {
        if constexpr (is_square) {
            compute_lu();
            return Derived(lu->inverse());
        } else {
            throw std::logic_error("inverse is only available for square matrices.");
        }
    }
    // 求解线性方程组
    std::vector<T> LU_solve(const std::vector<T>& b) const {
        if constexpr (is_square) {
            compute_lu();
            return lu->solve(b);
        } else {
            throw std::logic_error("LU_solve is only available for square matrices.");
        }
    }
};
//标量乘法（左乘）定义
template <typename U, typename D, size_t R, size_t C>
D operator*(U scalar, const RealMatrixBase<U, D, R, C>& mat) {
    return mat * scalar;
}