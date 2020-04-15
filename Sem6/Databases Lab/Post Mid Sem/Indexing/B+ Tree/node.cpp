#include <iostream>
#include <bits/stdc++.h>
#include "node.h"
using namespace std;
#define T 2
#define D 2
#define MAX_KEYS (2*T-1)
#define MIN_KEYS (T-1)
#define MAX_DATA_KEYS (2*D)
#define MIN_DATA_KEYS (D)

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
    if(!this->is_leaf() || i >= this->data_child_count || i < 0) return NULL;
    return this->data_children[i];
}

//********************************************************************************//
void node::set_data_child(int head_data, data* child){
    if(this->key_count < MAX_KEYS){
        int index = this->find_index(head_data);
        this->insert_at_index(true,false,(void*) &head_data,index);
        this->insert_at_index(false,true,(void*) &child, index+1);   
        child->parent = this;        
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