#include <cstdio>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <chrono>
#include <cmath>
#include <vector>
#include <random>
#include <gl/glut.h>
#include <windows.h>
using namespace std;

typedef pair<int,int> pr;

const int Size = 30;

vector<pr> P;

int n; //城市数量

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count()); //随机种子

inline double Dis(pr a, pr b) { //计算两点之间距离
    return sqrt((a.first-b.first)*(a.first-b.first)+(a.second-b.second)*(a.second-b.second));
}

struct Chromosome{ //染色体
    vector<int> a;
    Chromosome() { //随机初始化
        a.resize(n);
        for(int i=0; i<n; ++i) a[i]=i;
        shuffle(a.begin(),a.end(),rng);
    }
    int & operator [] (int i) { return a[i]; }
    double Path_length() const { //计算当前状态的长度
        double s=0;
        for(int i=0; i<n; ++i) s+=Dis(P[a[i]],P[(a[(i+1)%n])]);
        return s;
    }
	friend bool operator < (const Chromosome &a, const Chromosome &b) {
		return a.Path_length()<b.Path_length();
	}
	friend bool operator == (const Chromosome &a, const Chromosome &b) {
		return a.a==b.a;
	}
    friend void Cross(Chromosome &a, Chromosome &b) { //染色体交叉，采用上课时讲的部分映射交叉法
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
        int l=rng()%n, r=rng()%n; //允许变异失败
        swap(a[l],a[r]);
    }
};

void init() {
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION); //设置投影矩阵
	gluOrtho2D(0.0,600.0,0.0,600.0); //二维视景区域
	glPointSize(5.0); //一个点的大小（这里的点不是像素点）
}

void playPoints() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_POINTS);
	for(int i=0; i<int(P.size()); ++i) {
		glVertex2d(P[i].first,P[i].second);
	}
	glEnd();
	glFlush();
}

void playLines(Chromosome &C) {
	playPoints();
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINES);
	for(int i=0; i<n; ++i) {
		glVertex2d(P[C[i]].first,P[C[i]].second);
		glVertex2d(P[C[(i+1)%n]].first,P[C[(i+1)%n]].second);
	}
	glEnd();
	glFlush();
}

void Generate(vector<Chromosome> &C, vector<int> &P) { //繁殖一次
	int pos1=rng()%P[Size-1], pos2=rng()%P[Size-1];
	Chromosome a, b;
	for(int i=0; i<Size; ++i) {
		if(pos1<P[i]) { a=C[i]; break; }
	}
	for(int i=0; i<Size; ++i) {
		if(pos2<P[i]) { b=C[i]; break; }
	}
	Cross(a,b); //先交叉一下
	Mutation(a); //然后再各自变异一下
	Mutation(b);
	C.push_back(a); //放到种群里面
	C.push_back(b);
}

void GA() {
	vector<Chromosome> C(Size);
	sort(C.begin(),C.end());
	playLines(C[0]); //查看当前最优性状
	for(int t=0; t<30; ++t) { //繁殖50代
		vector<int> P(Size);
		for(int i=0; i<Size; ++i) {
			P[i]=int(1/C[i].Path_length()*1000000);
			if(i) P[i]+=P[i-1];
		}
		for(int i=0; i<1000; ++i) Generate(C,P); //每一代繁殖1000次
		//for(int i=0; i<30; ++i) printf("%.2f%c", C[i].Path_length(), " \n"[i==29]);
		sort(C.begin(),C.end());
		unique(C.begin(),C.end()); //去掉相同性状
		while(C.size()>Size) C.pop_back(); //淘汰劣势
		playLines(C[0]); //查看当前最优性状
		printf("%d: %.2f\n", t+1, C[0].Path_length());
		//int cnt=0;
		//for(int i=0; i<Size-1; ++i) if(C[i]==C[i+1]) cnt++;
		//printf("cnt=%d\n", cnt);
	}
	printf("最短路径长度：%.2f\n", C[0].Path_length());
}

void mouse(GLint button, GLint action, GLint gx, GLint gy) { //定义鼠标的点击操作
	int x=gx;
	int y=600-gy;
	if(button==0 && action==0 && int(P.size())<n) { //左键放置城市
		P.push_back(pr(x,y));
		playPoints();
	}
	if(button==2 && action==0 && int(P.size())==n) { //右键开始执行GA算法
		GA();
		system("Pause");
	}
}

int main(int argc, char** argv) {
	printf("请输入城市数量啦："); cin>>n;
	printf("接下来请在二维平面中选择 %d 个城市\n鼠标左键选择城市，右键开始GA算法：\n", n);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowPosition(400,20);
	glutInitWindowSize(600,600);
	glutCreateWindow("TSP");

	init();
	glutDisplayFunc(playPoints);
	glutMouseFunc(mouse);
	glutMainLoop();
}
