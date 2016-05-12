#include "gdbstring.h"
#include <cstdio>

int main()
{
    GdbString x = "Taner";
    printf("x=%s\n",x.cstr());
}