#include "graphutils.h"

int main(){
    printf("Testing Utils -> GdbVector...\n");
    GdbVector<int>v(4);
    printf("vector size = %lu\n",v.size());
    v.display();
    v.add(1);
    v.add(2);
    v.add(3);
    v.add(4);
    v.display();
    v.add(5); // fire a resize here 
    v.display();
    printf("vector size = %lu\n",v.size());
    
    return 0;
}