#pragma once
#include <vector>
#include <memory>
#include <iomanip>
#include <optional>
#include "MatrixBase.hpp"
template <typename T>
concept Arithmetic = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
    { a - b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
    { a / b } -> std::same_as<T>;
    { a += b } -> std::same_as<T&>;
    { a -= b } -> std::same_as<T&>;
    { a *= b } -> std::same_as<T&>;
    { a /= b } -> std::same_as<T&>;
};
template <Arithmetic T,typename Derived, size_t Rows, size_t Cols>
class RealMatrixBase :public MatrixBase<T,Derived,Rows,Cols> {
protected:
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
    template <class input, size_t OtherRows, size_t OtherCols>
    auto operator+(const RealMatrixBase<T,input,OtherRows,OtherCols>& rhs) const {
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
    template <class input, size_t OtherRows, size_t OtherCols>
    auto operator-(const RealMatrixBase<T,input,OtherRows,OtherCols>& rhs) const {
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
    template <class input, size_t OtherRows, size_t OtherCols>
    auto operator*(const RealMatrixBase<T,input,OtherRows,OtherCols>& rhs) const {
        if(Cols != OtherRows) throw std::invalid_argument("Matrix multiplication dimension mismatch");
        RealMatrixBase<T, input, Rows, OtherCols> result;
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
    auto operator*(double scalar) const {
        Derived result(Rows,Cols);
        #pragma omp parallel for
        for(size_t i = 0; i < Rows; ++i) {
            for(size_t j = 0; j < Cols; ++j)
                result(i,j) *= scalar;
        }
        return result;
    }
    //标量乘法（左乘）
    template <class input, size_t OtherRows, size_t OtherCols>
    friend T operator*(double scalar, const RealMatrixBase<T,input,OtherRows,OtherCols>& mat);
    //移动赋值
    template <class input, size_t OtherRows, size_t OtherCols>
    Derived& operator=(MatrixBase<T,input,OtherRows,OtherCols>&& other) noexcept {
        if(this != &other) {
            this->data = std::move(other.data);
        }
        return *this;
    }
    //复制赋值
    template <class input, size_t OtherRows, size_t OtherCols>
    Derived& operator=(const MatrixBase<T,input,OtherRows,OtherCols>& other) noexcept{
        if(this != &other) {
            this->data = other.data;
        }
        return *this;
    }
    // //解行列式
    // template <typename U=Derived>
    // typename std::enable_if<is_square, double>::type det() const noexcept{
    //     double determinant = 1.0;
    //     for (size_t i = 0; i < cols; ++i) {
    //         determinant *= lu.value().get_U().operator()(i, i);
    //     }
    //     if (lu.value().get_swap_count() % 2 != 0) {
    //         determinant = -determinant;
    //     }
    //     return determinant;
    // }
    // // 矩阵求逆(函数内并行)
    // template <typename U=Derived>
    // typename std::enable_if<is_square, Derived>::type inv() const {
    //     std::is_same<Derived,Square_Matrix>::value?Derived inverse(size):Derived inverse(size,size);
    //     // 空矩阵
    //     std::is_same<Derived,Square_Matrix>::value?Derived identity(size):Derived identity(size,size);
    //     for (size_t i = 0; i < size; ++i) {
    //         identity(i, i) = 1.0;
    //     }
    //     #pragma omp parallel for
    //     // 逐列求解逆矩阵
    //     for (size_t i = 0; i < size; ++i) {
    //             // 前向替代：解 L * y = b
    //             std::vector<double> y(size, 0.0);
    //             for (size_t j = 0; j < size; ++j) {
    //                 y[j] = identity(j, i);
    //                 for (size_t k = 0; k < j; ++k) {
    //                     y[j] -= lu.value().get_L().operator()(j, k) * y[k];
    //                 }
    //             }
    //             // 后向替代：解 U * x = y
    //             std::vector<double> x(size, 0.0);
    //             for (int j = size - 1; j >= 0; --j) {
    //                 x[j] = y[j];
    //                 for (size_t k = j + 1; k < size; ++k) {
    //                     x[j] -= lu.value().get_U().operator()(j, k) * x[k];
    //                 }
    //                 x[j] /= lu.value().get_U().operator()(j, j);
    //             }
    //             // 将结果写入逆矩阵
    //             for (size_t j = 0; j < size; ++j) {
    //                 inverse(j, i) = x[j];
    //             }
    //     }
    //     return inverse;
    // }
};
//标量乘法（左乘）定义
template <typename T, class input, size_t Rows, size_t Cols>
auto operator*(double scalar, const RealMatrixBase<T,input,Rows,Cols>& mat) {
    return mat * scalar;
}