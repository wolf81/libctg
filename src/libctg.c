#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libctg.h"
#include "utils.c"

ErrorCode last_error = SUCCESS;

Grid* newGrid(int width, int height, int* values) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (grid == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    int length = width * height;

    grid->width = width;
    grid->height = height;
    grid->length = length;
    grid->values = (int*)malloc(sizeof(int) * length);
    if (grid->values == NULL) {
        freeGrid(grid);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        grid->values[i] = values[i];
    }

    return grid;
}

void freeGrid(Grid* grid) {
    if (grid) {
        free(grid->values);
        free(grid);
    }
}

void printGrid(const Grid* grid) {
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            printf("%d ", grid->values[y * grid->width + x]);
        }
        printf("\n");
    }
}

Grid* parseGrid(const char* input) {
    // trim leading & trailing whitespace and add newline at end for parsing
    input = strcat(trim((char*)input), "\n");

    // Parse width & height from first line.
    int width = 0, height = 0;
    if (sscanf(input, "%d %d\n", &width, &height) != 2) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    int length = width * height;
    int *values = (int *)malloc(sizeof(int) * length);
    if (values == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    // Parse each subsequent line
    const char *ptr = strchr(input, '\n') + 1; // Move past the first line

    int i = 0;

    // Temporary buffer for each line
    char line[256];

    // Loop to process each subsequent line
    while (*ptr != '\0') {
        // Find the next line's end or end of string
        char *line_end = strchr(ptr, '\n');
        size_t line_len = (line_end) ? (line_end - ptr) : strlen(ptr);
        
        // Copy the line into the buffer and null terminate it
        strncpy(line, ptr, line_len);
        line[line_len] = '\0';

        // Split the line into tokens and add to the grid
        char *token = strtok(line, " ");
        while (token) {
            values[i++] = atoi(token);  // Convert token to integer
            token = strtok(NULL, " ");
        }

        // Move the pointer to the next line
        ptr = line_end ? (line_end + 1) : NULL;
    }

    if (i != length) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    Grid *grid = newGrid(width, height, values);

    return grid;
}

static bool inBounds(const Grid* grid, int x, int y) {
    // Calculate the index for (x, y) in the 1D array
    int index = y * grid->width + x;
    return (index >= 0 && index < grid->length);
}

// Check if a move is valid
bool isValidMove(const Grid* grid, Move* move) {
    // Check if the move is within the bounds of the grid
    if (!inBounds(grid, move->x, move->y)) {
        return false;  // Out of bounds
    }

    int index = move->y * grid->width + move->x;
    int value = grid->values[index];

    // Cannot move if value is 0
    if (value == 0) {
        return false;
    }

    // Calculate the target position based on the direction
    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;

    if (tx < 0 || tx > grid->width) {
        return false;
    }

    // Check if the target position is within bounds
    if (!inBounds(grid, tx, ty)) {
        return false;  // Out of bounds after applying direction
    }

    return true;
}

int getValue(const Grid* grid, int x, int y) {
    if (!inBounds(grid, x, y)) {
        return -1;
    }

    int index = y * grid->width + x;
    return grid->values[index];    
}
