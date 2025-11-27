#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

const unsigned int VAR_LIST_LEN = 100;

tree_errors tree_init(tree_t** tree)
{
    assert(tree);


    (*tree) = (tree_t*) calloc(1, sizeof(tree_t));
    if ((*tree) == NULL)
        return ALLOCATION_ERROR;

    (*tree)->root = NULL;
    (*tree)->number_of_variables = 0;
    (*tree)->variable_list = (variable_t*) calloc(VAR_LIST_LEN, sizeof(variable_t));

    if ((*tree)->variable_list == NULL)
        return ALLOCATION_ERROR;

    return NO_ERROR;
}

tree_errors node_init(node_t** node, tree_elem_t value, type_t type, node_t* parent)
{
    assert(node);

    *node = (node_t*) calloc(1, sizeof(node_t));

    if (*node == NULL)
        return ALLOCATION_ERROR;

    (*node)->type = type;
    (*node)->value = value;
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->parent = parent;

    return NO_ERROR;
}

void node_destroy(node_t* node)
{
    assert(node);

    if (node->left != NULL)
        node_destroy(node->left);
    if (node->right != NULL)
        node_destroy(node->right);

    free(node);

    return;
}

void tree_destroy(tree_t* tree)
{
    assert(tree);

    node_destroy(tree->root);

    for (int i = 0; i < tree->number_of_variables; i++)
        free(tree->variable_list[i].var_name);

    free(tree->variable_list);
    free(tree);
}
