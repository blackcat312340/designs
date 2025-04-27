#include "headfiles.h"
#include "headfiles.h"
#include "division.h"
#include "mod.h"
#include "primary_root_and_exponent.h"
//包含雅各比符号，勒让德符号，同余式结构体，以及解决办法，中国剩余定理
struct Legander_symbol {//勒让德符号
	int a;
	int p;
	Legander_symbol(int a,int p) {//雅各比符号分母为奇素数
		if(p<=2||!is_primenumber(p)) {
			cout<<"p is not legal"<<endl;
			exit(0);
		}
		this->a=a;
		this->p=p;
	}
	int caculate_dir() {
		if(a==1) {
			return 1;
		}
		if(a==p-1||a==-1) {
			return int(pow(-1,(p-1)/2));
		}
		if(a==2) {
			return int(pow(-1,(pow(p,2)-1)/8));
		}
		return 0;
	}
	int quadratic_reciprocity_law() {//二次互反律，前是两者互素，其实也就是都为奇素数
		if(is_primenumber(a)) {
			int temp=a;
			a=p;
			p=temp;
			return int(pow(-1,(a-1)*(p-1)/4));//正负变换
		}
		else {
			return 0;
		}
	}
	void simply() {
		a=a%p;
	}
	int caculate_decompose() {//用标准素分解解决
		simply();
		vector<int> factors;
		unordered_map<int,int> degs;
		StandardDecomposed(a,factors,degs);
		int count=1;
		for(int i:factors) {
			Legander_symbol tempj(i,p);
			//cout<<"a"<<tempj.a<<endl;
			//cout<<"p"<<tempj.p<<endl;
			count*=pow(tempj.resolve(),degs[i]);
		}
		return count;
	}
	int resolve() {//这里是递归解决的
		simply();
		if(a==2||a==1||a==p-1) {
			return caculate_dir();
		}
		else {
			if(is_primenumber(a)) {
				int q=quadratic_reciprocity_law();
				simply();
				return q*resolve();
			}
			else {
				return caculate_decompose();
			}
		}
	}
};
struct Jacobi_symbol {//雅各比符号
	int a;
	int m;
	Jacobi_symbol(int a,int m) {
		if(m<=2) {
			cout<<"m is not legal"<<endl;
			exit(0);
		}
		this->a=a;
		this->m=m;
	}
	int resolve() {
		if(is_primenumber(m)) {
			Legander_symbol temp(a,m);
			return temp.resolve();
		}
		else {
			vector<int> factors;
			unordered_map<int,int> degs;
			StandardDecomposed(m,factors,degs);
			int count=1;
			for(int i:factors) {
				Legander_symbol temp(a,i);
				count*=temp.resolve();
			}
			return count;
		}
	}
};
struct CongruenceEquation {
	vector<int> keys;
	int mod;
	int deg;
	int flag=0;//flag==1 意味着除最高次项外其他次均为零项
	//一般构造
	CongruenceEquation(vector<int> &list,int mod) {
        keys=list;
        this->mod=mod;
        deg=list.size()-1;
    }
	//一次构造
	CongruenceEquation(int a ,int b,int mod) {
        keys.push_back(-b);
		keys.push_back(a);
        this->mod=mod;
        deg=1;
    }
	//二次构造
	CongruenceEquation(int a ,int b,int c,int mod) {
        keys.push_back(-b);
		keys.push_back(a);
		keys.push_back(c);
        this->mod=mod;
        deg=2;
    }
	//高次构造
	CongruenceEquation(int deg,int mod) {
		for(int i=0;i<deg;i++) {
			keys.push_back(0);
		}
		keys.push_back(1);
		this->mod=mod;
		this->deg=deg;
		flag=1;
	}
	//一次可解性判断
	int iscanberesolve_deg1() {
		if (keys[0] % GetMaxCommonFactor(keys[1], mod) == 0) {
			//cout << "can be resolved and number of resolutions is " << GetMaxCommonFactor(keys[1], mod)<<endl;//定理1：一次同余式有解的充分必要条件是（a，m）|b，且解数为（a，m）
			return GetMaxCommonFactor(keys[1], mod);
		}
		else {
			return 0;
		}
	}
	void Simplfy() {

	}
	//降模次
    bool lower_mod_deg1(int p,int deg,int &result) {
		vector<int> tem;
		if(deg>2) {
			Simplify_mod_p(keys,p,tem);
		}
		else {
			tem=keys;
		}
		CongruenceEquation temp(tem, p);//多项式降次后再计算
		int x;
        temp.resolve_deg1_dir_one(x);
        //cout<<"x"<<x<<endl;
		vector<int> newkeys;
		derivation(keys, newkeys);
		if(GetMaxCommonFactor2(caculate(newkeys,x),p)==1) {///这里非常重要，公式不好记就理解代码顺序
			for (int i = 1; i < deg; i++) {
                vector<int> tem;
                int x1=caculate(keys, x)%int( pow(p, i + 1));
                int x2=caculate(newkeys, x)% int(pow(p, i + 1));
                tem.push_back(x1);
                //cout<<"x1"<<x1<<endl;
                tem.push_back(x2*int(pow(p,i)));
                //cout<<"x2"<<x2<<endl;
                //cout<<"falh"<<caculate(newkeys, x)% int(pow(p, i + 1))<<endl;
				CongruenceEquation temp(tem, int(pow(p, i + 1)));
                int y;
                temp.resolve_deg1_dir_one(y);
                //cout<<"y"<<y<<endl;
				if ( x< 0) {
					x = x + pow(p, i) * y;
				}
				else {
					x = x + pow(p, i) * y;
				}
			}
            result=x%int(pow(p,deg));
			return 1;
		}
		else {
			return 0;
		}
	}
    //解决一次同余式直接方法
	bool resolve_deg1_dir_one(int &result) {
		int num=iscanberesolve_deg1();
		if (num!=0) {
            vector<int> temp;
            temp.push_back(keys[0]/num);
            temp.push_back(keys[1]/num);
			CongruenceEquation sim(temp, mod / num);
			if (sim.keys[0] == -1) {
				for (int i = 1; i < sim.mod; i++) {//注意逆元可能存在的范围(设m是一个正整数，则整数a是模m简化剩余的充分必要条件是整数a是模m可逆元)
                    if (sim.keys[1] * i % sim.mod == (-sim.keys[0]) % sim.mod) {
						result= i;
						return 0;
					}
				}
			}
			else {
                vector<int> tem;
                tem.push_back(-1);
                tem.push_back(sim.keys[1]);
				CongruenceEquation temp(tem, sim.mod);//求解ax，与1模m同余的解
				int tempx;
				temp.resolve_deg1_dir_one(tempx);
				result= (-sim.keys[0] * tempx) % sim.mod;
				return 0;
			}
		}
		return 1;
	}
	bool resolve_deg1_dir_mul(vector<int> &result) {
		int num=iscanberesolve_deg1();
		if (num!=0) {
            vector<int> temp;
            temp.push_back(keys[0]/num);
            temp.push_back(keys[1]/num);
			CongruenceEquation sim(temp, mod / num);
			if (sim.keys[0] == -1) {
				for (int i = 1; i < sim.mod; i++) {//注意逆元可能存在的范围(设m是一个正整数，则整数a是模m简化剩余的充分必要条件是整数a是模m可逆元)
                    if (sim.keys[1] * i % sim.mod == (-sim.keys[0]) % sim.mod) {
						result.push_back(i);
					}
					if(result.size()==num) {
						return 0;
					}
				}
			}
			else {
                vector<int> tem;
                tem.push_back(-1);
                tem.push_back(sim.keys[1]);
				CongruenceEquation temp(tem, sim.mod);//求解ax，与1模m同余的解
				int tempx;
				temp.resolve_deg1_dir_one(tempx);
				tem[0]=keys[0]*tempx;
				tem[1]=1;
				CongruenceEquation tempp(tem,sim.mod);
				tempp.resolve_deg1_dir_mul(result);
				return 0;
			}
		}
		return 1;
	}
	//解决一次同余式分解方法
    void resolve_deg1_decompose(int &result) {
		result=0;
        vector<int> factors;
        unordered_map<int,int> degs;
        StandardDecomposed(mod,factors,degs);
		vector<CongruenceEquation*> list;
        for(int i:factors) {
            int temp;
            lower_mod_deg1(i,degs[i],temp);
			vector<int> templist;
			templist.push_back(-temp);
			templist.push_back(1);
			list.push_back(new CongruenceEquation(templist,i));
        }
		int sum = mod;//模的积
		int x = 0;
		for (CongruenceEquation* &i : list) {
			int tempm = sum / i->mod;
            vector<int> tem;
            tem.push_back(-1);
            tem.push_back(tempm);
			CongruenceEquation tempc(tem,i->mod);
			int tempM;
            tempc.resolve_deg1_dir_one(tempM);
			x += tempm * tempM * (-(i->keys[0]));
		}
		result= x%sum;
    }
	//降模次
	bool lower_mod_deg_high(int p,int deg,vector<int> &result) {
		CongruenceEquation temp(keys, p);
		vector<int> xs;
		temp.resolve_deg_high_dir(xs);
		if(xs.size()==0){
			return 0;
		}
		else {
			for(int x:xs) {
				vector<int> newkeys;
				derivation(keys, newkeys);
				if(GetMaxCommonFactor2(caculate(newkeys,x),p)==1) {
					for (int i = 1; i < deg; i++) {
                		vector<int> tem;
                		int x1=caculate(keys, x)%int( pow(p, i + 1));
                		int x2=caculate(newkeys, x)% int(pow(p, i + 1));
                		tem.push_back(x1);
                //cout<<"x1"<<x1<<endl;
                		tem.push_back(x2*int(pow(p,i)));
                //cout<<"x2"<<x2<<endl;
                //cout<<"falh"<<caculate(newkeys, x)% int(pow(p, i + 1))<<endl;
						CongruenceEquation temp(tem, int(pow(p, i + 1)));
                		int y;
                		temp.resolve_deg1_dir_one(y);
                //cout<<"y"<<y<<endl;
						x = x + pow(p, i) * y;
					}
					int re=x%int(pow(p,deg));
					if(re<0) {
						re+=int(pow(p,deg));
					}
            		result.push_back(re);
				}
				else {
					return 0;
				}
			}
		}
		return 1;
	}
	//解决高次或二次同余式直接方法,不建议用
	void resolve_deg_high_dir(vector<int> &result) {
		for(int i=1;i<mod;i++) {
			if(caculate(keys,i,mod)%mod==0) {
				result.push_back(i);
			}
		}
	}
	//解决高次或二次同余式分解方法
	void resolve_deg_high_decompose(vector<int> &result) {//dfs解决求解
		vector<int> factors;
        unordered_map<int,int> degs;
        StandardDecomposed(mod,factors,degs);
		vector<int> w;//系数数组
		vector<vector<int>> results;
		for(int i:factors) {
			int tempm = mod / int(pow(i,degs[i]));
            vector<int> tem;
            tem.push_back(-1);
            tem.push_back(tempm);
			CongruenceEquation tempc(tem,int(pow(i,degs[i])));
			int tempM;
            tempc.resolve_deg1_dir_one(tempM);
			w.push_back(tempm * tempM);
			vector<int> tempre;
			lower_mod_deg_high(i,degs[i],tempre);
			if(tempre.size()==0) {
				cout<<"无解"<<endl;
				return;
			}
			results.push_back(tempre);
		}
		int ind=0;
		int sum=0;
		sum_all(result,results,w,ind,sum);
	}
};
struct crt {//中国剩余定理
	vector<CongruenceEquation*> funs;
	crt(vector<CongruenceEquation*> &input) {
		funs = input;
	}
	bool iscanberesolve(int &sum) {//crt要求模两两互素
		vector<int> mods;
		for (CongruenceEquation* &temp : funs) {
			sum *= temp->mod;
			mods.push_back(temp->mod);
		}
		for(int i=0;i<mods.size();i++) {
            for(int j=0;j<mods.size();j++) {
                vector<int> temp;
                if(i!=j) {
                    GetCommonFactors(mods[i],mods[j],temp);
                    if (temp.size() != 1) {
			            return false;
		            }
                }
            }
        }
        return true;
	}
	void prework() {//crt要求一次项系数为一
		for (CongruenceEquation* &temp : funs) {
			if (temp->keys[1] != 1) {
                vector<int> tem;
                tem.push_back(-1);
                tem.push_back(temp->keys[1]);
				CongruenceEquation tempc(tem, temp->mod);
				int tempx;
                tempc.resolve_deg1_dir_one(tempx);
				temp->keys[1] = 1;
				temp->keys[0] = -(-temp->keys[0] * tempx % temp->mod);
			}
		}
	}
	int resolve() {
		int sum = 1;//模的积
		prework();
		if (iscanberesolve(sum)) {
			int x = 0;
			for (CongruenceEquation* &i : funs) {
				int tempm = sum / i->mod;
                vector<int> tem;
                tem.push_back(-1);
                tem.push_back(tempm);
				CongruenceEquation tempc(tem,i->mod);
				int tempM;
                tempc.resolve_deg1_dir_one(tempM);
				x += tempm * tempM * (-(i->keys[0]));
			}
			return x%sum;
		}
	}

};