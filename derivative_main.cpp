#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "file_using.h"
#include "derivative.h"

const unsigned int NUMBER_OF_FILES = 1;

int main(int argc, char* argv[])
{
    tree_t* tree = NULL;
    node_t* node_cpy = NULL;

    if(check_file_founded(argc, NUMBER_OF_FILES))
        return FILES_NOT_FOUNDED_ERROR;
    tree_init(&tree);

    read_tree(argv[1], tree);

    node_cpy = node_copy(tree->root, NULL);
    tree_dump(node_cpy, tree);

    printf("%lf\n", node_calculate(tree->root, tree));

    node_destroy(node_cpy);
    tree_destroy(tree);
}

bool check_file_founded(int argc, unsigned int number_of_files)
{
    if (argc < number_of_files)
    {
        fprintf(stderr, "Файлы не найдены. Пожалуйста, передайте программе файл с выражением.\n");
        return 1;
    }

    return 0;
}
