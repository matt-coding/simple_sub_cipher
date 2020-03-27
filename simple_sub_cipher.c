#include <sys/mman.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void generate_key(char *key);
size_t get_file_contents(char *file_name, char **file_contents);

int main(int argc, char *argv[])
{
    FILE *output_file;
    int c, i, position;
    char *input_file_name = NULL, *input_file_contents = NULL, *output_file_name = NULL, *encryption_key = NULL;
    const char *source_map, *dest_map;
    const char english_alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char generated_encryption_key[26];
    size_t input_file_length;
    bool decrypt = false;

    if(argc == 1)
    {

        printf("simple_sub_cipher: try 'simple_sub_cipher -h' for more information.\n");
        return 1;
    }

    while ((c = getopt (argc, argv, "dhk:o:")) != -1)
        switch (c)
        {
            case 'd':
                //decrypt
                decrypt = true;
                break;
            case 'h':
                printf("This encrypts data via a simple substitution cipher. Usage: simple_sub_cipher <input_file>\n");
                printf("-d Decrypt rather than encrypt the input.\n");
                printf("-k Supply 26 length key. Default is random key. E.g. ILDMEFGUNAJQRSTKYOPBZCHVWX\n");
                printf("-o Write to output file.\n");
                return 0;
            case 'k':
                encryption_key = optarg;
                break;
            case 'o':
                //Write to output file
                output_file_name = optarg;
                break;
            case '?':
                if (optopt == 'k' || optopt == 'o')
                {
                    fprintf(stderr, "simple_sub_cipher error: Option -%c requires an argument.\n", optopt);
                    exit(EXIT_FAILURE);
                }
                else
                {
                    fprintf(stderr, "simple_sub_cipher error: Unknown option character.\n");
                    exit(EXIT_FAILURE);
                }
            default:
                exit(EXIT_FAILURE);
    }

    // We should only have <input_file> left.
    if(argc != optind + 1)
    {

        fprintf(stderr, "simple_sub_cipher error: Incorrect number of arguments. \n");
        exit(EXIT_FAILURE);
    }

    if(encryption_key == NULL)
    {
        if(decrypt)
        {
            fprintf(stderr, "simple_sub_cipher error: Need key for decryption.\n");
            exit(EXIT_FAILURE);
        }
        generate_key(generated_encryption_key);
        printf("Generated encryption key: ");
        for (i = 0; i < 26; i++)
        {
            putchar(generated_encryption_key[i]);
        }
        printf("\n");
        encryption_key = generated_encryption_key;
    }
    else
    {
        if(strlen(encryption_key) != 26)
        {
            fprintf(stderr, "simple_sub_cipher error: Incorrect key length. \n");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < 26; i++)
        {
            if(encryption_key[i] < 65 || encryption_key[i] > 90)
            {
                fprintf(stderr, "simple_sub_cipher error: Please provide uppercase key consisting only of characters A-Z.\n");
                exit(EXIT_FAILURE);
            }
        }
    }



    if (output_file_name != NULL)
    {
        output_file = fopen(output_file_name, "w");
        if(output_file == NULL)
        {
            fprintf(stderr, "lfib error: Couldn't open %s for writing.\n", output_file_name);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        output_file = stdout;
        printf("Encrypted data: \n");
    }

    //Validation done.

    input_file_name = argv[optind];
    input_file_length = get_file_contents(input_file_name, &input_file_contents);

    if(decrypt)
    {
        source_map = encryption_key;
        dest_map = english_alphabet;
    }
    else
    {
        source_map = english_alphabet;
        dest_map = encryption_key;
    }

    for (i = 0; i < input_file_length; i++)
    {
        char *temp;
        if((temp = strchr(source_map, toupper(input_file_contents[i]))) != NULL)
        {
            position = temp - source_map;
            fputc(dest_map[position], output_file);
        }
    }

    munmap(input_file_contents, input_file_length);

    if (output_file_name == NULL)
    {
        printf("\n");
    }

    printf("Encryption complete.\n");

    return 0;
}

void generate_key(char *key)
{
    int i, j;
    int rand_num;
    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    srand((unsigned int)time(NULL));
    for(i = 25; i > 0; i--)
    {
        rand_num = (rand() % (i + 1));
        key[i] = alphabet[rand_num];

        //Move array back 1
        for (j = rand_num; j < 26; j++)
        {
            alphabet[j] = alphabet[j + 1];
        }
    }
    key[0] = alphabet[0];
}

size_t get_file_contents(char *file_name, char **file_contents)
{
    FILE *fptr;
    size_t file_size;

    fptr = fopen(file_name, "r");

    if(fptr == NULL)
    {
        fprintf(stderr, "simple_sub_cipher error: Couldn't open %s \n", file_name);
        exit(EXIT_FAILURE);
    }

    fseek(fptr, 0L, SEEK_END);
    file_size = ftell(fptr);

    *file_contents = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fileno(fptr), 0);

    fclose(fptr);

    return file_size;
}