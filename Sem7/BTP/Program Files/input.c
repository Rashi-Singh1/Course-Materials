#include <stdio.h>

int main(){
    int x = 0, y = 2;
    for(int i = 0 ; i < 2; i++){
        y += x;
        x++;
    }
    if(x == 0) printf("x is equal to 0");
    else if(x == 1) printf("x is equal to 1");
    else printf("x is not 0 or 1");
    return 0;
}


