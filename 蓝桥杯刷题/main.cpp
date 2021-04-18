
//1:抽签   -- p2

/*O(n^4)*/
#include<cstdio>
int main(){
	int n, m, k[50];
	scanf_s("%d %d", &n, &m);
	for (int i = 0; i < n; i++){
		scanf_s("%d", &k[i]);
	}
	for (int a = 0; a < n; a++){
		for (int b = 0; b < n; b++){
			for (int c = 0; c < n; c++){
				for (int d = 0; d < n; d++){
					if (k[a] + k[b] + k[c] + k[d] == m){
						printf("Yes");
						return 0;
					}
				}
			}
		}
	}
	
	return 0;
}


/*O(n^3*logn)*/
/*排序算nlogn 循环3次*logn :用二分来简化最内存的数值获取  然后就是想办法：（k[c] + k[d])能不能提前优化 */
//#include <iostream>
//#include <algorithm>
//using namespace std;
//
//int n, m, k[50];
//bool binary(int x){
//	int left = 0, r = n - 1;
//	while (left < r){
//		int mid = (r - left) / 2 + left;
//		if (k[mid] < x){
//			left = mid + 1;
//		}
//		else{
//			r = mid;
//		}
//	}
//	return k[left] == x;
//}
//int main(){
//	
//	scanf_s("%d %d", &n, &m);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &k[i]);
//	}
//	sort(k, k + n);
//	for (int a = 0; a < n; a++){
//		for (int b = 0; b < n; b++){
//			for (int c = 0; c < n; c++){
//				if (binary(m - (k[a] + k[b] + k[c]) )){
//					printf("yes");
//					return 0;
//				}
//			}
//		}
//	}
//	
//	return 0;
//}


/*O(n^2*logn)*/
/*想办法得到任意两个数相加的和  java可以用map直接获取两数的和可以达到 n^2*/
//#include <iostream>
//#include <algorithm>
//using namespace std;
//
//const int max_n = 50;
//int n, m, k[max_n];
//int num[max_n*max_n];
//bool binary(int x){
//	int left = 0, r = n*n - 1;
//	while (left < r){
//		int mid = (r - left) / 2 + left;
//		if (num[mid] < x){
//			left = mid + 1;
//		}
//		else{
//			r = mid;
//		}
//	}
//	return num[left] == x;
//}
//int main(){
//	
//	scanf_s("%d %d", &n, &m);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &k[i]);
//	}
//	for (int a = 0; a < n; a++){
//		for (int b = 0; b < n; b++){
//			num[a*n + b] = k[a] + k[b];
//		}
//	}
//	sort(num, num + n*n);
//	for (int a = 0; a < n; a++){
//		for (int b = 0; b < n; b++){
//			if (binary(m - (k[a] + k[b]))){
//				printf("yes");
//				return 0;
//			}
//		}
//	}
//	
//	return 0;
//}


//2:三角形   -- p16
/*O^3*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 100;
//
//int main(){
//
//	int n, a[MAX_N];
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &a[i]);
//	}
//	sort(a, a + n);
//	for (int i = 0; i < n; i++){
//		for (int b = i + 1; b < n; b++){
//			for (int c = b + 1; c < n; c++){
//				if (a[i] + a[b]>a[c]){
//					printf("res:%d", a[i] + a[b] + a[c]);
//				}
//			}
//		}
//	}
//	return 0;
//}


/*O(n*logn): 当已经排序好了之后，相邻的比较就可以获得所有情况（max）,
比如：a<b<c<d 不会存在 a+b >c 然后漏掉a+b>d这种情况，因为要取最大。a+b>c那么c+b>d 所以不用考虑隔着的数据
*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//
//int main(){
//
//	int n, a[MAX_N];
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &a[i]);
//	}
//	sort(a, a + n);
//	int res = 0;
//	for (int i = 0; i < n-2; i++){
//		if (a[i] + a[i + 1] > a[i + 2]){
//			res = a[i] + a[i + 1] + a[i + 1];
//		}
//	}
//	printf("\n%d", res);
//	return res;
//}


//3:Ants   -- p18   
/*容易想到：最短时间是所有蚂蚁都朝向距离最短的一端；
  最长时间时需要考虑两只蚂蚁相遇后的情况，左A和右B相遇后，可以看做B仍然向左，A仍然向右，因为速度都相同，对结果并没有什么影响*/
