#include "graphutils.h"

int main(){
    printf("Testing Utils -> GdbVector...\n");

    GdbVector<int>v((int)4);
    
    printf("vector size = %lu\n",v.size());
    v.display();
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.display();
    v.push_back(5); // fire a resize here 
    v.display();
    printf("vector size = %lu\n",v.size());
    
    return 0;
}