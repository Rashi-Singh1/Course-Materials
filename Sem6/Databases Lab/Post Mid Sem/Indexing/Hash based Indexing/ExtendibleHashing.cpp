#include <iostream>
#include <math.h>
#include <bitset>
#include <string>
using namespace std;
#define BUCKET_SIZE 3

class node
{
private:
public:
    node* next;
    int data;
    node(int data){
        this->next = NULL;
        this->data = data;                 
    }
};

//represents a bucket, not the directory
class list{
private:
public:
    int local_depth;
    int count;
    node* head;
    list();
    bool safe_to_insert(){
        return this->count < BUCKET_SIZE;
    }
    void insert(int key);                       //simply adds if possible else ignores
    void insert_node(node* new_node);
    node* search(int key);
    void remove(node* prev, node* cur);         //same as delete but do not free space
    void print();
    void clear(){

    }
    list* copy_list();
};

list::list(){
    this->local_depth = 1;
    this->count = 0;
    this->head = NULL;
}

void list::insert(int key){                     //simply adds if possible else ignores
    if(safe_to_insert()){
        node* new_node = new node(key);
        if(this->head) new_node->next = this->head;
        this->head = new_node;
        (this->count)++;
    }
}

void list::insert_node(node* new_node){
    if(safe_to_insert()){
        if(this->head) new_node->next = this->head;
        this->head = new_node;
        (this->count)++;
    }
}

node* list::search(int key){
    node* cur = this->head;
    while(cur){
        if(cur->data == key) return cur;
        cur = cur->next;
    }
    return NULL;
}

void list::remove(node* prev, node* cur){       //same as delete but do not free space
    if(prev) prev->next = cur->next;
    else this->head = cur->next;
    (this->count)--;
}

void list::print(){
    node* cur = head;
    while(cur){
        cout<<cur->data<<" ";
        cur = cur->next;
    }
}

list* list::copy_list(){
    node* cur = this->head;
    list* new_list = new list();

    while(cur){
        new_list->insert_node(cur);
        cur = cur->next;
    }

    //reverse the list
    cur = new_list->head;
    node* prev = NULL;
    node* next = cur->next;
    while(cur){
        cur->next = prev;
        prev = cur;
        cur = next;
        next = cur == NULL ? NULL : cur->next;
    }
    new_list->head = prev;
    return new_list;
}

//contains the directory
class Hash{
private:
    int global_depth;
    list** dir;
    string get_binary(int key);

public:
    Hash();
    int hash_func(int key);
    void insert(int key);
    node* search(int key);
    void del(int key){}
    bool is_index_entry_null(int index);
    void clear(){}
    void print();
    void double_dir();
    int get_index_type_0(int index);
    int get_index_type_1(int index);
    int get_dir_size();
    list* split_list(int base, int local_depth);
    void redistribute_data(list* type1, list* type2, int base, int local_depth);
};

Hash::Hash(){
    this->global_depth = 1;
    this->dir = new list*[2];                 //actually size of list is pow(2,global_depth)
    for (int i = 0; i < 2; i++)
    {
        this->dir[i] = NULL;
    }
    
}

int Hash::get_dir_size(){
    return pow(2,this->global_depth);
}

string Hash::get_binary(int key){
    if(key < 0) return "";
    string output = "";
    while(key){
        output=(char)((key % 2)+'0') + output;
        key/=2;
    }
    output = output == "" ? "0" : output;
    return output;
}

int Hash::hash_func(int key){
    int factor = get_dir_size();
    return ((key % (factor)) + factor) % (factor);    
}

bool Hash::is_index_entry_null(int index){
    return (this->dir[index] == NULL ? true : false);
}

//this local_depth is before splitting
list* Hash::split_list(int base, int local_depth){
    if(this->is_index_entry_null(base)) return NULL;
    int index1 = base;
    int index2 = pow(2,local_depth) + base;
    int divide = pow(2,local_depth + 1);
    list* new_list = new list();
    for(int i = 0;i < this->get_dir_size();i++){
        if(i % divide == index2) this->dir[i] = new_list;
    }
    return new_list;
}

