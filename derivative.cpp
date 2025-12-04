#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

#define dL node_derivate(node->left, tree, derivating_variable)
#define dR node_derivate(node->right, tree, derivating_variable)
#define cL node_copy(node->left)
#define cR node_copy(node->right)
#define NUM_(value) node_init((tree_elem_t) {.number_value = value}, NUMBER_TYPE, NULL, NULL)
#define ADD_(left, right) node_init((tree_elem_t) {.operator_name = ADD}, OPERATOR_TYPE, left, right)
#define SUB_(left, right) node_init((tree_elem_t) {.operator_name = SUB}, OPERATOR_TYPE, left, right)
#define MUL_(left, right) node_init((tree_elem_t) {.operator_name = MUL}, OPERATOR_TYPE, left, right)
#define DIV_(left, right) node_init((tree_elem_t) {.operator_name = DIV}, OPERATOR_TYPE, left, right)
#define DEG_(left, right) node_init((tree_elem_t) {.operator_name = DEG}, OPERATOR_TYPE, left, right)
#define SIN_(left) node_init((tree_elem_t) {.operator_name = SIN}, OPERATOR_TYPE, left, NULL)
#define COS_(left) node_init((tree_elem_t) {.operator_name = COS}, OPERATOR_TYPE, left, NULL)
#define TG_(left) node_init((tree_elem_t) {.operator_name = TG}, OPERATOR_TYPE, left, NULL)
#define CTG_(left) node_init((tree_elem_t) {.operator_name = CTG}, OPERATOR_TYPE, left, NULL)
#define ARCSIN_(left) node_init((tree_elem_t) {.operator_name = ARCSIN}, OPERATOR_TYPE, left, NULL)
#define ARCCOS_(left) node_init((tree_elem_t) {.operator_name = ARCCOS}, OPERATOR_TYPE, left, NULL)
#define ARCTG_(left) node_init((tree_elem_t) {.operator_name = ARCTG}, OPERATOR_TYPE, left, NULL)
#define ARCCTG_(left) node_init((tree_elem_t) {.operator_name = ARCCTG}, OPERATOR_TYPE, left, NULL)
#define LN_(left) node_init((tree_elem_t) {.operator_name = LN}, OPERATOR_TYPE, left, NULL)
#define EXP_(left) node_init((tree_elem_t) {.operator_name = EXP}, OPERATOR_TYPE, left, NULL)
#define SQRT_(left) node_init((tree_elem_t) {.operator_name = SQRT}, OPERATOR_TYPE, left, NULL)
#define UNAR_MINUS_(left) node_init((tree_elem_t) {.operator_name = UNAR_MINUS}, OPERATOR_TYPE, left, NULL)

node_t* node_derivate(node_t* node, const tree_t* const tree, const char* const derivating_variable)
{
    assert(node);
    assert(tree);
    assert(derivating_variable);

    if (node->type == NUMBER_TYPE)
        return node_init((tree_elem_t) {.number_value = 0}, NUMBER_TYPE, NULL, NULL);
    else if (node->type == VARIABLE_TYPE)
    {
        if (strcmp(derivating_variable, tree->variable_list[node->value.variable_number].var_name) == 0)
            return node_init((tree_elem_t) {.number_value = 1}, NUMBER_TYPE, NULL, NULL);
        else
            return node_init((tree_elem_t) {.number_value = 0}, NUMBER_TYPE, NULL, NULL);
    }
    else if (node->type == OPERATOR_TYPE)
    {
        switch(node->value.operator_name)
        {
            case ADD:
                return ADD_(dR, dL);
            case SUB:
                return  SUB_(dR, dL);
            case MUL:
                return ADD_(MUL_(dL, cR), MUL_(dR, cL));
            case DIV:
                return DIV_(SUB_(MUL_(dL, cR), MUL_(dR, cL)), DEG_(cR, NUM_(2)));
            case DEG:
                if (node->left->type == NUMBER_TYPE)
                    return MUL_(dR, MUL_(LN_(cL), DEG_(cL, cR)));
                else if (node->right->type == NUMBER_TYPE)
                    return MUL_(dL, MUL_(cR, DEG_(cL, SUB_(cR, NUM_(1)))));
                else
                    return NULL;//TODO
            case SIN:
                return MUL_(dL, COS_(cL));
            case COS:
                return MUL_(dL, MUL_(NUM_(-1), SIN_(cL)));
            case TG:
                return DIV_(dL, DEG_(COS_(cL), NUM_(2)));
            case CTG:
                return MUL_(dL, DIV_(NUM_(-1), DEG_(SIN_(cL), NUM_(2))));
            case ARCSIN:
                return DIV_(dL, SQRT_(SUB_(NUM_(1), DEG_(cL, NUM_(2)))));
            case ARCCOS:
                return MUL_(NUM_(-1), DIV_(dL, SQRT_(SUB_(NUM_(1), DEG_(cL, NUM_(2))))));
            case ARCTG:
                return DIV_(dL, ADD_(NUM_(1), DEG_(cL, NUM_(2))));
            case ARCCTG:
                return MUL_(NUM_(-1), DIV_(dL, ADD_(NUM_(1), DEG_(cL, NUM_(2)))));
            case EXP:
                return MUL_(dL, EXP_(cL));
            case LN:
                return DIV_(dL, cL);
            case SQRT:
                return DIV_(dL, MUL_(NUM_(2), SQRT_(cL)));
            case UNAR_MINUS:
                return UNAR_MINUS_(dL);
            default:
                printf("ERROR: unknown operator");
                return NULL;
        }
    }

    return NULL;
}

node_t* node_copy(node_t* node)
{
    assert(node);

    node_t* left = NULL;
    node_t* right = NULL;

    if (node->left != NULL) left = node_copy(node->left);
    if (node->right != NULL) right = node_copy(node->right);

    return node_init(node->value, node->type, left, right);
}


#undef dL
#undef dR
#undef cL
#undef cR
#undef NUM_
#undef ADD_
#undef SUB_
#undef MUL_
#undef DIV_
#undef DEG_
#undef SIN_
#undef COS_
#undef TG_
#undef CTG_
#undef ARCSIN_
#undef ARCCOS_
#undef ARCTG_
#undef ARCCTG_
#undef LN_
#undef EXP_
#undef SQRT_
#undef UNAR_MINUS_
