#ifndef UTILITY_H
#define UTILITY_H

#define _ASSERT(expr) if (!(expr)) aFailed(__FILE__, __LINE__)

void aFailed(char *fileName, int line) {
    AssertionFailed(fileName, line);
}

int stringCompare(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
    if (*s1 == '\0')
        return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

int strlen(const char *s) {
    int length = 0;
    while (*s++ != 0) { ++length; }
    return length;
}

//WARNING: This function has never been tested - use with care
void *memset (void *ptr, int value, int num) {
    unsigned char *cvt = (unsigned char *)ptr;
    while (num--) {
        *cvt = (unsigned char)value;
        ++cvt;
    }
    return ptr;
}


#endif // UTILITY_H

