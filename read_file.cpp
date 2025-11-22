#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

const int MAX_LEN = 10;

tree_errors read_tree(char* const file_name, tree_t* tree)
{
    assert(file_name);
    assert(tree);

    FILE* input_address = NULL;
    struct stat statistics = {};
    char* buffer = NULL;
    char* buffer_begin = NULL;
    int buffer_len = 0;
    char symbol = 0;
    tree_errors error = NO_ERROR;

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

    //for (int i = 0; i < buffer_len; i++)
    //    printf("%c", buffer[i]);

    //printf("\n");

    buffer_begin = buffer;

    if (check_file_closing(input_address))
    {
        printf("Error while closing file!");
        error = FILE_CLOSING_ERROR;
    }

    tree->root = read_node(&buffer, NULL, tree);

    free(buffer_begin);

    return error;
}

node_t* read_node(char** buffer, node_t* parent, tree_t* tree)
{
    assert(buffer);
    assert(*buffer);
    assert(tree);

    char str[MAX_LEN] = {0};
    double value = 0;
    node_t* node = NULL;

    //printf("buffer at start = '%c'\n", **buffer);

    if (**buffer == '(')
    {
        *buffer += sizeof(char);

        if (isdigit(**buffer) || (**buffer == '-' && isdigit(*(*buffer + sizeof(char)))))
        {
            sscanf(*buffer, "%lf", &value);
            while (**buffer != 'n' && **buffer != '(')
                *buffer += sizeof(char);

            node_init(&node, value, NUMBER_TYPE, parent);
        }
        else if (**buffer == '+')
        {
            node_init(&node, ADD, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '-')
        {
            node_init(&node, SUB, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '*')
        {
            node_init(&node, MUL, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '/')
        {
            node_init(&node, DIV, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '^')
        {
            node_init(&node, DEG, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else
        {
            for (int i = 0; i < MAX_LEN; i++)
            {
                if ((*buffer)[i] != '(' && (*buffer)[i] != 'n' && (*buffer)[i+1] != 'i' && (*buffer)[i+2] != 'l')
                    str[i] = (*buffer)[i];
                else
                {
                    str[i] = '\0';
                    break;
                }
            }
            //printf("str = %s\n", str);

            if (!check_operation(&node, str, parent))
            {
                make_new_var(&node, str, tree, parent);
            }

            *buffer += strlen(str) * sizeof(char);
        }

        //printf("buffer before node->left = \"%c\"\n", **buffer);
        node->left = read_node(buffer, node, tree);
        //printf("buffer before node->right = \"%c\"\n", **buffer);
        node->right = read_node(buffer, node, tree);

        if (**buffer == ')')
            *buffer += sizeof(char);
        else
        {
            printf("ERROR: buffer = %c\n", **buffer);
            return NULL;
        }

        return node;
    }
    else if ((**buffer) == 'n' && *(*buffer+1) == 'i' && *(*buffer+2) == 'l')
    {
        *buffer += 3 * sizeof(char);
        return NULL;
    }
    else
    {
        printf("ERROR: buffer = %c\n", **buffer);
        return NULL;
    }
}

bool check_operation(node_t** node, char* str, node_t* parent)
{
    assert(str);

    if (strcmp(str, "sin") == 0)
        node_init(node, SIN, OPERATOR_TYPE, parent);
    else if (strcmp(str, "cos") == 0)
        node_init(node, COS, OPERATOR_TYPE, parent);
    else if (strcmp(str, "tg") == 0)
        node_init(node, TG, OPERATOR_TYPE, parent);
    else if (strcmp(str, "ctg") == 0)
        node_init(node, CTG, OPERATOR_TYPE, parent);
    else if (strcmp(str, "arcsin") == 0)
        node_init(node, ARCSIN, OPERATOR_TYPE, parent);
    else if (strcmp(str, "arccos") == 0)
        node_init(node, ARCCOS, OPERATOR_TYPE, parent);
    else if (strcmp(str, "arctg") == 0)
        node_init(node, ARCTG, OPERATOR_TYPE, parent);
    else if (strcmp(str, "arcctg") == 0)
        node_init(node, ARCCTG, OPERATOR_TYPE, parent);
    else if (strcmp(str, "exp") == 0)
        node_init(node, EXP, OPERATOR_TYPE, parent);
    else if (strcmp(str, "ln") == 0)
        node_init(node, LN, OPERATOR_TYPE, parent);
    else
        return false;


    return true;
}

tree_errors make_new_var(node_t** node, char* str, tree_t* tree, node_t* parent)
{
    assert(tree);
    assert(str);

    for (int i = 0; i < tree->number_of_variables; i++)
        if (strcmp(str, tree->variable_list[i].var_name) == 0)
        {
            node_init(node, i, VARIABLE_TYPE, parent);
            return NO_ERROR;
        }

    tree->variable_list[tree->number_of_variables].var_name = strdup(str);

    if (tree->variable_list[tree->number_of_variables].var_name == NULL)
        return ALLOCATION_ERROR;

    if (node_init(node, tree->number_of_variables, VARIABLE_TYPE, parent))
        return ALLOCATION_ERROR;

    tree->number_of_variables++;

    return NO_ERROR;
}
