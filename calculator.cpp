#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include "file_using.h"
#include "derivative.h"

#define LV node_calculate(node->left, tree)
#define RV node_calculate(node->right, tree)
//если функция одного аргумента, то он - левый

double node_calculate(node_t* node, tree_t* tree)
{
    assert(tree);

    if (node->type == NUMBER_TYPE)
        return node->value;
    else if (node->type == VARIABLE_TYPE)
        return tree->variable_list[(int) node->value].var_value;
    else if (node->type == OPERATOR_TYPE)
    {
        switch((int)node->value)
        {
        case ADD:
            return LV + RV;
        case SUB:
            return LV - RV;
        case MUL:
            return LV * RV;
        case DIV:
            return LV / RV;
        case DEG:
            return pow(LV, RV);
        case SIN:
            return sin(LV);
        case COS:
            return cos(LV);
        case TG:
            return tan(LV);
        case CTG:
            return 1/tan(LV);
        case ARCSIN:
            return asin(LV);
        case ARCCOS:
            return acos(LV);
        case ARCTG:
            return atan(LV);
        case ARCCTG:
            return M_PI/2 - atan(LV);
        case EXP:
            return exp(LV);
        case LN:
            return log(LV);
        default:
            printf("ERROR: unknown operator");
            return NAN;
        }
    }

    return NAN;
}

#undef LV
#undef RV
