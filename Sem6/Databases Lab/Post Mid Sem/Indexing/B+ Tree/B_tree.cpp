#include <iostream>
#include <bits/stdc++.h>
using namespace std;
#include "B_tree.h"
#define T 2
#define D 2
#define MAX_KEYS (2*T-1)
#define MIN_KEYS (T-1)
#define MAX_DATA_KEYS (2*D)
#define MIN_DATA_KEYS (D)

BTree::BTree(){
    this->root = NULL;
}

//requires root NON NULL
void BTree::insert_data_child(data* child, int index){
    if(this->root != NULL) this->root->insert_at_index(false,true,(void*) &child, index);
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

