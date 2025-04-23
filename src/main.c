#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SUCCESS = 0,
    ERR_MEMORY_ALLOCATION = 1,
    ERR_INVALID_INPUT = 2,
} ErrorCode;

typedef struct
{
    int width;
    int height;
    int* values;
} Grid;

ErrorCode last_error = SUCCESS;

Grid* newGrid(int width, int height, int* values) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (grid == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    int num_values = width * height;

    grid->width = width;
    grid->height = height;
    grid->values = (int*)malloc(sizeof(int) * num_values);
    if (grid->values == NULL) {
        free(grid);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    for (int i = 0; i < num_values; i++) {
        grid->values[i] = values[i];
    }

    return grid;
}

void printGrid(Grid* grid) {
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            printf("%d ", grid->values[y * grid->width + x]);
        }
        printf("\n");
    }
}

Grid* parseGrid(const char* input) {
    // Parse width & height from first line.
    int width = 0, height = 0;
    if (sscanf(input, "%d %d\n", &width, &height) != 2) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    int num_values = width * height;
    int *values = (int *)malloc(sizeof(int) * num_values);
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

    if (i != num_values) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    Grid *grid = newGrid(width, height, values);

    return grid;
}

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

    printGrid(grid);

    return EXIT_SUCCESS;
}