//#include<stdio.h>
//#include<algorithm>
//
//using namespace std;
////const int MAX_N = 1000000;
//int x[1000001];
//int main(){
//	int n, L;
//	scanf_s("%d %d",&L, &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &x[i]);
//	}
//	int max = 0, min = 0;
//	for (int i = 0; i < n; i++){
//		int temp = x[i]>(L - x[i]) ? x[i] : L - x[i];
//		if (temp>max) max = temp;
//
//		temp = x[i]<(L - x[i]) ? x[i] : L - x[i];
//		if (temp>min) min = temp;//注意是要所有蚂蚁都掉下去，故取每个蚂蚁的最短时间 取其中最大
//	}
//	printf("max:%d min%d", max, min);
//	return 0;
//}


//4:部分和问题   -- p30   dfs
/*试着两种写法*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//int n, a[MAX_N];
//int k = 0;
//bool dfs(int i,int sum){
//	if (i == n) return sum == k;
//	//选
//	if (dfs(i + 1, sum + a[i]) ){
//		return true;
//	}
//	//不选
//	if (dfs(i + 1, sum)){
//		return true;
//	}
//		
//	return false;	
//}
//
//int main(){
//
//	
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &a[i]);
//	}
//	printf("\n");
//	scanf_s("%d", &k);
//	if (dfs(0, 0)){
//		printf("66666666666");
//	}
//	
//
//	return 0;
//}


//5:Lake Counting   -- p32  dfs
/*多练多练*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//int n;
//const int MAX_N = 1000;
//
//char a[MAX_N][MAX_N + 1];
//void dfs(int x, int y){
//	a[x][y] = '.';
//
//	for (int i = -1; i <= 1; i++){//遍历8个方向
//		for (int j = -1; j <= 1; j++){
//			int x1 = x + i, y1 = y + j;
//			if (x1 >= 0 && y1 >= 0 && x1 < 0 && y1 < n&&a[x1][y1] == 'W') dfs(x1, y1);
//		}
//	}
//}
//int main(){
//
//	
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &a[i]);
//	}
//
//	int res = 0;
//	for (int i = 0; i < n; i++){
//		for (int j = 0; j < n; j++){
//			if (a[i][j] == 'W'){
//				dfs(i, j);
//				res++;
//			}
//		}
//	}
//	printf("%d\n", res);
//	return 0;
//}


//6:迷宫最短路径  -- p34  
/*值得手操一遍*/
//#include<iostream>
//#include<algorithm>
//#include<queue>
//
//using namespace std;
//int n=10;
//int N=10, M=10;
//const int MAX_N = 100;
//const int INF = 1000;
//char  a[MAX_N][MAX_N + 1] = { 
//{ '#', 'S', '#', '#', '#', '#', '#', '#', '.' ,'#'},
//{ '.', '.', '.', '.', '.', '.', '#', '.', '.', '#' },
//{ '.', '#', '.', '#', '#', '.', '#', '#', '.', '#' },
//{ '.', '#', '.', '.', '.', '.', '.', '.', '.', '.' },
//{ '#', '#', '.', '#', '#', '.', '#', '#', '#', '#' },
//{'.', '.', '.', '.', '#', '.', '.', '.', '.', '#' },
//{ '#', '#', '#', '#', '#', '#', '#', '#', '.', '#' },
//{ '.', '.', '.', '.', '.', '.', '.', '.', '.', '.' },
//{ '.', '#', '#', '#', '#', '.', '#', '#', '#', '.' },
//{ '.', '.', '.', '.', '#', '.', '.', '.', 'G', '#' }
//};
//typedef pair<int, int> p;
//int fs=0, fe=1;
////学习一点定义四个方向的方法以及上一题8个方向的方法
//int dx[4] = { -1, 0, 0, 1 }, dy[4] = { 0, -1, 1, 0 };
//
//int dfs(){
//	int res = 0, count = 0;
//	int  dis[10][10];
//	for (int i = 0; i < M; i++){
//		for (int j = 0; j < N; j++){
//			dis[i][j] = INF;
//		}
//	}
//	queue<p> queque;
//	queque.push(p(fs, fe));
//	dis[fs][fe] = 0;
//	while (queque.size()){
//		p temp = queque.front(); queque.pop();
//		
//		for (int i = 0; i < 4; i++){
//			int x = dx[i] + temp.first, y = dy[i] + temp.second;
//			if (a[x][y] != '#'&&x >= 0 && y >= 0 && x < M&&y < N&&dis[x][y] == INF){//在边界内；没有被访问过；不是墙
//				queque.push(p(x, y));
//				dis[x][y] = dis[temp.first][temp.second] + 1;
//				if (x == 9 && y == 8) {
//					break;
//				}
//			}
//		}
//	}
//
//	return dis[9][8];
//}
//int main(){
//
//	//scanf_s("%d %d ", &M, &N);
//	printf(" %d  ", dfs());
//	
//	return 0;
//}


