#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

static node_t* number_calculate(node_t* node, tree_t* tree);
static node_t* delete_zeros(node_t** node, tree_t* tree);

const double INACCURACY = 10e-5;

node_t* equation_simplification(node_t* node, tree_t* tree)
{
    assert(node);
    assert(tree);

    node = number_calculate(node, tree);
    node = delete_zeros(&node, tree);
    node = number_calculate(node, tree);
    node = delete_zeros(&node, tree);

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

node_t* delete_zeros(node_t** node, tree_t* tree)
{
    assert(node);
    assert(*node);
    assert(tree);

    node_t* left = (*node)->left;
    node_t* right = (*node)->right;
    node_t* old_node = *node;

    if ((*node)->left != NULL) delete_zeros(&(*node)->left, tree);
    if ((*node)->right != NULL) delete_zeros(&(*node)->right, tree);

    *node = number_calculate(*node, tree);

    left = (*node)->left;
    right = (*node)->right;
    old_node = *node;

    if ((*node)->right == NULL || (*node)->left == NULL)
        return *node;

    if ((*node)->type == OPERATOR_TYPE)
    {
        if ((*node)->value.operator_name == ADD)
        {
            if ((*node)->left->type == NUMBER_TYPE && (*node)->left->value.number_value < INACCURACY)
            {
                *node = (*node)->right;

                free(left);
                free(old_node);
            }
            if ((*node)->right->type == NUMBER_TYPE && (*node)->right->value.number_value < INACCURACY)
            {
                *node = (*node)->left;

                free(right);
                free(old_node);
            }
        }
        else if ((*node)->value.operator_name == SUB)
        {
            if ((*node)->left->type == NUMBER_TYPE && (*node)->left->value.number_value < INACCURACY)
            {
                (*node)->value.operator_name = UNAR_MINUS;
                (*node)->left = right;
                (*node)->right = NULL;

                free(left);
            }
            if ((*node)->right->type == NUMBER_TYPE && (*node)->right->value.number_value < INACCURACY)
            {
                *node = (*node)->left;

                free(right);
                free(old_node);
            }
        }
        else if ((*node)->value.operator_name == MUL)
        {
            if ((*node)->right->type == NUMBER_TYPE && (*node)->right->value.number_value < INACCURACY)
            {
                *node = (*node)->right;

                node_destroy(left);
                free(old_node);
            }
            else if ((*node)->left->type == NUMBER_TYPE && (*node)->left->value.number_value < INACCURACY)
            {
                *node = (*node)->left;

                node_destroy(right);
                free(old_node);
            }
            else if ((*node)->right->type == NUMBER_TYPE && abs((*node)->right->value.number_value - 1) < INACCURACY)
            {
                *node = (*node)->left;

                free(right);
                free(old_node);
            }
            else if ((*node)->left->type == NUMBER_TYPE && abs((*node)->left->value.number_value - 1) < INACCURACY)
            {
                *node = (*node)->right;

                free(left);
                free(old_node);
            }
        }
        else if ((*node)->value.operator_name == DIV)
        {
            if ((*node)->left->type == NUMBER_TYPE && (*node)->left->value.number_value < INACCURACY)
            {
                *node = (*node)->left;

                node_destroy(right);
                free(old_node);
            }
            else if ((*node)->right->type == NUMBER_TYPE && abs((*node)->right->value.number_value - 1) < INACCURACY)
            {
                *node = (*node)->left;

                free(right);
                free(old_node);
            }
        }
        else if ((*node)->value.operator_name == DEG)
        {
           if ((*node)->left->type == NUMBER_TYPE && (*node)->left->value.number_value < INACCURACY)
            {
                *node = (*node)->left;

                node_destroy(right);
                free(old_node);
            }
            if ((*node)->right->type == NUMBER_TYPE && (*node)->right->value.number_value < INACCURACY)
            {
                *node = node_init((tree_elem_t) {.number_value = 1}, NUMBER_TYPE, NULL, NULL);

                node_destroy(right);
                node_destroy(left);
                free(old_node);
            }
        }
    }

    return *node;
}

