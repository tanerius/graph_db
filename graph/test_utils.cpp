#include "graphutils.h"

int main(){
    
    printf("Testing Utils...\n");
    
    struct Counter c;
    c.value = 0;


    GdbString x = "Taner";
    
    printf("x=%s\n",x.cstr());
    x = "Taner Selim";
    printf("x=%s\n",x.cstr());
    GdbString y = x + " Inci";
    printf("y=%s\n",y.cstr());
    y = y + " Erin";
    printf("y=%s\n",y.cstr());
    printf("x=%s\n",x.cstr());
    GdbString z = x+y;
    printf("z=%s\n",z.cstr());

    GdbString a1 = "test";
    GdbString a2 = "test";
    GdbString a3 = "test2";
    if(a1==a2){
        printf("a1 == a2\n");
    }
    else{
        printf("a1 != a2\n");
    }

    if(a1==a3){
        printf("a1 == a3\n");
    }
    else{
        printf("a1 != a3\n");
    }

    GdbString a4 = z.subString(3,5);
    printf("a4 is a subString: %s\n",a4.cstr());

    GdbNumeric int_number = 3;
    printf("int_number is a our special int: %d\n",int_number.int_val());

    GdbNumeric float_number = 4.0f;
    printf("float_number is a our special float: %f\n",float_number.float_val());
    
    return 0; 
}