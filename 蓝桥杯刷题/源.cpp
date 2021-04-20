//#include<stdio.h>
//#include<stdlib.h>
//#include<conio.h>
//#define maxsize 20
//#define datatype char
//typedef struct
//{
//	datatype data[maxsize];
//	int top;
//}SeqStack;
//void InitStack(SeqStack*s)
//{
//	s->top = -1;
//}
//int Push(SeqStack*s, datatype x)
//{
//	if (s->top == maxsize - 1)
//		return 0;
//	s->data[++s->top] = x;
//	return 1;
//}
//int Pop(SeqStack*s, datatype *x)
//{
//	if (s->top == -1)
//		return 0;
//	*x = s->data[s->top--];
//	return 1;
//}
//int GetTop(SeqStack*s, datatype *x)
//{
//	if (s->top == -1)
//		return 0;
//	*x = s->data[s->top];
//	return 1;
//}
//char menu(void)
//{
//	char ch;
//	system("cls");
//	printf("\n"); printf("\n");
//	printf("           顺序栈操作     \n");
//	printf("       ====================\n");
//	printf("           请选择\n");
//	printf("           1.入栈\n");
//	printf("           2.出栈\n");
//	printf("           3.取栈顶元素\n");
//	printf("           0.退出\n");
//	printf("      ====================\n");
//	printf("       选择（0,1,2,3):");
//	ch = getchar();
//	return(ch);
//}
//int main()
//{
//	SeqStack st;
//	int flag = 1, k;
//	datatype x;
//	char choice;
//	InitStack(&st);
//	do{
//		choice = menu();
//		switch (choice)
//		{
//		case '1':
//			printf("请输入入栈数据=?");
//			scanf_s("%d", &x);
//			k = Push(&st, x);
//			if (k) printf("入栈结束.");
//			else printf("栈为空");
//			_getch();
//			break;
//		case'2':
//			k = Pop(&st, &x);
//			if (k) printf("出栈数据=%d\n", x);
//			else printf("栈为空");
//			_getch();
//			break;
//		case'3':
//			k = GetTop(&st, &x);
//			if (k) printf("栈顶元素=%d\n", x);
//			else printf("栈为空");
//			_getch();
//			break;
//		case'0':flag = 0; break;
//		}
//	} while (flag == 1);
//}

/*
		试题 J: 字串排序
*/
//#include <iostream>
//using namespace std;
//const int N = 1e4 + 10;
//char ans[N], res[N];
//int n, len;
//bool judge()
//{
//	int i = len;
//	while (ans[i] == res[i] && i) i--;
//	return res[i] < ans[i];
//}
//void dfs(int now, int maxn, int m, int sum) {
//	if (sum == n)
//	{
//		if (m < len || (m == len && judge()))
//		{
//			len = m;
//			for (int i = 1; i <= len; i++) 	ans[i] = res[i];
//		}
//		return;
//	}
//	if (now >= 26) return;
//	for (int i = 1; i <= maxn; i++)
//	{
//		int temp = sum + m * i;
//		if (temp > n) return;
//		res[m + i] = char(now + 'a');
//		dfs(now + 1, i, m + i, temp);
//	}
//}
//
//int main()
//{
//	len = 0x3f3f3f3f;
//	cout << len;
//	scanf_s("%d", &n);
//	dfs(0, 8, 0, 0);
//	for (int i = len; i >= 1; i--)
//		putchar(ans[i]);
//	return 0;
//}


#include <iostream>
#include<stack>
#include<string.h>
using namespace std;

int main()
{
	char str[100];//eg  ([)]
	cin >> str;
	int len = strlen(str);
	int status[100] = { 0 }; // 每个字符的状态；
	for (int i = 0; i<len; i++){
		if (str[i] == ')'){
			for (int j = i; j >= 0; j--){
				if (str[j] == '('&&status[j] == 0){//没有被匹配过
					status[i] = status[j] = 1; break;
				}
				else if (str[j] == '['&&status[j] == 0){//存在[(])这种情况
					break;
				}
			}
		}
		else if (str[i] == ']'){
			for (int j = i; j >= 0; j--){
				if (str[j] == '['&&status[j] == 0){
					status[i] = status[j] = 1; break;
				}
				else if (str[j] == '('&&status[j] == 0){//存在[(])这种情况
					break;
				}
			}
		}
	}
	for (int i = 0; i<len; i++){
		char ch = 0;
		if (status[i] == 0){
			if (str[i] == '(' || str[i] == ')'){
				cout << "()";
			}
			else
				cout << "[]";
		}
		else cout << str[i];
	}
	return 0;
}
