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
    bool safe_to_insert();    
    void insert(int key);                       //simply adds if possible else ignores
    void insert_node(node* new_node);
    node* search(int key);
    void del(int key);
    void remove(node* prev, node* cur);         //same as delete but do not free space
    void print();
    void clear(){}
    list* copy_list();
    void insert_node_anyway(node* new_node);
};

bool list::safe_to_insert(){
    return this->count < BUCKET_SIZE;
}

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

void list::insert_node_anyway(node* new_node){
    if(this->head) new_node->next = this->head;
    this->head = new_node;
    (this->count)++;
}


node* list::search(int key){
    node* cur = this->head;
    while(cur){
        if(cur->data == key) return cur;
        cur = cur->next;
    }
    return NULL;
}

void list::del(int key){
    node* cur = this->head;
    node* prev = NULL;
    while(cur){
        if(cur->data == key){
            if(prev) prev->next = cur->next;
            else this->head = cur->next;
            (this->count)--;
            free(cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }
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
    int hash_func(int key);
    bool is_index_entry_null(int index);
    int get_local_depth(int index);
    int get_index_type_0(int index);
    int get_index_type_1(int index);
    int get_count(int index);
    int get_dir_size();
    void double_dir();
    list* split_list(int index);
    void redistribute_data(list* type1, list* type2, int index);
    void combine_list(int index1, int index2, int key);
    void half_dir();
public:
    Hash();
    void insert(int key);
    node* search(int key);
    void del_without_compaction(int key);
    void del_with_compaction(int key);
    void clear(){}
    void print();
};

//PRIVATE FUNCTIONS :
int Hash::get_dir_size(){
    return pow(2,this->global_depth);
}

int Hash::get_count(int index){
    return this->is_index_entry_null(index) ? 0 : this->dir[index]->count;
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
/*
    Logic : 
    If i have 000 and 100 (i.e. local depth = 2) pointing to same list
    then base : 00 and index1 : 000 and index2 : 100 and divide : 1000
    therefore for all indices in the table, whenever we get last three bits as index2 (100), point it to an empty list
    To check the last three bits, take mod wrt divide
*/
list* Hash::split_list(int index){
    if(this->is_index_entry_null(index)) return NULL;
    int index1 = this->get_index_type_0(index);
    int index2 = this->get_index_type_1(index);
    int divide = pow(2,this->get_local_depth(index) + 1);
    list* new_list = new list();
    for(int i = 0;i < this->get_dir_size();i++){
        if(i % divide == index2) this->dir[i] = new_list;
    }
    return new_list;
}

//this local_depth is after splitting (i.e. 1 + past value) 
/*
    Logic : 
    If i have 000 and 100 (i.e. local depth = 2) pointing to same list
    then base : 00 and index1 : 000 and index2 : 100 and divide : 1000
    i have type2 as the empty list to which the indices ending with index2 point
    now just check, if for the current key, we get the index as index2, then move it to type2 list
*/
void Hash::redistribute_data(list* type1, list* type2, int index){
    if(type1 == NULL || type2 == NULL) return;
    int index1 = this->get_index_type_0(index);
    int index2 = this->get_index_type_1(index);
    int divide = pow(2,this->get_local_depth(index) + 1);
    // cout<<"in redistribution num1 : "<<index1<<" num2 : "<<index2<<endl;
    node* cur = type1->head;
    node* prev = NULL;
    node* next = cur == NULL ? NULL : cur->next;
    while(cur){
        if(cur->data % divide == index2) {                                  //checking which list it belongs to
            type1->remove(prev,cur);
            cur->next = NULL;
            type2->insert_node(cur);
        }
        prev = cur;
        cur = next;
        next = cur == NULL ? NULL : cur->next;                              //this is needed, bcs we are reusing nodes, so there next is changing, so cant use cur = cur->next
    }
}

int Hash::get_local_depth(int index){
    return this->is_index_entry_null(index) ? 0 : this->dir[index]->local_depth;
}

//both these functions assume dir is not split yet
//this gives the base, i.e the value of the local_depth no of bits from end(of this index)
//ex for 100 and local_depth 2, it will 00
int Hash::get_index_type_0(int index){
    return index % (int)pow(2, this->get_local_depth(index));
}

//this gives the 1_index type entry
//for the above example, it will 100 type entry
int Hash::get_index_type_1(int index){
    return pow(2,this->get_local_depth(index)) + this->get_index_type_0(index);
}

/*
    Logic : once doubled, to map the entries do - 
    if in old table some entry is NULL, in new table 0_old_entry ans 1_old_entry both would be NULL
    if not NULL, then 0_old_entry and 1_old_entry both would point to the old entry
*/
void Hash::double_dir(){
    int old_size = this->get_dir_size();
    list** new_dir = new list* [(int)pow(2,this->global_depth + 1)];
    for(int i = 0; i < old_size;i++)
    {
        int index0 = i;
        int index1 = index0 + old_size;                                     //index of 1_old_entry
        if(!(this->is_index_entry_null(i))){                                //if invalid do nothing bcs 1.... and 0.... already NULL in new_dir
            new_dir[index0] = new_dir[index1] = this->dir[i];
        }
        else{
            new_dir[index0] = NULL;
            new_dir[index1] = NULL;
        }
    }
    (this->global_depth)++;
    free(this->dir);                                                        //free the space of dir
    this->dir = new_dir;                                                  
}

void Hash::combine_list(int index1, int index2, int key){
    // cout<<"Entering combine list"<<endl;
    if(this->is_index_entry_null(index1) || this->is_index_entry_null(index2)) return;
    node* cur = this->dir[index2]->head;
    node* next = cur == NULL ? NULL : cur->next;
    int final_local_depth = max(0,max(this->get_local_depth(index1), this->get_local_depth(index2)) - 1);
    int final_count = this->get_count(index1) + this->get_count(index2);
    while(cur){
        // cout<<"cur : "<<cur->data<<endl;
        // cout<<"next : "; next == NULL ? cout<<"NULL"<<endl<<endl : cout<<next->data<<endl<<endl;
        cur->next = NULL;
        this->dir[index1]->insert_node_anyway(cur);
        cur = next;
        next = cur == NULL ? NULL : cur->next;
    }
    free(this->dir[index2]);    
    this->dir[index2] = this->dir[index1];
    (this->dir[index1]->local_depth) = final_local_depth;
    this->dir[index1]->count = final_count;
    // this->print();
}

void Hash::half_dir(){
    // cout<<"Entering half_dir vali redistribution"<<endl;
    int half_point = pow(2, this->global_depth - 1);

    //redistribution
    for(int i = half_point ; i < this->get_dir_size() ; i++){
        if(this->is_index_entry_null(i)) this->dir[i] = this->dir[i-half_point];
        else{
            if(this->is_index_entry_null(i-half_point)) this->dir[i-half_point];
            else{
                if(this->dir[i] != this->dir[i-half_point]) cout<<"Both are not equal vala case\n";         //check if this is ever possible and do we need to combine_lists then....if possible
                else continue;
            }
        }
    }
    // this->print();
    // cout<<"Entering half_dir vali resize"<<endl;
    //resize the directory now
    int old_size = this->get_dir_size();
    int new_size = old_size / 2;
    list** new_dir = new list* [new_size];
    for(int i = 0; i < new_size;i++)
    {
        new_dir[i] = this->dir[i];
    }
    (this->global_depth)--;
    free(this->dir);                                                        //free the space of dir
    this->dir = new_dir;   
    // this->print();
}

//PUBLIC FUNCTIONS : 
Hash::Hash(){
    this->global_depth = 1;
    this->dir = new list*[2];                 //actually size of list is pow(2,global_depth)
    for (int i = 0; i < 2; i++)
    {
        this->dir[i] = new list();
    }
    
}

void Hash::insert(int key){
    int index = this->hash_func(key);

    //case 1 : not initialized 
    if(this->is_index_entry_null(index)){
        // cout<<"Entering initialize and insert case\n";
        this->dir[index] = new list();
        this->dir[index]->insert(key);
    }

    //case 2 : no splitting
    else if(this->dir[index]->safe_to_insert()){
        // cout<<"Entering safe to insert case\n";
        this->dir[index]->insert(key);
    }

    else{
        int local_depth = this->dir[index]->local_depth;

        //case 3 : split the cur list into two parts and redistribute data
        if(local_depth < this->global_depth){   
            // cout<<"Entering splitting case\n";       
            list* type1 = this->dir[index];
            list* type2 = NULL;
            type2 = this->split_list(index);
            // this->print();
            if(type2 == NULL) cout<<"Error : new list is NULL\n";
            else{
                // cout<<"Performing redistribution\n";
                this->redistribute_data(type1, type2, index);
                (type1->local_depth)++;
                type2->local_depth = type1->local_depth;
                // this->print();
                this->insert(key);
            }
        }

        //case 4 : double the dir size
        else{       
            // cout<<"Entering doubling the dir case\n";
            this->double_dir();
            // this->print();
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

void Hash::del_without_compaction(int key){
    int index = hash_func(key);
    if(this->search(key) == NULL) return;
    if(!(this->is_index_entry_null(index))) {
        this->dir[index]->del(key);
    }
}

void Hash::del_with_compaction(int key){
    int index = this->hash_func(key);
    if(this->search(key) == NULL) return;
    if(!(this->is_index_entry_null(index))){

        int lower_base = index % (int)pow(2,this->get_local_depth(index) - 1);
        int index1 = lower_base;
        int index2 = lower_base + pow(2,this->get_local_depth(index) - 1);

        //case 1a : delete without compaction
        if(this->global_depth < 2 || this->is_index_entry_null(index1) || this->is_index_entry_null(index2)) {
            // cout<<"Enteing case 1a"<<endl;
            this->del_without_compaction(key);
        }

        //case 2 : delete with compaction
        else if(this->dir[index1] != this->dir[index2] && this->get_local_depth(index1) == this->get_local_depth(index2)){
            // cout<<"Entering del with compaction case"<<endl;
            if((this->get_count(index1) + this->get_count(index2) - 1) <= BUCKET_SIZE){
                this->combine_list(index1, index2, key);

                //check if dir needs to be halved
                bool half_directory = true;
                for (int i = 0; i < this->get_dir_size(); i++)
                {
                    if(this->get_local_depth(i) >= this->global_depth) {
                        half_directory = false;
                        break;
                    }
                }
                if(half_directory) this->half_dir();
                this->del_with_compaction(key);
            }
            else this->del_without_compaction(key);  
        }

        //case 1b : delete without compaction
        else {
            // cout<<"Enteing case 1a"<<endl;
            this->del_without_compaction(key); 
        }
    }
}

void Hash::print(){
    cout<<"d:"<<this->global_depth<<endl;
    for( int i = 0; i < this->get_dir_size() ; i++)
    {
        cout<<i<<" "<<this->get_binary(i)<<"     (d:";
        this->is_index_entry_null(i) ? cout<<"0, c:0, b: )" : cout<<this->get_local_depth(i)<<", c:"<<this->dir[i]->count<<",  b:"<<this->get_index_type_0(i)<<") : ";
        if(!(this->is_index_entry_null(i))) this->dir[i]->print();
        cout<<endl;
    }
    cout<<endl;
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
            hash.del_with_compaction(temp);
            hash.print();
            break;
        case 's' :

            cout<<"Search result for "<<temp<<" : "; hash.search(temp) != NULL ? cout<<(hash.search(temp))->data<<endl : cout<<"NULL"<<endl;
            break;
        case 'p' :
            hash.print();
            break;
        default:
            break;
        }
    }
    
    return 0;
}
