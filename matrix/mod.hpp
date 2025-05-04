#include <iostream>
#include <cmath>
#include <stdexcept>
int extend_gcd(int a, int b, int &x, int &y){
    if(b == 0){
        x = 1, y = 0;
        return a;
    }
    int ret=extend_gcd(b, a % b, x, y),tmp = x;
    x = y;
    y = tmp - (a / b) * y;
    return ret;
}
int mod_inverse(int a, int mod){
    int x, y, d=extend_gcd(a, mod, x, y);
    return d==1?(x % mod + mod) % mod:-1;
}
struct mod{
    int val;
    mod() {
        val=0;
    }
    mod(int val) {
        this->val=(val%26+26)%26;
    }
    mod operator+(const mod &a) const{
        return mod((val+a.val)%26);
    }
    mod operator-(const mod &a) const{
        return mod(((val-a.val)%26+26)%26);
    }
    mod operator-() const{
        return mod(((-val)%26+26)%26);
    }
    mod operator*(const mod &a) const{
        return mod((val*a.val)%26);
    }
    mod operator/(const mod &a) const{
        if(mod_inverse(a.val,26)==-1) throw std::invalid_argument("No inverse exists for this value.");
        return mod((val*mod_inverse(a.val,26))%26);
    }
    // 重载 << 运算符
    friend std::ostream& operator<<(std::ostream& os, const mod& m) {
        os << m.val;
        return os;
    }
    bool operator==(const mod& other) const {
        return val == other.val;
    }
    mod& operator+=(const mod& other) {
        val = (val + other.val) % 26; // 假设模数为 26
        return *this;
    }
    mod& operator-=(const mod& other) {
        val = ((val - other.val) % 26+26)%26; // 假设模数为 26
        return *this;
    }
    mod& operator*=(const mod& other) {
        val = (val * other.val) % 26; // 假设模数为 26
        return *this;
    }
    mod& operator/=(const mod& other) {
        val = (val * mod_inverse(other.val,26)) % 26; // 假设模数为 26
        return *this;
    }
};
namespace std {
    int abs(mod& m) {
        return std::abs(m.val); // 使用内置的 std::abs 处理整数
    }
}