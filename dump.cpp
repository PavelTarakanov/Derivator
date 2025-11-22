#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

tree_errors tree_dump(node_t* root, tree_t* tree)
{
    assert(root);

    FILE* dump_address = NULL;
    char root_address[10] = "Z";

    if (check_file_opening("dump.txt" , &dump_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(dump_address, "digraph{\n");

    node_pointer_dump(root, dump_address, root_address);

    fprintf(dump_address, "}");

    if (check_file_closing(dump_address))
        return FILE_CLOSING_ERROR;

    if (system("dot dump.txt -T png -o pointer_dump.png") != 0)
        return GRAPH_MAKING_ERROR;

    if (check_file_opening("dump.txt" , &dump_address, "w+"))
        return FILE_OPENING_ERROR;

    dump_address = NULL;
    strcpy(root_address, "Z");

    if (check_file_opening("dump.txt" , &dump_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(dump_address, "digraph{\n");

    node_visual_dump(root, dump_address, root_address, tree);

    fprintf(dump_address, "}");

    if (check_file_closing(dump_address))
        return FILE_CLOSING_ERROR;

    if (system("dot dump.txt -T png -o visual_dump.png") != 0)
        return GRAPH_MAKING_ERROR;

    return NO_ERROR;
}

tree_errors node_pointer_dump(node_t* node, FILE* const dump_address, char* node_way)
{
    assert(node);
    assert(dump_address);

    char *left_way = (char*) calloc(strlen(node_way) + 2, sizeof(char));
    char *right_way = (char*) calloc(strlen(node_way) + 2 , sizeof(char));
    tree_errors error = NO_ERROR;

    if (left_way == NULL || right_way == NULL)
        return ALLOCATION_ERROR;

    strcpy(left_way, node_way);
    strcpy(right_way, node_way);

    if (node->left == NULL && node->right == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %lf | {left = %s | right = %s}}\"];\n",
                node_way, node->parent, node->value, "NULL", "NULL");
    else if (node->left == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %lf | {left = %s | right = %p}}\"];\n",
            node_way, node->parent, node->value, "NULL" , node->right);
    else if (node->right == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %lf | {left = %p | right = %s}}\"];\n",
            node_way, node->parent, node->value, node->left, "NULL");
    else
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %lf | {left = %p | right = %p}}\"];\n",
            node_way, node->parent, node->value, node->left, node->right);

    if (node->left != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(left_way, "L"));
        error = node_pointer_dump(node->left, dump_address, left_way);

        if (error)
            return error;
    }
    if (node->right != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(right_way, "R"));
        error = node_pointer_dump(node->right, dump_address, right_way);

        if (error)
            return error;
    }

    free(left_way);
    free(right_way);

    return error;
}

tree_errors node_visual_dump(node_t* node, FILE* const dump_address, char* node_way, tree_t* tree)
{
    assert(node);
    assert(dump_address);

    char *left_way = (char*) calloc(strlen(node_way) + 2, sizeof(char));
    char *right_way = (char*) calloc(strlen(node_way) + 2 , sizeof(char));
    tree_errors error = NO_ERROR;

    if (left_way == NULL || right_way == NULL)
        return ALLOCATION_ERROR;

    strcpy(left_way, node_way);
    strcpy(right_way, node_way);


    fprintf(dump_address, "\t%s", node_way);
    if (node->type == OPERATOR_TYPE)
        fprintf(dump_address, "[shape = record, style=\"filled\",fillcolor=\"lightgreen\", ");
    else if (node->type == VARIABLE_TYPE)
        fprintf(dump_address, "[shape = record, style=\"filled\",fillcolor=\"lightblue\", ");
    else if (node->type == NUMBER_TYPE)
        fprintf(dump_address, "[shape = record, style=\"filled\",fillcolor=\"yellow\", ");
    else
        return TYPE_ERROR;

    print_label(dump_address, node, tree);

    if (node->left != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(left_way, "L"));
        error = node_visual_dump(node->left, dump_address, left_way, tree);

        if (error)
            return error;
    }
    if (node->right != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(right_way, "R"));
        error = node_visual_dump(node->right, dump_address, right_way, tree);

        if (error)
            return error;
    }

    free(left_way);
    free(right_way);

    return error;
}

tree_errors print_label(FILE* dump_address, node_t* node, tree_t* tree)
{
    assert(dump_address);
    assert(node);

    if (node->type == NUMBER_TYPE)
        fprintf(dump_address, "label=\"{value = %lf}\"];\n", node->value);
    else if (node->type == OPERATOR_TYPE)
    {
        if (operator_print(dump_address, node))
            return UNKNOWN_OPERATOR_ERROR;
    }
    else if (node->type == VARIABLE_TYPE)
        fprintf(dump_address, "label=\"{value = %s}\"];\n", tree->variable_list[(int) node->value].var_name);

    return NO_ERROR;
}

tree_errors operator_print(FILE* dump_address, node_t* node)
{
    assert(dump_address);
    assert(node);

    switch((int)node->value)
    {
        case ADD:
            fprintf(dump_address, "label=\"{value = +}\"];\n");
            break;
        case SUB:
            fprintf(dump_address, "label=\"{value = -}\"];\n");
            break;
        case MUL:
            fprintf(dump_address, "label=\"{value = *}\"];\n");
            break;
        case DIV:
            fprintf(dump_address, "label=\"{value = /}\"];\n");
            break;
        case DEG:
            fprintf(dump_address, "label=\"{value = ^}\"];\n");
            break;
        case SIN:
            fprintf(dump_address, "label=\"{value = sin}\"];\n");
            break;
        case COS:
            fprintf(dump_address, "label=\"{value = cos}\"];\n");
            break;
        case TG:
            fprintf(dump_address, "label=\"{value = tg}\"];\n");
            break;
        case CTG:
            fprintf(dump_address, "label=\"{value = ctg}\"];\n");
            break;
        case ARCSIN:
            fprintf(dump_address, "label=\"{value = arcsin}\"];\n");
            break;
        case ARCCOS:
            fprintf(dump_address, "label=\"{value = arccos}\"];\n");
            break;
        case ARCTG:
            fprintf(dump_address, "label=\"{value = arctg}\"];\n");
            break;
        case ARCCTG:
            fprintf(dump_address, "label=\"{value = arcctg}\"];\n");
            break;
        case EXP:
            fprintf(dump_address, "label=\"{value = exp}\"];\n");
            break;
        case LN:
            fprintf(dump_address, "label=\"{value = ln}\"];\n");
            break;
        default:
            printf("ERROR: unknown operator");
            return UNKNOWN_OPERATOR_ERROR;
        }

    return NO_ERROR;
}
