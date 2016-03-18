#include <stdio.h>
#include <string>
#include <malloc.h>

char* convert(char* s, int numRows) {
	if(numRows <= 1)
	{
		return s;
	}
    /*char **Str = (char **)malloc(sizeof(char *)*numRows);
    int len = strlen(s);
    int i;
    int *fu = (int *)malloc(sizeof(int)*numRows);
    for(i = 0;i < numRows;i++)
    {
        Str[i] = (char *)malloc((sizeof(char)*(len/numRows))+1);
        Str[i][0] = '\0';
        fu[i] = 0;
    }
    int j = 0;
    for(i = 0;s[i] != '\0';i++)
    {
		if((i/numRows%2)==0)
        {
			j = i%numRows;
			Str[j][fu[j]] = s[i];
		}
		else
		{
			j = numRows - i%numRows -2;
			Str[j][fu[j]] = s[i];
		}
		fu[j]++;
        Str[j][fu[j]] = '\0';
    }
    
    char *S = (char *)malloc(sizeof(char)*len);
    for(i = 0;i < numRows;i++)
    {
        strcpy(S,Str[i]);
    }
    free(fu);
    free(Str);*/
	//×Ö·û´®³¤¶È
	int len = strlen(s);
	printf("×Ö·û´®³¤¶È£º%d\n",len);
	char * str = (char *)malloc(sizeof(char)*(len+1));
	int i = 0,j = 0,k = 0;
	int dev = 2*(numRows-1);
	//¼ÆËãZagZig×Ö·û´®
	for(i = 0;i < numRows;i++)
	{
		for(j = i;j < len;j += dev)
		{
			str[k++] = s[j];
			str[k] = '\0';
			if(i == 0||i == numRows-1)
				continue;
			int next = j + dev - i * 2;
			if(next < len)
			{
				str[k++] = s[next];
				str[k] = '\0';
			}
		}
	}
    return str;
}

//void main()
//{
//	printf("%s",convert("fdsasdauihefquhfegwqfiwq",4));
//	printf("\nfdsasdauihefquhfegwqfiwq");
//	getchar();
//
//}