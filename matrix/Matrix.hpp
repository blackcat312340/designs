#pragma once
#include <functional>
#include <future>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <iomanip>
#include <optional>
#include "threadpool.hpp"
class Matrix {
    private:
        std::vector<std::vector<double>> data;
        size_t rows;
        size_t cols;
        static constexpr size_t PARALLEL_THRESHOLD = 64; //并行阈值
        // 缓存 LU 分解结果
        mutable std::unique_ptr<Matrix> cached_L; // 缓存的L矩阵
        mutable std::unique_ptr<Matrix> cached_U; // 缓存的U矩阵
        mutable int cached_swap_count = 0;      // 缓存的行交换次数
        mutable bool lu_cached = false;        // 是否已缓存LU分解
        // 清空缓存
        void clear_cache() {
            cached_L.reset();
            cached_U.reset();
            cached_swap_count = 0;
            lu_cached = false;
        }
        Matrix(const std::vector<std::vector<double>>& input) noexcept
            : data(std::move(input)),
              rows(data.size()),
              cols(data.empty() ? 0 : data[0].size()) {}
    public:
        //构造器
        Matrix(size_t rows, size_t cols, double init = 0.0)
            : rows(rows), cols(cols), data(rows, std::vector<double>(cols, init)) {}
        //移动构造
        Matrix(Matrix&& other) noexcept
            : data(std::move(other.data)),
              rows(other.rows),
              cols(other.cols) {}
        //从二维vector构造
        Matrix(std::vector<std::vector<double>>&& input) noexcept
            : data(std::move(input)),
              rows(data.size()),
              cols(data.empty() ? 0 : data[0].size()) {}
        //访问元素（可修改值）
        double& operator()(size_t i, size_t j) {
            if(i >= rows || j >= cols) 
                throw std::out_of_range("Matrix indices out of range");
            return data[i][j];
        }
        //访问元素（只读）
        const double& operator()(size_t i, size_t j) const {
            if(i >= rows || j >= cols)
                throw std::out_of_range("Matrix indices out of range");
            return data[i][j];
        }
        // LU 分解
        void lu_decompose() const {
            if (lu_cached) return; // 如果已缓存，直接返回
            if (rows != cols) throw std::logic_error("LU decomposition requires square matrix");
            Matrix U(this->data); // 复制当前矩阵,初始化U矩阵
            Matrix L(rows, cols, 0.0); // 初始化 L 矩阵为零矩阵
            int swap_count = 0;
            for (size_t i = 0; i < rows; ++i) {
                // 部分选主元
                size_t pivot_row = i;
                for (size_t j = i + 1; j < rows; ++j) {
                    if (std::abs(U(j, i)) > std::abs(U(pivot_row, i))) {
                        pivot_row = j;
                    }
                }
                // 如果主元为零，矩阵是奇异的
                if (U(pivot_row, i) == 0.0) throw std::runtime_error("Matrix is singular");
                // 行交换
                if (pivot_row != i) {
                    std::swap(U.data[i], U.data[pivot_row]);
                    ++swap_count;
                }
                // 设置 L 的对角线元素为 1
                L(i, i) = 1.0;
                // 消元
                for (size_t j = i + 1; j < rows; ++j) {
                    double factor = U(j, i) / U(i, i);
                    L(j, i) = factor;
                    for (size_t k = i; k < cols; ++k) {
                        U(j, k) -= factor * U(i, k);
                    }
                }
            }
            // 缓存 LU 分解结果
            cached_L = std::make_unique<Matrix>(std::move(L));
            cached_U = std::make_unique<Matrix>(std::move(U));
            cached_swap_count = swap_count;
            lu_cached = true;
        }
        //获取L矩阵
        const Matrix& get_L() const {
            lu_decompose();
            return *cached_L;
        }
        //获取U矩阵
        const Matrix& get_U() const {
            lu_decompose();
            return *cached_U;
        }
        //获取行交换次数
        int get_swap_count() const {
            lu_decompose();
            return cached_swap_count;
        }
        //矩阵切片(函数内并行优化)
        Matrix slice(size_t row_start, size_t row_end, size_t col_start, size_t col_end) const {
            if(row_start >= rows || row_end > rows || col_start >= cols || col_end > cols)
                throw std::out_of_range("Matrix slice indices out of range");
            Matrix result(row_end - row_start, col_end - col_start);
            std::vector<std::future<void>> futures;
            futures.reserve(row_end - row_start);
            for(size_t i = row_start; i < row_end; ++i) {
                futures.emplace_back(pool.enqueue([this, &result, i, row_start, col_start] {
                    for(size_t j = col_start; j < result.cols; ++j) {
                        result(i - row_start, j - col_start) = this->data[i][j];
                    }
                }));
            }
            for(auto& f : futures) f.get();
            return result;
        }
        //矩阵转置(函数内并行优化)
        Matrix transpose() const {
            Matrix result(cols, rows);
            std::vector<std::future<void>> futures;
            futures.reserve(rows);
            for(size_t i = 0; i < rows; ++i) {
                futures.emplace_back(pool.enqueue([this, &result, i] {
                    for(size_t j = 0; j < cols; ++j) {
                        result(j, i) = this->data[i][j];
                    }
                }));
            }
            for(auto& f : futures) f.get();
            return result;
        }
        //矩阵加法（函数内并行优化）
        Matrix operator+(const Matrix& rhs) const {
            if(rows != rhs.rows || cols != rhs.cols) throw std::invalid_argument("Matrix addition dimension mismatch");
            Matrix result(rows, cols);
            std::vector<std::future<void>> futures;
            futures.reserve(rows);
            for(size_t i = 0; i < rows; ++i) {
                futures.emplace_back(pool.enqueue([this, &rhs, &result, i] {
                    for(size_t j = 0; j < cols; ++j)
                        result(i, j) = this->data[i][j] + rhs.data[i][j];
                }));
            }
            for(auto& f : futures) f.get();
            return result;
        }
        //矩阵减法（函数内并行优化）
        Matrix operator-(const Matrix& rhs) const {
            if(rows != rhs.rows || cols != rhs.cols) throw std::invalid_argument("Matrix subtraction dimension mismatch");
            Matrix result(rows, cols);
            std::vector<std::future<void>> futures;
            futures.reserve(rows);
            for(size_t i = 0; i < rows; ++i) {
                futures.emplace_back(pool.enqueue([this, &rhs, &result, i] {
                    for(size_t j = 0; j < cols; ++j)
                        result(i, j) = this->data[i][j] - rhs.data[i][j];
                }));
            }
            for(auto& f : futures) f.get();
            return result;
        }
        //解行列式
        double det() const {
            lu_decompose();
            double determinant = 1.0;
            for (size_t i = 0; i < rows; ++i) {
                determinant *= cached_U->operator()(i, i);
            }
            if (cached_swap_count % 2 != 0) {
                determinant = -determinant;
            }
            return determinant;
        }
        //解方程Ax=b，返回x
        std::vector<double> solve(const Matrix& A, const std::vector<double>& b) {
            // 前向替代：解 L * y = b
            lu_decompose();
            std::vector<double> y(A.rows, 0.0);
            for (size_t i = 0; i < A.rows; ++i) {
                y[i] = b[i];
                for (size_t j = 0; j < i; ++j) {
                    y[i] -= cached_L->operator()(i, j) * y[j];
                }
            }
            // 后向替代：解 U * x = y
            std::vector<double> x(A.rows, 0.0);
            for (int i = A.rows - 1; i >= 0; --i) {
                x[i] = y[i];
                for (size_t j = i + 1; j < A.cols; ++j) {
                    x[i] -= cached_U->operator()(i, j) * x[j];
                }
                x[i] /= cached_U->operator()(i, i);
            }
            return x;
        }
        // 矩阵求逆(函数内并行)
        Matrix inv() const {
            lu_decompose();
            Matrix inverse(rows, cols);
            // 单位矩阵
            Matrix identity(rows, cols, 0.0);
            for (size_t i = 0; i < rows; ++i) {
                identity(i, i) = 1.0;
            }
            std::vector<std::future<void>> futures;
            futures.reserve(cols);
            // 逐列求解逆矩阵
            for (size_t i = 0; i < cols; ++i) {
                futures.emplace_back(pool.enqueue([this,&identity,i,&inverse]{
                    // 前向替代：解 L * y = b
                    std::vector<double> y(rows, 0.0);
                    for (size_t j = 0; j < rows; ++j) {
                        y[j] = identity(j, i);
                        for (size_t k = 0; k < j; ++k) {
                            y[j] -= cached_L->operator()(j, k) * y[k];
                        }
                    }
                    // 后向替代：解 U * x = y
                    std::vector<double> x(rows, 0.0);
                    for (int j = rows - 1; j >= 0; --j) {
                        x[j] = y[j];
                        for (size_t k = j + 1; k < cols; ++k) {
                            x[j] -= cached_U->operator()(j, k) * x[k];
                        }
                        x[j] /= cached_U->operator()(j, j);
                    }
                    // 将结果写入逆矩阵
                    for (size_t j = 0; j < rows; ++j) {
                        inverse(j, i) = x[j];
                    }
                }));
            }
            for (auto& f : futures) {
                f.get(); // 等待所有线程完成
            }
            return inverse;
        }
        //矩阵乘法(函数内并行优化)
        Matrix operator*(const Matrix& rhs) const {
            if (cols != rhs.rows) {
                throw std::invalid_argument("Matrix multiplication dimension mismatch");
            }
            Matrix result(rows, rhs.cols); // 初始化结果矩阵
            std::vector<std::future<void>> futures;
            futures.reserve(rows);
            for (size_t i = 0; i < rows; ++i) {
                futures.emplace_back(pool.enqueue([this, &rhs, &result, i] {
                    for (size_t j = 0; j < rhs.cols; ++j) {
                        for (size_t k = 0; k < cols; ++k) {
                            result(i, j) += this->data[i][k] * rhs.data[k][j];
                        }
                    }
                }));
            }
            for (auto& f : futures) {
                f.get(); // 等待所有线程完成
            }
            return result;
        }
        //标量乘法（右乘）
        Matrix operator*(double scalar) const {
            Matrix result(this->data);
            std::vector<std::future<void>> futures;
            futures.reserve(rows);
            for(size_t i = 0; i < rows; ++i) {
                futures.emplace_back(pool.enqueue([&result, scalar, i, this] {
                    for(size_t j = 0; j < cols; ++j)
                        result(i, j) *= scalar;
                }));
            }
            for(auto& f : futures) f.get();
            return result;
        }
        //标量乘法（左乘）
        friend Matrix operator*(double scalar, const Matrix& mat) {
            return mat * scalar;
        }
        //移动赋值
        Matrix& operator=(Matrix&& other) noexcept {
            if(this != &other) {
                data = std::move(other.data);
                rows = other.rows;
                cols = other.cols;
            }
            return *this;
        }
        //打印矩阵
        void print(const std::string& title = "") const {
            if(!title.empty()) std::cout << title << "\n";
            for(const auto& row : data) {
                for(double val : row)
                    std::cout << std::setw(10) << std::fixed << std::setprecision(4) << val;
                std::cout << "\n";
            }
        }
    private:
        //串行列式计算
        double serial_det() const {
            double result = 0.0;
            for(size_t i = 0; i < rows; ++i) {
                auto sub = minor(0, i);
                result += ((i & 1) ? -1.0 : 1.0) * data[0][i] * sub.serial_det();
            }
            return result;
        }
        //获取余子式（移动优化）
        Matrix minor(size_t row, size_t col) const {
            std::vector<std::vector<double>> sub_data;
            sub_data.reserve(rows-1);
            for(size_t i = 0; i < rows; ++i) {
                if(i == row) continue;
                std::vector<double> new_row;
                new_row.reserve(cols-1);
                for(size_t j = 0; j < cols; ++j) {
                    if(j != col) new_row.push_back(data[i][j]);
                }
                sub_data.push_back(std::move(new_row));
            }
            return Matrix(std::move(sub_data));
        }
    };
    