#include "graphutils.h"

int main(){
    printf("Testing Utils -> GdbVector...\n");

    GdbVector<int>v((int)4);
    
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    if(v.empty()){
        printf("Vector is empty\n");
    }
    else{
        printf("Vector is not empty\n");
    }
    v.display();
    v.pushBack(1);
    v.pushBack(2);
    v.pushBack(3);
    v.pushBack(4);
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    v.display();
    v.pushBack(5); // fire a resize here 
    v.display();
    v.insert(2,99); // in position 2 insert element 99
    v.display();
    v.fastInsert(2,88); // in position 2 fast_insert element 88
    v.display();
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    if(v.empty()){
        printf("Vector is empty\n");
    }
    else{
        printf("Vector is not empty\n");
    }
    v.erase(6);
    v.display();
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    v.erase(2,1);
    v.display();
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    v.erase(1,v.toEnd(1));
    v.display();
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    v[0] = 5;
    v.display();
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    v.pushBack(6);
    v.pushBack(7);
    v.display();
    printf("vector size = %lu\n",v.size());
    printf("vector maxSize = %lu\n",v.maxSize());
    GdbVector<int>v2((int)4);
    v2.pushBack(5);
    v2.pushBack(6);
    v2.pushBack(7);
    v2.display();
    printf("vector2 size = %lu\n",v2.size());
    printf("vector2 maxSize = %lu\n",v2.maxSize());
    assert(v==v2);
    printf("Equality operator passed!\n");
    GdbVector<int>v3((int)4);
    v3.pushBack(5);
    v3.pushBack(8);
    v3.pushBack(7);
    assert(v3!=v2);
    printf("Inquality operator passed!\n");
    v2.swap(0,2);
    v2.display();
    printf("vector2 size = %lu\n",v2.size());
    printf("vector2 maxSize = %lu\n",v2.maxSize());

    printf("\nTesting Singleton Logger ...\n");
    GdbLoggerMain::Instance()->Log("This is a generic log message");

    
    return 0;
}