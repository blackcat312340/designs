#pragma once
#include <vector>
#include <memory>
#include <iomanip>
#include <optional>
template <typename T,class Derived, size_t Rows, size_t Cols>
class MatrixBase {
protected:
    std::vector<std::vector<T>> data;
public:
    // 显式定义拷贝构造函数
    MatrixBase(const MatrixBase& other) noexcept
    : data(other.data){}
    //基类构造函数
    MatrixBase(T init = T()) noexcept
    : data(Rows, std::vector<T>(Cols,init)) {}
    //移动构造
    MatrixBase(MatrixBase&& other) noexcept
    : data(std::move(other.data)) {}
    //从二维vector移动构造
    MatrixBase(std::vector<std::vector<T>>&& input) noexcept
    : data(std::move(input)) {}
    //用二维vector复制构造
    MatrixBase(const std::vector<std::vector<T>>& input) noexcept
    : data(input) {}
    //获取行数
    size_t get_rows() const noexcept { return Rows; }
    //获取列数
    size_t get_cols() const noexcept { return Cols; }
    //行交换
    auto swap(size_t i,size_t j) const {
        if(i >= Rows || j >= Rows) throw std::logic_error("Matrix indices out of range");
        Derived result(*static_cast<const Derived*>(this));
        std::swap(result.data[i], result.data[j]);
        return result;
    }
    // 矩阵切片(函数内并行优化)
    template <size_t row_start, size_t row_end, size_t col_start, size_t col_end>
    auto slice() const {
        if (row_start >= Rows || row_end > Rows || col_start >= Cols || col_end > Cols)
            throw std::out_of_range("Matrix slice indices out of range");
        // 计算子矩阵的行列数（编译期常量）
        constexpr size_t NewRows = row_end - row_start;
        constexpr size_t NewCols = col_end - col_start;
        MatrixBase<T, NewRows, NewCols> result;
        #pragma omp parallel for
        for (size_t i = row_start; i < row_end; ++i) {
            for (size_t j = col_start; j < col_end; ++j) {
                result(i - row_start, j - col_start) = this->data[i][j];
            }
        }
        return Derived(static_cast<);
    }
    void print(const std::string& title = "") const {
        std::ostringstream buffer;
        if(!title.empty()) buffer << title << "\n";
        for(const auto& row : data) {
            for(auto val : row)
                buffer << std::setw(10) << std::fixed << std::setprecision(4) << val;
            buffer << "\n";
        }
        std::cout << buffer.str();
    }
    //访问元素（可修改值）
    T& operator()(size_t i, size_t j) {
        if(i >= Rows || j >= Cols) 
            throw std::out_of_range("Matrix indices out of range");
        return data[i][j];
    }
    //访问元素（只读）
    const T& operator()(size_t i, size_t j) const {
        if(i >= Rows || j >= Cols)
            throw std::out_of_range("Matrix indices out of range");
        return data[i][j];
    }
    //得到矩阵转置(函数内并行优化)
    auto transpose() const noexcept{
        MatrixBase<T, Derived, Cols, Rows> result;
        for(size_t i = 0; i < Rows; ++i) {
            for(size_t j = 0; j < Cols; ++j) {
                result(j,i)= this->data[i][j];
            }
        }
        return result;
    }
    //复制赋值
    Derived& operator=(const MatrixBase& other) noexcept {
        if (this != &other) {
            data = other.data;
        }
        return *static_cast<Derived*>(this);
    }
    //移动赋值
    Derived& operator=(MatrixBase&& other) noexcept {
        if(this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }
};