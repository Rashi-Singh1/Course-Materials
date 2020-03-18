#include <iostream>
#include <bits/stdc++.h>
using namespace std;
#define M 5
//  In direct hashing we assume that the universe of keys is sufficiently small
//  Therefore, H(x) = x
//  ****Here there is no need of chaining****

class Hash{

private:
    int size;
    bitset<M> bset;

public:
    Hash();                     //constructor
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

Hash::Hash()
{
    this->size = M;
}

void Hash::insert(int key)
{
    if(this->is_valid_key(key))
    {
        (this->bset)[key] = 1;
        //alter :  (this->bset).set(key);
    }
}

bool Hash::search(int key)
{
    if(this->is_valid_key(key))
    {
        return (this->bset)[key];
    }
    return false;
}

void Hash::del(int key)
{
    if(this->is_valid_key(key))
    {
        (this->bset)[key] = 0;
        //alter : (this->bset).reset(key);
    }
}

void Hash::print()
{
    for (int i = 0; i < this->size; i++)
    {
        cout<<(this->bset)[i]<<" ";
        //alter : cout<<(this->bset).test(i)<<" ";
    }
    cout<<endl<<endl;
}

int main(int argc, char const *argv[])
{
    int size;
    cout<<"Enter hash table size : "<<endl;
    cin>>size;
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
            cout<<"Search result for "<<temp<<" : "<<hash.search(temp)<<endl;
            break;
        default:
            break;
        }
    }
    
    return 0;
}
