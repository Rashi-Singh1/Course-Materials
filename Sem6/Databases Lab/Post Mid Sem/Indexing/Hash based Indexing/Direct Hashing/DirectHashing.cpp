#include <iostream>
using namespace std;

//  In direct hashing we assume that the universe of keys is sufficiently small
//  Therefore, H(x) = x
//  ****Here there is no need of chaining****

class Hash{

private:
    int size;
    bool* array;

public:
    Hash(int size);             //constructor
    void insert(int key);       //inserts value in the hash table
    bool search(int key);       //returns true if value is present
    bool is_valid_key(int key); //0 <= key < size
    void del(int key); 
    void print();      
};

bool Hash::is_valid_key(int key)
{
    return (key >= 0 and key < this->size);
}

Hash::Hash(int size)
{
    this->size = size;
    if(this->size > 0){
        array = (bool*) malloc(sizeof(bool) * size);
    }
}

void Hash::insert(int key)
{
    if(this->is_valid_key(key))
    {
        (this->array)[key] = true;
    }
}

bool Hash::search(int key)
{
    if(this->is_valid_key(key))
    {
        return (this->array)[key];
    }
    return false;
}

void Hash::del(int key)
{
    if(this->is_valid_key(key))
    {
        (this->array)[key] = false;
    }
}

void Hash::print()
{
    for (int i = 0; i < this->size; i++)
    {
        cout<<(this->array)[i]<<" ";
    }
    cout<<endl<<endl;
}

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
            cout<<"Search result for "<<temp<<" : "<<hash.search(temp);
            break;
        default:
            break;
        }
    }
    
    return 0;
}
