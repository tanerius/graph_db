#include "bobutils.h"

bool init_bobstring(Bobstring *s, const char *cp) {
    s->len = strlen(cp) + 1;
    s->str = (char*)malloc(s->len);
    if (!s->str) {   
        assert(FALSE);
        s->len = 0;
        s->str = NULL;
        return FALSE;
    }
    else {
        strcpy(s->str, cp);
        return TRUE;
    }
}