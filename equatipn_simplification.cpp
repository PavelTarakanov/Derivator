#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

node_t* equation_simplification(node_t* node, tree_t* tree)
{
    assert(node);

    node = number_calculate(node, tree);
    //node = delete_zeros(node, tree);

    return node;
}

node_t* number_calculate(node_t* node, tree_t* tree)
{
    assert(node);
    assert(tree);

    if (node->type != OPERATOR_TYPE)
        return node;

    if (node->type == OPERATOR_TYPE && node->value.operator_name < SIN)
    {
        if (node->left->type == NUMBER_TYPE && node->right->type == NUMBER_TYPE)
        {
            node->value.number_value = node_calculate(node, tree);
            node->type = NUMBER_TYPE;
            free(node->left);
            free(node->right);
            node->left = NULL;
            node->right = NULL;
        }
        else
        {
            if (node->left != NULL) node->left = number_calculate(node->left, tree);
            if (node->right != NULL) node->right = number_calculate(node->right, tree);

            if (node->left->type == NUMBER_TYPE && node->right->type == NUMBER_TYPE)
            {
                node->value.number_value = node_calculate(node, tree);
                node->type = NUMBER_TYPE;
                free(node->left);
                free(node->right);
                node->left = NULL;
                node->right = NULL;
            }
        }
    }
    else
    {
        if (node->left->type == NUMBER_TYPE)
        {
            node->value.number_value = node_calculate(node, tree);
            node->type = NUMBER_TYPE;
            free(node->left);
            node->left = NULL;
        }
        else
        {
            if (node->left != NULL) number_calculate(node->left, tree);

            if (node->left->type == NUMBER_TYPE)
            {
                node->value.number_value = node_calculate(node, tree);
                node->type = NUMBER_TYPE;
                free(node->left);
                node->left = NULL;
            }
        }
    }
    return node;
}
/*
node_t* delete_zeros(node_t* node, tree_t* tree)
{
    assert(node);
    assert(tree);

    if (node->type == OPERATOR_TYPE && node->value < SIN)
    {
        if (node->left->value == 0)
        {
            switch
        }
    }
    return node;
}
*/
