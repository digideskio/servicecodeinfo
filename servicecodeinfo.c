// servicecodeinfo.c -- report service code allocation policies
// Placed into the public domain by the author, Eddie Kohler, on 8/30/05

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

static void print(unsigned long l);
static int specbyte(int c);
static int asciicode(int c);

int main(int argc, char **argv)
{
    int i, j;
    unsigned long n;
    char *endp;
    
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0)
        || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        printf("Usage: servicecodeinfo NUMBER [NUMBER...]\n"
"   or: servicecodeinfo -n NAME [NAME...]\n");
        exit(0);
    }
    
    printf("Service Code           ASCII   Allocation Policy\n"
"---------------------- -----   ---------------------------------\n");
    if (strcmp(argv[1], "-n") == 0)
        for (i = 2; i < argc; i++)
            if (strlen(argv[i]) > 4 || strlen(argv[i]) < 1)
                printf("%s: bad length (1 char min, 4 chars max)\n", argv[i]);
            else {
                for (n = 0x20202020, j = 3; j >= 0; j--)
                    if (strlen(argv[i]) > j)
                        n = (n >> 8) | (((unsigned char) argv[i][j]) << 24);
                print(n);
            }
    else
        for (i = 1; i < argc; i++) {
            errno = 0;
            n = strtoul(argv[i], &endp, 0);
            if (*endp || n > 4294967295UL || errno == ERANGE || errno == EINVAL)
                printf("%s: need number 0-4294967295 (or try -n)\n", argv[i]);
            else
                print(n);
        }

    return 0;
}

void print(unsigned long n)
{
    const char *type;
    char ascii[5];
    int i, space;
    if (n == 0)
        type = "Reserved";
    else if (((n >> 24) & 0xFF) == 0x3F)
        type = "Private Use";
    else if (specbyte((n >> 24) & 0xFF) && specbyte((n >> 16) & 0xFF)
             && specbyte((n >> 8) & 0xFF) && specbyte(n & 0xFF))
        type = "Specification Required";
    else
        type = "First Come First Serve (FCFS)";
    for (i = space = 0; i < 4; i++) {
        int c = (n >> (24 - i*8)) & 0xFF;
        if (asciicode(c) && !space)
            ascii[i] = c;
	else if (c == ' ' && i > 0)
	    ascii[i] = ' ', space = 1;
        else
            ascii[0] = 0;
    }
    printf("%-23lu%4.4s    %s\n", n, ascii, type);
}

int specbyte(int c)
{
    return c == 32 || (c >= 45 && c <= 57) || (c >= 65 && c <= 90);
}

int asciicode(int c)
{
    return c == 42 || c == 43 || (c >= 45 && c <= 57)
	|| (c >= 63 && c <= 90) || c == 95 || (c >= 97 && c <= 122);
}
