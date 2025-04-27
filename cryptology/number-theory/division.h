#pragma once
#include "headfiles.h"
using namespace std;
//得到参数i的因数，默认flag=0的情况下考虑负数域，不想要负数域的话请手动添加第三个参数为1
void GetFactors(int i, vector<int>& factors,bool flag=0) {//记录因数,i为所记录因数的倍数
	if (i == 0) {
		cout << "0为所有非零整数的倍数";    //这里之所以是非零整数的倍数，而不是所有整数的倍数，是因为定义1.1.1中明确说明了因数不可以是0
		return;
	}
	int stopline = i;
	int absolute = i;//这里用绝对值是为了方便编程，实际上一个数与其相反数的因数集合是相同的。
	if (i < 0) {
		stopline = -i;
		absolute = -i;
	}
	factors.push_back(1);//1为所有整数的因数
	if (flag == 0) {
		factors.push_back(-1);
	}
	for (int n = 2; n < absolute; n++) {
		if (absolute % n == 0 && stopline > n) {//由定义1.1.1知:设a，b为两个整数，其中b！=0，如果存在整数q使得等式a=q*b成立（即a%b==0为真），则称b
			factors.push_back(n);           //整除a或者a被b整除，记作b|a，并把b叫做a的因数，把a叫做b的倍数。
			if (flag == 0) {
				factors.push_back(-n);
			}         //由1.1.1下注知：当b遍历整数的所有因数时，-b也遍历整数a的所有因数，故-b也是a的因数，故将-b也一同加入容器
			if (absolute / n > n) {             //特别注意：符号的不确定性也是一个数的因数，与数本身不可比较大小的原因
				stopline = absolute / n;    //b遍历所有a的因数时，a/b也遍历a的所有因数，故将i/n加入容器
				factors.push_back(i / n);
				if (flag == 0) {
					factors.push_back(0 - i / n);
				}
			}
		}
	}
	if (flag == 0) {
		factors.push_back(-i);
	}
           //任何非零整数即是其自身的倍数，也是其自身的因数
	factors.push_back(i);
}

