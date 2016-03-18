#include "stdio.h"


//本程序需要考虑数据边界问题 考虑同一问题的题目还有字符转数字，还有求中位数以及以后所有数据数据相加减、相乘、求幂运算都需考虑数据是否
void binarySearch(int num,int findnum)
{
	if(findnum >= num)
		return ;
	int start = 1;
	int end = num;
	int tfind;
	int count = 0;
	while(start < end)
	{
		tfind = (start + end) / 2;
		count ++;
		if(tfind == findnum)
		{
			break;
		}
		else
		{
			if(tfind < findnum)
			{
				start = tfind;
			}
			else
			{
				end = tfind;
			}
		}
	}
	printf("寻找了%d次！\n",count);
}

//void main()
//{
//	int num;
//	int findnum;
//	printf("请输入数字上限和要寻找的数：\n");
//	scanf("%d %d",&num,&findnum);
//	binarySearch(num,findnum);
//	getchar();
//	char buf[50];
//	::gets(buf);
//}