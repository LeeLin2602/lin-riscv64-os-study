#include "mem.h"

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memcpy(void *d, const void *s, size_t n);
char *strcpy(char *dest, const char *src);
int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char* strtok(char *s, const char *delim);
char* strtok_r(char *s, const char *delim, char **last);
char* strdup(const char *src);
char* strncpy(char* dest, const char* src, size_t n);

