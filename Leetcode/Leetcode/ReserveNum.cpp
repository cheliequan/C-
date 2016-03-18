#include <stdio.h>
#include <math.h>

int reverse(int x) {
    long long num = 0;
	long long MAX = 2147483647;
    bool fu = false;
    if(x < 0)
        fu = true;
    int oNum = abs(x);
    while(oNum > 0)
    {
        num = num *10 + oNum % 10;
        oNum = oNum / 10;
    }
    if(fu)
	{
		if(num > MAX+1)
			return 0;
		return -num;
	}
	else{
		if(num > MAX)
			return 0;
		return num;
	}
}

//void main()
//{
//	printf("%d",reverse(-123));
//	getchar();
//
//}