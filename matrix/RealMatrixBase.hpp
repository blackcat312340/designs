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
template <Arithmetic T, typename Derived>
class RealMatrixBase :public MatrixBase<T,Derived> {
private:
    // 使用 std::optional 存储 LU_Matrix
    mutable std::optional<LU_Matrix<MatrixBase<T, Derived>>> lu;
    // 计算 LU 分解
    void compute_lu() const {
        if (!lu.has_value()) {
            lu.emplace(static_cast<const Derived&>(*this));
        }
    }
public:
    using value_type = MatrixBase<T,Derived>::value_type;
    using MatrixBase<T,Derived>::MatrixBase;
    using MatrixBase<T,Derived>::get_cols;
    using MatrixBase<T,Derived>::get_rows;
    using MatrixBase<T,Derived>::swap;
    using MatrixBase<T,Derived>::slice;
    using MatrixBase<T,Derived>::print;
    using MatrixBase<T,Derived>::operator();
    using MatrixBase<T,Derived>::transpose;
    using MatrixBase<T,Derived>::operator=;
    //获取余子式
    Derived get_minor(size_t row, size_t col) const {
        if (row >= this->Rows || col >= this->Cols) {
            throw std::out_of_range("Matrix indices out of range");
        }
        std::vector<std::vector<T>> sub_data;
        sub_data.reserve(this->Rows - 1);
        #pragma omp parallel for
        for (size_t i = 0; i < this->Rows; ++i) {
            if (i == row) continue;
            std::vector<T> new_row;
            new_row.reserve(this->Cols - 1);
            for (size_t j = 0; j < this->Cols; ++j) {
                if (j != col) new_row.push_back(this->data[i][j]);
            }
            sub_data.push_back(std::move(new_row));
        }
        return Derived(sub_data);
    }
    //矩阵加法（函数内并行优化）
    template <typename input>
    Derived operator+(const MatrixBase<T,input>& rhs) const {
        if(this->Rows != rhs.get_rows() || this->Cols != rhs.get_cols()) throw std::invalid_argument("Matrix addition dimension mismatch");
        std::vector<std::vector<T>> result(this->Rows, std::vector<T>(this->Cols, T()));
        #pragma omp parallel for
        for(size_t i = 0; i < this->Rows; ++i) {
            for(size_t j = 0; j < this->Cols; ++j)
                result[i][j] = this->data[i][j] + rhs(i,j);
        }
        return Derived(result);
    }
    //矩阵减法（函数内并行优化）
    template <typename input>
    Derived operator-(const MatrixBase<T,input>& rhs) const {
        if(this->Rows != rhs.get_rows() || this->Cols != rhs.get_cols()) throw std::invalid_argument("Matrix addition dimension mismatch");
        std::vector<std::vector<T>> result(this->Rows, std::vector<T>(this->Cols, T()));
        #pragma omp parallel for
        for(size_t i = 0; i < this->Rows; ++i) {
            for(size_t j = 0; j < this->Cols; ++j)
            result[i][j] = this->data[i][j] + rhs(i,j);
        }
        return Derived(result);
    }
    //矩阵乘法(函数内并行优化)
    template <typename input>
    input operator*(const MatrixBase<T,input>& rhs) const {
        if(this->Cols != rhs.get_rows()) throw std::invalid_argument("Matrix multiplication dimension mismatch");
        std::vector<std::vector<T>> result(this->Rows, std::vector<T>(rhs.get_cols(), T()));
        #pragma omp parallel for
        for (size_t i = 0; i < this->Rows; ++i) {
            for (size_t j = 0; j < rhs.get_cols(); ++j) {
                for (size_t k = 0; k < this->Cols; ++k) {
                    result[i][j] += this->data[i][k] * rhs(k, j);
                }
            }
        }
        return input(result);
    }
    //标量乘法（右乘）
    Derived operator*(T scalar) const {
        std::vector<std::vector<T>> result(this->data);
        #pragma omp parallel for
        for(size_t i = 0; i < this->Rows; ++i) {
            for(size_t j = 0; j < this->Cols; ++j)
                result[i][j] *= scalar;
        }
        return Derived(result);
    }
    //标量乘法（左乘）
    template <typename U, typename D>
    friend D operator*(U scalar, const RealMatrixBase<U, D>& mat);
    //异或操作符重载
    Derived operator^(const Derived& rhs) const {
        if(this->Rows != rhs.get_rows() || this->Cols != rhs.get_cols()) throw std::invalid_argument("Matrix addition dimension mismatch");
        std::vector<std::vector<T>> result(this->Rows, std::vector<T>(this->Cols, T()));
        #pragma omp parallel for
        for(size_t i = 0; i < this->Rows; ++i) {
            for(size_t j = 0; j < this->Cols; ++j)
                result[i][j] = this->data[i][j] ^ rhs(i,j);
        }
        return Derived(result);
    }
    //单位矩阵
    Derived identity(size_t size) const {
        std::vector<std::vector<T>> result(size, std::vector<T>(size, T()));
        #pragma omp parallel for
        for (size_t i = 0; i < size; ++i) {
            result[i][i] = T(1);
        }
        return Derived(result);
    }
    //解行列式
    T det() const {
        if (this->Rows == this->Cols) {
            compute_lu();
            return lu->determinant();
        } else {
            throw std::logic_error("Determinant is only available for square matrices.");
        }
    }
    //矩阵求逆(函数内并行)
    Derived inv() const {
        if (this->Rows == this->Cols) {
            compute_lu();
            return Derived(lu->inverse());
        } else {
            throw std::logic_error("inverse is only available for square matrices.");
        }
    }
    // 求解线性方程组
    std::vector<T> LU_solve(const std::vector<T>& b) const {
        if (this->Rows == this->Cols) {
            compute_lu();
            return lu->solve(b);
        } else {
            throw std::logic_error("LU_solve is only available for square matrices.");
        }
    }
};
//标量乘法（左乘）定义
template <typename U, typename D>
D operator*(U scalar, const RealMatrixBase<U, D>& mat) {
    return mat * scalar;
}