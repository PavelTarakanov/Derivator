#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include "file_using.h"
#include "derivative.h"

#define NUM_(value) node_init((tree_elem_t) {.number_value = value}, NUMBER_TYPE, NULL, NULL)
#define VAR_(number) node_init((tree_elem_t) {.variable_number = number}, VARIABLE_TYPE, NULL, NULL)
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

const int START_STR_LEN = 10;

node_t* infix_read(char* file_name, tree_t* tree)
{
    assert(file_name);
    assert(tree);

    FILE* input_address = NULL;
    struct stat statistics = {};
    char* buffer = NULL;
    char* buffer_begin = NULL;
    int buffer_len = 0;
    char symbol = 0;
    node_t* value = NULL;

    if (check_file_opening(file_name, &input_address, "r"))
        return NULL;

    if (fstat(fileno(input_address), &statistics))
        return NULL;

    buffer = (char*) calloc(statistics.st_size, sizeof(char));

    if (buffer == NULL)
        return NULL;

    for (int i = 0; i < statistics.st_size; i++)
    {
        symbol = fgetc(input_address);

        if (isgraph(symbol) || symbol < 0)//для русского
        {
            buffer[buffer_len] = symbol;
            buffer_len++;
        }
    }

    for (int i = 0; i < buffer_len; i++)
        printf("%c", buffer[i]);

    printf("\n");

    buffer_begin = buffer;

    if (check_file_closing(input_address))
        printf("Error while closing file!");

    value = get_g(&buffer, tree);

    make_parents(value, NULL);
    
    free(buffer_begin);

    return value;
}

node_t* get_g(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    node_t* val = get_e(buffer, tree);

    if (**buffer != '$')
        printf("Syntax_Error in get_g\nBuffer = %s\n", *buffer);

    (*buffer)++;
    return val;
}

node_t* get_e(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    node_t* val = get_t(buffer, tree);
    char op = '\0';
    node_t* val_2 = NULL;

    while (**buffer == '+' || **buffer == '-')
    {
        op = **buffer;
        (*buffer)++;
        val_2 = get_t(buffer, tree);

        if (op == '+')
            val = ADD_(val, val_2);
        else
            val = SUB_(val, val_2);
    }

    return val;
}

node_t* get_t(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    node_t* val = get_minus(buffer, tree);
    char op = '\0';
    node_t* val_2 = NULL;

    while (**buffer == '*' || **buffer == '/')
    {
        op = **buffer;
        (*buffer)++;

        val_2 = get_minus(buffer, tree);

        if (op == '*')
            val = MUL_(val, val_2);
        else
            val = DIV_(val, val_2);
    }

    return val;
}

node_t* get_minus(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    if (**buffer != '-')
        return get_d(buffer, tree);

    (*buffer)++;
    return UNAR_MINUS_(get_d(buffer, tree));
}

node_t* get_d(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    node_t* val = get_p(buffer, tree);
    node_t* val_2 = NULL;

    while (**buffer == '^')
    {
        (*buffer)++;

        val_2 = get_p(buffer, tree);

        val = DEG_(val, val_2);
    }

    return val;
}

node_t* get_p(char** buffer, tree_t* tree)
{
    assert(buffer);

    node_t* val = 0;

    if (**buffer == '(')
    {
        (*buffer)++;
        val = get_e(buffer, tree);

        if (**buffer == ')')
            (*buffer)++;
        else
            printf("Syntax_Error in get_p\nBuffer = %s\n", *buffer);

        return val;
    }
    else
        val = get_n(buffer);

    if (val == NULL)
        val = get_v(buffer, tree);

    if (val == NULL)
        val = get_f(buffer, tree);

    return val;
}

