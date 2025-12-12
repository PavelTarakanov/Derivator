#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

static tree_errors node_visual_dump(node_t* node, FILE* const dump_address, char* node_way, tree_t* tree);
static tree_errors print_label(FILE* dump_address, node_t* node, tree_t* tree);
static tree_errors operator_print(FILE* dump_address, node_t* node);


tree_errors tree_dump(node_t* root, tree_t* tree)
{
    assert(root);
    assert(tree);

    FILE* dump_address = NULL;
    char root_address[100] = "Z";

    if (check_file_opening("dump.txt" , &dump_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(dump_address, "digraph{\n");

    node_visual_dump(root, dump_address, root_address, tree);

    fprintf(dump_address, "}");

    if (check_file_closing(dump_address))
        return FILE_CLOSING_ERROR;

    if (system("dot dump.txt -T png -o visual_dump.png") != 0)
        return GRAPH_MAKING_ERROR;

    if (check_file_opening("latex_dump.tex" , &dump_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(dump_address, "\\begin{equation}\n\t");

    if (node_latex_dump(root, tree, dump_address))
        return DUMP_ERROR;

    fprintf(dump_address, "\n\\end{equation}\n");

    if (check_file_closing(dump_address))
        return FILE_CLOSING_ERROR;

    return NO_ERROR;
}

tree_errors node_latex_dump(node_t* node, const tree_t* tree, FILE* const dump_address)
{
    assert(node);
    assert(tree);
    assert(dump_address);

    if (node->type == NUMBER_TYPE)
        fprintf(dump_address, "%.3lf", node->value.number_value);
    else if (node->type == VARIABLE_TYPE)
        fprintf(dump_address, "%s", tree->variable_list[node->value.variable_number].var_name);
    else if (node->type == OPERATOR_TYPE)
    {
        if (node->value.operator_name < SIN && node->value.operator_name != DIV)
        {
            if (node->parent != NULL)
            {
                if (node->parent->value.operator_name >= MUL &&
                    (node->value.operator_name == ADD || node->value.operator_name == SUB))
                    fprintf(dump_address, "(");
                if (node->parent->value.operator_name >= DEG &&
                (node->value.operator_name == MUL || node->value.operator_name == DIV))
                fprintf(dump_address, "(");
            }

            node_latex_dump(node->left, tree, dump_address);

            switch(node->value.operator_name)
            {
                case ADD:
                    fprintf(dump_address, "+");
                    break;
                case SUB:
                    fprintf(dump_address, "-");
                    break;
                case MUL:
                    fprintf(dump_address, "*");
                    break;
                case DEG:
                    fprintf(dump_address, "^{");
                    break;
                case DIV:
                case SIN:
                case COS:
                case TG:
                case CTG:
                case ARCSIN:
                case ARCCOS:
                case ARCTG:
                case ARCCTG:
                case LN:
                case SQRT:
                case EXP:
                case UNAR_MINUS:
                default:
                    printf("ERROR! Unknown operator - %d\n", node->value.operator_name);
                    return UNKNOWN_OPERATOR_ERROR;
            }

            node_latex_dump(node->right, tree, dump_address);
            if (node->value.operator_name == DEG) fprintf(dump_address, "}");

            if (node->parent != NULL)
            {
                if (node->parent->value.operator_name >= MUL &&
                    (node->value.operator_name == ADD || node->value.operator_name == SUB))
                    fprintf(dump_address, ")");
                if (node->parent->value.operator_name >= DEG &&
                (node->value.operator_name == MUL || node->value.operator_name == DIV))
                fprintf(dump_address, ")");
            }
        }
        else if (node->value.operator_name == DIV)
        {
            fprintf(dump_address, "\\frac{");

            node_latex_dump(node->left, tree, dump_address);

            fprintf(dump_address, "}{");

            node_latex_dump(node->right, tree, dump_address);

            fprintf(dump_address, "}");
        }
        else if (node->value.operator_name > DEG && node->value.operator_name != EXP && node->value.operator_name != SQRT)
        {
            switch(node->value.operator_name)
            {
                case SIN:
                    fprintf(dump_address, "\\sin");
                    break;
                case COS:
                    fprintf(dump_address, "\\cos");
                    break;
                case TG:
                    fprintf(dump_address, "\\tan");
                    break;
                case CTG:
                    fprintf(dump_address, "\\cot");
                    break;
                case ARCSIN:
                    fprintf(dump_address, "\\arcsin");
                    break;
                case ARCCOS:
                    fprintf(dump_address, "\\arccos");
                    break;
                case ARCTG:
                    fprintf(dump_address, "\\arctan");
                    break;
                case ARCCTG:
                    fprintf(dump_address, "\\arccot");
                    break;
                case LN:
                    fprintf(dump_address, "\\ln");
                    break;
                case UNAR_MINUS:
                    fprintf(dump_address, "-");
                    break;
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case DEG:
                case EXP:
                case SQRT:
                default:
                    printf("ERROR! Unknown operator - %d\n", node->value.operator_name);;
                    return UNKNOWN_OPERATOR_ERROR;
            }

            node_latex_dump(node->left, tree, dump_address);
        }
        else if (node->value.operator_name == EXP)
        {
            fprintf(dump_address, "e^{");
            node_latex_dump(node->left, tree, dump_address);
            fprintf(dump_address, "}");
        }
        else if (node->value.operator_name == SQRT)
        {
            fprintf(dump_address, "\\sqrt{");
            node_latex_dump(node->left, tree, dump_address);
            fprintf(dump_address, "}");
        }
        else
        {
            printf("ERROR! Unknown operator - %d\n", node->value.operator_name);
            return UNKNOWN_OPERATOR_ERROR;
        }
    }
    else
        return UNKNOWN_OPERATOR_ERROR;

    return NO_ERROR;
}

tree_errors node_visual_dump(node_t* node, FILE* const dump_address, char* node_way, tree_t* tree)
{
    assert(node);
    assert(dump_address);
    assert(node_way);
    assert(tree);

    char *left_way = (char*) calloc(strlen(node_way) + 2, sizeof(char));
    char *right_way = (char*) calloc(strlen(node_way) + 2 , sizeof(char));
    tree_errors error = NO_ERROR;

    if (left_way == NULL || right_way == NULL)
        return ALLOCATION_ERROR;

    strcpy(left_way, node_way);
    strcpy(right_way, node_way);


    fprintf(dump_address, "\t%s", node_way);
    if (node->type == OPERATOR_TYPE)
        fprintf(dump_address, "[shape = record, style=\"filled\",fillcolor=\"lightgreen\", ");
    else if (node->type == VARIABLE_TYPE)
        fprintf(dump_address, "[shape = record, style=\"filled\",fillcolor=\"lightblue\", ");
    else if (node->type == NUMBER_TYPE)
        fprintf(dump_address, "[shape = record, style=\"filled\",fillcolor=\"yellow\", ");
    else
        return TYPE_ERROR;

    print_label(dump_address, node, tree);

    if (node->left != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(left_way, "L"));
        error = node_visual_dump(node->left, dump_address, left_way, tree);

        if (error)
            return error;
    }
    if (node->right != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(right_way, "R"));
        error = node_visual_dump(node->right, dump_address, right_way, tree);

        if (error)
            return error;
    }

    free(left_way);
    free(right_way);

    return error;
}

tree_errors print_label(FILE* dump_address, node_t* node, tree_t* tree)
{
    assert(dump_address);
    assert(node);
    assert(tree);

    if (node->type == NUMBER_TYPE)
        fprintf(dump_address, "label=\"{%lf}\"];\n", node->value.number_value);
    else if (node->type == OPERATOR_TYPE)
    {
        if (operator_print(dump_address, node))
            return UNKNOWN_OPERATOR_ERROR;
    }
    else if (node->type == VARIABLE_TYPE)
        fprintf(dump_address, "label=\"{%s}\"];\n", tree->variable_list[node->value.variable_number].var_name);

    return NO_ERROR;
}

tree_errors operator_print(FILE* dump_address, node_t* node)
{
    assert(dump_address);
    assert(node);

    switch(node->value.operator_name)
    {
        case ADD:
            fprintf(dump_address, "label=\"{+}\"];\n");
            break;
        case SUB:
            fprintf(dump_address, "label=\"{-}\"];\n");
            break;
        case MUL:
            fprintf(dump_address, "label=\"{*}\"];\n");
            break;
        case DIV:
            fprintf(dump_address, "label=\"{/}\"];\n");
            break;
        case DEG:
            fprintf(dump_address, "label=\"{^}\"];\n");
            break;
        case SIN:
            fprintf(dump_address, "label=\"{sin}\"];\n");
            break;
        case COS:
            fprintf(dump_address, "label=\"{cos}\"];\n");
            break;
        case TG:
            fprintf(dump_address, "label=\"{tg}\"];\n");
            break;
        case CTG:
            fprintf(dump_address, "label=\"{ctg}\"];\n");
            break;
        case ARCSIN:
            fprintf(dump_address, "label=\"{arcsin}\"];\n");
            break;
        case ARCCOS:
            fprintf(dump_address, "label=\"{arccos}\"];\n");
            break;
        case ARCTG:
            fprintf(dump_address, "label=\"{arctg}\"];\n");
            break;
        case ARCCTG:
            fprintf(dump_address, "label=\"{arcctg}\"];\n");
            break;
        case EXP:
            fprintf(dump_address, "label=\"{exp}\"];\n");
            break;
        case LN:
            fprintf(dump_address, "label=\"{ln}\"];\n");
            break;
        case SQRT:
            fprintf(dump_address, "label=\"{sqrt}\"]\n");
            break;
        case UNAR_MINUS:
            fprintf(dump_address, "label=\"{-}\"]\n");
            break;
        default:
            printf("ERROR: unknown operator");
            return UNKNOWN_OPERATOR_ERROR;
        }

    return NO_ERROR;
}
