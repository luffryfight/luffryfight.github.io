
//1:��ǩ   -- p2

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
/*������nlogn ѭ��3��*logn :�ö����������ڴ����ֵ��ȡ  Ȼ�������취����k[c] + k[d])�ܲ�����ǰ�Ż� */
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
/*��취�õ�������������ӵĺ�  java������mapֱ�ӻ�ȡ�����ĺͿ��Դﵽ n^2*/
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


//2:������   -- p16
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


/*O(n*logn): ���Ѿ��������֮�����ڵıȽϾͿ��Ի�����������max��,
���磺a<b<c<d ������� a+b >c Ȼ��©��a+b>d�����������ΪҪȡ���a+b>c��ôc+b>d ���Բ��ÿ��Ǹ��ŵ�����
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
/*�����뵽�����ʱ�����������϶����������̵�һ�ˣ�
  �ʱ��ʱ��Ҫ������ֻ������������������A����B�����󣬿��Կ���B��Ȼ����A��Ȼ���ң���Ϊ�ٶȶ���ͬ���Խ����û��ʲôӰ��*/
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
//		if (temp>min) min = temp;//ע����Ҫ�������϶�����ȥ����ȡÿ�����ϵ����ʱ�� ȡ�������
//	}
//	printf("max:%d min%d", max, min);
//	return 0;
//}


//4:���ֺ�����   -- p30   dfs
/*��������д��*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//int n, a[MAX_N];
//int k = 0;
//bool dfs(int i,int sum){
//	if (i == n) return sum == k;
//	//ѡ
//	if (dfs(i + 1, sum + a[i]) ){
//		return true;
//	}
//	//��ѡ
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
/*��������*/
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
//	for (int i = -1; i <= 1; i++){//����8������
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


//6:�Թ����·��  -- p34  
/*ֵ���ֲ�һ��*/
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
////ѧϰһ�㶨���ĸ�����ķ����Լ���һ��8������ķ���
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
//			if (a[x][y] != '#'&&x >= 0 && y >= 0 && x < M&&y < N&&dis[x][y] == INF){//�ڱ߽��ڣ�û�б����ʹ�������ǽ
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


/*�������Ǽ���̰������*/
//7:Ӳ������  -- p39
/*����ʹ����õģ����Ｔ��ʹ����ֵ����;; ϣ�����ִ����˼·�Ĵ����ܹ�������������������*/
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
//		int num = min(sum / values[i], Co[i]);//�����ȡ��ǰ����Ӳ����
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



//8:�����������-- p40
/*��Ҫ����һ��̰�ĵĹ��򣺲�̰ͬ�Ĺ����в�ͬ�����
1.ÿ��ѡȡ���翪ʼ��
2.ÿ��ѡȡ���������			-- ֻ������㷨����ȷ��
3.ÿ��ѡȡ��ʱ��̵�
4.ÿ��ѡȡ�ص����ٵ�

����⻹�и�����ѧϰ�ĵ� ������� ��<k,v>ֵ����Ҳ���������ж�Ӧ��ϵ������ͬ������******
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
//	printf("����ǣ�%d\n", solve());
//
//	return 0;
//}



//9:Best Cow Line -- p43
/*̰���㷨��ע��һ�㣺����β���ַ���ͬʱ����Ҫ��Ҫ�Ƚ�*/
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
//		for (int i = 0; a + i < b; i++){//��a��ʼ��������������ʹ�������˵�ıȽϷ�ת����ַ���
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
/*̰���㷨/�㷨���ǵö���ϰ�����˲����ͻ���������ʹ����˼·Ҳ����д- . - */
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
//	//ע�⣺���벻һ���������e
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
/*̰���㷨:������̰��˼·���Ǻ��Լ����һ���ģ����Ǵ��и��ŵĽ⣬�����������ɣ�
	������Ҫע����ǣ��ϲ�����С������֮�����ľ��Ҫ�Ż�ԭľ�����飬��ȥ���������ȵ�ľ�壻
		���滹Ҫһ�����õĽⷨ�����˾�һ��д�˰ɣ���
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
//		//ע��/.....�о�û��Ҳ��Ӱ�찡
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
	���ȶ��еķ�ʽ��ÿ��ȡ��С��������Ȼ���ٷ���ճ�ͺ�ģ�
	ͬ��������������ȡ�����������ٰѺϳɵİ����һ����������������ά�����򣬵��Ǻ�����nlogn��ά������
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
//		return a > b;///��С����  �����Ҫ�Ӵ�С �ĳ� '<'  
//	}
//};
//int main(){
//	//�����С����Ķ��У���Ϊ����Ĭ���Ǵ����
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
	������Ҫע���ǣ�ԭ˼·��ֻ���ڼ���վ�ſ��Լ��ͣ�
					new˼·��ÿ����һ������վ������Ϊ��������ڸü���վ���͵�Ȩ������û����ʱ�Ϳ���ѡ����������ŵ�һ�������ͣ�������
	����һ��վ������B[I];
	���Ͳ���ʱ���������м���ֱ���͹�Ϊֹ��
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
//	//������Ϊ���һ������վҲ���յ�
//	a[n] = l;
//	b[n] = 0;
//	n++;
//	//----------
//	priority_queue<int>  queue;
//	//pos��ǰΪֹ res ���  tank �����е�����
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
//		//�����ͺ�
//		tank -= next_dis;
//		//����λ��
//		pos = a[i];
//		//����վ���
//		queue.push(b[i]);
//	}
//	//���м���վ���ӹ�����
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



//��ϰset �� Map
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


/*���鼯��ʵ�ֺ� ����Ӧ��*/

