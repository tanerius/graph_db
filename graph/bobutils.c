#include "bobutils.h"

/*
    Bobstring routines
*/
int bobstring_concat(Bobstring_t *s, const char *cp) {
    // cant concat to self
    if(s->str == cp)
        return -1;
    
    if(cp == NULL)
        return -1;

    Bobuint_t len = strlen(cp);

    if(len <= 0)
        return -1;

    s->len = s->len + len;
    char *old_string = s->str;
    s->str = (char*)malloc(sizeof(char) * s->len);

    if(!s->str) {
        s->len = 0;
        s->str = NULL;
        free(old_string);
        return -1;
    }

    strcpy(s->str, old_string);
    strcat(s->str, cp);
    free(old_string); // release memory of previous string
    return 0;
}

int bobstring_init(Bobstring_t *s, const char *cp) {
    s->len = strlen(cp) + 1;
    s->str = (char*)malloc(s->len);

    if (!s->str) {   
        assert(FALSE);
        s->len = 0;
        s->str = NULL;
        return -1;
    }

    strcpy(s->str, cp);
    return 0;
}
