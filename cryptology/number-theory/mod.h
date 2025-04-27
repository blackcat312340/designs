#pragma once
#include "headfiles.h"
#include "division.h"
///这章的函数目前是没怎么用的（除欧拉函数外啊），但是应当是做成符号重载之类的函数辅助多项式或者单项同余式化简或者运算
bool is_Congruence_modulo1(int a, int b, int m) {//定理1：设m是一个正整数，a，b是两个整数，则a与b模m同余的充分必要条件是存在整数k，使得a=b+km
	if (a == b) {//自反性，还有对称性，传递性
		return 1;
	}
	if ((a - b) % m == 0) {
		return 1;
	}
	return 0;
}
bool is_Congruence_modulo2(int a, int b, int m) {//定理3：整数a，b模m同余的充分必要条件是a，b被m除的余数相同
	if (a == b) {//自反性
		return 1;
	}
	if (a%m==b%m) {
		return 1;
	}
	return 0;
}
struct congruence {
	int a;
	int b;
	int mod;
	congruence(int a, int b, int mod) {
		this->a = a;
		this->b = b;
		this->mod = mod;
	}
};
congruence* congruence_plus(congruence& a, congruence& b) {//加法
	if (a.mod = b.mod) {
		return new congruence(a.a + b.a, a.b + b.b, a.mod);
	}
	else {
		cout << "the two have different mod";
		return NULL;
	}
}
congruence* congruence_minus(congruence& a, congruence& b) {//减法                    //同模才可加减乘
	if (a.mod = b.mod) {
		return new congruence(a.a - b.a, a.b - b.b, a.mod);
	}
	else {
		cout << "the two have different mod";
		return NULL;
	}
}
congruence* congruence_times(congruence& a, congruence& b) {//乘法
	if (a.mod = b.mod) {
		return new congruence(a.a * b.a, a.b * b.b, a.mod);
	}
	else {
		cout << "the two have different mod";
		return NULL;
	}
}
//这里注意，没有除法
congruence* congruence_Simplify(congruence& a) {//定理8：设m为一个正整数，ad，bd模m同余，如果（d，m）=1，则a，b模m同余；
	vector<int> commonfactors;
	GetCommonFactors(a.a, a.b, commonfactors);
	sort(commonfactors.begin(), commonfactors.end(), greater<int>());
	for (int i : commonfactors) {
		if (GetMaxCommonFactor2(i, a.mod) == 1) {
			return new congruence(a.a / i, a.b / i, a.mod);
		}
	}
	cout << "can not simplify";
	return NULL;
}
congruence* k_times(congruence& a, int k) {//定理九
	return new congruence(k * a.a, k * a.b, k * a.mod);
}
//congruence* d_division(congruence& a, int d) {//定理10：a，b模m同余，有设d是，a，b，m的任一公因数，则a/d，b/d模m/d同余
	//vector<int> list;
	//list.push_back(a.a);
	//list.push_back(a.b);
	//list.push_back(a.mod);
	//vector<int> result;
	//GetCommonFactors(list, result);
	//sort(result.begin(), result.end());
	//return new congruence(a.a / result[0], a.b / result[0], a.mod / result[0]);//这里偷懒，只用第一个除
//}
//定理11：设m是正整数，a，b模m同余，如果d|m，d>0，则a，b模d同余
//定理同余12：设m1,m2,m3...是正整数,且a，b模mi，i=1，2，3...则a，b模m1，m2，m3...的最小公倍数同余
//推论：设m1,m2,m3...是两两互素的正整数，且a，b模mi，i=1，2，3...，则a，b模所有m的乘积同余
struct  residue_class {//剩余类
	int mod;
	int a;
	vector<int> c;
	residue_class(int a, int mod) {
		this->a = a;
		this->mod = mod;
		for (int i = 0; i < mod; i++) {//c默认范围
			c.push_back(i);
		}
	}
	congruence* getcongruence(int i) {
		return new congruence(a,c[i], mod);
	}
	void isSimplify() {//简化剩余类
		vector<int> temp;
		GetCommonFactors(a, mod, temp);//m与i互素
		if (temp.size()>1) {
			cout << "is not a simplify residue class";
		}
	}//从不同简化剩余类里面挑取一个数组成了简化剩余系
};
//注意到，若两个剩余类相等，则两个剩余系的等价划分条件(a)相同，且两个等价划分条件不相同的剩余系没有交集
//互相不属于同一个剩余类的m个整数叫做模m的一个完全剩余系，即m个整数两两模m不同余
//设m是正整数，（a，m）=1，b是任意整数，若x遍历模m的一个完全剩余系，则ax+b也遍历模m的一个完全剩余系
//若（m1，m2）=1，m1>0,m2>0，而x1，x2分别遍历模m1，m2的完全剩余系，则m2x1+m1x2遍历模m1m2的完全剩余系
//互相不属于同一个简化剩余类的m个整数叫做模m的一个简化剩余系，即m个整数两两模m不同余
//设m是正整数，（a，m）=1，若x遍历模m的一个简化剩余系，则ax也遍历模m的一个简化剩余系
//定理4：设m是一个正整数，（a，m）=1，则存在整数b，1<=b<m，使得a*b，与1模m同余
//若（m1，m2）=1，m1>0,m2>0，而x1，x2分别遍历模m1，m2的简化剩余系，则m2x1+m1x2遍历模m1m2的简化剩余系
int Euler(int n) {
	vector<int> factors;
	vector<int> prime_factors;
	GetFactors(n, factors);
	GetPrimeFactors1(factors, prime_factors);
	float result = n;
	for (int i : prime_factors) {
		float temp = float(1) - float(1) / i;
		result *= temp;
	}
	return result;
}
//定理8：设n是一个正整数，则所有n的正因数的欧拉函数值的和为n
//欧拉定理：设m是大于1的整数，（a，m）=1，则a**m的欧拉函数值，与1模m同余
//费马定理：设p是素数，则对任何整数a，有a**p与a模p同余
//威尔森定理：设p是一个素数，则（p-1）！与-1模p同余
void binumber(int i,vector<int> &result) {
	int q;
	int r;
	while (1) {
		OJLD(i, 2, q, r);
		result.push_back(r);
		i = q;
		if (q == 0) {
			break;
		}
	}
}
int pow_quick(int x, int y) {//模复平方
	vector<int> bi;
	binumber(y, bi);
	long result = 1;
	for (int i = bi.size() - 1; i > -1; i--) {
		if (i == 0) {
			result *= pow(x, bi[i]);
			break;
		}
		result *= pow(x, bi[i]);
		result *= result;
	}
	return result;
}