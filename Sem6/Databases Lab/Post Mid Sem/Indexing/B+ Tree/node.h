#if !defined(NODE_H)
#define NODE_H

#include "data.h"

class node{
private:
    node*   split();
    int     find_index(int data);
public:
    int*    array;
    node*   parent;
    node**  children;
    data**  data_children;
    int     key_count;
    int     child_count;
    int     data_child_count;
    node();
    bool    is_leaf();
    int     get_ith_key(int i);
    node*   get_ith_child(int i);
    data*   get_ith_data_child(int i);
    int     get_child_index();
    void    remove_at_index(int index);
    void    insert_at_index(bool key_or_child, bool is_data_child, void* item, int index);
    void    set_data_child(int head_data, data* child);
    void    merge(int index);
    void    insert(int data, node* children, node** root);
    void    print();
};

#endif // NODE_H