/*接下来是几个贪心问题*/
//7:硬币问题  -- p39
/*优先使用最好的，这里即先使用面值最大的;; 希望这种代码简单思路的代码能够信手拈来啊！！多练*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//int Co[6];
//int values[6] = { 1, 5, 10, 50, 100, 500 };
//
//int slove(int sum){
//	int res = 0;
//	for (int i = 5; i >= 0; i--){
//		int num = min(sum / values[i], Co[i]);//最多能取当前面额的硬币数
//		sum -= values[i] * num;
//		res += num;
//	}
//	return res;
//}
//int main(){
//
//	for (int i = 0; i < 6; i++){
//		scanf_s("%d", &Co[i]);
//	}
//	int A = 0;
//	scanf_s("%d", &A);
//	printf("\n%d", slove(A));
//	return 0;
//}



//8:区间调度问题-- p40
/*需要定义一个贪心的规则：不同贪心规则有不同结果；
1.每次选取最早开始的
2.每次选取最早结束的			-- 只有这个算法是正确的
3.每次选取耗时最短的
4.每次选取重叠最少的

这道题还有个可以学习的点 就是如何 把<k,v>值排序，也就是两个有对应关系的数组同步排序******
*/
//#include<iostream>
//#include<algorithm>
//
//const int MAX_N = 100000;
//using namespace std;
//int n, s[MAX_N],t[MAX_N];
//pair<int, int> con[MAX_N];
//
//int solve(){
//	for (int i = 0; i < n; i++){
//		con[i].first = t[i];
//		con[i].second = s[i];
//	}
//	sort(con, con + n);
//	int res = 0, start = 0;
//	for (int i = 0; i < n; i++){
//		if (start <= con[i].second){
//			start = con[i].first;
//			res++;
//		}
//	}
//	
//	return res;
//}
//int main(){
//
//
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &s[i]);
//	}
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &t[i]);
//	}
//	printf("结果是：%d\n", solve());
//
//	return 0;
//}



//9:Best Cow Line -- p43
/*贪心算法：注意一点：当首尾的字符相同时，需要需要比较*/
//#include<iostream>
//#include<algorithm>
//#include<string>
//using namespace std;
//const int MAX_N = 1000;
//int n;
//char s[MAX_N] = "";
//
//
//void solve(){
//	int a = 0, b = n - 1;
//	while (a <= b){
//		bool flag = false;
//		for (int i = 0; a + i < b; i++){//从a开始，向后遍历；或者使用题解上说的比较反转后的字符串
//			if (s[a + i] < s[b - i]){
//				flag = true; break;
//			}
//			else if (s[a + i] > s[b - i]){
//				flag = false; break;
//			}
//		}
//		if (flag){
//			putchar(s[a++]);
//		}
//		else{
//			putchar(s[b--]);
//		}
//	}
//}
//int main(){
//	
//	scanf_s("%d", &n);
//	cin >> s;
//	solve();
//
//	getchar();
//	return 0;
//}



//10:Saurman's Army -- p45
/*贪心算法/算法还是得多练习，久了不练就会手生，即使有了思路也很难写- . - */
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//int n, x[MAX_N];
//int R = 0;
//
//
//void solve(){
//	//注意：输入不一定是有序的e
//	sort(x, x + n);
//	int res = 0;
//	int index = 0,a=0;
//	while (a < n){
//		index = a;
//		while (a<n &&x[a] <= x[index] + R){
//			a++;
//		}
//		index = a-1;
//		while (a<n&&x[a] <= x[index] + R){
//			a++;
//		}
//		res++;
//	}
//	cout << res;
//}
//int main(){
//	scanf_s("%d", &n);
//	scanf_s("%d", &R);
//
//
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &x[i]);
//	}
//	solve();
//	return 0;
//}


