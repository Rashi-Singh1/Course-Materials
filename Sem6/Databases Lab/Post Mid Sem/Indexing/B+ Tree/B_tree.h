#if !defined(B_TREE_H)
#define B_TREE_H
#include "node.h"

class BTree{
private:
    void insert_util(node* cur, int data);
    bool search_util(node* cur, int data);
    void del_util(node* cur, int data);
    node* route_util(node* cur, int data);
public:
    node* root;
    BTree();
    void insert(int data);
    void insert_data_child(data* child, int index);
    bool search(int data);
    void del(int data);
    void print();
    node* route(int data);
};

#endif // B_TREE_H
