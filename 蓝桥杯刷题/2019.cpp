//  5  8  7
//-5  2  7  8
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
#define ll long long
using namespace std;
int main()
{
    ios::sync_with_stdio(false);cin.tie(0);cout.tie(0);
    int n, m;
    ll t, sum = 0, min_num;

    vector<ll> v;
    cin >> n >> m;
    for (int i = 0; i < n + m + 1; i++)
    {
        cin >> t;
        v.push_back(t);
        sum += t;
    }
    sort(v.begin(), v.end(), [](ll a, ll b) { return a > b; });

    min_num = v[v.size() - 1];

    if (m == 0) //û�м���
    {
        cout << sum << endl;
    }
    else //�м���
    {
        if (min_num >= 0) //û�и���
        {
            cout << sum - 2 * min_num << endl;
        }
        else //�и���
        {
            sum = v[0];
            for (int i = 1; i < n + m + 1; i++)
                sum += fabs(v[i]);
            cout << sum << endl;
        }
    }
    //system("pause");
    return 0;
}
/*//2019_08  Ӧ��������d��С������ Ȼ�� an=a0+d��n-1��;
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
using namespace std;
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    int m;
    vector<int> v;
    cin >> m;
    for (int i = 0, t; i < m; i++)
    {
        cin >> t;
        v.push_back(t);
    }
    sort(v.begin(), v.end());

    int d = INT32_MAX;
    for (int i = 1; i < v.size(); i++)
    {
        int t = fabs(v[i] - v[i - 1]);
        d = min(d, t);
    }
    int k = v[m - 1] - v[0];
    cout << (k == 0 ? m : (k % d == 0 ? k / d + 1 : k + 1)) << endl;

    //system("pause");
    return 0;
}
*/

/*
2019_04
#include <iostream>
#include<string.h>
using namespace std;

int main()
{
	char str[100];//eg  ([)]
	cin >> str;
	int len = strlen(str);
	int status[100] = { 0 }; // ÿ���ַ���״̬��
	for (int i = 0; i<len; i++){
		if (str[i] == ')'){
			for (int j = i; j >= 0; j--){
				if (str[j] == '('&&status[j] == 0){//û�б�ƥ���
					status[i] = status[j] = 1; break;
				}
				else if (str[j] == '['&&status[j] == 0){//����[(])�������
					break;
				}
			}
		}
		else if (str[i] == ']'){
			for (int j = i; j >= 0; j--){
				if (str[j] == '['&&status[j] == 0){
					status[i] = status[j] = 1; break;
				}
				else if (str[j] == '('&&status[j] == 0){//����[(])�������
					break;
				}
			}
		}
	}
	for (int i = 0; i<len; i++){
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

*/

/*


#include<algorithm>
#include<iostream>
#include<vector>
#include<cstring>
using namespace std;

int judge(int x){//�ҵ�2����4�ͷ���1
    string str=to_string(x);
    if(str.find('2')!=string::npos||str.find('4')!= string::npos){
        return 1;
    }
    return 0;
}
int main(){
    int res=0;
    for(int i=1;i<2019/3;i++){//ֻ��˵����2��û˵������2�ı���
        if(judge(i)) continue;
        for(int j=i+1;j<2019;j++){//�ڶ�������i-2019
            if(judge(j)) continue;
            if(j>=2019-(i+j)) // 2019 -��i+j�����ڵ��������������ظ�   ע����ڷ���
                break;
            if(!judge(2019-(i+j))){//����ж��£�����������������
                res++;
            }
        }
    }
    cout<<res;
    //cout<<"1123"<<endl;
    return 0;
}


*/