//11:Fence Repair -- p45
/*贪心算法:基本的贪心思路还是和自己想的一样的，但是答案有更优的解，但是慢慢来吧；
	本题需要注意的是：合并了最小的两个之后的新木板要放回原木板数组，并去除这俩长度的木板；
		后面还要一个更好的解法；算了就一起写了吧；！
*/
//#include<iostream>
//#include<algorithm>
//#include<math.h>
//using namespace std;
//const int MAX_N = 1000;
//int n, l[MAX_N];
//
//
//void solve(){
//	int res = 0;
//	while (n > 1){
//		int min1=0, min2 = 1;
//		if (l[min1] > l[min2])	swap(min1, min2);
//		for (int i = 2; i < n; i++){
//			if (l[i] < l[min1]){
//				min2 = min1;
//				min1 = i;
//			}
//			else if (l[i]<l[min2]){
//				min2 = i;
//			}
//		}
//
//		int sum = l[min1] + l[min2];
//		res += sum;
//		//注意/.....感觉没有也不影响啊
//		//if (min1 == n - 1) swap(min1, min2);
//		l[min1] = sum;
//		l[min2] = l[n - 1];
//		n--;
//	}
//	cout << res << endl;
//}
//int main(){
//
//
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &l[i]);
//	}
//	solve();
//	return 0;
//}


//12:Fence Repair -- p76
/*
	优先队列的方式，每次取最小的俩个，然后再放入粘和后的；
	同样，可以先排序，取出两个数后再把合成的板加上一起排序；这俩个都是维持有序，但是后者是nlogn来维持有序
*/

//#include<iostream>
//#include<algorithm>
//#include<queue>
//
//using namespace std;
//const int MAX_N = 1000;
//long long int n, L[MAX_N];
//
//struct cmp
//{
//	bool operator () (long long int &a, long long int &b)
//	{
//		return a > b;///从小到大  如果想要从大到小 改成 '<'  
//	}
//};
//int main(){
//	//定义从小到大的队列；因为队列默认是大根堆
//	priority_queue<long long int, vector<long long int>, cmp > queue;
//	
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &L[i]);
//		queue.push(L[i]);
//	}
//	int res=0;
//	while (queue.size()>1){
//		int num1 = queue.top();
//		queue.pop();
//		int num2 = queue.top();
//		queue.pop();
//		int sum = num1 + num2;
//		res += sum;
//		queue.push(sum);
//	}
//	cout << res << endl;
//	return 0;
//}




//2.4:Expedition	-- p74
/*
	本题需要注意是：原思路：只有在加油站才可以加油；
					new思路：每经过一个加油站可以认为，获得了在该加油站加油的权利；当没有油时就可以选择其中最大优的一个来加油！！！！
	经过一个站，放入B[I];
	当油不够时，弹出队列加油直到油够为止；
*/
//#include<iostream>
//#include<algorithm>
//#include<queue>
//using namespace std;
//const int MAX_N = 1000;
//int n, a[MAX_N],b[MAX_N],l,p;
//
//
//int solve(){
//	//这里认为最后一个加油站也是终点
//	a[n] = l;
//	b[n] = 0;
//	n++;
//	//----------
//	priority_queue<int>  queue;
//	//pos当前为止 res 结果  tank 油箱中的油量
//	int pos = 0, res = 0, tank = p;
//	for (int i = 0; i < n; i++){
//		int next_dis = a[i] - pos;
//		while (tank < next_dis){
//			if (queue.size() < 1){
//				cout << "1231312" << endl;
//				return -1;
//			}
//			tank += queue.top();
//			queue.pop();
//			res++;
//		}
//		//计算油耗
//		tank -= next_dis;
//		//更新位置
//		pos = a[i];
//		//加油站入队
//		queue.push(b[i]);
//	}
//	//所有加油站都加过油了
//	/*if (pos + tank < l){
//		return -1;
//	}*/
//	return res;
//}
//int main(){
//
//	
//	scanf_s("%d %d %d", &n,&l,&p);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &a[i]);
//	}
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &b[i]);
//	}
//
//	cout << solve() << endl;
//	return 0;
//}



