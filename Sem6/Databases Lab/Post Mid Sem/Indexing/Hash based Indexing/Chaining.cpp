#include <iostream>
using namespace std;

class node{
    public:
        int data;
        node* next;
        node(int data){
            this->data = data;
            this->next = NULL;
        }
};

class List{
    public:
        node* head;
        int count;                                                      //not necessary
        List(){
            this->head = NULL;
            this->count = 0;
        }

        void insert(int key){
            if(this->head == NULL) this->head = new node(key);      //free this in clear
            else {
                node* new_node = new node(key);                     //free this in clear
                new_node->next = this->head;
                this->head = new_node;
            }
            (this->count)++;
        }

        bool search(int key){
            node* cur = this->head;
            while(cur){
                if(cur->data == key) return true;
                cur = cur->next;
            }
            return false;
        }

        void del(int key){
            node* cur = this->head;
            node* prev = NULL;
            while(cur){
                if(cur->data == key){
                    if(prev) prev->next = cur->next;
                    else this->head = cur->next;
                    free(cur);
                    break;
                }
                prev = cur;
                cur = cur->next;
            }
            (this->count)--;
        }

        void print(){
            node* cur = this->head;
            while(cur){
                cout<<cur->data<<" ";
                cur = cur->next;
            }
            cout<<endl;
        }

        void clear(){

        }
};

class Hash{
    private:
        int size;
        List** array;
    public:
        Hash(int size){
            this->size = size;
            this->array = new List* [this->size];                                   //free this in clear
        }

        int hash_func(int key){
            return ((key % (this->size)) + this->size) % (this->size);    
        }

        bool is_index_non_empty(int index){
            return (this->array[index] == NULL ? false : true);
        }

        void insert(int key){
            int index = hash_func(key);
            if(!this->is_index_non_empty(index)) (this->array)[index] = new List();     //free this in clear
            (this->array)[index]->insert(key);
        }

        bool search(int key){
            int index = hash_func(key);
            if(this->is_index_non_empty(index)) return this->array[index]->search(key);
            return false;
        }

        void del(int key){
            int index = hash_func(key);
            if(is_index_non_empty(index)) {
                this->array[index]->del(key);
                if(this->array[index]->head == NULL) {
                    free(this->array[index]);
                    this->array[index] = NULL;
                }
            }
        }

        void print(){
            for (int i = 0; i < this->size; i++)
            {
                cout<<i<<" : "<<endl;
                if(this->is_index_non_empty(i)) {
                    cout<<"\t";
                    (this->array)[i]->print();
                }
            }
            cout<<endl;
        }

        void clear(){

        }
};


int main(int argc, char const *argv[])
{
    int size;
    cout<<"Enter hash table size : "<<endl;
    cin>>size;
    Hash hash = Hash(size);

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
            cout<<"Search result for "<<temp<<" : "<<hash.search(temp)<<endl;
            break;
        default:
            break;
        }
    }
    
    return 0;
}
