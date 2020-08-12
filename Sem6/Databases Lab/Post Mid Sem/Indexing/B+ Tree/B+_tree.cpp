#include <iostream>
#include <bits/stdc++.h>
#include "B+_tree.h"
using namespace std;
#define T 2
#define D 2
#define MAX_KEYS (2*T-1)
#define MIN_KEYS (T-1)
#define MAX_DATA_KEYS (2*D)
#define MIN_DATA_KEYS (D)

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

bool BPTree::search(int value){
    if(this->tree) {
        node* root = this->tree->route(value);
        
    }
    else{
        if(start){
            if(value > this->start->get_ith_key(this->start->key_count - 1)) {
                if(this->start->next) return this->start->next->search(value);
            }
            else return this->start->search(value);
        }
    }
    return false;
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