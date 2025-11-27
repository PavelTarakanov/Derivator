#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

const int NUMBER_OF_FILES = 1;

int main(int argc, char* argv[])
{
    tree_t* tree = NULL;
    node_t* node_cpy = NULL;
    node_t* node_d = NULL;
    char derivating_var[] = "x";

    if(check_file_founded(argc, NUMBER_OF_FILES))
        return FILES_NOT_FOUNDED_ERROR;
    tree_init(&tree);

    read_tree(argv[1], tree);

    printf("%d\n", infix_read("infix_input.txt"));

    node_cpy = node_copy(tree->root, NULL);
    node_d = node_derivate(tree->root, tree, NULL, derivating_var);
    node_d = equation_simplification(node_d, tree);
    tree_dump(node_d, tree);

    printf("%lf\n", node_calculate(tree->root, tree));

    node_destroy(node_cpy);
    node_destroy(node_d);
    tree_destroy(tree);
}

bool check_file_founded(int argc, int number_of_files)
{
    if (argc < number_of_files)
    {
        fprintf(stderr, "Файлы не найдены. Пожалуйста, передайте программе файл с выражением в инфикскной форме.\n");
        return 1;
    }

    return 0;
}
