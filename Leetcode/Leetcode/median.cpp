#include "stdio.h"



float findKth(int a[], int m, int b[], int n, int k)  
{  
    //always assume that m is equal or smaller than n  
    if (m > n)  
        return findKth(b, n, a, m, k);  
    if (m == 0)
    {
        return b[k-1]; 
    }
    if (k == 1)  
        return a[0]<b[0]?a[0]:b[0];  
    //divide k into two parts  
    int pa = k / 2 < m?k/2:m;
    int pb = k - pa;  
    if (a[pa - 1] < b[pb - 1])  
        return findKth(a + pa, m - pa, b, n, k - pa);  
    else if (a[pa - 1] > b[pb - 1])  
        return findKth(a, m, b + pb, n - pb, k - pb);  
    else  
        return a[pa - 1];  
}  
float findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size) {
    if((nums1Size + nums2Size)%2 == 0)
    {
        float num = findKth(nums1, nums1Size,nums2, nums2Size, (nums1Size + nums2Size)/2+1);
        num += findKth(nums1, nums1Size,nums2, nums2Size, (nums1Size + nums2Size)/2);
        return num/2;
    }
    
    return findKth(nums1, nums1Size,nums2, nums2Size, (nums1Size + nums2Size)/2+1);
}

//void main()
//{
//	int A[] = {1,3};
//	int B[] = {2,4,5,6,7,8,9};
//
//	int al = sizeof(A)/sizeof(int);
//	int bl = sizeof(B)/sizeof(int);
//
//	int median = findMedianSortedArrays(A,al,B,bl);
//	printf("中位数是：%d",median);
//	getchar();
//
//}