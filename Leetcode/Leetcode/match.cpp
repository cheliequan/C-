#include "stdio.h"
#include "string.h"


bool isMatch(char* s, char* p) {
    int i,j = 0;
    for(i = 0;s[i] != '\0';i++)
    {
        if(s[i] == p[j]||p[j] == '.')
        {
            j++;
        }
        else if(p[j] == '*')
        {
            j++;
            if(p[j] == '\0')
                return true;
			while(p[j] == p[j+2])
				j += 2;
            while(s[i] != p[j]&&s[i] != '\0')
            {
                i++;
            }
            while(s[i] == s[i+1]&&s[i] != '\0')
                i++;
            if(s[i] == p[j])
                j++;
        }
        else
        {
            int k = strlen(s);
            int l = strlen(p);
            for(;k >= 0;k--)
            {
                if(s[k] == p[l]||p[l] == '.')
                {
                    l--;
                }
                else if(p[l] == '*')
                {
                    l--;
					while(p[l] == p[l-2])
						l -= 2;
                    while(s[k] != p[l]&&k >= 0)
                    {
                        k--;
                    }
                    while(s[k] == s[k-1]&&k >= 0)
                        k--;
                    l--;
                }
                
                if(l < 0)
                    break;
            }
            if(k < 0)
                return true;
            else
                return false;
        }
    }
    if(p[j] == '\0')
        return true;
    else
        return false;
    
}

//void main()
//{
//	char *s = "ab";
//	char *p =  ".*c";
//	if(::isMatch(s,p))
//		printf("succeed!");
//	else
//		printf("Failed!");
//	getchar();
//}