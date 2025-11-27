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

node_t* add_derivate(node_t* node, const tree_t* const tree, node_t* parent, const char* const derivating_variable)
{
    assert(node);
    assert(tree);
    assert(derivating_variable);

    node_t* new_node = NULL;
    tree_elem_t value = {};

    value.operator_name = ADD;
    node_init(&new_node, value, OPERATOR_TYPE, parent);
    new_node->left = dL;
    new_node->right = dR;

    return new_node;
}

node_t* sub_derivate(node_t* node, const tree_t* const tree, node_t* parent, const char* const derivating_variable)
{
    assert(node);
    assert(tree);
    assert(derivating_variable);

    node_t* new_node = NULL;
    tree_elem_t value = {};

    value.operator_name = SUB;
    node_init(&new_node, value, OPERATOR_TYPE, parent);
    new_node->left = dL;
    new_node->right = dR;

    return new_node;
}

node_t* mul_derivate(node_t* node, const tree_t* const tree, node_t* parent, const char* const derivating_variable)
{
    assert(node);
    assert(tree);
    assert(derivating_variable);

    node_t* new_node = NULL;
    node_t* new_node_1 = NULL;
    node_t* new_node_2 = NULL;
    tree_elem_t value = {};

    value.operator_name = ADD;
    node_init(&new_node, value, OPERATOR_TYPE, parent);
    value.operator_name = MUL;
    node_init(&new_node_1, value, OPERATOR_TYPE, new_node);
    node_init(&new_node_2, value, OPERATOR_TYPE, new_node);

    new_node->left = new_node_1;
    new_node->right = new_node_2;
    new_node_1->left = dL;
    new_node_1->right = node_copy(node->right, new_node_1);
    new_node_2->left = dR;
    new_node_2->right = node_copy(node->left, new_node_1);

    return new_node;
}
#undef dL
#undef dR
