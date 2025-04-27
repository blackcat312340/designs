#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <initializer_list>
#include <queue>
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
    ll abs;
	i>0?abs=i:abs=-i;
    factors.emplace_back(1);
    if(flag==0) factors.emplace_back(-1);
	for (ll n = 2; n <= sqrt(abs); n++) {
		if (abs % n == 0) {
            factors.emplace_back(n);
			if (flag==0) factors.emplace_back(-n);
            factors.emplace_back(i/n);
			if (flag==0) factors.emplace_back(i/-n);
			}
	}
    factors.emplace_back(i);
    if (flag==0) factors.emplace_back(-i);
    return factors;
}
// 并行计算两个有序列表的交集（基础函数）
static std::vector<ll> ParallelIntersection(const std::vector<ll>& a, const std::vector<ll>& b) {
    std::vector<ll> result;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
    return result;
}
// 并行分治求交集（递归函数）
static std::vector<ll> ParallelCommonFactors(const std::vector<std::vector<ll>>& allFactors, size_t start, size_t end) {
    if (start == end) return allFactors[start];  // 单个列表直接返回
    if (end - start == 1) return ParallelIntersection(allFactors[start], allFactors[end]);  // 两个列表直接求交

    // 分治：将任务拆分为左右两部分并行计算
    size_t mid = start + (end - start) / 2;
    auto future = pool.enqueue(ParallelCommonFactors,allFactors,start,mid);

    // 当前线程处理右半部分
    std::vector<ll> right = ParallelCommonFactors(allFactors, mid + 1, end);
    std::vector<ll> left = future.get();  // 等待左半部分完成

    // 合并左右结果
    return ParallelIntersection(left, right);
}
//得到多个数的正公因数
std::vector<ll> GetCommonFactors(std::initializer_list<ll> args) {
    if(args.size()==0) return {};
    if(args.size()==1) return GetFactors(*args.begin());
    std::vector<std::future<std::vector<ll>>> results;
    for(ll arg:args) {
        results.emplace_back(pool.enqueue(GetFactors,arg,true));
    }
    std::vector<std::vector<ll>> allfactors;
    for(auto &temp:results) {
        std::vector<ll> factors=temp.get();
        std::sort(factors.begin(),factors.end());
        allfactors.push_back(std::move(factors));
    }
    return ParallelCommonFactors(allfactors,0,allfactors.size()-1);
}