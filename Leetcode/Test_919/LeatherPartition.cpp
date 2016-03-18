// 皮革划分问题
#include <iostream>
#include <math.h>

using namespace std;

//坐标点
struct Point{
	int x;
	int y;
};

//计算最佳划分某一边的三角形底边和高的数量
int findBestCombination(int RXY,int bottom,int high,int &Bnum,int &Hnum)
{
	int i = 0;
	Bnum = i;
	int j = 0;
	Hnum = RXY / high;
	int min = RXY;
	while(bottom * i <= RXY)
	{
		j = (RXY - bottom * i) / high;
		if(min > (RXY - bottom * i - high * j))
		{
			Bnum = i;
			Hnum = j;
			min = RXY - bottom * i - high * j;
		}
		if(min == 0)
			break;
		i ++;
	}
	return min;
}

//计算最大划分数
int findBigPartion(int Rlength,int Rhigh,int bottom,int high){
	if((Rlength % bottom == 0 && Rhigh % high == 0) || (Rlength % high == 0 && Rhigh % bottom == 0))
	{
		return (Rlength * Rhigh) / (bottom * high / 2);
	}
	else if(Rlength % bottom == 0 || Rhigh % high == 0)
	{
		return (Rlength / bottom) * (Rhigh / high) * 2;
	}
	else if(Rhigh % bottom == 0 || Rlength % high == 0)
	{
		return (Rlength / high) * (Rhigh / bottom) * 2;
	}
	else
	{
		int bnum = 0,hnum = 0,bnum1 = 0,hnum1 = 0;
		int Rlmin = findBestCombination(Rlength,bottom,high,bnum,hnum);
		int Rhmin = findBestCombination(Rhigh,bottom,high,bnum1,hnum1);
		if(Rlmin == 0)
			return ((Rlength * Rhigh) - (Rlength * Rhmin)) / (bottom * high / 2);
		else if(Rhmin == 0)
			return ((Rlength * Rhigh) - (Rhigh * Rlmin)) / (bottom * high / 2);
		else 
			return (Rlength * Rhigh - Rlength * Rhmin - Rhigh * Rlmin + Rhmin * Rlmin) / (bottom * high / 2);
	}
}

void getRlengthOrHigh(Point *point,int &Rlength,int &Rhigh){
	Rlength = abs(point[0].x - point[1].x) > abs(point[0].x - point[2].x)? abs(point[0].x - point[1].x):abs(point[0].x - point[2].x);
	Rhigh = abs(point[0].y - point[1].y) > abs(point[0].y - point[2].y)? abs(point[0].y - point[1].y):abs(point[0].y - point[2].y);
}

int main(void)
{
	Point point[4];
	int bottom = 0, high = 0;
	while(true){
		//
		printf("please enter point(x,y):\n");
		for(int i = 0;i < 4;i++)
		{
			scanf("%d %d",&(point[i].x),&(point[i].y));
		}
		//
		printf("please enter bottom and high:\n");
		scanf("%d %d",&bottom,&high);

		//
		int Rlength = 0;
		int Rhigh = 0;
		getRlengthOrHigh(point,Rlength,Rhigh);
		int num = findBigPartion(Rlength,Rhigh,bottom,high);
		
		printf("最大切割数：%d;剩余面积：%d\n",num,Rlength * Rhigh - bottom * high * num / 2);
	}
	return 0;
}