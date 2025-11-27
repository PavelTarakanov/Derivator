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
    node_t* node = NULL;
    tree_elem_t value = {};

    if (**buffer == '(')
    {
        *buffer += sizeof(char);

        if (isdigit(**buffer) || (**buffer == '-' && isdigit(*(*buffer + sizeof(char)))))
        {
            sscanf(*buffer, "%lf", &value.number_value);
            while (**buffer != 'n' && **buffer != '(')
                *buffer += sizeof(char);

            node_init(&node, value, NUMBER_TYPE, parent);
        }
        else if (**buffer == '+')
        {
            value.operator_name = ADD;
            node_init(&node, value, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '-')
        {
            value.operator_name = SUB;
            node_init(&node, value, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '*')
        {
            value.operator_name = MUL;
            node_init(&node, value, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '/')
        {
            value.operator_name = DIV;
            node_init(&node, value, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
        }
        else if (**buffer == '^')
        {
            value.operator_name = DEG;
            node_init(&node, value, OPERATOR_TYPE, parent);
            *buffer += sizeof(char);
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

            if (!check_operation(&node, str, parent))
            {
                make_new_var(&node, str, tree, parent);
            }

            *buffer += strlen(str) * sizeof(char);
        }

        node->left = read_node(buffer, node, tree);
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
    assert(node);
    assert(str);

    tree_elem_t value = {};

    if (strcmp(str, "sin") == 0)
    {
        value.operator_name = SIN;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "cos") == 0)
    {
        value.operator_name = COS;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "tg") == 0)
    {
        value.operator_name = TG;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "ctg") == 0)
    {
        value.operator_name = CTG;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "arcsin") == 0)
    {
        value.operator_name = ARCSIN;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "arccos") == 0)
    {
        value.operator_name = ARCCOS;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "arctg") == 0)
    {
        value.operator_name = ARCTG;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "arcctg") == 0)
    {
        value.operator_name = ARCCTG;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "exp") == 0)
    {
        value.operator_name = EXP;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "ln") == 0)
    {
        value.operator_name = LN;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else if (strcmp(str, "sqrt") == 0)
    {
        value.operator_name = SQRT;
        node_init(node, value, OPERATOR_TYPE, parent);
    }
    else
        return false;


    return true;
}

tree_errors make_new_var(node_t** node, char* str, tree_t* tree, node_t* parent)
{
    assert(tree);
    assert(str);

    tree_elem_t value = {};

    for (int i = 0; i < tree->number_of_variables; i++)
        if (strcmp(str, tree->variable_list[i].var_name) == 0)
        {
            value.variable_number = i;
            node_init(node, value, VARIABLE_TYPE, parent);
            return NO_ERROR;
        }

    tree->variable_list[tree->number_of_variables].var_name = strdup(str);

    value.variable_number = tree->number_of_variables;

    if (tree->variable_list[tree->number_of_variables].var_name == NULL)
        return ALLOCATION_ERROR;

    if (node_init(node, value, VARIABLE_TYPE, parent))
        return ALLOCATION_ERROR;

    tree->number_of_variables++;

    return NO_ERROR;
}