//练习set 和 Map
//#include<iostream>
//#include<algorithm>
//#include<set>
//#include<map>
//
//using namespace std;
//
//int main(){
//	set<int> s;
//	for (int i = 0; i < 10; i++){
//		s.insert(i);
//	}
//	set<int>::iterator  ite;
//	ite=s.find(10);
//	if (s.end() != ite) cout << "get" << endl;
//	else cout << "not get " << endl;
//	if (s.count(3) != 0) cout << "get" << endl;
//	else cout << "not get " << endl;
//
//
//	for (ite = s.begin(); ite != s.end(); ++ite){
//		cout << *ite << " ";
//	}
//	return 0;
//}


/*并查集的实现和 做题应用*/

//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//
//const int MAX_N = 1000;
//
//int par[MAX_N];//父节点
//int ranks[MAX_N];
////i节点的高度
////当par[i]=i时，i是所在树的根；
//
////初始化n个节点
//void init(int n){
//	for (int i = 0; i < n; i++){
//		par[i] = i;
//		ranks[i] = 0;
//	}
//}
//
////查找某个值的根节点
//int find(int x){
//	if (par[x] == x){
//		return x;
//	}
//	return par[x] = find(par[x]);
//}
//
//bool same(int x, int y){
//	return find(x) == find(y);
//}
////合并x和y为一个集合：合并规则，高度低的一边接在另一个树是哪个
//void unite(int x, int y){
//	int ix = find(x);
//	int iy = find(y);
//
//	if (ix == iy) return; //根节点相同，在同一颗树上
//	//x所在的树高度较低
//	if (ranks[ix] < ranks[iy]){
//		par[ix] = iy;
//	}
//	else{
//		par[iy] = ix;
//		//高度相同；
//		if (ranks[ix] == ranks[iy]) ranks[ix]++;//是想加的节点的高度++;
//	}
//}
////int main(){
////
////	init(10);
////	unite(1, 5);
////	unite(2, 5);
////	unite(3, 4);
////	unite(3, 2);
////	return 0;
////}
//

//13:食物链	-- p88
/*并查集是维护"同一数组"（同一类元素）的数据结构  并查集牛逼。*/
/* 用例
100 7
1 2 2 2 1 2 1
101 1 2 3 1 3 5
1 2 3 3 3 1 5
*/
//
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//int n,k, m[MAX_N],X[MAX_N],Y[MAX_N];
//
//
//int solve(){
//	init(n*3);
//	int res = 0;
//	for (int i = 0; i < k; i++){
//		int t = m[i],x = X[i],y = Y[i];
//		if (x < 0 || x >= n || y < 0 || y >= n) {
//			res++; 
//			continue;
//		}
//		if (t == 1){//第一种信息
//			//属于同一类:
//			if (same(x, y + n) || same(x, y + 2 * n)){//不在同一个种类中
//				res++; 
//			}
//			else{
//				unite(x, y);
//				unite(x + n, y + n);
//				unite(x + 2 * n, y + 2 * n);
//			}
//		}
//		else{
//			//捕食关系
//			if (same(x, y) || same(x, y + 2 * n)){//是同一个类或者是天敌
//				res++; 
//			}
//			else{
//				unite(x, y + n);
//				unite(x + n, y + 2 * n);
//				unite(x + 2 * n, y);
//			}
//		}
//	}
//	return res;
//}
//int main(){
//
//
//	scanf_s("%d %d ", &n ,&k);
//	for (int i = 0; i < k; i++){
//		scanf_s("%d", &m[i]);
//	}
//	for (int i = 0; i < k; i++){
//		scanf_s("%d", &X[i]);
//	}
//	for (int i = 0; i < k; i++){
//		scanf_s("%d", &Y[i]);
//	}
//
//	cout << solve();
//	return 0;
//}


