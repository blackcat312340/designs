#include <iostream>
#include <string>
#include <optional>
#include <random>
#include "D:\github\designs\matrix\Matrix.hpp"
template <typename T>
class LinearRegression {
private:
    Matrix<T> weights; // 存储权重向量，包括偏置项
    T learning_rate;   // 学习率
    size_t max_iters;  // 最大迭代次数
    bool use_gradient_descent; // 是否使用梯度下降
public:
    // 构造函数，支持初始化学习率和迭代次数
    LinearRegression(T lr = 0.01, size_t iters = 1000, bool gradient_descent = true)
        : learning_rate(lr), max_iters(iters), use_gradient_descent(gradient_descent) {}
    // 随机初始化权重
    void initialize_weights(size_t num_features) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> dist(-0.001, 0.001);
        weights = Matrix<T>(1, num_features + 1); // 包括偏置项
        for (size_t i = 0; i < weights.get_cols(); ++i) {
            weights(0, i) = dist(gen);
        }
    }
    // 训练模型
    void fit(const Matrix<T>& X, const Matrix<T>& y) {
        size_t num_samples = X.get_rows();
        size_t num_features = X.get_cols();
        // 对输入数据进行归一化
        Matrix<T> X_normalized = normalize(X);
        // 初始化权重
        initialize_weights(num_features);
        // 添加偏置项到输入矩阵
        Matrix<T> X_bias = add_bias(X);
        weights.print("Initial weights: ");
        if (use_gradient_descent) {
            // 使用梯度下降优化
            for (size_t iter = 0; iter < max_iters; ++iter) {
                // 计算预测值
                Matrix<T> y_pred = predict(X);
                y_pred.print("y_pred");
                // 计算损失
                T loss = compute_loss(y, y_pred);
                // 打印损失（可选）
                //if (iter % 100 == 0) {
                    std::cout << "Iteration " << iter << ", Loss: " << loss << std::endl;
                //}
                // 计算梯度
                Matrix<T> gradient = (X_bias.transpose() * (y_pred - y)) * (1.0 / double(num_samples));
                gradient.print("梯度");
                // 更新权重
                weights = weights - learning_rate * gradient.transpose();
                weights.print("权重");
            }
        } else {
            // 使用正规方程法
            auto X_transpose = X_bias.transpose();
            auto XtX = X_transpose * X_bias;
            auto XtX_inv = XtX.inv();
            auto XtX_inv_Xt = XtX_inv * X_transpose;
            weights = XtX_inv_Xt * y;
            weights = weights.transpose(); // 转置权重矩阵
        }
    }
    // 使用模型进行预测
    Matrix<T> predict(const Matrix<T>& X) const {
        Matrix<T> X_normalized = X;
        for (size_t j = 0; j < X.get_cols(); ++j) {
            for (size_t i = 0; i < X.get_rows(); ++i) {
                X_normalized(i, j) = (X(i, j) - feature_means[j]) / (feature_stds[j] + 1e-8);
            }
        }
        Matrix<T> X_bias = add_bias(X_normalized);
        return X_bias * weights.transpose();
    }
    // 打印权重向量
    void print_weights() const {
        std::cout << "Weights (including bias): ";
        weights.print();
        std::cout << std::endl;
    }
private:
    // 添加偏置项到输入矩阵
    Matrix<T> add_bias(const Matrix<T>& X) const {
        Matrix<T> X_bias(X.get_rows(), X.get_cols() + 1);
        #pragma omp parallel for
        for (size_t i = 0; i < X.get_rows(); ++i) {
            X_bias(i, 0) = 1.0; // 偏置项
            for (size_t j = 0; j < X.get_cols(); ++j) {
                X_bias(i, j + 1) = X(i, j);
            }
        }
        return X_bias;
    }
    // 计算均方误差 (MSE) 损失
    T compute_loss(const Matrix<T>& y_true, const Matrix<T>& y_pred) const {
        Matrix<T> diff = y_pred - y_true;
        T loss = (diff.transpose() * diff)(0, 0) / y_true.get_rows();
        return loss;
    }
    std::vector<T> feature_means;
    std::vector<T> feature_stds;
    Matrix<T> normalize(const Matrix<T>& X) {
        // 计算并保存均值和标准差
        Matrix<T> X_norm=X;
        feature_means.resize(X.get_cols());
        feature_stds.resize(X.get_cols());
        for (size_t j = 0; j < X.get_cols(); ++j) {
            T mean = 0, stddev = 0;
            for (size_t i = 0; i < X.get_rows(); ++i) {
                mean += X(i, j);
            }
            mean /= X.get_rows();
            for (size_t i = 0; i < X.get_rows(); ++i) {
                stddev += (X(i, j) - mean) * (X(i, j) - mean);
            }
            stddev = std::sqrt(stddev / X.get_rows());
            feature_means[j] = mean;
            feature_stds[j] = stddev;
            for (size_t i = 0; i < X.get_rows(); ++i) {
                X_norm(i, j) = (X(i, j) - mean) / (stddev + 1e-8);
            }
        }
        return X_norm;
    }
};