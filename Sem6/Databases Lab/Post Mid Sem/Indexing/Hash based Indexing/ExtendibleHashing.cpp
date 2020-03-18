#include <iostream>
#include <math.h>
#include <bitset>
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
    list(){
        this->local_depth = 1;
        this->count = 0;
        this->head = NULL;
    }

    bool safe_to_insert(){
        return this->count < BUCKET_SIZE;
    }

    //simply adds if possible else ignores
    void insert(int key){
        if(safe_to_insert()){
            node* new_node = new node(key);
            if(this->head) new_node->next = this->head;
            this->head = new_node;
            (this->count)++;
        }
    }

    void insert_node(node* new_node){
        if(safe_to_insert()){
            if(this->head) new_node->next = this->head;
            this->head = new_node;
            (this->count)++;
        }
    }

    node* search(int key){
        node* cur = this->head;
        while(cur){
            if(cur->data == key) return cur;
            cur = cur->next;
        }
        return NULL;
    }

    //same as delete but do not free space
    void remove(node* prev, node* cur){
        if(prev) prev->next = cur->next;
        else this->head = cur->next;
        (this->count)--;
    }

    void print(){
        node* cur = head;
        while(cur){
            cout<<cur->data<<" ";
            cur = cur->next;
        }
    }
};

//contains the directory
class Hash{
    private:
        int global_depth;
        list** dir;
        string get_binary(int num){
            bitset<32> bset (num);
            string bin = bset.to_string();
            int k = -1;
            for(int i = 0 ; i < bin.size() ; i++){
                if(bin[i] = '0') k++;
                else break;
            }
            return k < bin.size()-1 ? bin.substr(k + 1) : "0";
        }

        string get_binary2(int key){
            if(key < 0) return "";
            string output = "";
            while(key){
                output=(char)((key % 2)+'0') + output;
                key/=2;
            }
            output = output == "" ? "0" : output;
            return output;
        }

    public:
        Hash();
        int hash_func(int key);
        int get_factor();
        void insert(int key);
        node* search(int key);
        void del(int key){}
        bool is_valid_index(int index);
        void clear(){}
        void print();

};

Hash::Hash(){
    this->global_depth = 1;
    this->dir = new list*[2];                 //actually size of list is pow(2,global_depth)
    for (int i = 0; i < 2; i++)
    {
        this->dir[i] = NULL;
    }
    
}

int Hash::get_factor(){
    return pow(2,this->global_depth);
}

int Hash::hash_func(int key){
    return ((key % (this->get_factor())) + this->get_factor()) % (this->get_factor());    
}

bool Hash::is_valid_index(int index){
    return (this->dir[index] == NULL ? false : true);
}

void Hash::insert(int key){
    int index = this->hash_func(key);

    //case 1 : not initialized 
    if(!(this->is_valid_index(index))){
        this->dir[index] = new list();
        this->dir[index]->insert(key);
    }

    //case 2 : no splitting
    else if(this->dir[index]->safe_to_insert()){
        this->dir[index]->insert(key);
    }

    else{
        int local_depth = this->dir[index]->local_depth;
        if(local_depth < this->global_depth){                       //case 3 : split the cur list into two parts
            int new_index = pow(2,local_depth) + index;
            if(!(this->is_valid_index(new_index))){                 //new index should be empty 1.... part
                this->dir[new_index] = new list();
                node* cur = this->dir[index]->head;
                node* prev = NULL;
                while(cur){
                    int cur_index = this->hash_func(cur->data);
                    if(cur_index == index) ;                        //nothing to do as the cur node belongs in 0.... part
                    else{
                        this->dir[index]->remove(prev,cur);                             //remove from 0.... part
                        if(this->dir[index]->count == 0) this->dir[index] = NULL;
                        this->dir[new_index]->insert_node(cur);                         //add to 1.... part
                    }
                    prev = cur;
                    cur = cur->next;
                } 
                this->insert(key);                                  //try inserting after splitting once                                               
            }
            else cout<<"Error : new index already has something\n";
        }
        else{                                                       //case 4 : double the dir size
            
        }
    }

}

node* Hash::search(int key){
    int index = this->hash_func(key);
    if(this->is_valid_index(index)){
        return this->dir[index]->search(key);
    }
    return NULL;
}

void Hash::print(){
    for( int i = 0; i < this->get_factor() ; i++)
    {
        cout<<this->get_binary2(i)<<" : ";
        if(this->is_valid_index(i)) this->dir[i]->print();
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
