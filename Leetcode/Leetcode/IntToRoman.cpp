#include "stdio.h"
#include <malloc.h>

#include <string>

char* intToRoman(int num) {
	char *RM = new char(32);
	int i = 0;
	i = num / 1000;
	int k = 0;
	num = num % 1000;

	if(i != 0)
	{
		for(int m = 0;m < i;m++)
			RM[k++] = 'M';
	}

	i = num / 100;
	num = num % 100;
	if(i == 9)
	{
		RM[k++] = 'C';
		RM[k++] = 'M';
	}
	else if(i >= 5&&i < 9)
	{
		RM[k++] = 'D';
		for(int d = 0;d < i - 5;d++)
			RM[k++] = 'C';
	}
	else if(i == 4)
	{
		RM[k++] = 'C';
		RM[k++] = 'D';
	}
	else
	{
		for(int d = 0;d < i;d++)
			RM[k++] = 'C';
	}

	i = num / 10;
	num = num % 10;
	if(i == 9)
	{
		RM[k++] = 'M';
		RM[k++] = 'C';
	}
	else if(i >= 5&&i < 9)
	{
		RM[k++] = 'L';
		for(int d = 0;d < i - 5;d++)
			RM[k++] = 'X';
	}
	else if(i == 4)
	{
		RM[k++] = 'X';
		RM[k++] = 'L';
	}
	else
	{
		for(int d = 0;d < i;d++)
			RM[k++] = 'X';
	}

	i = num;
	if(i == 9)
	{
		RM[k++] = 'I';
		RM[k++] = 'X';
	}
	else if(i >= 5&&i < 9)
	{
		RM[k++] = 'V';
		for(int d = 0;d < i - 5;d++)
			RM[k++] = 'I';
	}
	else if(i == 4)
	{
		RM[k++] = 'I';
		RM[k++] = 'V';
	}
	else
	{
		for(int d = 0;d < i;d++)
			RM[k++] = 'I';
	}
	RM[k] = '\0';
	return RM;
}


int romanToInt(char* s) {
	int num = 0;
	int n = 0;
	bool D = false,L = false,V = false;
	for(int i = 0;s[i] != '\0';i++)
	{
		if(s[i] == 'M')
		{
			num += 1000;
		}
		else if(s[i] == 'C')
		{
			if(s[i+1] == 'M')
			{
				num += 900;
				i++;
			}
			else
			{
				n += 100;
			}
		}
		else if(s[i] == 'D')
		{
			if(n != 0)
			{
				n = 500 - n;
				num += n;
				n = 0;
			}
			else
			{
				n = 500;
			}
		}
		else if(s[i] == 'X')
		{
			if(n >= 100)
			{
				num += n;
				n = 0;
			}
			if(s[i+1] == 'C')
			{
				num += 90;
				i++;
			}
			else
			{
				n += 10;
			}
		}
		else if(s[i] == 'L')
		{
			if(n >= 100)
			{
				num += n;
				n = 0;
			}
			if(n != 0)
			{
				num += 50 - n;
				n = 0;
			}
			else
				n = 50;
		}
		else if(s[i] == 'I')
		{
			if(n >= 10)
			{
				num += n;
				n = 0;
			}
			if(s[i+1] == 'X')
			{
				num += 9;
				i++;
			}
			else
			{
				n += 1;
			}
		}
		else if(s[i] == 'V')
		{
			if(n >= 10)
			{
				num += n;
				n = 0;
			}
			if(n != 0)
			{
				num += 5 - n;
				n = 0;
			}
			else
				n = 5;
		}
	}
	if(n != 0)
		num += n;
	return num; 
}

//void main()
//{
//	int num = 621;
//	char RoMan[32];
//	strcpy(RoMan,::intToRoman(num));
//	printf("RoMan Number is : %s\n",RoMan);
//	printf("Roman to number is %d",romanToInt(RoMan));
//	getchar();
//}