#include "stdio.h"


//��������Ҫ�������ݱ߽����� ����ͬһ�������Ŀ�����ַ�ת���֣���������λ���Լ��Ժ���������������Ӽ�����ˡ��������㶼�迼�������Ƿ�
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
	printf("Ѱ����%d�Σ�\n",count);
}

//void main()
//{
//	int num;
//	int findnum;
//	printf("�������������޺�ҪѰ�ҵ�����\n");
//	scanf("%d %d",&num,&findnum);
//	binarySearch(num,findnum);
//	getchar();
//	char buf[50];
//	::gets(buf);
//}