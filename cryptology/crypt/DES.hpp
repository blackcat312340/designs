#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
class DES {
private:
    static constexpr int BLOCK_SIZE = 64;  // 数据块大小
    static constexpr int HALF_BLOCK_SIZE = 32;  // 半块大小
    static constexpr int KEY_SIZE = 56;  // 密钥大小
    static constexpr int SUBKEY_SIZE = 48;  // 子密钥大小
    std::vector<std::bitset<SUBKEY_SIZE>> key_schedule; // 16轮子密钥
    std::vector<int> shift_schedule;                    // 循环左移位数
    std::vector<int> initial_permutation_table;  // 初始置换表
    std::vector<int> inverse_permutation_table;  // 逆置换表
    std::vector<int> expansion_table;            // 扩展置换表
    std::vector<int> p_permutation_table;        // P置换表
    std::vector<std::vector<int>> s_boxes[8];    // S盒
    std::vector<int> PC1;                        // 初始密钥置换表
    std::vector<int> PC2;                        // 压缩置换表
public:
    DES() {
        // 初始化置换表、S盒等
        initialize_tables();
    }
    // 初始化置换表和 S 盒
    void initialize_tables() {
        initial_permutation_table = {
            58, 50, 42, 34, 26, 18, 10, 2,
            60, 52, 44, 36, 28, 20, 12, 4,
            62, 54, 46, 38, 30, 22, 14, 6,
            64, 56, 48, 40, 32, 24, 16, 8,
            57, 49, 41, 33, 25, 17, 9, 1,
            59, 51, 43, 35, 27, 19, 11, 3,
            61, 53, 45, 37, 29, 21, 13, 5,
            63, 55, 47, 39, 31, 23, 15, 7
        };

        inverse_permutation_table = {
            40, 8, 48, 16, 56, 24, 64, 32,
            39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30,
            37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28,
            35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26,
            33, 1, 41, 9, 49, 17, 57, 25
        };

        // 初始化 S 盒
        s_boxes[0] = {
            {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
            {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
            {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
            {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
        };
        s_boxes[1] = {
            {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
            {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
            {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
            {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
        };
        s_boxes[2] = {
            {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
            {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
            {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
            {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
        };
        s_boxes[3] = {
            {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
            {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
            {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
            {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
        };
        s_boxes[4] = {
            {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
            {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
            {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
            {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
        };
        s_boxes[5] = {
            {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
            {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
            {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
            {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
        };
        s_boxes[6] = {
            {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
            {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
            {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
            {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
        };
        s_boxes[7] = {
            {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
            {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
            {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
            {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
        };
        p_permutation_table = {
            16, 7, 20, 21,
            29, 12, 28, 17,
            1, 15, 23, 26,
            5, 18, 31, 10,
            2, 8, 24, 14,
            32, 27, 3, 9,
            19, 13, 30, 6,
            22, 11, 4, 25
        };
        PC1 = {
            57, 49, 41, 33, 25, 17, 9,
            1, 58, 50, 42, 34, 26, 18,
            10, 2, 59, 51, 43, 35, 27,
            19, 11, 3, 60, 52, 44, 36,
            63, 55, 47, 39, 31, 23, 15,
            7, 62, 54, 46, 38, 30, 22,
            14, 6, 61, 53, 45, 37, 29,
            21, 13, 5, 28, 20, 12, 4
        };
        PC2 = {
            14, 17, 11, 24, 1, 5,
            3, 28, 15, 6, 21, 10,
            23, 19, 12, 4, 26, 8,
            16, 7, 27, 20, 13, 2,
            41, 52, 31, 37, 47, 55,
            30, 40, 51, 45, 33, 48,
            44, 49, 39, 56, 34, 53,
            46, 42, 50, 36, 29, 32
        };
        shift_schedule = {1, 1, 2, 2, 2, 2, 2, 2,1, 2, 2, 2, 2, 2, 2, 1};
        expansion_table = {
            32, 1, 2, 3, 4, 5,
            4, 5, 6, 7, 8, 9,
            8, 9, 10, 11, 12, 13,
            12, 13, 14, 15, 16, 17,
            16, 17, 18, 19, 20, 21,
            20, 21, 22, 23, 24, 25,
            24, 25, 26, 27, 28, 29,
            28, 29, 30, 31, 32, 1
        };
    }
    // 通用置换函数
    template <size_t N,size_t M>
    void permute(const std::bitset<N>& input, std::bitset<M>& output,const std::vector<int>& table) {
        for (size_t i = 0; i < M; ++i) {
            output[i] = input[table[i] - 1];
        }
    }
    // 初始置换
    std::bitset<BLOCK_SIZE> initial_permutation(const std::bitset<BLOCK_SIZE>& input) {
        std::bitset<BLOCK_SIZE> permuted;
        permute(input, permuted, initial_permutation_table);
        return permuted;
    }
    // 逆置换
    std::bitset<BLOCK_SIZE> inverse_permutation(const std::bitset<BLOCK_SIZE>& input) {
        std::bitset<BLOCK_SIZE> permuted;
        permute(input, permuted, inverse_permutation_table);
        return permuted;
    }
    // S 盒替换
    std::bitset<HALF_BLOCK_SIZE> s_box_substitution(const std::bitset<48>& input) {
        std::bitset<HALF_BLOCK_SIZE> output;
        for (int i = 0; i < 8; ++i) {
            int row = (input[i * 6] << 1) | input[i * 6 + 5];
            int col = (input[i * 6 + 1] << 3) | (input[i * 6 + 2] << 2) |
                      (input[i * 6 + 3] << 1) | input[i * 6 + 4];
            int value = s_boxes[i][row][col];
            for (int j = 0; j < 4; ++j) {
                output[i * 4 + j] = (value >> (3 - j)) & 1;
            }
        }
        return output;
    }
    // Feistel 函数
    std::bitset<HALF_BLOCK_SIZE> feistel(const std::bitset<HALF_BLOCK_SIZE>& right, const std::bitset<SUBKEY_SIZE>& subkey) {
        // 1. 扩展置换
        std::bitset<48> expanded;
        permute(right, expanded, expansion_table);
        // for(int i = 0; i < 48; ++i) {
        //     std::cout<<expanded[i];
        // }
        // std::cout<<std::endl;
        // 2. 与子密钥异或
        std::bitset<48> xored = expanded ^ subkey;
        // for(int i = 0; i < 48; ++i) {
        //     std::cout<<xored[i];
        // }
        // std::cout<<std::endl;
        // 3. S 盒替换
        std::bitset<HALF_BLOCK_SIZE> substituted = s_box_substitution(xored);
        // for(int i = 0; i < HALF_BLOCK_SIZE; ++i) {
        //     std::cout<<substituted[i];
        // }
        // std::cout<<std::endl;
        // 4. P 置换
        std::bitset<HALF_BLOCK_SIZE> permuted;
        permute(substituted, permuted, p_permutation_table);
        // for(int i = 0; i < HALF_BLOCK_SIZE; ++i) {
        //     std::cout<<permuted[i];
        // }
        // std::cout<<std::endl;
        return permuted;
    }
    // 加密函数
    std::bitset<BLOCK_SIZE> encrypt(const std::bitset<BLOCK_SIZE>& plaintext, const std::bitset<BLOCK_SIZE>& key) {
        // 1. 初始置换
        std::bitset<BLOCK_SIZE> permuted = initial_permutation(plaintext);
        // 2. 分割为左右两部分
        std::bitset<HALF_BLOCK_SIZE> left, right;
        for (int i = 0; i < HALF_BLOCK_SIZE; ++i) {
            left[i] = permuted[i];
            right[i] = permuted[i + HALF_BLOCK_SIZE];
        }
        // 3. 生成子密钥
        generate_subkeys(key);
        // 4. 16 轮 Feistel 加密
        for (int i = 0; i < 16; ++i) {
            std::bitset<HALF_BLOCK_SIZE> temp = left;
            left = right;
            std::bitset<HALF_BLOCK_SIZE> temp2 = feistel(right, key_schedule[i]);
            // for(int i = 0; i < HALF_BLOCK_SIZE; ++i) {
            //     std::cout<<temp2[i];
            // }
            // std::cout<<std::endl;
            right = temp^feistel(right, key_schedule[i]);
            // for(int i = 0; i < HALF_BLOCK_SIZE; ++i) {
            //     std::cout<<left[i];
            // }
            // std::cout<<std::endl;
            // for(int i = 0; i < HALF_BLOCK_SIZE; ++i) {
            //     std::cout<<right[i];
            // }
            // std::cout<<std::endl;
            // for(int j = 0; j < KEY_SIZE; ++j) {
            //     std::cout<<key_schedule[i][j];
            // }
            // std::cout<<std::endl;
        }
        std::bitset<BLOCK_SIZE> combined;
        for (int i = 0; i < HALF_BLOCK_SIZE; ++i) {
            combined[i] = left[i];
            combined[i + HALF_BLOCK_SIZE] = right[i];
        }
        return inverse_permutation(combined);
    }
    // 子密钥生成
    void generate_subkeys(const std::bitset<BLOCK_SIZE>& key) {
        // 1. 初始密钥置换 (PC-1)
        std::bitset<KEY_SIZE> permuted_key;
        permute(key, permuted_key, PC1);
        // 2. 分割为左右两部分
        std::bitset<28> left, right;
        for (int i = 0; i < 28; ++i) {
            left[i] = permuted_key[i];
            right[i] = permuted_key[i + 28];
        }
        std::cout<<std::endl;
        // 3. 生成 16 轮子密钥
        key_schedule.resize(16);
        for (int round = 0; round < 16; ++round) {
            // 循环左移
            int shifts = shift_schedule[round];
            left = left >> shifts | left << (28 - shifts);
            right = right >> shifts | right << (28 - shifts);
            // for(int i = 0; i < 28; ++i) {
            //     std::cout<<left[i];
            // }
            // std::cout<<std::endl;
            // for(int i = 0; i < 28; ++i) {
            //     std::cout<<right[i];
            // }
            // std::cout<<std::endl;
            // 合并左右部分
            std::bitset<56> combined;
            for (int i = 0; i < 28; ++i) {
                combined[i] = left[i];
                combined[i + 28] = right[i];
            }
            // 压缩置换 (PC-2)
            permute(combined, key_schedule[round], PC2);
        }
    }
};