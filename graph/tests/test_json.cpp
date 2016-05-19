#include "../gdbjson.hpp"


int main()
{
    printf("Testing GdbJson...\n");

    GdbJson j;
    j.readJson("{\"foo\":\"bar\"}");
    GdbString s = j.getValueString("foo");
    printf("The value of foo=%s\n",s.cstr());
    return 0;
}