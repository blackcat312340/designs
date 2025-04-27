#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <set>
#include <algorithm>
using namespace std;
//一些通用方法
    //求和dfs
	void sum_all(vector<int> &result,vector<vector<int>> &array,vector<int> w,int i,int sum) {
		if(i==array.size()) {
			result.push_back(sum);
			return;
		}
		for(int j=0;j<array[i].size();j++) {
			sum+=array[i][j]*w[i];
			sum_all(result,array,w,i+1,sum);
			sum-=array[i][j]*w[i];
		}
	}
	//多项式减法,并且a的次数高于或等于b
	void Polynomial_Subtraction(vector<int> &a,vector<int> &b,vector<int> &result) {
		for(int i=0;i<a.size();i++) {
			if(i<b.size()) {
				result.push_back(a[i]-b[i]);
			}
			else {
				result.push_back(a[i]);
			}
		}
		for(int i=result.size()-1;i>=0;i--) {
			if(result[i]==0) {
				result.pop_back();
			}
			else {
				break;
			}
		}
	}
	//多项式除法,并且a的次数高于b
	void Polynomial_division(vector<int> &a,vector<int> &b,vector<int> &result) {
		vector<int> aa=a;
		while (true){
			vector<int> ap;
			int temp=aa.size()-b.size();
			vector<int> t;
			for(int i=0;i<temp;i++) {
				t.push_back(0);
			}
			for(int i:b) {
				t.push_back(i);
			}
			Polynomial_Subtraction(aa,t,ap);
			if(ap.size()<b.size()) {
				result=ap;
				break;		
			}
			aa=ap;
		}
	}
	//简化同余式左侧高次项，使最高项小于p
	void Simplify_mod_p (vector<int> keys,int p ,vector<int> &result) {
		 vector<int> g;
		 g.push_back(0);
		 g.push_back(-1);
		 for(int i=2;i<p;i++) {
			g.push_back(0);
		 }
		 g.push_back(1);
		 Polynomial_division(keys,g,result);
	}
    // 求导式
	void derivation(vector<int>& keys, vector<int>& newkeys) {
		for (int i = 1; i < keys.size(); i++) {
			newkeys.push_back(i * keys[i]);
		}
	}
    //验算
	int caculate(vector<int>& keys, int x,int mod=1) {
		int result = 0;
		for (int i = 1; i < keys.size(); i++) {
			result += (int(pow(x, i))%mod) * keys[i];
		}
		return result+keys[0];
	}
    //限制函数，限制参数n保持在under下
int limit(int n, int under) {
	if (n >= under) {
		return under-1;
	}
	else {
		return n;
	}
}