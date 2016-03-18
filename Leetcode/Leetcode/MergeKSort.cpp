#include <iostream>
#include <vector>

using namespace std;

/**
* Definition for singly-linked list.
* struct ListNode {
*     int val;
*     ListNode *next;
*     ListNode(int x) : val(x), next(NULL) {}
* };
*/

struct ListNode {
	int val;    
	ListNode *next;
	//ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:

	ListNode* merge(ListNode *l1,ListNode *l2)
	{
		if(l1 == NULL)
			return l2;
		else if(l2 == NULL)
			return l1;
		ListNode* me = NULL;
		ListNode * t1,* t2,*t3;
		if(l1->val < l2->val)
		{
			t3 = me = l1;
			t1 = l1->next;
			t2 = l2;
		}
		else
		{
			t3 = me = l2;
			t2 = l2->next;
			t1 = l1;
		}
		while(t1 != NULL&&t2 != NULL)
		{
			if(t1->val < t2->val)
			{
				t3->next = t1;
				t1 = t1->next;
			}
			else
			{
				t3->next = t2;
				t2 = t2->next;
			}
			t3 = t3->next;
		}
		if(t1 == NULL)
			t3->next = t2;
		else
			t3->next = t1;
		return me;
	}

	ListNode*mergeHelp(vector<ListNode *> lists,int low,int high)
	{
		if(high > low)
		{
			if(high-low == 1)
			{
				return merge(lists[low],lists[high]);
			}
			else
			{
				return merge(mergeHelp(lists,low,(high+low)/2),mergeHelp(lists,(low+high)/2+1,high));
			}
		}
		else if(high == low)
		{
			return lists[low];
		}
		else
		{
			return mergeHelp(lists,high,low);
		}
	}

	ListNode* mergeKLists(vector<ListNode*>& lists) {
		if(lists.size() == 0)
			return NULL;
		if(lists.size() == 1)
			return lists[0];
		if(lists.size() == 2)
		{
			return merge(lists[0],lists[1]);
		}
		if(lists.size() > 2)
		{
			return mergeHelp(lists,0,lists.size()-1);
		}
	}
};

ListNode *initList(int *num)
{
	ListNode *list = NULL,*t1;
	int len = sizeof(num)/sizeof(int);
	for(int i = 0;i < len;i++)
	{
		if(list == NULL)
		{
			list = new ListNode;
			t1 = list;
		}
		else
		{
			t1->next = new ListNode;
			t1 = t1->next;
		}
		t1->val = num[i];
	}
	t1->next = NULL;
	return list;
}

void show(ListNode *list)
{
	printf("排序结果如下:\n");
	ListNode *t1;
	for(t1 = list;t1 != NULL;t1 = t1->next)
	{
		if(t1->next == NULL)
		{
			printf("%d\n",t1->val);
			break;
		}
		printf("%d ,",t1->val);
	}
}

//void main()
//{
//	Solution so;
//	int num1[] = {1};
//	int num2[] = {0};
//	int num3[] = {6};
//	vector<ListNode *> str;
//	str.clear();
//	str.push_back(::initList(num1));
//	str.push_back(::initList(num2));
//	str.push_back(::initList(num3));
//	show(so.mergeKLists(str));
//
//	getchar();
//}