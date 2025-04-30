#include <stdlib.h>
#include <stdio.h>
#include "ctg.h"

int main() {
    // Input string
    const char *input = 
        "8 5\n"
        "0 0 0 0 0 0 0 0\n"
        "7 0 0 0 0 0 0 7\n"
        "3 1 2 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0\n"
        "0 0 0 0 0 0 0 0\n";

    Grid *grid = ctg_io_grid_from_string(input);
    if (grid == NULL) {
        return EXIT_FAILURE;
    }

    char* grid_string = ctg_io_grid_to_string(grid);
    if (grid_string) {
        printf("%s", grid_string);        
    }

    GridIterator iter;
    ctg_iterator_init(&iter, grid);
    int x, y, value;
    while (ctg_iterator_next(&iter, &x, &y, &value)) {
        printf("Cell (%d %d): %d\n", x, y, value);
    }

    ctg_move_revert(grid);
    grid_string = ctg_io_grid_to_string(grid);
    if (grid_string) {
        printf("%s", grid_string);        
    }    

    ctg_grid_destroy(grid);

    return EXIT_SUCCESS;
}