//前两个参数分别是要对比的两个数的因数集合，后一个参数是结果集合
void GetCommonFactors(vector<int> &factors_a, vector<int> &factors_b, vector<int>& commonfactors) {
	unordered_map <int,int> map;
	vector<int>factors;
	for(int i:factors_a) {
		map[i]++;
		factors.push_back(i);
	}
	for(int i:factors_b) {
		if(map[i]==0) {
			factors.push_back(i);
		}
		map[i]++;
	}
	for(int i:factors) {
		if(map[i]>1) {
			commonfactors.push_back(i);
		}
	}
}
//得到两个数的公因数
void GetCommonFactors(int a, int b,vector<int> &commonfactors) {
	vector<int> factors_a, factors_b;
	GetFactors(a, factors_a,1);
	if (a == b) {
		sort(factors_a.begin(), factors_a.end());
		commonfactors = factors_a;
		return;
	}
	GetFactors(b, factors_b,1);
	GetCommonFactors(factors_a,factors_b,commonfactors);
}
//定义2：设n!=0或者+-1，若除了+-1，与+-n以外n没有其他因数，则n叫做质数
//定理1.1.1：设a，b！=0，c！=0是三个整数，若b|a，c|b，则c|a。由这条定理我们可以做到获得正整数i的质因数,这是因为若i的因数n可被某一非零整数q整除，而q又可被某一正整数p整除，那么由定理知n也可被p整除,
//（这里的n不为1，p，q不为1和n）如此n便有1与其自身以外的因数，故n不为质数
//特别提醒这个函数不好用，要用得到素因数请用GetPrimeFactor2
void GetPrimeFactors1(vector<int>& factors, vector<int>& primefactors) {
	for (int i : factors) {
		if (i > 1) {
			bool flag = 0;
			vector<int> temp1;
			GetFactors(i, temp1);//得到i的因数
			//for (int i : temp1) {
				//cout << i << " ";
			//}
			//cout << endl;
			for (int j : temp1) {
				if (j > 1) {
					for (int k = 2; k < j; k++) {
						if (j % k == 0) {
							flag = 1;
							break;
						}//判断i的某一因数是否可被除1与自身之外的数整除
					}
					if (flag == 1) {
						break;
					}
				}
			}
			if (flag == 0) {
				primefactors.push_back(i);
			}
		}
	}
}
void GetPrimeFactors2(vector<int>& factors, vector<int>& primefactors);
//定理1.1.2：设a，b，c！=0是三个整数，若c|a，c|b，则c|a+b与c|a-b
//定理1.1.3：设a，b，c！=0是三个整数，若c|a，c|b，则对任意整数s，t，有c|（s*a+b*t）
//定理1.1.4：设整数c！=0，若整数a1，a2，a3，···，an都是c的倍数，则对任意n个整数s1，···，sn，s1*a1+···+sn*an是c的倍数
//复习例题：
//若ax+by=1，a|n，b|n，证明ab|n，（所有字母均在整数域，且a，b不同时为0）
//ax+by=1，左右同乘n/ab，有xn/b+yn/a=n/ab，又a|n，b|n；x，y均为整数，故等式左边为整数，设其为q（q！=0），则q=n/ab，即ab=nq，故ab|n
//而且值得注意的是ax+by=1可变形x=（1-by）/a，其中x，y均为整数，但是1/a在a！=1的情况下不为整数，那么在此前提下，b/a不可能为整数，即b，a互质
//定理1.1.5：设a,b都是非零整数，若a|b，b|a，则a=+b或-b。
//定理1.1.6：设n是一个正合数，p是n的一个大于一的最小正因数，则p是素数，且p<=sprt(n)
//素数的判别法1
void GetPrimeNumberNnder(int n, vector<int>& result) {
	if (n < 2) {
		return;
	}
	int nuder = sqrt(n);
	vector<int> temp1;
	vector<int> temp2;
	for (int i = 2; i <= nuder; i++) {
		temp1.push_back(i);
	}
	GetPrimeFactors2(temp1, temp2);//得到不超过sqrt（n）的素数
	vector<int> temp3;
	for (int i = 2; i <= n; i++) {
		temp3.push_back(i);
	}
	for (int i : temp2) {
		int j = i;
		while (i * j <= n) {//从p*p开始剔除
			temp3[i * j - 2] = 0;
			j++;
		}
	}
	for (int i : temp3) {//剔除得到素数的倍数
		if (i != 0) {
			result.push_back(i);
		}
	}
}
//定理1.1.7：设n是一个正整数，如果对所有的素数p<=sqrt(n),都有n不能整除p，则n是素数
//建议要找素数用这个，快多了
void GetPrimeFactors2(vector<int>& factors, vector<int>& primefactors) {
	for (int i : factors) {
		if (i > 1) {
			int sq = sqrt(i);
			vector<int> temp;
			GetPrimeNumberNnder(sq, temp);//得到小于sqrt（n）的素数集合
			bool flag = 0;
			for (int j : temp) {
				if (i % j == 0 && i != j) {//判断能不能被整除
					flag = 1;
					break;
				}
			}
			if (flag == 0) {
				primefactors.push_back(i);
			}
		}
	}
}
bool is_primenumber(int n) {
	vector<int> temp;
	GetPrimeNumberNnder(int(sqrt(n)), temp);
	for(int i:temp) {
		if(n%i==0) {
			return 0;
		}
	}
	return 1;
}
//定理1.1.8：素数有无穷多个。
//定理1.1.9：设a，b是两个整数，其中b大于0，则存在唯一的整数q，r使得a=bq+r，0<=r<b
//定理1.1.10：设a，b是两个整数，其中b大于0，则对任意整数c，存在唯一的整数q，r使得a=bq+r，c<=r<b+c
//定义4：设x是一个实数，我们称x的整数部分为小于或等于x的最大整数，记作[x].这时我们有[x]<=x<=[x]+1
//欧几里得除法，最后两个参数为引用类型可以记录结果。这里用绝对值最小余数
void OJLD(int a, int b, int& q, int& r) {
	q = a / b;
	r = a - b * q;
	if (r > b / 2) {
		q++;
		r = r - b;
	}
}
//定义1：设a1,a2,,..,an是n（n>2）个整数，若整数d|ak（k=1，2，3，...,n)，则称d是a1,a2,...,an的一个公因数。
//若a1,a2,,..,an不全为零,则整数a1,a2,,..,an的所有公因数中最大的一个公因数叫做最大公因数。记作（a1,a2,,..,an）。
//特别，当（a1,a2,,..,an）=1，称a1,a2,,..,an互质或者互素
//性质1：设a，b是两个整数，则（b，a）=（a，b）
//性质2：设a，b是两个正整数，如果b|a，则（a，b）=a。
//性质3：设p是一个素数，a为整数，如果a不能被p整除，则p与a互素
//性质4：绝对值不敏感
//性质5：正负不敏感
//性质6：设b是任一正整数，则（0，b）=b


