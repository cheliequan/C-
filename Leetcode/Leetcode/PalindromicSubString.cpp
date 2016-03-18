#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include <string.h>
#include <string>


//时间复杂度为O(n^2)
bool IsPalindromicString(char * s,int start,int end)
{
	if(start == end)
		return true;
	if(end == start+1)
	{
		if(s[start] == s[end])
			return true;
		else
			return false;
	}
	if(IsPalindromicString(s,start+1,end-1)&&s[start] == s[end])
		return true;
	else
		return false;
}


char* longestPalindrome(char* s) {
	int i,j;
	int len = strlen(s);
	int start = 0,end = 0,maxlen = 0;
	for(i = 0;s[i] != '\0';i++)
	{
		for(j = len-1;j >= i;j--)
		{
			if(IsPalindromicString(s,i,j))
			{
				if(j-i > maxlen)
				{
					start = i;
					end = j;
					maxlen = j - i;
				}
			}
		}
		if(len - i <= maxlen)
			break;
	}
	char *subString = (char *)malloc(sizeof(char)*(maxlen+1));
	for(i = 0,j = start;j <= end;j++,i++)
	{
		subString[i] = s[j];
	}
	subString[i] = '\0';
	return subString;
}

//void main()
//{
//	char s[] = "ibvjkmpyzsifuxcabqqpahjdeuzaybqsrsmbfplxycsafogotliyvhxjtkrbzqxlyfwujzhkdafhebvsdhkkdbhlhmaoxmbkqiwiusngkbdhlvxdyvnjrzvxmukvdfobzlmvnbnilnsyrgoygfdzjlymhprcpxsnxpcafctikxxybcusgjwmfklkffehbvlhvxfiddznwumxosomfbgxoruoqrhezgsgidgcfzbtdftjxeahriirqgxbhicoxavquhbkaomrroghdnfkknyigsluqebaqrtcwgmlnvmxoagisdmsokeznjsnwpxygjjptvyjjkbmkxvlivinmpnpxgmmorkasebngirckqcawgevljplkkgextudqaodwqmfljljhrujoerycoojwwgtklypicgkyaboqjfivbeqdlonxeidgxsyzugkntoevwfuxovazcyayvwbcqswzhytlmtmrtwpikgacnpkbwgfmpavzyjoxughwhvlsxsgttbcyrlkaarngeoaldsdtjncivhcfsaohmdhgbwkuemcembmlwbwquxfaiukoqvzmgoeppieztdacvwngbkcxknbytvztodbfnjhbtwpjlzuajnlzfmmujhcggpdcwdquutdiubgcvnxvgspmfumeqrofewynizvynavjzkbpkuxxvkjujectdyfwygnfsukvzflcuxxzvxzravzznpxttduajhbsyiywpqunnarabcroljwcbdydagachbobkcvudkoddldaucwruobfylfhyvjuynjrosxczgjwudpxaqwnboxgxybnngxxhibesiaxkicinikzzmonftqkcudlzfzutplbycejmkpxcygsafzkgudy";
//	printf("%s",longestPalindrome(s));
//	getchar();
//}

//class Solution {
//public:
//	// Transform S into T.  
//	// For example, S = "abba", T = "^#a#b#b#a#$".  
//	// ^ and $ signs are sentinels appended to each end to avoid bounds checking  
//	string preProcess(string s) {  
//		int n = s.length();  
//		if (n == 0) return "^$";  
//		string ret = "^";  
//		for (int i = 0; i < n; i++)  
//			ret += "#" + s.substr(i, 1);  
//
//		ret += "#$";  
//		return ret;  
//	}  
//
//	string longestPalindrome(string s) {
//		string T = preProcess(s);  
//		int n = T.length();  
//		int *P = new int[n];  
//		int C = 0, R = 0;  
//		for (int i = 1; i < n-1; i++) {  
//			int i_mirror = 2*C-i; // equals to i' = C - (i-C)  
//
//			P[i] = (R > i) ? min(R-i, P[i_mirror]) : 0;  
//
//			// Attempt to expand palindrome centered at i  
//			while (T[i + 1 + P[i]] == T[i - 1 - P[i]])  
//				P[i]++;  
//
//			// If palindrome centered at i expand past R,  
//			// adjust center based on expanded palindrome.  
//			if (i + P[i] > R) {  
//				C = i;  
//				R = i + P[i];  
//			}  
//		}  
//
//		// Find the maximum element in P.  
//		int maxLen = 0;  
//		int centerIndex = 0;  
//		for (int i = 1; i < n-1; i++) {  
//			if (P[i] > maxLen) {  
//				maxLen = P[i];  
//				centerIndex = i;  
//			}  
//		}  
//		delete[] P;  
//
//
//		return s.substr((centerIndex - 1 - maxLen)/2, maxLen);  
//	}  
//};