//14:线段上格点的个数	-- p113
/*数学问题*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//int L = 0, M = 0;
//const int max = 100;
//int start[100]; int ends[100];
//int tree[100];
//void init(int n){
//	for (int i = 0; i<n; i++){
//		tree[i] = 1;
//	}
//}
//void deset(int x, int y){
//	for (int i = x; i <= y; i++){
//		tree[i] = 0;
//	}
//}
//int count(){
//	int nums = 0;
//	for (int i = 0; i<L; i++){
//		if (tree[i] == 1) nums++;
//	}
//	printf("sss:%d",nums);
//	return nums;
//}
//int main(){
//	scanf_s("%d %d", &L, &M);
//	init(L);
//	for (int i = 0; i<M; i++){
//		int temp1 = 0, temp2 = 0;
//		scanf_s("%d %d", &temp1, &temp2);
//		deset(temp1, temp2);
//	}
//	printf("%d", count());
//}



//3.1.2:Cable master   --p140
/*		 21年4.4日 和学皇在acw做了道二分，然后练习二分的题
 本题和《分巧克力》类似
用例：
4 11
8.02 7.43 4.57 5.39
*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//int n, k;double l[MAX_N];
//
//int getInt(int size){
//	int nums = 0;
//	for (int i = 0; i < n; i++ ){
//		nums += l[i] / size;
//		if (nums>k){
//			return nums;
//		}
//		else if (nums == k) break;
//	}
//	return nums;
//}
//void solve(){
//	double l = 0, r = 100000;
//	while (l < r - (1e-4)){
//		int mid = (l + r) / 2;
//		if (getInt(mid) < k) r = mid;
//		else l = mid + 1;
//	}
//	cout << l - 1;
//}
//int main(){
//
//
//	cin >> n >> k;
//	for (int i = 0; i < n; i++){
//		cin >> l[i];
//	}
//	sort(l, l+n);
//	solve();
//
//
//	return 0;
//}


//2.5.3:二分图		--p97
/*		 21年4.8日 几天没做题了，在看并发-.-
是用深搜做的，和这篇的知识无关啊。
用例：
4 11
8.02 7.43 4.57 5.39
*/
//#include<iostream>
//#include<algorithm>
//#include<vector>
//using namespace std;
//const int MAX_N = 1000;
//int v, color[MAX_N];//v是定点数  color定点i的颜色
//vector<int> G[MAX_N];//定点i
//
//bool dfs(int x,int c){
//	//设置该 点已经访问过
//	color[x] = c;
//	//遍历该点周围
//	for (int i = 0; i < G[x].size(); i++){
//		int node = G[x][i];
//		if (color[node] == 0 && !dfs(node,-c)){
//			return false;
//		}
//		if (color[node] == c){
//			return false;
//		}
//	}
//	return true;
//}
//bool solve(){
//
//	for (int i = 0; i < v; i++){
//		if (color[i] != 0){//没有被访问过
//			if (!dfs(i,1)){
//				return false;
//			}
//		}
//	}
//	return true;
//}
//int main(){
//
//	
//	//scanf_s("%d", &v);
//	v = 4;
//	G[0].push_back(1);
//	G[0].push_back(3);
//	G[1].push_back(0);
//	G[1].push_back(2);
//	G[2].push_back(1);
//	G[2].push_back(3);
//	G[3].push_back(0);
//	G[3].push_back(2);
//
//	if (solve())
//		cout << "yes" << endl;
//	return 0;
//}


//几种求最短路径的方法		--p100

/*Bellman-Ford算法：单源最短路问题，求它到其他所有点的最短路的问题
复杂度：O(V*E)
*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//int n, a[MAX_N];
////从顶点from指向顶点to的权值为cost的边
//struct edge{ int from, to, cost; };
//
//edge es[MAX_N];
//int  d[MAX_N];//最短距离
//int V, E;//V是顶点，E是边数
//int INF = 10000;
////从顶点s出发到所有点的最短距离
//void shortest_path(int s){
//	for (int i = 0; i < V; i++) d[i] = INF;
//	d[s] = 0;
//	while (true){
//		bool flag = false;
//		for (int i = 0; i < E; i++){//遍历所有边
//			edge e = es[i];
//			if (d[e.from] != INF&&d[e.from] + d[e.cost] < d[e.to]){
//				d[e.to] = d[e.from] + d[e.cost];
//				flag = true;
//			}
//		}
//		if (!flag) return; //表示所有边都到达过了;
//	}
//	
//}
//
////返回true则存在负圈
//bool find_negative_loop(){
//	memset(d, 0, sizeof(d));
//
//	for (int i = 0; i < V; i++){
//		for (int j = 0; j < E; i++){
//			edge e = es[j];
//			if (d[e.to]>d[e.from] + e.cost){
//				d[e.to] = d[e.from] + e.cost;
//				//第V次仍然更新了就说明存在负圈；
//				if (V == i + 1) return true;
//			}
//		}
//	}
//	return false;
//}
//int main(){
//
//
//	scanf_s("%d", &n);
//	for (int i = 0; i < n; i++){
//		scanf_s("%d", &a[i]);
//	}
//
//	return 0;
//}

