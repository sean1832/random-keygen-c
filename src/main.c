#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "info.h"

void generate_byte_key(unsigned char *buffer, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        buffer[i] = (unsigned char)(rand() % 256); // 0 - 255
    }
}

void base64_encode(const unsigned char *input, char **output, size_t length)
{
    // base64 encoding table
    const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i = 0, j = 0;
    unsigned char char_array_3[3], char_array_4[4];
    size_t output_length = 4 * ((length + 2) / 3); // 4/3 times the input length because 3 bytes will be converted to 4 bytes
    *output = (char *)malloc(output_length + 1);   // +1 for null-terminator

    if (*output == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    while (length--) // while there are bytes left
    {
        char_array_3[i++] = *(input++); // get 3 bytes
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;                                     // 1st 6 bits
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4); // 2nd 6 bits
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6); // 3rd 6 bits
            char_array_4[3] = char_array_3[2] & 0x3f;                                            // 4th 6 bits

            for (i = 0; i < 4; i++)
            {
                (*output)[j++] = b64[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) // if there are bytes left
    {
        for (int k = i; k < 3; k++)
        {
            char_array_3[k] = '\0'; // pad with null bytes
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;                                     // 1st 6 bits
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4); // 2nd 6 bits
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6); // 3rd 6 bits
        char_array_4[3] = char_array_3[2] & 0x3f;                                            // 4th 6 bits

        for (int k = 0; k < i + 1; k++)
        {
            (*output)[j++] = b64[char_array_4[k]];
        }

        while (i++ < 3)
        {
            (*output)[j++] = '='; // pad with '='
        }
    }
    (*output)[j] = '\0'; // null-terminate the string
}

void parse_options(int argc, char const *argv[], char *type)
{
    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--type") == 0)
        {
            if (i + 1 < argc)
            {
                strncpy(type, argv[i + 1], 3);
                type[4] = '\0'; // null-terminate the string
            }
            else
            {
                fprintf(stderr, "No type specified\n");
                exit(1);
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    srand((unsigned int)time(NULL)); // Seed the RNG once

    if (argc == 1 || strcmp(argv[argc - 1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        print_help();
    }
    else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
    {
        print_version();
    }
    else
    {
        char type[5] = "b64";            // default type
        parse_options(argc, argv, type); // parse options

        size_t length = (size_t)strtoul(argv[1], NULL, 10); // convert string to unsigned long (base 10)
        if (length == 0)
        {
            fprintf(stderr, "Invalid length\n");
            return 1;
        }

        // generate the key
        unsigned char *key = (unsigned char *)malloc(length);
        if (key == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        generate_byte_key(key, length);

        // determine the output type
        if (strcmp(type, "b64") == 0)
        {
            char *b64_key = NULL;
            base64_encode(key, &b64_key, length);
            if (b64_key != NULL)
            {
                printf("%s\n", b64_key);
                free(b64_key); // free the memory
            }
        }
        else if (strcmp(type, "hex") == 0)
        {
            for (size_t i = 0; i < length; i++)
            {
                printf("%02x ", key[i]);
            }
            printf("\n");
        }
        else
        {
            fprintf(stderr, "Invalid type\n");
            return 1;
        }
    }
    return 0;
}
