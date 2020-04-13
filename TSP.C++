#include "bits/stdc++.h"
using namespace std;

typedef pair<int,int> pr;

vector<pr> Points;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count()); //随机种子

inline double Dis(pr a, pr b) { //计算两点之间距离
    return sqrt((a.first-b.first)*(a.first-b.first)+(a.second-b.second)*(a.second-b.second));
}

struct Chromosome{ //染色体
    int n;
    vector<int> a;
    Chromosome(int V=0): n(V) { //随机初始化
        a.resize(V);
        for(int i=0; i<n; ++i) a[i]=i;
        shuffle(a.begin(),a.end(),rng);
    }
    int & operator [] (int i) { return a[i]; }
    double Path_length() { //计算当前状态的长度
        double s=0;
        for(int i=0; i<n; ++i) s+=Dis(Points[a[i]],Points[(a[i]+1)%n]);
        return s;
    }
    friend void Cross(Chromosome &a, Chromosome &b) { //染色体交叉，采用上课时讲的部分映射交叉法
        int n=a.n;
        int l=rng()%n, r=rng()%n;
        if(l>r) swap(l,r);
        vector<int> s1, s2;
        for(int i=l; i<=r; ++i) {
            s1.push_back(b[i]);
            s2.push_back(a[i]);
        }
        for(int i=0; i<r-l+1; ++i) {
            for(int j=0; j<n; ++j) if(a[j]==s1[i]) { swap(a[l+i],a[j]); break; }
            for(int j=0; j<n; ++j) if(b[j]==s2[i]) { swap(b[l+i],b[j]); break; }
        }
    }
    friend void Mutation(Chromosome &a) { //发生变异，采用上课时讲的互换变异法
        int n=a.n;
        int l=rng()%n, r=rng()%n;
        if(r==l) r=(r+1)%n; //防止变异失败
        swap(a[l],a[r]);
    }
};

int main() {

}