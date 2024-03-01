#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "info.h"

#if defined(_WIN32) || defined(_WIN64) // Windows
#include <windows.h>
#include <wincrypt.h>
#else // Unix
#include <unistd.h>
#include <fcntl.h>
#endif

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

void parse_options(int argc, char *argv[], char *type, unsigned int *seed, size_t *length)
{
    *length = 0; // Initialize to 0 to detect unspecified length

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--type") == 0)
        {
            if (i + 1 < argc)
            {
                strncpy(type, argv[i + 1], 4);
                type[4] = '\0'; // Correctly null-terminate the string
                i++;            // Skip the next argument as it has been processed
            }
            else
            {
                fprintf(stderr, "No type specified\n");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--seed") == 0)
        {
            if (i + 1 < argc)
            {
                *seed = (unsigned int)strtoul(argv[i + 1], NULL, 10);
                i++; // Skip the next argument as it has been processed
            }
            else
            {
                fprintf(stderr, "No seed specified\n");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            print_version();
            exit(0);
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_help();
            exit(0);
        }
        else
        {
            // Attempt to parse the length if not already set
            if (*length == 0)
            {
                *length = (size_t)strtoul(argv[i], NULL, 10);
                if (*length == 0)
                {
                    fprintf(stderr, "Invalid length specified\n");
                    exit(1);
                }
            }
            else
            {
                fprintf(stderr, "Unexpected argument: %s\n", argv[i]);
                exit(1);
            }
        }
    }

    if (*length == 0)
    {
        fprintf(stderr, "Length must be specified\n");
        exit(1);
    }
}

// Get a random seed from the system RNG
unsigned int get_random_seed()
{
    unsigned int seed = 0;
#if defined(_WIN32) || defined(_WIN64) // Windows
    HCRYPTPROV Prov;
    if (!CryptAcquireContext(&Prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        fprintf(stderr, "CryptAcquireContext failed\n");
        exit(1);
    }
    if (!CryptGenRandom(Prov, sizeof(seed), (BYTE *)&seed))
    {
        fprintf(stderr, "CryptGenRandom failed\n");
        exit(1);
    }
    CryptReleaseContext(Prov, 0);
#else // Unix
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open /dev/urandom\n");
        exit(1);
    }
    if (read(fd, &seed, sizeof(seed)) != sizeof(seed))
    {
        fprintf(stderr, "Failed to read from /dev/urandom\n");
        exit(1);
    }
    close(fd);
#endif
    return seed;
}

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        print_help();
        return 1;
    }

    char type[5] = "b64";                  // default type
    unsigned int seed = get_random_seed(); // default seed
    size_t length = 0;

    parse_options(argc, argv, type, &seed, &length); // Parse options including length

    srand(seed); // Seed the RNG

    // generate the key
    unsigned char *key = (unsigned char *)malloc(length);
    if (key == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    generate_byte_key(key, length);
    if (strcmp(type, "b64") == 0)
    {
        char *b64_key;
        base64_encode(key, &b64_key, length);
        printf("%s\n", b64_key);
        free(b64_key); // Free the allocated memory for b64_key
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
        fprintf(stderr, "Invalid type specified\n");
        return 1;
    }

    free(key); // Free the allocated memory for key
    return 0;
}
