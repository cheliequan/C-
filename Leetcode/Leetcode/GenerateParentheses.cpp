#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
	bool find(vector<string> str,string s)
	{
		vector<string>::iterator iter;
		for(iter = str.begin();iter != str.end();iter++)
		{
			if(s.compare(iter->c_str()) == 0)
				return true;
		}
		return false;
	}
	//µ›πÈ«ÛΩ‚
	void generateP(int left,int right, int n,string s,vector<string> &str)
	{
		if(n < 1&&left == 0&&right == 0)
		{
			str.push_back("");
			return ;
		}
		if(left > right&&left < n)
		{
			generateP(left+1,right,n,s+"(",str);
			generateP(left,right+1,n,s+")",str);
		}
		else if(left == right&&left < n)
		{
			generateP(left+1,right,n,s+"(",str);
		}
		else if(left > right&&left == n)
		{
			generateP(left,right+1,n,s+")",str);
		}
		else if(left == n&&right == n)
		{
			if(!find(str,s))
			{
				str.push_back(s);
			}
		}
	}
    vector<string> generateParenthesis(int n) {
        /*vector<string> str;
		vector<string>::iterator iter;
		str.clear();
        if(n == 1)
        {
			str.push_back("()");
        }
		else
		{
			int i;
			::string ss,ss1;
			ss.clear();
			ss1.clear();
			for(i = 1;i < n;i++)
			{
				ss.append("(");
				ss1.append(")");
				::string s;
				vector<string> mi;
				mi.clear();
				mi = generateParenthesis(n-i);
				for(iter = mi.begin();iter != mi.end();iter++)
				{

					s.clear();
					s.append(ss+ss1);
					s.append(iter->c_str());
					if(!find(str,s))
					{
						str.push_back(s);
					}
					s.clear();
					s.append(ss);
					s.append(iter->c_str());
					s.append(ss1);
					if(!find(str,s))
					{
						str.push_back(s);
					}
					s.clear();
					s.append(iter->c_str());
					s.append(ss+ss1);
					if(!find(str,s))
					{
						str.push_back(s);
					}
				}
			}
		}*/
		vector<string> str;
		str.clear();
		int left = 0,right = 0;
		string s;
		s.clear();
		generateP(left,right,n,s,str);
		return str;
	}
	
};


//void main()
//{
//	Solution so;
//	
//	vector<string> str = so.generateParenthesis(6);
//	vector<string>::iterator iter;
//	for(iter = str.begin();iter != str.end();iter++)
//	{
//		printf("%s\n",iter->c_str());
//	}
//	printf("%d",str.size());
//	getchar();
//}