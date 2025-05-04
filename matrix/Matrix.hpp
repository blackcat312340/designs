#pragma once
#include <vector>
#include <iostream>
#include "omp.h"
#include "RealMatrixBase.hpp"
/*
Matrix类
提供默认填充构造，Matrix移动构造，二维vector的复制构造和移动构造(都使用移动语义优化）；
提供获得行数列数的接口；
提供矩阵乘法，加法，减法，等于重载；
提供矩阵行交换，矩阵余子式，矩阵切片，访问单个元素
使用omp实现函数内并行
*/
template <typename T,size_t Rows,size_t Cols>
class Matrix : public RealMatrixBase<T, Matrix<T,Rows,Cols>, Rows, Cols> {
    public:
        //继承构造函数
        using RealMatrixBase<T,Matrix,Rows,Cols>::RealMatrixBase;
        //获得列数
        using RealMatrixBase<T,Matrix,Rows,Cols>::get_cols;
        //获得行数
        using RealMatrixBase<T,Matrix,Rows,Cols>::get_rows;
        //获取余子式（移动优化）
        using RealMatrixBase<T,Matrix,Rows,Cols>::get_minor;
        //行交换
        using RealMatrixBase<T,Matrix,Rows,Cols>::swap;
        //矩阵切片(函数内并行优化)
        using RealMatrixBase<T,Matrix,Rows,Cols>::slice;
        //打印矩阵
        using RealMatrixBase<T,Matrix,Rows,Cols>::print;
        //访问元素
        using RealMatrixBase<T,Matrix,Rows,Cols>::operator();
        //得到矩阵转置(函数内并行优化)
        using RealMatrixBase<T,Matrix,Rows,Cols>::transpose;
        //矩阵加法（函数内并行优化）
        using RealMatrixBase<T,Matrix,Rows,Cols>::operator+;
        //矩阵减法（函数内并行优化）
        using RealMatrixBase<T,Matrix,Rows,Cols>::operator-;
        //矩阵乘法(函数内并行优化)
        using RealMatrixBase<T,Matrix,Rows,Cols>::operator*;
        //移动（复制）赋值
        using RealMatrixBase<T,Matrix,Rows,Cols>::operator=;
        using RealMatrixBase<T,Matrix,Rows,Cols>::det;
        using RealMatrixBase<T,Matrix,Rows,Cols>::inv;
        //隐式转换为其他类型的矩阵
        template <typename U>
        operator Matrix<U, Rows, Cols>() const {
            Matrix<U, Rows, Cols> result;
            for (size_t i = 0; i < Rows; ++i) {
                for (size_t j = 0; j < Cols; ++j) {
                    result(i, j) = static_cast<U>(this->data[i][j]);
                }
            }
            return result;
        }
};