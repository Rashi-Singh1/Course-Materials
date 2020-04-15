#include <iostream>
#include <bits/stdc++.h>
using namespace std;
#define T 2
#define D 2
#define MAX_KEYS (2*T-1)
#define MIN_KEYS (T-1)
#define MAX_DATA_KEYS (2*D)
#define MIN_DATA_KEYS (D)

class node;
class data;
class BTree;
class BPTree;

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
    data();
    void insert(int value, BTree** tree);
    int  get_ith_key(int i);
    void print();
};

data::data(){
    this->array = new int [MAX_DATA_KEYS + 1];
    this->key_count = 0;
    this->next = NULL;
    this->prev = NULL;
    // this->parent = NULL;
}

int data::get_ith_key(int i){
    if(i >=0 && i < this->key_count) return this->array[i];
    return -1;
}

int data::find_index(int value){
    if(this->key_count == 0) return 0;
    if(this->key_count > MAX_DATA_KEYS) return -1;
    for(int i = 0; i < this->key_count; i++){
        if(this->array[i] < value) continue;
        else return i;
    }
    return this->key_count;
}

void data::insert_at_index(int val, int index){
    if(index <= MAX_DATA_KEYS && index <= this->key_count && index >= 0){
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

void data::split(){
    if(this->key_count <= MAX_DATA_KEYS) return;
    data* new_list = new data();
    int next_index = MIN_DATA_KEYS;
    int new_index = 0;
    for(int i = next_index; i < this->key_count; i++){
        new_list->array[new_index++] = this->array[i];
    }
    new_list->key_count = this->key_count - MIN_DATA_KEYS;
    this->key_count = MIN_DATA_KEYS;
    new_list->next = this->next;
    if(this->next) this->next->prev = new_list;
    this->next = new_list;
    new_list->prev = this;
}

void data::insert(int value, BTree** tree){
    int index = this->find_index(value);
    this->insert_at_index(value,index);
    if(this->key_count > MAX_DATA_KEYS) {
        this->split();
        if(this->next){
            int head_data = this->get_ith_key(this->key_count - 1);
            head_data+=this->next->get_ith_key(0) + 1;
            head_data/=2;
            // if(this->parent) this->parent->set_data_child(head_data, this);
            // else{
            //     (*tree) = new BTree();
            //     (*tree)->insert(head_data);

            //     //later use root ka function insert_data_child, when index finalized for that function
            //     (*tree)->insert_data_child(this, 0);
            //     (*tree)->insert_data_child(this->next, 1);
            //     this->parent = (*tree)->root;
            //     this->next->parent = (*tree)->root;
            // }
        }
    }
}

void data::print(){
    for(int i = 0 ; i < this->key_count; i++) cout<<this->array[i]<<" ";
    cout<<" -> ";
}

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

node::node(){
    this->parent = NULL;
    this->array = new int [MAX_KEYS + 1];
    this->children = new node* [MAX_KEYS + 2]; 
    this->data_children = new data* [MAX_KEYS + 2];                 //data* replaces a node* in children vala array, so no of children still same                    
    this->key_count = 0;
    this->child_count = 0;
    this->data_child_count = 0;
}

bool node::is_leaf(){
    return (this->child_count > 0 ? false : true);
}

int node::get_ith_key(int i){
    if(i >= key_count || i < 0) return -1;
    return this->array[i];
}

node* node::get_ith_child(int i){
    if(this->is_leaf() || i > this->key_count || i < 0) return NULL;
    return this->children[i];
}

data* node::get_ith_data_child(int i){
    if(!this->is_leaf() || i >= this->child_count || i < 0) return NULL;
    return this->data_children[i];
}

//********************************************************************************//
void node::set_data_child(int head_data, data* child){
    if(this->key_count < MAX_KEYS){
        int index = this->find_index(head_data);
        this->insert_at_index(true,false,(void*) &head_data,index);
        this->insert_at_index(false,false,(void*) &child, index);           //confirm this index
    }
}

//assuming this is called for leaves
void node::remove_at_index(int index){
    if(index >=0 && index < this->key_count){
        for(int i = index; i < this->key_count - 1; i++){
            int temp = this->get_ith_key(i+1);
            this->array[i] = temp;
        }
        (this->key_count)--;
    }
}

int node::get_child_index(){
    if(this->parent == NULL) return -1;
    for(int i = 0; i<this->parent->child_count; i++){
        if(this->parent->get_ith_child(i) == this) return i;
    }
    return -1;
}

void node::print(){
    for(int i = 0 ; i < this->key_count; i++){
        cout<<this->array[i]<<" ";
    }
    cout<<endl;
    for(int i = 0; i< this->child_count; i++){
        this->children[i]->print();
    }
    for(int i = 0; i< this->data_child_count; i++){
        this->data_children[i]->print();
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

void node::insert_at_index(bool key, bool is_data_child, void* item, int index){
    if(is_data_child){
        data* val = *((data**) item);
        if(this->is_leaf() && index <= MAX_KEYS + 1 && index <= this->data_child_count && index >= 0){
            if(index == this->data_child_count) {
                this->data_children[(this->data_child_count)++] = val;
                return;
            }
            data* next_val = val;
            data* temp;
            for(int i = index; i < this->data_child_count; i++){
                temp = this->data_children[i];
                this->data_children[i] = next_val;
                next_val = temp;
            }
            this->data_children[(this->data_child_count)++] = next_val;
        }
    }
    else{
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
}

//if key count == 2, then make it cur, rather than making it a new merged child
void node::merge(int index){
    if(index >= 0 && index < this->key_count && this->child_count > 0){
        node* child1 = this->get_ith_child(index);
        node* child2 = this->get_ith_child(index + 1);
        if(child2->key_count + child1->key_count + 1 > MAX_KEYS) return;
        int new_ind = child1->key_count;
        child1->array[new_ind++] = this->get_ith_key(index);
        for(int i = 0; i < child2->key_count; i++){
            child1->array[new_ind++] = child2->get_ith_key(i);
        }
        if(this->key_count == 1) {
            free(this->array);
            this->array = child1->array;
            this->child_count = 0;
            this->key_count = child1->key_count + child2->key_count + 1;
        }
        else{
            this->remove_at_index(index);
            child1->key_count = child1->key_count + child2->key_count + 1;
            this->children[index] = child1;
            node* temp;
            if(index+1 < this->child_count - 1) temp = this->get_ith_child(index+2);
            for(int i = index+1; i < this->child_count -1 ; i++){
                temp = this->children[i+1];
                this->children[i] = temp;
            }
            (this->child_count)--;
        }
        free(child2);
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
    //assuming children is the right child of place where data is to be inserted
    int index = this->find_index(data);
    this->insert_at_index(true,false, (void*) &data, index);
    this->insert_at_index(false,false, (void*) &children, index + 1);

    //split after adding
    if(this->key_count > MAX_KEYS){
        int head_data = this->array[MIN_KEYS];
        node* new_node = this->split();
        
        if(this->parent){
            this->parent->insert(head_data, new_node, root);
        }
        else{
            node* head_node = new node();
            *(root) = head_node;

            //left child is set here
            head_node->children[0] = this;
            (head_node->child_count)++;

            //set parent for both
            this->parent = head_node;
            new_node->parent = head_node;

            head_node->insert(head_data, new_node, root);
        }
    }
}

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

BTree::BTree(){
    this->root = NULL;
}

//requires root NON NULL
void BTree::insert_data_child(data* child, int index){
    if(this->root != NULL) this->root->insert_at_index(false,true,(void*) child, index);
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

bool BTree::search(int data){
    return this->search_util(this->root, data);
}

bool BTree::search_util(node* cur, int data){
    if(cur != NULL){
        if(data < cur->get_ith_key(0)) this->search_util(cur->get_ith_child(0), data);
        for(int i = 0; i < cur->key_count; i++){
            if(data == cur->get_ith_key(i)) return true;
            this->search_util(cur->get_ith_child(i + 1), data);
        }
    }
    return false;
}

void BTree::del(int data){
    this->del_util(this->root, data);
}

void BTree::del_util(node* cur, int data){
    if(cur == NULL || cur->key_count == 0) return;
    if(data < cur->get_ith_key(0)) {
        cout<<"route to first child"<<endl;
        this->del_util(cur->get_ith_child(0), data);
    }
    else if(data > cur->get_ith_key(cur->key_count - 1)){
        cout<<"Route to last child"<<endl;
        this->del_util(cur->get_ith_child(cur->child_count - 1),data);
    }
    else 
        for(int i = 0 ; i < cur->key_count; i++){

            //actual del
            if(cur->get_ith_key(i) == data){
                cout<<"key with data found"<<endl;

                //case 1: leaf
                if(cur->is_leaf()){

                    //case 1: safe to remove
                    if(cur->key_count > MIN_KEYS) {
                        cout<<"Safe to remove from leaf"<<endl;
                        cur->remove_at_index(i);
                    }
                    
                    //case 2 : merge or borrow or free
                    else{
                        //case 2a : borrow or merge
                        if(cur->parent){
                            int child_index = cur->get_child_index();
                            if(child_index == -1) cout<<"Error : child index not found\n";

                            node *left_sibling = NULL, *right_sibling = NULL;
                            if(child_index > 0){
                                left_sibling = cur->parent->get_ith_child(child_index - 1);
                            }
                            if(child_index < ((cur->parent->child_count) - 1)){
                                right_sibling = cur->parent->get_ith_child(child_index + 1);
                            }
                            if(left_sibling == NULL && right_sibling == NULL) cout<<"Error : only one child of parent\n";
                            else{

                                //borrow cases
                                //borrow from left sibling
                                if(left_sibling && left_sibling->key_count > MIN_KEYS){
                                    cout<<"Borrow from left sibling"<<endl;
                                    cur->remove_at_index(i);
                                    cur->insert(cur->parent->get_ith_key(child_index - 1), 
                                                NULL,
                                                &(this->root));
                                    cur->parent->array[child_index - 1] = left_sibling->get_ith_key(left_sibling->key_count - 1);
                                    left_sibling->remove_at_index(left_sibling->key_count - 1);
                                }

                                //borrow from right sibling
                                else if(right_sibling && right_sibling->key_count > MIN_KEYS){
                                    cout<<"Borrow from right sibling"<<endl;
                                    cur->remove_at_index(i);
                                    cur->insert(cur->parent->get_ith_key(child_index), 
                                                NULL,
                                                &(this->root));
                                    cur->parent->array[child_index] = right_sibling->get_ith_key(0);
                                    right_sibling->remove_at_index(0);
                                }

                                //merge cases
                                //left_sibling->key_count + (cur->key_count - 1) + 1(for the parent ka key)
                                else if(left_sibling && (left_sibling->key_count + cur->key_count<= MAX_KEYS)){
                                    cout<<"Merge with left sibling"<<endl;
                                    cur->remove_at_index(i);
                                    cur->parent->merge(child_index - 1);
                                }

                                //merge with right sibling
                                else if(right_sibling && (right_sibling->key_count + cur->key_count<= MAX_KEYS)){
                                    cout<<"Merge with right sibling"<<endl;
                                    cur->remove_at_index(i);
                                    cur->parent->merge(child_index);
                                }
                            }
                        }

                        //case 2b : means only root node, therefore simply del
                        else {
                            cout<<"safely remove from root"<<endl;
                            cur->remove_at_index(i);
                            if(cur->key_count == 0) {
                                this->root == NULL;
                                free(cur);
                            }
                        }
                    }
                }

                //case 2: go to leaf
                else{
                    cout<<"Route to leaf"<<endl;
                    if(cur->get_ith_child(i+1)->key_count > cur->get_ith_child(i)->key_count){
                        cur->array[i] = cur->get_ith_child(i+1)->get_ith_key(0);
                        this->del_util(cur->get_ith_child(i+1),cur->array[i]);
                    }else{
                        cur->array[i] = cur->get_ith_child(i)->get_ith_key(cur->get_ith_child(i)->key_count - 1);
                        this->del_util(cur->get_ith_child(i),cur->array[i]);
                    }
                }
            }

            //ow route
            else if(cur->get_ith_key(i) < data && data < cur->get_ith_key(i + 1)) {
                cout<<"key not found, route"<<endl;
                this->del_util(cur->get_ith_child(i+1), data);
            }
        }
    
}

void BTree::print(){
    if(this->root == NULL) return;
    this->root->print();
    cout<<endl;
}

node* BTree::route(int data){
    if(this->root == NULL) return NULL;
    return this->route_util(this->root, data);
}

node* BTree::route_util(node* cur, int data){
    if(cur == NULL || cur->key_count == 0) return NULL;
    if(cur->is_leaf()) return cur;
    if(data < cur->get_ith_key(0)) return this->route_util(cur->get_ith_child(0), data);
    else if(data >= cur->get_ith_key(cur->key_count - 1)) return this->route_util(cur->get_ith_child(cur->child_count - 1), data);
    else 
        for(int i = 0; i < cur->key_count - 1; i++){
            if( cur->get_ith_key(i) <= data && data < cur->get_ith_key(i+1)) return this->route_util(cur->get_ith_child(i), data);
        }
}


class BPTree{
private:
    BTree* tree;
    data*  start;
public:
    BPTree();
    void insert(int value);
    void print();
};

BPTree::BPTree(){
    this->tree = NULL;
    this->start = NULL;
}

void BPTree::insert(int value){
    if(this->tree == NULL){
        if(this->start == NULL) this->start = new data();
        this->start->insert(value, &(this->tree));
    }else{
        node* root = this->tree->route(value);
        if(root == NULL) cout<<"Error : non null tree returned null route\n";
        else{
            if(root->data_child_count == 0) cout<<"Error : routed root has 0 data_child_count\n";
            else{
                if(value < root->get_ith_key(0)) root->get_ith_data_child(0)->insert(value, &(this->tree));
                else if(value >= root->get_ith_key(root->key_count - 1)) root->get_ith_data_child(root->data_child_count-1)->insert(value, &(this->tree));
                else {
                    for (int i = 0; i < root->key_count - 1; i++){
                        if( root->get_ith_key(i) <= value && value < root->get_ith_key(i+1)) root->get_ith_data_child(i)->insert(value, &(this->tree));
                    }
                }
            }
        }
    }
}

void BPTree::print(){
    if(this->tree) this->tree->print();
    else
        if(this->start){
            data* cur = this->start;
            while(cur){
                cur->print();
                cur = cur->next;
            }
            cout<<endl;
        }
}

int main(int argc, char const *argv[])
{
    BPTree tree = BPTree();
    cout<<"MAX_KEYS : "<<MAX_KEYS<<" MIN_KEYS : "<<MIN_KEYS<<endl;
    cout<<"MAX_DATA_KEYS : "<<MAX_DATA_KEYS<<" MIN_DATA_KEYS : "<<MIN_DATA_KEYS<<endl;
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
        // case 'd' :
        //     tree.del(temp);
        //     tree.print();
        //     break;
        // case 's' :

        //     cout<<"Search result for "<<temp<<" : "<<tree.search(temp)<<endl; 
        //     break;
        case 'p' :
            tree.print();
            break;
        default:
            break;
        }
    }
    return 0;
}