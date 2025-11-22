#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

#define dL node_derivate(node->left, tree, new_node, derivating_variable)
#define dR node_derivate(node->right, tree, new_node, derivating_variable)

node_t* node_derivate(node_t* node, tree_t* tree, node_t* parent, char* derivating_variable)
{
    assert(node);
    assert(tree);

    node_t* new_node = NULL;

    if (node->type == NUMBER_TYPE)
        node_init(&new_node, 0, NUMBER_TYPE, parent);
    else if (node->type == VARIABLE_TYPE)
    {
        if (strcmp(derivating_variable, tree->variable_list[(int) node->value].var_name) == 0)
            node_init(&new_node, 1, NUMBER_TYPE, parent);
        else
            node_init(&new_node, 0, NUMBER_TYPE, parent);
    }
    else if (node->type == OPERATOR_TYPE)
    {
        switch((int)node->value)
        {
        case ADD:
        {
            node_init(&new_node, ADD, OPERATOR_TYPE, parent);
            new_node->left = dL;
            new_node->right = dR;
            break;
        }
        case SUB:
            node_init(&new_node, SUB, OPERATOR_TYPE, parent);
            new_node->left = dL;
            new_node->right = dR;
            break;
        case MUL:
            node_init(&new_node, ADD, OPERATOR_TYPE, parent);
            break;
        case DIV:
            break;
        case DEG:
            break;;
        case SIN:
            break;
        case COS:
            break;
        case TG:
            break;
        case CTG:
            break;
        case ARCSIN:
            break;
        case ARCCOS:
            break;
        case ARCTG:
            break;
        case ARCCTG:
            break;
        case EXP:
            break;
        case LN:
            break;
        default:
            printf("ERROR: unknown operator");
            return NULL;
        }
    }


    return new_node;
}

node_t* node_copy(node_t* node, node_t* parent)
{
    assert(node);

    node_t* node_cpy = NULL;

    node_init(&node_cpy, node->value, node->type, parent);

    if (node->left != NULL) node_cpy->left = node_copy(node->left, node_cpy);
    else node_cpy->left = NULL;

    if (node->right != NULL) node_cpy->right = node_copy(node->right, node_cpy);
    else node_cpy->right = NULL;

    return node_cpy;
}


#undef dL
#undef dR