//性质7：设a, b, c是三个不全为零的整数。如果a ＝ bq + c, 其中q是整数，则有(a, b) ＝(b, c)。
//性质8：设a，b是任意两个正整数，则（a，b）=rn，其中rn是广义欧几里得除法最后一个非零余数
int GetMaxCommonFactor(int i, int j) {
	if (i == 0 || j == 0) {
		return i==0?j:i;
	}
	int q;
	int r;
	OJLD(i, j, q, r);
	if (r < 0) {
		r = -r;
	}
	if (r != 0) {
		return GetMaxCommonFactor(j, r);
	}
	else {
		return j;//性质6：设b是任一正整数，则（0，b）=b
	}
}
//定理5：定理设a，b是任意两个正整数,则Sn*a+Tn*b=（a，b）
//对于n=0，1，2，···，这里Sn，Tn归纳地定义为：S0=1，S1=0，Sj=Sj-2-Qj-1*Sj-1，T0=1，T1=0，Tj=Tj-2-Qj-1*Tj-1，（j=2，3，4，···，n）
//其中Qj是广义欧几里得除法中的不完全商
class FIVE {
public:
	int count = 0;
	vector<int> S, T, R, Q;
	FIVE(int a, int b) {
		S.push_back(1);
		S.push_back(0);
		T.push_back(0);
		T.push_back(1);
		R.push_back(a);
		R.push_back(b);//注意这里的Q不存在初始化
		GetSandT();
		Print();
	}
	void GetSandT() {
		if (R[count + 1] == 0) {
			cout << S[count] << " " << T[count] << endl;
			return;
		}
		else {
			Q.push_back(R[count] / R[count + 1]);
			if (count > 0) {
				S.push_back(S[count - 1] - Q[count - 1] * S[count]);//显然的R的运算中用的数组下标要大S与T的下标一
				T.push_back(T[count - 1] - Q[count - 1] * T[count]);
			}
			R.push_back(R[count] - Q[count] * R[count + 1]);
			count++;
			GetSandT();
		}
	}
	void Print() {
		cout << setw(5) << "j";
		cout << setw(5) << "q";
		cout << setw(5) << "rj";
		cout << setw(7) << "rj+1";
		cout << setw(7) << "sj-1";
		cout << setw(5) << "sj";
		cout << setw(7) << "tj-1";
		cout << setw(5) << "tj" << endl;
		for (int i = 0; i <= count; i++) {
			cout << setw(5) << i;
			if (i > 0) {
				cout << setw(5) << Q[i - 1];
			}
			else {
				cout << setw(5) << " ";
			}
			cout << setw(5) << R[i];
			cout << setw(7) << R[i + 1];
			if (i > 0) {
				cout << setw(7) << S[i - 1];
			}
			else {
				cout << setw(7) << " ";
			}
			cout << setw(5) << S[i];
			if (i > 0) {
				cout << setw(7) << T[i - 1];
			}
			else {
				cout << setw(7) << " ";
			}
			cout << setw(5) << T[i] << endl;
		}
	}
};
//定理6：（a，b）=1的充分必要条件是：存在整数s，t，使得sa+tb=1
//设四个整数a, b, c, d满足关系式：ad － bc＝1,则(a, b)＝1, (a, c)＝1, (d, b)＝1, (d, c)＝1。
//设 a，b 是任意两个不全为零的整数，d是正整数， 则d=（a，b） 的充要条件是：（1）d|a，d|b；（2）若e|a，e|b，则e|d。
//设a ，b 是任意两个不全为零的整数.若 m是任一正整数 则,(am,bm)=(a,b)m.
//若非零整数d 满足d|a,d|b, 则(a/d,b/d)=(a,b)/|d|
//特别的：d=(a,b),(a/d,b/d)=1
//设a1,a2,...an是n个整数，且a1!=0,令(a1,a2)=d2,(d2,a3)=d3,...(dn-1,an)=dn,则(a1,a2,...,an)=dn;
//设a ,b 是两个正整数 若 a被 b除的最小正余数是r, 则2**a- 1被 2**b-1除的最小正余数是2**r-1.
//设a， b是两个正整数,则2**a-1与2**b-1的最大公因数是2**(a,b)-1.
//(2**a- 1, 2**b-1)=1的充分必要条件是(a,b)=1


//整除的进一步性质
//定理1：设a，b，c是三个整数，且b！=0，c！=0.若（a，c）=1，则（ab，c）=（b，c） ///其实学习这些定理之后不难发现，因数就像是一个数的性质或者说组成部分，
																				  // 或者说像是线性代数里面的极大线性无关组，标识着一个属的基本性质
//推论：设a，b，c是三个整数，且c！=0，如果c|ab，（a，c）=1，则c|b
//定理2：设p是素数，若p|ab，则p|a或p|b                                            //素数的“特质”只有一个，且该特质为其本身故不能分解。故其特质也不可能同时出现在两个数中
//定理3：设a，b，c是整数，若（a，c）=1，（b，c）=1，则（ab，c）=1    

