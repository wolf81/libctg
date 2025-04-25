#include <stdlib.h>
#include <stdio.h>
#include "libctg.h"

int main() {
    // Input string
    const char *input = 
        "8 5\n"
        "0 0 0 0 0 0 0 0\n"
        "7 0 0 0 0 0 0 7\n"
        "3 1 2 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0\n";

    Grid *grid = parseGrid(input);
    if (grid == NULL) {
        return EXIT_FAILURE;
    }

    char* grid_string = toString(grid);
    if (grid_string) {
        printf("%s", grid_string);        
    }

    freeGrid(grid);

    return EXIT_SUCCESS;
}
