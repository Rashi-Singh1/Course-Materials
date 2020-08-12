#if !defined(DATA_H)
#define DATA_H

class node;
class BTree;

class data{
private:
    int  find_index(int value);
    void insert_at_index(int val, int index);
    void split();
public:
    int* array;
    int  key_count;
    data* next;
    data* prev;
    node* parent;
    data();
    void insert(int value, BTree** tree);
    int  get_ith_key(int i);
    bool search(int value);
    void print();
};

#endif // DATA_H
