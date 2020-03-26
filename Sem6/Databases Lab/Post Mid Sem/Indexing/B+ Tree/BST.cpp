#include <iostream>
using namespace std;

class node{
private:
public:
    int data;
    node* left;
    node* right;
    node(int data){
        this->data = data;
        this->left = NULL;
        this->right = NULL;
    }
};

class BST{
private:
    node* root;
    void insert_util(node* cur, int data);
    void print_util(node* cur);
    void search_util(node* cur, int data, node** output);
    void del_util(node* parent, node* cur, int data);
    bool is_leaf(node* cur);
    bool is_cur_left_of_parent(node* parent, node* cur);
public:
    BST();
    void insert(int data);
    node* search(int data);
    void del(int data);
    void print();
};

//PRIVATE FUNCTIONS
bool BST::is_leaf(node* cur){
    if(cur == NULL) return false;
    if(cur->left == NULL and cur->right == NULL) return true;
    return false;
}

void BST::insert_util(node* cur, int data){
    if(cur == NULL) return;

    //actual insertion
    if(data <= cur->data){
        if(cur->left == NULL) cur->left = new node(data);
        else this->insert_util(cur->left, data);
    }
    else{
        if(cur->right == NULL) cur->right = new node(data);
        else this->insert_util(cur->right, data);
    }
}

void BST::print_util(node* cur){
    if(cur == NULL) return;
    this->print_util(cur->left);
    cout<<cur->data<<" ";
    this->print_util(cur->right);
}

void BST::search_util(node* cur, int data, node** output){
    if(cur == NULL || *output != NULL) return;
    if(cur->data == data) {
        *output = cur;
        return;
    }
    else{
        if(data < cur->data) this->search_util(cur->left, data, output);
        else this->search_util(cur->right, data, output);
    }
}

void BST::del_util(node* parent, node* cur, int data){
    if(cur == NULL) return;
    if(cur->data == data){
        //leaf case
        if(this->is_leaf(cur)) {

            //only one node : the root node
            if(parent == NULL) this->root = NULL;
            else{
                if(this->is_cur_left_of_parent(parent, cur)) parent->left = NULL;
                else parent->right = NULL;
            }
            free(cur);
        }
        else{
            //get left most node on the right side of cur
            if(cur->right != NULL){
                node* leftmost = cur->right;
                node* leftmost_parent = cur;
                while(leftmost && leftmost->left){
                    if(leftmost->left) {
                        parent = leftmost;
                        leftmost = leftmost->left;
                    }
                }
                cur->data = leftmost->data;
                if(leftmost_parent){
                    if(this->is_cur_left_of_parent(leftmost_parent,leftmost)) leftmost_parent->left = leftmost->right;
                    else leftmost_parent->right = leftmost->right;
                }
                else cout<<"Error : leftmost_parent does not exist\n";
                free(leftmost);
            }

            //get right most node on the left side of cur
            //note that, since cur is not a leaf, one of these cases must occur
            else{
                node* rightmost = cur->left;
                node* rightmost_parent = cur;
                while(rightmost && rightmost->right){
                    if(rightmost->right) {
                        parent = rightmost;
                        rightmost = rightmost->right;
                    }
                }
                cur->data = rightmost->data;
                if(rightmost_parent){
                    if(!(this->is_cur_left_of_parent(rightmost_parent,rightmost))) rightmost_parent->right = rightmost->left;
                    else rightmost_parent->left = rightmost->right;
                }
                else cout<<"Error : rightmost_parent does not exist\n";
                free(rightmost);
            }
        }
    }
    else{
        if(data < cur->data) this->del_util(cur, cur->left, data);
        else this->del_util(cur, cur->right, data);
    }

}

bool BST::is_cur_left_of_parent(node* parent, node* cur){
    if(parent == NULL || cur == NULL) return false;
    if(parent->left == cur) return true;
    else return false;
}
//PUBLIC FUNCTIONS

BST::BST(){
    this->root = NULL;
}

void BST::insert(int data){
    if(this->root) this->insert_util(this->root,data);
    else this->root = new node(data);
}

node* BST::search(int data){
    node* output = NULL;
    if(this->root) this->search_util(this->root, data, &output);
    return output;
}

void BST::print(){
    if(this->root) this->print_util(this->root);
    cout<<endl;
}

void BST::del(int data){
    if(this->root) this->del_util(NULL, this->root, data);
}

int main(int argc, char const *argv[])
{
    BST bst = BST();

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
            bst.insert(temp);
            bst.print();
            break;
        case 'd' :
            bst.del(temp);
            bst.print();
            break;
        case 's' :

            cout<<"Search result for "<<temp<<" : "; bst.search(temp) != NULL ? cout<<(bst.search(temp))->data<<endl : cout<<"NULL"<<endl;
            break;
        case 'p' :
            bst.print();
            break;
        default:
            break;
        }
    }
    
    return 0;
}