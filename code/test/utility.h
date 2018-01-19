#ifndef UTILITY_H
#define UTILITY_H

#include "syscall.h"

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





// The following utility function is taken from https://www.geeksforgeeks.org/implement-itoa/

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char temp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = temp;
        start++;
        end--;
    }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    char isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

#endif // UTILITY_H

