#include "string.h"

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memcpy(void *d, void *s, size_t n) {
    unsigned char *p = (unsigned char *)d;
    unsigned char *q = (unsigned char *)s;
    while (n--) {
        *p++ = *q++;
    }
    return s;
}

char *strcpy(char *dest, const char *src) {
    char *save = dest;
    while ((*dest++ = *src++))
        ;
    return save;
}
