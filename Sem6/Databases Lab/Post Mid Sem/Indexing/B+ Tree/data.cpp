#include <iostream>
#include <bits/stdc++.h>
#include "B_tree.h"
using namespace std;
#define T 2
#define D 2
#define MAX_KEYS (2*T-1)
#define MIN_KEYS (T-1)
#define MAX_DATA_KEYS (2*D)
#define MIN_DATA_KEYS (D)

data::data(){
    this->array = new int [MAX_DATA_KEYS + 1];
    this->key_count = 0;
    this->next = NULL;
    this->prev = NULL;
    this->parent = NULL;
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
            if(this->parent) {
                this->parent->set_data_child(head_data, this->next);
            }
            else{
                (*tree) = new BTree();
                (*tree)->insert(head_data);
                (*tree)->insert_data_child(this, 0);
                (*tree)->insert_data_child(this->next, 1);
                this->parent = (*tree)->root;
                this->next->parent = (*tree)->root;
            }
        }
    }
}

void data::print(){
    for(int i = 0 ; i < this->key_count; i++) cout<<this->array[i]<<" ";
    cout<<" -> ";
}