#include <stdio.h>
#include <string.h>
#include "info.h"

struct Info
{
    char name[30], description[100], author[20];
    int major, minor, patch;
};

struct Info get_info()
{
    struct Info i;

    // Using strncpy for safer string copy
    strncpy(i.name, "Random Key Generator", (sizeof(i.name) - 1));
    strncpy(i.author, "Zeke Zhang", (sizeof(i.author) - 1));
    strncpy(i.description, "Generates a random key of a specified length.", (sizeof(i.description) - 1));

    i.major = 0;
    i.minor = 0;
    i.patch = 1;

    return i;
}

void print_help()
{
    struct Info i = get_info();
    printf("%s v%i.%i.%i\n", i.name, i.major, i.minor, i.patch);
    printf("%s\n", i.description);
    printf("Usage: keygen [length] --options\n");

    printf("  length\t\t(Required) The length of the key in bytes to generate\n");
    printf("Options:\n");
    printf("  -t, --type [b64|hex]\t\tThe type of key to generate\n");
    printf("  -s, --seed [number]\t\tThe seed to use for the RNG\n");

    printf("\n");
    printf("  -h, --help\t\tDisplay this information\n");
    printf("  -v, --version\t\tDisplay version information\n");
}

void print_version()
{
    struct Info i = get_info();
    printf("%d.%d.%d\n", i.major, i.minor, i.patch);
}