//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//
//const int MAX_N = 1000;
//
//int par[MAX_N];//���ڵ�
//int ranks[MAX_N];
////i�ڵ�ĸ߶�
////��par[i]=iʱ��i���������ĸ���
//
////��ʼ��n���ڵ�
//void init(int n){
//	for (int i = 0; i < n; i++){
//		par[i] = i;
//		ranks[i] = 0;
//	}
//}
//
////����ĳ��ֵ�ĸ��ڵ�
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
////�ϲ�x��yΪһ�����ϣ��ϲ����򣬸߶ȵ͵�һ�߽�����һ�������ĸ�
//void unite(int x, int y){
//	int ix = find(x);
//	int iy = find(y);
//
//	if (ix == iy) return; //���ڵ���ͬ����ͬһ������
//	//x���ڵ����߶Ƚϵ�
//	if (ranks[ix] < ranks[iy]){
//		par[ix] = iy;
//	}
//	else{
//		par[iy] = ix;
//		//�߶���ͬ��
//		if (ranks[ix] == ranks[iy]) ranks[ix]++;//����ӵĽڵ�ĸ߶�++;
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

//13:ʳ����	-- p88
/*���鼯��ά��"ͬһ����"��ͬһ��Ԫ�أ������ݽṹ  ���鼯ţ�ơ�*/
/* ����
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
//		if (t == 1){//��һ����Ϣ
//			//����ͬһ��:
//			if (same(x, y + n) || same(x, y + 2 * n)){//����ͬһ��������
//				res++; 
//			}
//			else{
//				unite(x, y);
//				unite(x + n, y + n);
//				unite(x + 2 * n, y + 2 * n);
//			}
//		}
//		else{
//			//��ʳ��ϵ
//			if (same(x, y) || same(x, y + 2 * n)){//��ͬһ������������
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


//14:�߶��ϸ��ĸ���	-- p113
/*��ѧ����*/
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
/*		 21��4.4�� ��ѧ����acw���˵����֣�Ȼ����ϰ���ֵ���
 ����͡����ɿ���������
������
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


//2.5.3:����ͼ		--p97
/*		 21��4.8�� ����û�����ˣ��ڿ�����-.-
�����������ģ�����ƪ��֪ʶ�޹ذ���
������
4 11
8.02 7.43 4.57 5.39
*/
//#include<iostream>
//#include<algorithm>
//#include<vector>
//using namespace std;
//const int MAX_N = 1000;
//int v, color[MAX_N];//v�Ƕ�����  color����i����ɫ
//vector<int> G[MAX_N];//����i
//
//bool dfs(int x,int c){
//	//���ø� ���Ѿ����ʹ�
//	color[x] = c;
//	//�����õ���Χ
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
//		if (color[i] != 0){//û�б����ʹ�
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


//���������·���ķ���		--p100

/*Bellman-Ford�㷨����Դ���·���⣬�������������е�����·������
���Ӷȣ�O(V*E)
*/
//#include<iostream>
//#include<algorithm>
//
//using namespace std;
//const int MAX_N = 1000;
//int n, a[MAX_N];
////�Ӷ���fromָ�򶥵�to��ȨֵΪcost�ı�
//struct edge{ int from, to, cost; };
//
//edge es[MAX_N];
//int  d[MAX_N];//��̾���
//int V, E;//V�Ƕ��㣬E�Ǳ���
//int INF = 10000;
////�Ӷ���s���������е����̾���
//void shortest_path(int s){
//	for (int i = 0; i < V; i++) d[i] = INF;
//	d[s] = 0;
//	while (true){
//		bool flag = false;
//		for (int i = 0; i < E; i++){//�������б�
//			edge e = es[i];
//			if (d[e.from] != INF&&d[e.from] + d[e.cost] < d[e.to]){
//				d[e.to] = d[e.from] + d[e.cost];
//				flag = true;
//			}
//		}
//		if (!flag) return; //��ʾ���б߶��������;
//	}
//	
//}
//
////����true����ڸ�Ȧ
//bool find_negative_loop(){
//	memset(d, 0, sizeof(d));
//
//	for (int i = 0; i < V; i++){
//		for (int j = 0; j < E; i++){
//			edge e = es[j];
//			if (d[e.to]>d[e.from] + e.cost){
//				d[e.to] = d[e.from] + e.cost;
//				//��V����Ȼ�����˾�˵�����ڸ�Ȧ��
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

