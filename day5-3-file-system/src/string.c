#include "string.h"

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memcpy(void *d, const void *s, size_t n) {
    unsigned char *p = (unsigned char *)d;
    unsigned char *q = (unsigned char *)s;
    while (n--) {
        *p++ = *q++;
    }
    return d;
}

char *strcpy(char *dest, const char *src) {
    char *save = dest;
    while ((*dest++ = *src++))
        ;
    return save;
}

int strlen(const char *s) {
	int len = 0;
	while(*s) len++, s++;
	return len;	
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char* strdup(const char *src) {
    char *dst = (char *)malloc(strlen(src) + 1);
    if (dst == NULL) return NULL;
    char *ptr = dst;
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dst;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char* strtok(char *s, const char *delim) {
	static char *last;
	return strtok_r(s, delim, &last);
}

char* strtok_r(char *s, const char *delim, char **last) {
	char *spanp;
	int c, sc;
	char *tok;
	if (s == NULL && (s = *last) == NULL)
		return (NULL);
	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}
	if (c == 0) {		/* no non-delimiter characters */
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;
	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}
