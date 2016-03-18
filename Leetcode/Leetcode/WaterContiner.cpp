#include "stdio.h"


int maxArea(int* height, int heightSize) {
    int left = 0,right = heightSize-1;
    int MaxV = 0;
    int water = 0;
    int h;
    while(left < right)
    {
        h = height[left] < height[right]?height[left]:height[right];
        water = (right-left)*h;
        if(water > MaxV)
        {
            MaxV = water;
        }
        height[left] < height[right]?left++:right--;
    }
    return MaxV;
}

//void main()
//{
//	int A[] = {1,2,3,4,5,6,7,8,9,10};
//
//	int len = sizeof(A)/sizeof(int);
//
//	printf("最大容水量：%d\n",maxArea(A,len));
//
//	getchar();
//
//}