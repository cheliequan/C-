#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Solution {
public:

	void letter(string digits,string s,vector<string> &str)
	{
		char data[9][5] = {"","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
		if(digits.size() == 0)
		{
			str.push_back(s);
			return ;
		}
		char t = digits.at(0);
		for(int i = 0;data[t-'1'][i]!= '\0';i++)
		{
			letter(digits.substr(1,digits.size()-1),s + data[t-'1'][i],str);
		}
	}
    vector<string> letterCombinations(string digits) {
		vector<string> str;
		str.clear();
		char data[9][5] = {"","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
		if(digits.size() <= 0)
		{
			//str.push_back("");
			return str;
		}	
		else
		{
			string s;
			s.clear();
			string ss = digits.substr(1,digits.size()-1);
			printf("%s\n",ss.c_str());
			letter(digits,s,str);
			return str;
		}
    }
};


//void main()
//{
//	Solution so;
//	
//	vector<string> str = so.letterCombinations("45325");
//	vector<string>::iterator iter;
//	for(iter = str.begin();iter != str.end();iter++)
//	{
//		printf("%s\n",iter->c_str());
//	}
//	printf("%d",str.size());
//	getchar();
//}