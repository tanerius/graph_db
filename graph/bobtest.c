#include "bobutils.h"
int main() {
    Bobstring *str = (Bobstring*)malloc(sizeof(Bobstring));
    init_bobstring(str,"Hello Танер");
    printf("This is the string %s\n", str->str);
    SafeFree(str->str);
    return 0;
}