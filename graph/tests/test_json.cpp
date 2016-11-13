#include "../gdbjson.hpp"


int main()
{
    printf("Testing GdbJson string...\n");

    GdbJson j;
    printf("The size of a GDBJson instance is: %lu bytes \n",sizeof(j));

    j.readJson("{\"arry\":{\"arry1\":\"one\"},\"foo\":\"bar\",\"height\":15, \"ratio\":2.3}",false);
    GdbString s = j.getValueString("foo");
    GdbVariant i = j.getValueInt("height");
    GdbVariant f = j.getValueFloat("ratio");
    printf("The value of foo=%s\n",s.cstr());
    printf("The value of height=%s\n",i.cstr());
    printf("The value of ratio=%s\n",f.cstr());
    s = j.getValueString("arry.arry1");
    printf("The value of arry.arry1=%s\n",s.cstr());

    printf("\nTesting GdbJson file...\n");
    j.readJson("test.json",true);
    s = j.getValueString("animals.rabbit");
    printf("The color of animals.rabbit is=%s\n",s.cstr());
    return 0;
}