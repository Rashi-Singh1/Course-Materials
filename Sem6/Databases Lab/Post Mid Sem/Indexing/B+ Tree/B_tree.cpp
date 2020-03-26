#include <iostream>
#include <bits/stdc++.h>
using namespace std;
#define T 2
#define MAX_KEYS (2*T-1)
#define MIN_KEYS (T-1)

class node{
private:
    node* split();
    int find_index(int data);
    void insert_at_index(bool key_or_child, void* item, int index);
public:
    int* array;
    node** children;
    node* parent;
    int key_count;
    int child_count;
    bool is_leaf();

    node();
    int get_ith_key(int i);
    node* get_ith_child(int i);
    void insert(int data, node* children, node** root);
    void print();
};

node::node(){
    this->parent = NULL;
    this->key_count = 0;
    this->child_count = 0;
    this->array = new int [MAX_KEYS + 1];
    this->children = new node* [MAX_KEYS + 2];
}

bool node::is_leaf(){
    return (this->child_count > 0 ? false : true);
}

int node::get_ith_key(int i){
    if(i >= key_count) return -1;
    return this->array[i];
}

node* node::get_ith_child(int i){
    if(this->is_leaf() || i > this->key_count) return NULL;
    return this->children[i];
}

void node::print(){
    for(int i = 0 ; i < this->key_count; i++){
        cout<<this->array[i]<<" ";
    }
    cout<<endl;
    for(int i = 0; i< this->child_count; i++){
        this->children[i]->print();
    }
}

int node::find_index(int data){
    if(this->key_count == 0) return 0;
    if(this->key_count > MAX_KEYS) return -1;
    for(int i = 0; i < this->key_count; i++){
        if(this->array[i] < data) continue;
        else {
            return i;
        }
    }
    return this->key_count;
}

void node::insert_at_index(bool key, void* item, int index){
    //to insert in array
    if(key){
        int val = *((int*) item);

        //inserts even if capacity full (helps in splitting later)
        if(index < MAX_KEYS + 1 && index <= this->key_count && index >= 0){
            if(index == this->key_count) {
                this->array[(this->key_count)++] = val;
                return;
            }
            int next_val = val;
            int temp;
            for(int i = index; i < this->key_count; i++){
                temp = this->array[i];
                this->array[i] = next_val;
                next_val = temp;
            }
            this->array[(this->key_count)++] = next_val;
        }
    }
    
    //to insert val in children
    else{
        node* val = *((node**) item);
        if(index <= MAX_KEYS + 1 && index <= this->child_count && index >= 0){
            if(index == this->child_count) {
                this->children[(this->child_count)++] = val;
                return;
            }
            node* next_val = val;
            node* temp;
            for(int i = index; i < this->child_count; i++){
                temp = this->children[i];
                this->children[i] = next_val;
                next_val = temp;
            }
            this->children[(this->child_count)++] = next_val;
        }
    }
}

node* node::split(){
    if(this->key_count < MAX_KEYS) return this;
    node* new_node = new node();
    int k = 0;
    int old_key_count = this->key_count;
    int old_child_count = this->child_count;

    for(int i = MIN_KEYS + 1; i < old_key_count; i++, k++){
        new_node->array[k] = this->array[i];
        (new_node->key_count)++;
        (this->key_count)--;
        if(!(this->is_leaf())) {
            new_node->children[k] = this->children[i];
            (new_node->child_count)++;
            (this->child_count)--;
        }
    }
    if(!(this->is_leaf())) {
        new_node->children[k] = this->children[old_child_count - 1];
        (new_node->child_count)++;
        (this->child_count)--;
    }

    //remove the head_data
    (this->key_count)--;
    new_node->parent = this->parent;
    return new_node;
}

void node::insert(int data, node* children, node** root){

    //simply insert
    //assuming children is the right child of place where it is to be inserted
    if(this->key_count < MAX_KEYS){
        int index = this->find_index(data);
        // cout<<"index : "<<index<<endl;
        this->insert_at_index(true, (void*) &data, index);
        this->insert_at_index(false, (void*) &children, index + 1);
    }

    //split after adding
    else{
        int index = this->find_index(data);
        this->insert_at_index(true, (void*) &data, index);
        this->insert_at_index(false, (void*) &children, index + 1);
        int head_data = this->array[MIN_KEYS];
        node* new_node = this->split();
        if(this->parent){
            this->parent->insert(head_data, new_node, root);
        }
        else{
            node* head_node = new node();
            *(root) = head_node;

            head_node->children[0] = this;
            (head_node->child_count)++;

            //set parent for both
            this->parent = head_node;
            new_node->parent = head_node;

            head_node->insert(head_data, new_node, root);
            // cout<<"head_node now : "; head_node->print();
        }
    }
}

class BTree{
private:
    node* root;
    void insert_util(node* cur, int data);
    void print_util(node* cur);
public:
    BTree();
    void insert(int data);
    void print();
};

BTree::BTree(){
    this->root = NULL;
}

void BTree::insert(int data){
    if(this->root == NULL) this->root = new node();
    this->insert_util(this->root, data);
}

void BTree::insert_util(node* cur, int data){
    if(cur == NULL) return;
    if(cur->is_leaf()) {
        cur->insert(data, NULL, &(this->root));
    }
    else{
        if(data < cur->get_ith_key(0)) this->insert_util(cur->get_ith_child(0), data);
        else if(data > cur->get_ith_key(cur->key_count - 1)) this->insert_util(cur->get_ith_child(cur->key_count), data);
        else{
            for(int i = 0; i< cur->key_count - 1;i++){
                if(cur->get_ith_key(i) < data && data <= cur->get_ith_key(i+1)) 
                    this->insert_util(cur->get_ith_child(i+1), data);
            }
        }
    }
}

void BTree::print(){
    if(this->root == NULL) return;
    this->root->print();
    // this->print_util(this->root);
    cout<<endl;
}

void BTree::print_util(node* cur){
    if(cur == NULL) return;
    if(!(cur->is_leaf())) this->print_util(cur->get_ith_child(0)); 
    for(int i = 0; i < cur->key_count; i++){
        cout<<cur->get_ith_key(i)<<" ";
        this->print_util(cur->get_ith_child(i+1));
    }
    // cur->print();
}


int main(int argc, char const *argv[])
{
    BTree tree = BTree();

    int no_of_inputs;
    cout<<"Enter no of inputs : "<<endl;
    cin>>no_of_inputs;

    cout<<"Enter input as follows : "<<endl<<"i <key> : to insert key"<<endl<<"d <key> : to delete key"<<endl<<"s <key> : to search key"<<endl<<endl;
    for (int i = 0; i < no_of_inputs; i++)
    {
        char type;
        int temp;
        cin>>type>>temp;
        switch (type)
        {
        case 'i' :
            tree.insert(temp);
            tree.print();
            break;
        case 'd' :
            // tree.del_with_compaction(temp);
            tree.print();
            break;
        case 's' :

            // cout<<"Search result for "<<temp<<" : "; tree.search(temp) != NULL ? cout<<(tree.search(temp))->data<<endl : cout<<"NULL"<<endl;
            break;
        case 'p' :
            tree.print();
            break;
        default:
            break;
        }
    }
    
    return 0;
}