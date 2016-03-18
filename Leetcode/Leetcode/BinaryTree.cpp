
#include <iostream>
#include <vector>
#include <queue>

using namespace std;


/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */


struct TreeNode{
	int val;
	TreeNode *left;
	TreeNode *right;
	//TreeNode(int x) : val(x), left(NULL), right(NULL) {};
};



class Solution {
public:
	vector<vector<int>> levelOrderBottom(TreeNode* root){
        vector<vector<int>> ret;  
            ret.clear();  
            if(root == NULL)  
                return ret;  
            queue<TreeNode*> S;  
            S.push(root);  
            S.push(NULL);  
            vector<int> tmp;  
            while(!S.empty()){  
                //travesal current level  
                TreeNode* p = S.front();  
                S.pop();  
                if(p!=NULL)  
                {  
                    tmp.push_back(p->val);  
                    if(p->left)   
                        S.push(p->left);  
                    if(p->right)  
                        S.push(p->right);  
                }else{  
                    if(!tmp.empty())  
                    {     
                        S.push(NULL);  
                        ret.push_back(tmp);  
                        tmp.clear();  
                    }  
                }  
            }  
            reverse(ret.begin(),ret.end());  
            return ret;  
    }
};

