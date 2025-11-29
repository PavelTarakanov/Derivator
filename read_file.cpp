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

    for (int i = 0; i < buffer_len; i++)
        printf("%c", buffer[i]);

    printf("\n");

    buffer_begin = buffer;

    if (check_file_closing(input_address))
    {
        printf("Error while closing file!");
        error = FILE_CLOSING_ERROR;
    }

    tree->root = read_node(&buffer, tree);

    free(buffer_begin);

    return error;
}

node_t* read_node(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(*buffer);
    assert(tree);

    char str[MAX_LEN] = {0};
    node_t* node = NULL;
    tree_elem_t value = {};
    node_t* left = NULL;
    node_t* right = NULL;

    if (**buffer == '(')
    {
        *buffer += sizeof(char);

        if (isdigit(**buffer) || (**buffer == '-' && isdigit(*(*buffer + sizeof(char)))))
        {
            sscanf(*buffer, "%lf", &value.number_value);
            while (**buffer != 'n' && **buffer != '(')
                *buffer += sizeof(char);

            left = read_node(buffer, tree);
            right = read_node(buffer, tree);
            node = node_init(value, NUMBER_TYPE, left, right);
        }
        else if (**buffer == '+')
        {
            value.operator_name = ADD;
            *buffer += sizeof(char);

            left = read_node(buffer, tree);
            right = read_node(buffer, tree);
            node = node_init(value, OPERATOR_TYPE, left, right);
        }
        else if (**buffer == '-')
        {
            value.operator_name = SUB;
            *buffer += sizeof(char);

            left = read_node(buffer, tree);
            right = read_node(buffer, tree);
            node = node_init(value, OPERATOR_TYPE, left, right);
        }
        else if (**buffer == '*')
        {
            value.operator_name = MUL;
            *buffer += sizeof(char);

            left = read_node(buffer, tree);
            right = read_node(buffer, tree);;
            node = node_init(value, OPERATOR_TYPE, left, right);
        }
        else if (**buffer == '/')
        {
            value.operator_name = DIV;
            *buffer += sizeof(char);

            left = read_node(buffer, tree);
            right = read_node(buffer, tree);
            node = node_init(value, OPERATOR_TYPE, left, right);
        }
        else if (**buffer == '^')
        {
            value.operator_name = DEG;
            *buffer += sizeof(char);

            left = read_node(buffer, tree);
            right = read_node(buffer, tree);
            node = node_init(value, OPERATOR_TYPE, left, right);
        }
        else
        {
            for (int i = 0; i < MAX_LEN; i++)
            {
                if ((*buffer)[i] != '(' && !((*buffer)[i] == 'n' && (*buffer)[i+1] == 'i' && (*buffer)[i+2] == 'l'))
                    str[i] = (*buffer)[i];
                else
                {
                    str[i] = '\0';
                    break;
                }
            }


            node = check_operation(tree, str, buffer, left, right);
            if (node == NULL)
            {
                node = make_new_var(str, tree, buffer, left, right);
            }
        }

        //node->left = read_node(buffer, node, tree);
        //node->right = read_node(buffer, node, tree);

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

node_t* check_operation(tree_t* tree, char* str, char** buffer, node_t* left, node_t* right)
{
    assert(tree);
    assert(str);

    tree_elem_t value = {};

    *buffer += strlen(str) * sizeof(char);

    if (strcmp(str, "sin") == 0)
    {
        value.operator_name = SIN;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "cos") == 0)
    {
        value.operator_name = COS;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "tg") == 0)
    {
        value.operator_name = TG;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "ctg") == 0)
    {
        value.operator_name = CTG;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "arcsin") == 0)
    {
        value.operator_name = ARCSIN;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "arccos") == 0)
    {
        value.operator_name = ARCCOS;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "arctg") == 0)
    {
        value.operator_name = ARCTG;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "arcctg") == 0)
    {
        value.operator_name = ARCCTG;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "exp") == 0)
    {
        value.operator_name = EXP;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "ln") == 0)
    {
        value.operator_name = LN;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }
    else if (strcmp(str, "sqrt") == 0)
    {
        value.operator_name = SQRT;
        left = read_node(buffer, tree);
        right = read_node(buffer, tree);
        return node_init(value, OPERATOR_TYPE, left, right);
    }

    *buffer -= strlen(str) * sizeof(char);
    return NULL;
}
node_t* make_new_var(char* str, tree_t* tree, char** buffer, node_t* left, node_t* right)
{
    assert(tree);
    assert(str);

    tree_elem_t value = {};
    *buffer += strlen(str) * sizeof(char);

    for (int i = 0; i < tree->number_of_variables; i++)
        if (strcmp(str, tree->variable_list[i].var_name) == 0)
        {
            value.variable_number = i;
            left = read_node(buffer, tree);
            right = read_node(buffer, tree);
            return node_init(value, VARIABLE_TYPE, left, right);
        }

    tree->variable_list[tree->number_of_variables].var_name = strdup(str);

    value.variable_number = tree->number_of_variables;

    if (tree->variable_list[tree->number_of_variables].var_name == NULL)
        return NULL;

    tree->number_of_variables++;

    left = read_node(buffer, tree);
    right = read_node(buffer, tree);
    return node_init(value, VARIABLE_TYPE, left, right);
}
