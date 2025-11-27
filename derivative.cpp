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

node_t* node_derivate(node_t* node, const tree_t* const tree, node_t* parent, const char* const derivating_variable)
{
    assert(node);
    assert(tree);
    assert(derivating_variable);

    node_t* new_node = NULL;
    node_t* new_node_1 = NULL;
    node_t* new_node_2 = NULL;
    node_t* new_node_3 = NULL;
    node_t* new_node_4 = NULL;
    node_t* new_node_5 = NULL;
    node_t* new_node_6 = NULL;
    node_t* new_node_7 = NULL;
    tree_elem_t value = {};

    if (node->type == NUMBER_TYPE)
    {
        value.number_value = 0;
        node_init(&new_node, value, NUMBER_TYPE, parent);
    }
    else if (node->type == VARIABLE_TYPE)
    {
        if (strcmp(derivating_variable, tree->variable_list[node->value.variable_number].var_name) == 0)
        {
            value.number_value = 1;
            node_init(&new_node, value, NUMBER_TYPE, parent);
        }
        else
        {
            value.number_value = 0;
            node_init(&new_node, value, NUMBER_TYPE, parent);
        }
    }
    else if (node->type == OPERATOR_TYPE)//TODO функции для всехпроизводных
    {
        switch(node->value.operator_name)
        {
            case ADD:
                return add_derivate(node, tree, parent, derivating_variable);
            case SUB:
                return sub_derivate(node, tree, parent, derivating_variable);
            case MUL:
                return mul_derivate(node, tree, parent, derivating_variable);
            case DIV:
                value.operator_name = DIV;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = DEG;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.operator_name = SUB;
                node_init(&new_node_2, value, OPERATOR_TYPE, new_node);

                value.operator_name = MUL;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_2);
                node_init(&new_node_4, value, OPERATOR_TYPE, new_node_2);

                value.number_value = 2;
                node_init(&new_node_5, value, NUMBER_TYPE, new_node_1);

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
                    value.operator_name = MUL;
                    node_init(&new_node, value, OPERATOR_TYPE, parent);
                    node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                    value.operator_name = LN;
                    node_init(&new_node_2, value, OPERATOR_TYPE, new_node);

                    value.operator_name = DEG;
                    node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

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
                    value.operator_name = MUL;
                    node_init(&new_node, value, OPERATOR_TYPE, parent);
                    node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                    value.number_value = node->right->value.number_value;
                    node_init(&new_node_2, value, NUMBER_TYPE, new_node);

                    value.operator_name = DEG;
                    node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                    value.number_value = node->right->value.number_value - 1;
                    node_init(&new_node_4, value, NUMBER_TYPE, new_node_3);

                    new_node->left = new_node_2;
                    new_node->right = new_node_1;
                    new_node_1->left = new_node_3;
                    new_node_1->right = dL;
                    new_node_3->left = node_copy(node->left, new_node_3);
                    new_node_3->right = new_node_4;
                }

                break;
            case SIN:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = COS;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = node_copy(node->left, new_node_1);
                break;
            case COS:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = MUL;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node_1);

                value.operator_name = SIN;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = node_copy(node->left, new_node_3);

                break;
            case TG:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = DIV;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = 1;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node_1);

                value.operator_name = DEG;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                value.number_value = 2;
                node_init(&new_node_4, value, NUMBER_TYPE, new_node_3);

                value.operator_name = COS;
                node_init(&new_node_5, value, OPERATOR_TYPE, new_node_3);

                new_node->left = new_node_1;
                new_node->right = dL;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = new_node_5;
                new_node_3->right = new_node_4;
                new_node_5->left = node_copy(node->left, new_node_5);

                break;
            case CTG:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = DIV;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = -1;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node_1);

                value.operator_name = DEG;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                value.number_value = 2;
                node_init(&new_node_4, value, NUMBER_TYPE, new_node_3);

                value.operator_name = SIN;
                node_init(&new_node_5, value, OPERATOR_TYPE, new_node_3);

                new_node->left = new_node_1;
                new_node->right = dL;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = new_node_5;
                new_node_3->right = new_node_4;
                new_node_5->left = node_copy(node->left, new_node_5);
                break;
            case ARCSIN:
                value.operator_name = DIV;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = SQRT;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.operator_name = SUB;
                node_init(&new_node_2, value, OPERATOR_TYPE, new_node_1);

                value.number_value = 1;
                node_init(&new_node_3, value, NUMBER_TYPE, new_node_2);

                value.operator_name = DEG;
                node_init(&new_node_4, value, OPERATOR_TYPE, new_node_2);

                value.number_value = 2;
                node_init(&new_node_5, value, NUMBER_TYPE, new_node_4);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_2->left = new_node_3;
                new_node_2->right = new_node_4;
                new_node_4->left = node_copy(node->left, new_node_4);
                new_node_4->right = new_node_5;

                break;
            case ARCCOS:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = DIV;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = -1;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node);

                value.operator_name = SQRT;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                value.operator_name = SUB;
                node_init(&new_node_4, value, OPERATOR_TYPE, new_node_3);

                value.number_value = 1;
                node_init(&new_node_5, value, NUMBER_TYPE, new_node_4);

                value.operator_name = DEG;
                node_init(&new_node_6, value, OPERATOR_TYPE, new_node_4);

                value.number_value = 2;
                node_init(&new_node_7, value, NUMBER_TYPE, new_node_6);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = new_node_4;
                new_node_4->left = new_node_5;
                new_node_4->right = new_node_6;
                new_node_6->left = node_copy(node->left, new_node_6);
                new_node_6->right = new_node_7;
                break;
            case ARCTG:
                value.operator_name = DIV;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = ADD;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = 1;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node_1);

                value.operator_name = DEG;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                value.number_value = 2;
                node_init(&new_node_4, value, NUMBER_TYPE, new_node_3);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->right = new_node_4;
                new_node_3->left = node_copy(node->left, new_node_4);
                break;
            case ARCCTG:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = DIV;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = -1;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node);

                value.operator_name = ADD;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                value.number_value = 1;
                node_init(&new_node_4, value, NUMBER_TYPE, new_node_3);

                value.operator_name = DEG;
                node_init(&new_node_5, value, OPERATOR_TYPE, new_node_3);

                value.number_value = 2;
                node_init(&new_node_6, value, NUMBER_TYPE, new_node_5);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = new_node_4;
                new_node_3->right = new_node_5;
                new_node_5->left = node_copy(node->left, new_node_5);
                new_node_5->right = new_node_6;
                break;
            case EXP:
                value.operator_name = MUL;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = EXP;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = node_copy(node->left, new_node_1);
                break;
            case LN:
                value.operator_name = DIV;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                new_node->left = dL;
                new_node->right = node_copy(node->left, new_node);
                break;
            case SQRT:
                value.operator_name = DIV;
                node_init(&new_node, value, OPERATOR_TYPE, parent);

                value.operator_name = MUL;
                node_init(&new_node_1, value, OPERATOR_TYPE, new_node);

                value.number_value = 2;
                node_init(&new_node_2, value, NUMBER_TYPE, new_node_1);

                value.operator_name = SQRT;
                node_init(&new_node_3, value, OPERATOR_TYPE, new_node_1);

                new_node->left = dL;
                new_node->right = new_node_1;
                new_node_1->left = new_node_2;
                new_node_1->right = new_node_3;
                new_node_3->left = node_copy(node->left, new_node_3);
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

    if (node_init(&node_cpy, node->value, node->type, parent))
        return NULL;

    if (node->left != NULL) node_cpy->left = node_copy(node->left, node_cpy);

    if (node->right != NULL) node_cpy->right = node_copy(node->right, node_cpy);

    return node_cpy;
}


#undef dL
#undef dR