node_t* get_f(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    int str_len = START_STR_LEN;
    char* var = (char*) calloc(str_len, sizeof(char));
    int i = 0;

    while (('a' <= **buffer && **buffer <= 'z') || ('A' <= **buffer && **buffer <= 'Z'))
    {
        //printf("%c\n", **buffer);
        if (i == str_len)
        {
            str_len *= 2;
            var = (char*) realloc(var, str_len);
        }
        var[i] = **buffer;
        (*buffer)++;
        i++;
    }

    //printf("operator = %s\n", var);

    if (strcmp(var, "sin") == 0)
    {
        free(var);
        return SIN_(get_p(buffer, tree));
    }
    else if (strcmp(var, "cos") == 0)
    {
        free(var);
        return COS_(get_p(buffer, tree));
    }
    else if (strcmp(var, "tg") == 0)
    {
        free(var);
        return TG_(get_p(buffer, tree));
    }
    else if (strcmp(var, "ctg") == 0)
    {
        free(var);
        return CTG_(get_p(buffer, tree));
    }
    else if (strcmp(var, "arscin") == 0)
    {
        free(var);
        return ARCSIN_(get_p(buffer, tree));
    }
    else if (strcmp(var, "arccos") == 0)
    {
        free(var);
        return ARCCOS_(get_p(buffer, tree));
    }
    else if (strcmp(var, "arctg") == 0)
    {
        free(var);
        return ARCTG_(get_p(buffer, tree));
    }
    else if (strcmp(var, "arcctg") == 0)
    {
        free(var);
        return ARCCTG_(get_p(buffer, tree));
    }
    else if (strcmp(var, "ln") == 0)
    {
        free(var);
        return LN_(get_p(buffer, tree));
    }
    else if (strcmp(var, "sqrt") == 0)
    {
        free(var);
        return SQRT_(get_p(buffer, tree));
    }

    printf("ERROR! Unknown operator in get_f\nbuffer = %s", *buffer);
    return NULL;
}

node_t* get_n(char** buffer)
{
    assert(buffer);

    double val = 0;
    char* buffer_at_start = *buffer;
    char exclamation_mark = '+';

    if (**buffer == '-')
        (*buffer)++;

    while ('0' <= **buffer && **buffer <= '9')
    {
        val = val*10 + (**buffer - '0');
        (*buffer)++;
    }
    if (buffer_at_start == *buffer || (exclamation_mark == '-' && *buffer == buffer_at_start + 1))
        return NULL;

    return NUM_(val);
}

node_t* get_v(char** buffer, tree_t* tree)
{
    assert(buffer);
    assert(tree);

    int str_len = START_STR_LEN;
    char* var = (char*) calloc(str_len, sizeof(char));
    int i = 0;

    while (('a' <= **buffer && **buffer <= 'z') || ('A' <= **buffer && **buffer <= 'Z') || **buffer == '_')
    {
        //printf("%c\n", **buffer);
        if (i == str_len)
        {
            str_len *= 2;
            var = (char*) realloc(var, str_len);
        }
        var[i] = **buffer;
        (*buffer)++;
        i++;
    }

    if (strcmp(var, "sin") * strcmp(var, "cos") * strcmp(var, "tg") * strcmp(var, "ctg") * strcmp(var, "arcsin") *
        strcmp(var, "arccos") * strcmp(var, "arctg") * strcmp(var, "arcctg") * strcmp(var, "ln") * strcmp(var, "sqrt") == 0)
    {
        (*buffer) -= strlen(var);
        free(var);
        return NULL;
    }

    //printf("buffer after get_v = %s\n", *buffer);

    return make_new_var(var, tree);
}

node_t* make_new_var(char* str, tree_t* tree)
{
    assert(tree);
    assert(str);

    for (int i = 0; i < tree->number_of_variables; i++)
        if (strcmp(str, tree->variable_list[i].var_name) == 0)
            return VAR_(i);

    tree->variable_list[tree->number_of_variables].var_name = strdup(str);

    if (tree->variable_list[tree->number_of_variables].var_name == NULL)
        return NULL;

    tree->number_of_variables++;

    free(str);

    return VAR_(tree->number_of_variables - 1);
}
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
