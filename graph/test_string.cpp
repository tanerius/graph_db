#include "gdbstring.h"
#include <cstdio>

int main()
{
    GdbString x = "Taner";
    printf("x=%s\n",x.cstr());
    GdbString y = " Selim";
    printf("y=%s\n",y.cstr());
    x = x+y;
    printf("x+y=%s\n",x.cstr());
    
}