//this local_depth is after splitting (i.e. 1 + past value) 
void Hash::redistribute_data(list* type1, list* type2, int base, int local_depth){
    if(type1 == NULL || type2 == NULL) return;
    int index1 = base;
    int index2 = pow(2,local_depth - 1) + base;
    int divide = pow(2,local_depth);
    cout<<"in redistribution num1 : "<<index1<<" num2 : "<<index2<<endl;
    node* cur = type1->head;
    node* prev = NULL;
    node* next = cur == NULL ? NULL : cur->next;
    while(cur){
        if(cur->data % divide == index2) {
            type1->remove(prev,cur);
            cur->next = NULL;
            type2->insert_node(cur);
        }
        prev = cur;
        cur = next;
        next = cur == NULL ? NULL : cur->next; 
    }
}

void Hash::insert(int key){
    int index = this->hash_func(key);

    //case 1 : not initialized 
    if(this->is_index_entry_null(index)){
        cout<<"Entering initialize and insert case\n";
        this->dir[index] = new list();
        this->dir[index]->insert(key);
    }

    //case 2 : no splitting
    else if(this->dir[index]->safe_to_insert()){
        cout<<"Entering safe to insert case\n";
        this->dir[index]->insert(key);
    }

    else{
        int local_depth = this->dir[index]->local_depth;

        //case 3 : split the cur list into two parts and redistribute data
        if(local_depth < this->global_depth){   
            cout<<"Entering splitting case\n";       
            list* type1 = this->dir[index];
            list* type2 = NULL;
            int base = index % (int)pow(2, type1->local_depth);
            type2 = this->split_list(base, type1->local_depth);
            this->print();
            if(type2 == NULL) cout<<"Error : new list is NULL\n";
            else{
                (type1->local_depth)++;
                type2->local_depth = type1->local_depth;
                cout<<"Performing redistribution\n";
                this->redistribute_data(type1, type2, base, type1->local_depth);
                this->print();
                this->insert(key);
            }
        }

        //case 4 : double the dir size
        else{       
            cout<<"Entering doubling the dir case\n";
            this->double_dir();
            this->print();
            this->insert(key);
        }
    }

}

node* Hash::search(int key){
    int index = this->hash_func(key);
    if(!(this->is_index_entry_null(index))){
        return this->dir[index]->search(key);
    }
    return NULL;
}

//both these functions assume dir is not doubled yet
int Hash::get_index_type_0(int index){
    return index > this->get_dir_size() ?  index - this->get_dir_size()  : index;
}

int Hash::get_index_type_1(int index){
    return index < this->get_dir_size() ? this->get_dir_size() + index : index;
}

void Hash::double_dir(){
    int old_size = this->get_dir_size();
    list** new_dir = new list* [(int)pow(2,this->global_depth + 1)];
    for(int i = 0; i < old_size;i++)
    {
        int index0 = i;
        int index1 = this->get_index_type_1(i);
        if(!(this->is_index_entry_null(i))){                                //if invalid do nothing bcs 1.... and 0.... already NULL in new_dir
            new_dir[index0] = new_dir[index1] = this->dir[i];
        }
        else{
            new_dir[index0] = NULL;
            new_dir[index1] = NULL;
        }
    }
    (this->global_depth)++;
    this->clear();                                                          //free the space of dir
    this->dir = new_dir;                                                  
}

void Hash::print(){
    cout<<"d:"<<this->global_depth<<endl;
    for( int i = 0; i < this->get_dir_size() ; i++)
    {
        cout<<this->get_binary(i)<<"     (d:";
        this->is_index_entry_null(i) ? cout<<"0)" : cout<<this->dir[i]->local_depth<<") : ";
        if(!(this->is_index_entry_null(i))) this->dir[i]->print();
        cout<<endl;
    }
} 

int main(int argc, char const *argv[])
{
    Hash hash = Hash();

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
            hash.insert(temp);
            hash.print();
            break;
        case 'd' :
            hash.del(temp);
            hash.print();
            break;
        case 's' :

            cout<<"Search result for "<<temp<<" : "; hash.search(temp) != NULL ? cout<<(hash.search(temp))->data<<endl : cout<<"NULL"<<endl;
            break;
        default:
            break;
        }
    }
    
    return 0;
}
