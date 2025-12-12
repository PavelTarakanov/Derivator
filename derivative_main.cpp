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

    if(check_file_founded(argc, NUMBER_OF_FILES))
        return FILES_NOT_FOUNDED_ERROR;

    tree_init(&tree);

    tree->root =  infix_read(argv[1], tree);

    derivator(tree);
    //Teilor(tree);

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
