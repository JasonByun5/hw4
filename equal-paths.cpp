#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool helper(Node * root, int depth, int &leafDepth);

bool equalPaths(Node * root)
{
    // Add your code below
    
    int leafDepth = -1;
    
    return helper(root, 0, leafDepth);
   
}

bool helper(Node * root, int depth, int &leafDepth){
  
  if (root == nullptr){
    return true;
  }

  if ((root->left == nullptr && root->right == nullptr)){
    if(leafDepth == -1){
      leafDepth = depth;
    }
    return depth == leafDepth; //compares to other lengths of leaves
  }
  

  return helper(root -> left, depth + 1, leafDepth)
    && helper(root->right, depth + 1, leafDepth);

  


}

