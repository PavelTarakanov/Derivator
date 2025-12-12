#ifndef DERIVATIVE_H
#define DERIVATIVE_H

enum type_t{
    OPERATOR_TYPE = 0,
    VARIABLE_TYPE = 1,
    NUMBER_TYPE = 2,
};

enum operations_t{
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    DEG = 5,
    SIN = 6,
    COS = 7,
    TG = 8,
    CTG = 9,
    ARCSIN = 10,
    ARCCOS = 11,
    ARCTG = 12,
    ARCCTG = 13,
    EXP = 14,
    LN = 15,
    SQRT = 16,
    UNAR_MINUS = 17,
};

union tree_elem_t{operations_t operator_name;
                  double number_value;
                  int variable_number;};


struct node_t{tree_elem_t value;
              type_t type;
              node_t* left;
              node_t* right;
              node_t* parent;
};

struct variable_t{char* var_name;
                  double var_value;
};

struct tree_t{node_t* root;
              variable_t* variable_list;
              int number_of_variables;
};


enum tree_errors{
    NO_ERROR = 0,
    ALLOCATION_ERROR = 1,
    FILE_OPENING_ERROR = 2,
    FILE_CLOSING_ERROR = 3,
    GRAPH_MAKING_ERROR = 4,
    FILES_NOT_FOUNDED_ERROR = 5,
    READING_ERROR = 6,
    FILE_STATISTICS_ERROR = 7,
    WAY_READING_ERROR = 8,
    TYPE_ERROR = 9,
    UNKNOWN_OPERATOR_ERROR = 10,
    DUMP_ERROR = 11,
};

tree_errors tree_init(tree_t** tree);
void tree_destroy(tree_t* tree);

node_t* node_init(tree_elem_t value, type_t type, node_t* left, node_t* right);
void node_destroy(node_t* node);
void make_parents(node_t* node, node_t* parent);

tree_errors tree_dump(node_t* root, tree_t* tree);
tree_errors node_latex_dump(node_t* node, const tree_t* tree, FILE* const dump_address);

double node_calculate(node_t* node, tree_t* tree);

bool check_file_founded(int argc, int number_of_files);

tree_errors derivator(tree_t* tree);
tree_errors Teilor(tree_t* tree);

node_t* infix_read(char* file_name, tree_t* tree);

node_t* equation_simplification(node_t* node, tree_t* tree);

#endif //DERIVATIVE_H
