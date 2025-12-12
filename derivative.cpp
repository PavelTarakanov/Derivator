#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

#define dL node_derivate(node->left, tree, derivating_variable, output_address)
#define dR node_derivate(node->right, tree, derivating_variable, output_address)
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

static node_t* node_derivate(node_t* node, const tree_t* const tree, const char* const derivating_variable, FILE* output_address);
static node_t* node_copy(node_t* node);
static double fac(int n);
tree_errors derivator(tree_t* tree)
{
    assert(tree);

    int derivate_number = 0;
    FILE* output_address = NULL;
    char derivating_var[256] = "";
    node_t* old_node = NULL;

    printf("Введите номер производной\n");
    while (scanf("%d", &derivate_number) != 1)
        printf("Введите номер производной\n");

    printf("Введите переменную дифференцирования, название переменной не должно содержать больше 255 символов\n");
    scanf("%255s", derivating_var);

    printf("Ведите значения переменных в точке дифференцирования\n");
    for (int i = 0; i < tree->number_of_variables; i++)
    {
        printf("%s = ", tree->variable_list[i].var_name);
        scanf("%lf", &(tree->variable_list[i].var_value));
    }
    if (check_file_opening("derivating.tex" , &output_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(output_address, "\\section*{Нахождение %d-й производной}\n", derivate_number);

    for (int i = 1; i < derivate_number + 1; i++)
    {
        fprintf(output_address, "\\subsection*{Найдём %d-ю производную}\n", i);
        old_node = tree->root;
        tree->root = node_derivate(tree->root, tree, derivating_var, output_address);
        tree->root = equation_simplification(tree->root, tree);
        make_parents(tree->root, NULL);
        node_destroy(old_node);
    }

    if (check_file_closing(output_address))
        return FILE_CLOSING_ERROR;

    tree_dump(tree->root, tree);

    printf("%lf\n", node_calculate(tree->root, tree));
    return NO_ERROR;
}

node_t* node_derivate(node_t* node, const tree_t* const tree, const char* const derivating_variable, FILE* output_address)
{
    assert(node);
    assert(tree);
    assert(derivating_variable);

    node_t* new_node = NULL;

    if (node->type == NUMBER_TYPE)
        new_node = NUM_(0);
    else if (node->type == VARIABLE_TYPE)
    {
        if (strcmp(derivating_variable, tree->variable_list[node->value.variable_number].var_name) == 0)
            new_node = NUM_(1);
        else
            new_node = NUM_(0);
    }
    else if (node->type == OPERATOR_TYPE)
    {
        switch(node->value.operator_name)
        {
            case ADD:
                new_node = ADD_(dR, dL);
                break;
            case SUB:
                new_node =  SUB_(dR, dL);
                break;
            case MUL:
                new_node = ADD_(MUL_(dL, cR), MUL_(dR, cL));
                break;
            case DIV:
                new_node = DIV_(SUB_(MUL_(dL, cR), MUL_(dR, cL)), DEG_(cR, NUM_(2)));
                break;
            case DEG:
                if (node->left->type == NUMBER_TYPE)
                    new_node = MUL_(dR, MUL_(LN_(cL), DEG_(cL, cR)));
                else if (node->right->type == NUMBER_TYPE)
                    new_node = MUL_(dL, MUL_(cR, DEG_(cL, SUB_(cR, NUM_(1)))));
                else
                    new_node = MUL_(DEG_(cL, cR), ADD_(MUL_(dR, LN_(cL)), MUL_(cR, DIV_(dL, cL))));
                break;
            case SIN:
                new_node = MUL_(dL, COS_(cL));
                break;
            case COS:
                new_node = MUL_(dL, MUL_(NUM_(-1), SIN_(cL)));
                break;
            case TG:
                new_node = DIV_(dL, DEG_(COS_(cL), NUM_(2)));
                break;
            case CTG:
                new_node = MUL_(dL, DIV_(NUM_(-1), DEG_(SIN_(cL), NUM_(2))));
                break;
            case ARCSIN:
                new_node = DIV_(dL, SQRT_(SUB_(NUM_(1), DEG_(cL, NUM_(2)))));
                break;
            case ARCCOS:
                new_node = MUL_(NUM_(-1), DIV_(dL, SQRT_(SUB_(NUM_(1), DEG_(cL, NUM_(2))))));
                break;
            case ARCTG:
                new_node = DIV_(dL, ADD_(NUM_(1), DEG_(cL, NUM_(2))));
                break;
            case ARCCTG:
                new_node = MUL_(NUM_(-1), DIV_(dL, ADD_(NUM_(1), DEG_(cL, NUM_(2)))));
                break;
            case EXP:
                new_node = MUL_(dL, EXP_(cL));
                break;
            case LN:
                new_node = DIV_(dL, cL);
                break;
            case SQRT:
                new_node = DIV_(dL, MUL_(NUM_(2), SQRT_(cL)));
                break;
            case UNAR_MINUS:
                new_node = UNAR_MINUS_(dL);
                break;
            default:
                printf("ERROR: unknown operator");
                new_node = NULL;
        }
    }
    fprintf(output_address, "\\begin{equation}\n"
                            "\t \\frac{d}{dx}(");

    node_latex_dump(node, tree, output_address);
    fprintf(output_address, ") = ");
    node_latex_dump(new_node, tree, output_address);

    fprintf(output_address, "\n\\end{equation}\n");

    if (new_node == NULL)
        printf("ERROR while derivating!\n");

    return new_node;
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

tree_errors Teilor(tree_t* tree)
{
    assert(tree);

    FILE* teilor_dump_address = NULL;
    FILE* derivating_dump_address = NULL;
    node_t* old_node = tree->root;
    int decomposition_degree = 0;
    char decomposition_var[256] = "";

    if (check_file_opening("teilor_dump.tex" , &teilor_dump_address, "w+"))
        return FILE_OPENING_ERROR;

    if (check_file_opening("derivating.tex" , &derivating_dump_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(teilor_dump_address, "\\begin{equation}\n\t");

    if (node_latex_dump(tree->root, tree, teilor_dump_address))
        return DUMP_ERROR;

    fprintf(teilor_dump_address, " = ");

    printf("Введите степень разложения: ");
    scanf("%d", &decomposition_degree);

    printf("Введите переменную разложения: ");
    scanf("%255s", decomposition_var);

    printf("Ведите значения переменных в точке разложения\n");
    for (int i = 0; i < tree->number_of_variables; i++)
    {
        printf("%s = ", tree->variable_list[i].var_name);
        scanf("%lf", &(tree->variable_list[i].var_value));
    }

    fprintf(teilor_dump_address, "%lf + ", node_calculate(tree->root, tree));

    for (int i = 1; i < decomposition_degree; i++)
    {
        tree->root = node_derivate(tree->root, tree, decomposition_var, derivating_dump_address);
        fprintf(teilor_dump_address, "%lf*(%s-%s_0)^%d + ",
                node_calculate(tree->root, tree)/fac(i), decomposition_var, decomposition_var, i);
        node_destroy(old_node);
        old_node = tree->root;
    }

    fprintf(teilor_dump_address, "o(%s-%s_0)^%d", decomposition_var, decomposition_var, decomposition_degree);

    fprintf(teilor_dump_address, "\n\\end{equation}\n");

    if (check_file_closing(teilor_dump_address))
        return FILE_CLOSING_ERROR;

    if (check_file_closing(derivating_dump_address))
        return FILE_CLOSING_ERROR;

    return NO_ERROR;
}

double fac(int n)
{
    int fac = 1;

    for (int i = 1; i <= n; i++)
    {
        fac *= i;
    }

    return (double) fac;
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
