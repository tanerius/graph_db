#include "graphutils.h"

int main(){
    printf("Testing Utils...\n");
    struct Counter c;
    c.value = 0;

    GdbString x = "Taner";
    printf("x=%s\n",x.get());
    x = "Taner Selim";
    printf("x=%s\n",x.get());
    GdbString y = x + " Inci";
    printf("y=%s\n",y.get());
    y = y + " Erin";
    printf("y=%s\n",y.get());
    printf("x=%s\n",x.get());
    GdbString z = x+y;
    printf("z=%s\n",z.get());

   
    return 0;
}