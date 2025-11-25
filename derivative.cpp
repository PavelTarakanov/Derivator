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
    node_t* new_node_1 = NULL;
    node_t* new_node_2 = NULL;
    node_t* new_node_3 = NULL;
    node_t* new_node_4 = NULL;
    node_t* new_node_5 = NULL;


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
                node_init(&new_node_1, MUL, OPERATOR_TYPE, new_node);
                node_init(&new_node_2, MUL, OPERATOR_TYPE, new_node);

                new_node->left = new_node_1;
                new_node->right = new_node_2;
                new_node_1->left = dL;
                new_node_1->right = node_copy(node->right, new_node_1);
                new_node_2->left = dR;
                new_node_2->right = node_copy(node->left, new_node_1);

                break;
            case DIV:
                node_init(&new_node, DIV, OPERATOR_TYPE, parent);
                node_init(&new_node_1, DEG, OPERATOR_TYPE, new_node);
                node_init(&new_node_2, SUB, OPERATOR_TYPE, new_node);
                node_init(&new_node_3, MUL, OPERATOR_TYPE, new_node_2);
                node_init(&new_node_4, MUL, OPERATOR_TYPE, new_node_2);
                node_init(&new_node_5, 2, NUMBER_TYPE, new_node_1);

                new_node->left = new_node_2;
                new_node->right = new_node_1;
                new_node_1->left = node_copy(node->right, new_node_1);
                new_node_1->right = new_node_5;
                new_node_2->left = new_node_3;
                new_node_2->right = new_node_4;
                new_node_3->left = dL;
                new_node_3->right = node_copy(node->right, new_node_3);
                new_node_4->left = dR;
                new_node_4->right = node_copy(node->left, new_node_4);

                break;
            case DEG:
                if (node->left->type == NUMBER_TYPE)
                {
                    node_init(&new_node, MUL, OPERATOR_TYPE, parent);
                    node_init(&new_node_1, MUL, OPERATOR_TYPE, new_node);
                    node_init(&new_node_2, LN, OPERATOR_TYPE, new_node);
                    node_init(&new_node_3, DEG, OPERATOR_TYPE, new_node_1);

                    new_node->left = new_node_2;
                    new_node->right = new_node_1;
                    new_node_1->left = new_node_3;
                    new_node_1->right = dR;
                    new_node_2->left = node_copy(node->left, new_node_2);
                    new_node_3->left = node_copy(node->left, new_node_3);
                    new_node_3->right = node_copy(node->right, new_node_3);
                }
                else if (node->right->type == NUMBER_TYPE)
                {
                    node_init(&new_node, MUL, OPERATOR_TYPE, parent);
                    node_init(&new_node_1, MUL, OPERATOR_TYPE, new_node);
                    node_init(&new_node_2, node->right->value, NUMBER_TYPE, new_node);
                    node_init(&new_node_3, DEG, OPERATOR_TYPE, new_node_1);
                    node_init(&new_node_4, node->right->value - 1, NUMBER_TYPE, new_node_3);

                    new_node->left = new_node_2;
                    new_node->right = new_node_1;
                    new_node_1->left = new_node_3;
                    new_node_1->right = dL;
                    new_node_3->left = node_copy(node->left, new_node_3);
                    new_node_3->right = new_node_4;
                }
                else
                {

                }
                break;
            case SIN:
                node_init(&new_node, MUL, OPERATOR_TYPE, parent);
                node_init(&new_node_1, COS, OPERATOR_TYPE, new_node);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = node_copy(node->left, new_node_1);
                break;
            case COS:
                node_init(&new_node, MUL, OPERATOR_TYPE, parent);
                node_init(&new_node_1, MUL, OPERATOR_TYPE, new_node);
                node_init(&new_node_2, -1, NUMBER_TYPE, new_node_1);
                node_init(&new_node_3, SIN, OPERATOR_TYPE, new_node_1);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = node_copy(node->left, new_node_3);

                break;
            case TG:
                node_init(&new_node, MUL, OPERATOR_TYPE, parent);
                node_init(&new_node_1, DIV, OPERATOR_TYPE, new_node);
                node_init(&new_node_2, 1, NUMBER_TYPE, new_node_1);
                node_init(&new_node_3, DEG, OPERATOR_TYPE, new_node_1);
                node_init(&new_node_4, 2, NUMBER_TYPE, new_node_3);
                node_init(&new_node_5, COS, OPERATOR_TYPE, new_node_3);

                new_node->left = new_node_1;
                new_node->right = dL;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = new_node_5;
                new_node_3->right = new_node_4;
                new_node_5->left = node_copy(node->left, new_node_5);

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
