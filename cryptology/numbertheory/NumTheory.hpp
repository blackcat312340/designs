#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <initializer_list>
#include <queue>
#include <atomic>
#include "threadpool.hpp"
typedef long long ll;
/*
不可直接调用
欧几里得算法
*/
static void OJLD(ll a, ll b, ll& q, ll& r) {
    q = a / b;
    r = a - b * q;
    if (r > b / 2) {
        q++;
        r = r - b;
    }
}
/*
不可直接调用
扩展欧几里得算法，用于算逆元
*/
static ll extend_gcd(ll a, ll b, ll &x, ll &y){
    if(b == 0){
        x = 1, y = 0;
        return a;
    }
    ll ret=extend_gcd(b, a % b, x, y),tmp = x;
    x = y;
    y = tmp - (a / b) * y;
    return ret;
}
/*最大公因数*/
ll GCD(ll i, ll j) {
    if (i == 0 || j == 0) return i==0?j:i;
    ll q,r;
    i<0?i=-i:0;
    j<0?j=-j:0;
    OJLD(i, j, q, r);
    if (r < 0) r = -r;
    if (r != 0) return GCD(j, r);
    else return j;
}
/*最小公倍数*/
ll LCM(ll a, ll b) {
    if (GCD(a, b) == 1) return a * b;
    else return a * b / GCD(a, b);
}
/*求逆
  返回-1即表示为没有逆元
*/
ll mod_inverse(ll a, ll mod){
    ll x, y, d=extend_gcd(a, mod, x, y);
    return d==1?(x % mod + mod) % mod:-1;
}
//得到参数i的因数，默认flag=1的情况下不考虑负数域，不想要负数域的话请手动添加第二个参数为1
std::vector<ll> GetFactors(ll i,bool flag=1) {
	if (i == 0) throw std::logic_error("Zero is a multiple of all non-zero llegers");
    std::vector<ll> factors;
    ll abs,limit = std::sqrt(abs);
	i>0?abs=i:abs=-i;
    flag==0?factors.reserve(limit + 1):factors.reserve(2 * limit + 2); // 预分配空间，避免多次扩容
    factors.push_back(1);
    if(flag==0) factors.push_back(-1);
	for (ll n = 2; n <= limit; n++) {
		if (abs % n == 0) {
            factors.push_back(n);
			if (flag==0) factors.push_back(-n);
            factors.push_back(i/n);
			if (flag==0) factors.push_back(i/-n);
			}
	}
    factors.push_back(i);
    if (flag==0) factors.push_back(-i);
    factors.shrink_to_fit();
    return factors;
}
// 并行计算两个有序列表的交集（基础函数）
static std::vector<ll> Parallel_Intersection(const std::vector<ll>& a, const std::vector<ll>& b) {
    std::vector<ll> result;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
    return result;
}
// 并行分治求交集（递归函数）
static std::vector<ll> Parallel_Common_Factors(const std::vector<std::vector<ll>>& allFactors, size_t start, size_t end) {
    if (start == end) return allFactors[start];  // 单个列表直接返回
    if (end - start == 1) return Parallel_Intersection(allFactors[start], allFactors[end]);  // 两个列表直接求交
    // 分治：将任务拆分为左右两部分并行计算
    size_t mid = start + (end - start) / 2;
    auto future = pool.enqueue(Parallel_Common_Factors,allFactors,start,mid);
    // 当前线程处理右半部分
    std::vector<ll> right = Parallel_Common_Factors(allFactors, mid + 1, end);
    std::vector<ll> left = future.get();  // 等待左半部分完成
    // 合并左右结果
    return Parallel_Intersection(left, right);
}
//得到多个数的正公因数
std::vector<ll> Get_Common_Factors(std::initializer_list<ll> args) {
    if(args.size()==0) return {};
    if(args.size()==1) return GetFactors(*args.begin());
    std::vector<std::future<std::vector<ll>>> results;
    results.reserve(args.size());// 预分配空间，避免多次扩容
    for(ll arg:args) {
        results.emplace_back(pool.enqueue(GetFactors,arg,true));
    }
    std::vector<std::vector<ll>> allfactors;
    allfactors.reserve(results.size());
    for(auto &temp:results) {
        std::vector<ll> factors=temp.get();
        std::sort(factors.begin(),factors.end());
        allfactors.push_back(factors);
    }
    return Parallel_Common_Factors(allfactors,0,allfactors.size()-1);
}
//多线程欧拉素数筛法
std::vector<ll> Parallel_Euler_Sieve(ll n) {
    if (n < 2) return {}; // 没有小于 2 的素数
    std::vector<ll> primes; // 存储素数
    std::vector<bool> is_prime(n + 1,true); // 标记是否为素数
    is_prime[0] = is_prime[1] = false; // 0 和 1 不是素数
    ll limit=std::sqrt(n);
    // 主线程处理小素数的倍数
    std::vector<std::future<void>> futures;
    futures.reserve(limit);
    for (ll i = 2; i <= limit; ++i) {
        if (is_prime[i]) {
            // 并行标记 i 的倍数
            futures.emplace_back(pool.enqueue([i, n, &is_prime]() {
                for (ll j = i * i; j <= n; j += i) {
                    is_prime[j] = false;
                }
            }));
        }
    }
    // 等待所有任务完成
    for (auto& future : futures) {
        future.get();
    }
    primes.reserve(n / std::log(n)); // 预分配空间，避免多次扩容
    // 收集所有素数
    for (ll i = 2; i <= n; ++i) {
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }
    primes.shrink_to_fit();
    return primes;
}
//计算欧拉函数
ll Euler_Function(ll n) {
    ll limit = std::sqrt(n),result = n;
    for (ll i = 2; i <= limit; ++i) {
        if (n % i == 0) {
            while (n % i == 0) n /= i;
            result -= result / i;
        }
    }
    if (n > 1) result -= result / n; // 如果 n>1 还剩下一个质因子,或者n是质数
    return result;
}
//得到素因数
std::vector<ll> Get_Prime_Factors_Parallel(ll n) {
    if (n < 2) return {}; // 没有小于 2 的素因数
    std::vector<ll> prime_factors;
    std::mutex factors_mutex; // 用于保护 prime_factors 的并发访问
    std::vector<std::future<void>> futures;
    prime_factors.reserve(std::log(n));
    // 处理 2 的倍数
    while (n % 2 == 0) {
        prime_factors.push_back(2);
        n /= 2;
    }
    // 处理奇数因数
    ll limit = std::sqrt(n);
    for (ll i = 3; i <= limit; i += 2) {
        if (n % i == 0) {
            // 为每个可能的因数创建一个任务
            futures.emplace_back(pool.enqueue([i, &n, &prime_factors, &factors_mutex]() {
                while (n % i == 0) {
                    {
                        std::lock_guard<std::mutex> lock(factors_mutex); // 加锁保护 prime_factors
                        prime_factors.push_back(i);
                    }
                    n /= i;
                }
            }));
        }
    }
    // 等待所有任务完成
    for (auto& future : futures) future.get();
    // 如果 n 本身是一个大于 2 的素数
    if (n > 1) prime_factors.push_back(n);
    prime_factors.shrink_to_fit();
    return prime_factors;
}
//素数判定
bool Is_Prime_Parallel(ll n) {
    if (n < 2) return false; // 0 和 1 不是素数
    if (n == 2 || n == 3) return true; // 2 和 3 是素数
    if (n % 2 == 0 || n % 3 == 0) return false; // 排除偶数和 3 的倍数
    ll limit = std::sqrt(n);
    std::atomic<bool> is_prime(true); // 用于标记是否为素数
    std::vector<std::future<void>> futures;
    ll block_size = 1000; // 每个任务检测的范围大小
    futures.reserve((limit - 5) / block_size + 1);
    for (ll start = 5; start <= limit; start += block_size) {
        ll end = std::min(start + block_size - 1, limit);
        futures.emplace_back(pool.enqueue([start, end, n, &is_prime]() {
            for (ll i = start; i <= end; i += 6) {
                if (!is_prime) return; // 如果已经确定不是素数，提前退出
                //6n-1到6n+5之间除了6n-1和6n+1不是2和3的倍数，其他都是合数
                if (n % i == 0 || n % (i + 2) == 0) {
                    is_prime = false;
                    return;
                }
            }
        }));
    }
    for (auto& future : futures) {
        future.get();
    }
    return is_prime;
}