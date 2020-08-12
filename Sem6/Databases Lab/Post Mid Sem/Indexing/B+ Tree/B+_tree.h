#if !defined(BP_TREE_H)
#define BP_TREE_H
#include "B_tree.h"

class BPTree{
private:
    BTree* tree;
    data*  start;
public:
    BPTree();
    void  insert(int value);
    void  del   (int value){}
    bool  search(int value);
    void  print ();
};

#endif // BP_TREE_H
