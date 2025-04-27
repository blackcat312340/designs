#include "headfiles.h"
#include "division.h"
#include "mod.h"
using namespace std;
//设a，m都是整数，m大于1，（a，m）=1，则使pow（a，e） 与1模m同余成立的最小正整数为e，则称a对模m的指数，指数也称为阶，ordm（a）
//注意如果（a，m）>1,则规定ordm（a）=0；
//默认m>1
//如果ordm（a）=m的欧拉函数值，则a叫做模m的原根
//若p与（p-1）/2都是奇素数，如果a是一个不被p整除的整数，且也不是模p的二次单位根，则ordp（a）=（p-1）/2或者p-1；
int get_ord(int m,int a) {
    a=a%m;
    int e=Euler(m);//所有阶都是模的欧拉函数值的因数
    vector<int> factors;
    GetFactors(e,factors,1);
    sort(factors.begin(),factors.end());
    vector<vector<int>> transfrombi;//转二进制
    for(int i:factors) {
        vector<int> temp;
        binumber(i,temp);
        transfrombi.push_back(temp);
    }
    for(int k=0;k<transfrombi.size();k++) {//这里为了防止大指数运算的困难性，用模复平方的思想解决
        int result=1;
        for(int i=transfrombi[k].size()-1;i>-1;i--) {
            if (i == 0) {
			    result *= int(pow(a, transfrombi[k][i]))%m;
			    break;
		    }
		    result *= int(pow(a, transfrombi[k][i]))%m;
		    result *= result;
            result =result%m;//模复平方每一步后加上取模
        }
        result =result%m;
        if(result==1) {
            return factors[k];
        }
    }
    return 0;
}
void get_exponent_table(int m) {
    vector<int> factors;
    unordered_map<int,int> result;
    factors.push_back(1);
    for(int i=2;i<m;i++) {
        vector<int> temp;
        GetCommonFactors(m,i,temp);
        if(temp.size()==1) {
            factors.push_back(i);
        }
    }//筛选互素的数
    for(int i:factors) {
        result[i]=get_ord(m,i);
    }
    cout<<setw(6)<<"a"<<setw(7)<<"orda"<<endl;
    for(int i:factors) {
        cout<<setw(6)<<i<<setw(7)<<result[i]<<endl;
    }
}
//定理三：若a，m互素，则pow(a,d),pow(a,k)模m同余的充分必要条件是d,k模ordm（a）同余
int cacucalte_on_exponent(int a,int q,int m) {//原根简化同余式的指数运算
    vector<int> temp;
    GetCommonFactors(a,m,temp);//要求a与m互素
    if(temp.size()>1) {
        cout<<"a and m should not have some factors"<<endl;
        exit(0);
    }
    int ord=get_ord(m,a);
    return int(pow(a,q%ord))%m;
}
//同余同阶，逆元同阶
//当a是模m的原根，则a的零次方到ordm（a）-1次方组成模m的简化剩余系
//定理四
int get_ord_on_exponent(int m,int a,int d) {
    vector<int> temp;
    GetCommonFactors(a,m,temp);//要求a与m互素
    if(temp.size()>1) {
        cout<<"a and m should not have some factors"<<endl;
        exit(0);
    }
    return get_ord(m,a)/GetMaxCommonFactor2(get_ord(m,a),d);
}
//推论：g是模m的原根。设d>=0为整数，则pow（g，d）是模m的原根，当且仅当(d,Euler(m))=1;
//原根个数：如果存在一个原根g，则模m有Euler(Euler(m))个不同的原根
void get_ind_table(int m) {
    vector<int> factors;
    factors.push_back(1);
    for(int i=2;i<m;i++) {
        vector<int> temp;
        GetCommonFactors(m,i,temp);
        if(temp.size()==1) {
            factors.push_back(i);
        }
    }//筛选互素的数
    int g=5;
    //for(int i:factors) {
        //if(get_ord(m,i)==Euler(m)) {
            //g=i;
            //break;
        //}
    //}
    //if(g==0) {
        //cout<<"there is not a g"<<endl; 
        //exit(0);
    //}
    sort(factors.begin(),factors.end());
    vector<vector<int>> transfrombi;//转二进制
    unordered_map<int,int> ind_table;
    for(int i:factors) {
        vector<int> temp;
        binumber(i,temp);
        transfrombi.push_back(temp);
    }
    vector<int> key;
    for(int k=0;k<transfrombi.size();k++) {//这里为了防止大指数运算的困难性，用模复平方的思想解决
        int result=1;
        for(int i=transfrombi[k].size()-1;i>-1;i--) {
            if (i == 0) {
			    result *= int(pow(g, transfrombi[k][i]))%m;
			    break;
		    }
		    result *= int(pow(g, transfrombi[k][i]))%m;
		    result *= result;
            result =result%m;//模复平方每一步后加上取模
        }
        result =result%m;
        key.push_back(result);
        ind_table[result]=factors[k];
    }
    sort(key.begin(),key.end());
    for(int i:key) {
        cout<<setw(6)<<i<<setw(6)<<ind_table[i]<<endl;
    }
}
