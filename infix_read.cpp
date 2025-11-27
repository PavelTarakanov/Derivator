#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include "file_using.h"
#include "derivative.h"

int infix_read(char* file_name)
{
    assert(file_name);

    FILE* input_address = NULL;
    struct stat statistics = {};
    char* buffer = NULL;
    char* buffer_begin = NULL;
    int buffer_len = 0;
    char symbol = 0;
    int value = 0;

    if (check_file_opening(file_name, &input_address, "r"))
        return FILE_OPENING_ERROR;

    if (fstat(fileno(input_address), &statistics))
        return FILE_STATISTICS_ERROR;

    buffer = (char*) calloc(statistics.st_size, sizeof(char));

    if (buffer == NULL)
        return ALLOCATION_ERROR;

    for (int i = 0; i < statistics.st_size; i++)
    {
        symbol = fgetc(input_address);

        if (isgraph(symbol) || symbol < 0)//для русского
        {
            buffer[buffer_len] = symbol;
            buffer_len++;
        }
    }

    for (int i = 0; i < buffer_len; i++)
        printf("%c", buffer[i]);

    printf("\n");

    buffer_begin = buffer;

    if (check_file_closing(input_address))
    {
        printf("Error while closing file!");
        return FILE_CLOSING_ERROR;
    }

    value = get_g(&buffer);
    free(buffer_begin);

    return value;
}

int get_g(char** buffer)
{
    assert(buffer);

    int val = get_e(buffer);

    if (**buffer != '$')
        printf("Syntax_Error in get_g\nBuffer = %s\n", *buffer);

    (*buffer)++;
    return val;
}

int get_e(char** buffer)
{
    assert(buffer);

    int val = get_t(buffer);
    char op = '\0';
    int val_2 = 0;

    while (**buffer == '+' || **buffer == '-')
    {
        op = **buffer;
        (*buffer)++;
        val_2 = get_t(buffer);
        if (op == '+')
            val += val_2;
        else
            val -= val_2;
    }

    return val;
}

int get_t(char** buffer)
{
    assert(buffer);

    int val = get_p(buffer);
    char op = '\0';
    int val_2 = 0;

    while (**buffer == '*' || **buffer == '/')
    {
        op = **buffer;
        (*buffer)++;
        val_2 = get_p(buffer);
        if (op == '*')
            val *= val_2;
        else
            val /= val_2;
    }

    return val;
}

int get_p(char** buffer)
{
    assert(buffer);

    int val = 0;

    if (**buffer == '(')
    {
        (*buffer)++;
        val = get_e(buffer);
        if (**buffer == ')')
            (*buffer)++;
        else
            printf("Syntax_Error in get_p\nBuffer = %s\n", *buffer);;
        return val;
    }
    else
        return get_n(buffer);
}

int get_n(char** buffer)
{
    assert(buffer);

    int val = 0;
    char* buffer_at_start = *buffer;

    while ('0' <= **buffer && **buffer <= '9')
    {
        val = val*10 + (**buffer - '0');
        (*buffer)++;
    }
    if (buffer_at_start == *buffer)
        printf("Syntax_Error in get_n\nBuffer = %s\n", *buffer);

    return val;
}