//定理5：设a，b是两个正整数，则：
//（1）[a,b]=ab/(a,b),即[a,b](a,b)=ab
//(2) 若a|m，b|m，则[a,b]|m
void GetMinCommonFactors1(int a, int b) {
	if (GetMaxCommonFactor(a, b) == 1) {//定理4（2）
		cout << a * b;
	}
	else {
		cout << a * b / GetMaxCommonFactor(a, b);//定理5（1）
	}
}
//设a1,a2,...an 是n 个整数, 若a1|m,a2|m,...an|m,则 m叫做a1,a2,...an 的一个公倍数.a1,a2,...an的所有公倍数中
//最小正整数叫做最小公倍数，记作[a1,a2,...an]
//定理4：设a，b是两个互素的正整数，则
//（1）a|m，b|m，则ab|m
//（2）[a,b]=ab.

//推论：设m,a,b是正整数，则[ma,mb]=m[a,b]
//设a1,a2,...an是n个整数，令[a1,a2]=m2,[d2,a3]=m3,...[dn-1,an]=mn,则[a1,a2,...,an]=mn;
//定理7：若m是整数a1,a2,...an的公倍数，则[a1,a2,a3...an]|m

//标准素分解如下。
void StandardDecomposed(int n, vector<int>& factors, unordered_map<int, int>& a) {
	vector<int> temp;
	GetFactors(n, temp);
	GetPrimeFactors2(temp, factors);
	for (int i : factors) {
		a[i]=0;
	}
	int rest = n;
	while (rest != 1&&rest!=0) {
		for (int i : factors) {
			if (rest % i == 0 && rest != 0) {
				a[i]++;
				rest = rest/i;
			}
			if (rest == 1) {
				break;
			}
		}
	}
}
int GetMaxCommonFactor2(int a, int b) {//定理4
	vector<int> factorsa;
	vector<int> factorsb;
	unordered_map<int, int> aa;
	unordered_map<int, int> bb;
	StandardDecomposed(a, factorsa, aa);
	StandardDecomposed(b, factorsb, bb);
	unordered_map<int, int> result;
	set<int> factors;
	int count = 0;
	for (int i : factorsa) {
		factors.emplace(i);
		count++;
	}
	for (int i : factorsb) {//找到素因数集合的并集
		factors.emplace(i);
		count++;
	}
	int sum = 1;
	for (int i : factors) {
		if (aa[i] < bb[i]) {
			sum *= pow(i, aa[i]);//对比指数大小
		}
		else {
			sum *= pow(i, bb[i]);
		}
	}
	return sum;
}
void GetMinCommonFactors2(int a, int b) {//定理4
	vector<int> factorsa;
	vector<int> factorsb;
	unordered_map<int, int> aa;
	unordered_map<int, int> bb;
	StandardDecomposed(a, factorsa, aa);
	StandardDecomposed(b, factorsb, bb);
	unordered_map<int, int> result;
	set<int> factors;
	int count = 0;
	for (int i : factorsa) {
		cout << i << ":" << aa[i] << " ,";
		factors.emplace(i);
		count++;
	}
	cout << endl;
	for (int i : factorsb) {
		cout << i << ":" << bb[i] << " ,";
		factors.emplace(i);
		count++;
	}
	cout << endl;
	int sum = 1;
	for (int i : factors) {
		if (aa[i] > bb[i]) {
			sum *= pow(i, aa[i]);
		}
		else {
			sum *= pow(i, bb[i]);
		}
	}
	cout << sum << endl;
}
//算数基本定理
//定理1：任一整数n（n>1）都可以表成素数的乘积.且在不考虑乘积次序的情况下, 表达式是唯一的。
//定理2：任一大于1的整数n能够唯一地表示成n=(p1**a1)(p2**a2)...(ps**as),ai>0,i=1,2,...s,其中p1,p2,...ps为素数，pi<pj(i<j),分解式
//叫做n的标准分解式
//定理3：设n是一个大于1的整数，且n=(p1**a1)(p2**a2)...(ps**as),ai>0,i=1,2,...s，则d|n(d>0)的充分必要条件是d=(p1**b1)(p2**b2)...(ps**b3),a1>=b1>=0,i=1,2,...s
//定理4：设a，b是任意连个正整数，且a=(p1**a1)(p2**a2)...(ps**as),ai>0,i=1,2,...s，b=(p1**b1)(p2**b2)...(ps**b3),i=1,2,...s，则 （a，b）=(p1**n1)(p2**n2)...(ps**ns),[a,b]=(p1**m1)(p2**m2)...(ps**ms)
//其中ni=min(ai,bi),mi=max(ai,bi),i=1,2,3,...s
//不定方程的解：ax+by=c有解的充分必要条件是(a,b)=c;