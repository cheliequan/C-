#include <stdio.h>

int missingNumber(int* nums, int numsSize) {
    int i = 0;
    int result = 0 ^ nums[0];
	printf("%d",result);
    for(i = 1;i < numsSize+1;i ++)
    {
        result = result ^ i;
        result = result ^ nums[i];
		printf("%d",result);
    }
    return result;
}

void main()
{
	int num[] = {0,1,2,3,5,6,7,8,9};
	int numsize = sizeof(num)/sizeof(int);
	printf("miss Number:%d\n",::missingNumber(num,numsize));
	getchar();
	return ;
}