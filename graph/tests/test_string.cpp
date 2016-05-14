#include "../gdbstring.h"
#include <cstdio>

int main()
{
    printf("Testing GdbString...\n");
    GdbString x = "Tane r";
    GdbString *x_tokenized = x.tokenizeString();
    printf("x=%s\n",x.cstr());
    x = x + " S.";
    printf("x+char*=%s\n",x.cstr());
    GdbString y = " Selim";
    printf("y=%s\n",y.cstr());
    x = x+y;
    printf("x+y=%s\n\n",x.cstr());

    x = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.";
    printf("len and rawLen = %lu %lu \n",x.length(),x.length_serialized());


    printf("Testing GdbVariant...\n");
    GdbVariant a = "1.2";
    GdbVariant b = 1.3f;
    GdbVariant c = 5;

    printf("a b c variants %s %s %s \n",a.cstr(),b.cstr(),c.cstr());

    // Test subString()
    GdbString ss = x.subString(2,3);
    printf("Substring(2,3) = %s \n",ss.cstr());
    printf("len and rawLen = %lu %lu \n",ss.length(),ss.length_serialized());


}