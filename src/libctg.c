#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libctg.h"
#include "utils.c"

ErrorCode last_error = SUCCESS;

void initMoveStack(MoveStack* stack, int capacity) {
    stack->moves = malloc(sizeof(Move) * capacity);
    stack->size = 0;
    stack->capacity = capacity;
}

void freeMoveStack(MoveStack* stack) {
    if (stack) {
        free(stack->moves);
        stack->moves = NULL;
        stack->size = stack->capacity = 0;        
    }
}

// Resize the MoveStack when it is full
bool resizeMoveStack(MoveStack* stack) {
    // Double the capacity of the stack
    int new_capacity = stack->capacity * 2;
    
    // Reallocate memory for the stack
    Move* new_moves = realloc(stack->moves, sizeof(Move) * new_capacity);
    
    // Check if realloc failed
    if (new_moves == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;  // Set the error code
        return false;
    }
    
    // Update stack properties only if reallocation succeeded
    stack->moves = new_moves;
    stack->capacity = new_capacity;
    
    return true;
}

Grid* initGrid(int width, int height, int* values) {
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
    grid->score = 0;

    if (grid->values == NULL) {
        destroyGrid(grid);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        int value = values[i];
        grid->values[i] = value;
        grid->score += value;
    }

    // Initialize the move history stack
    initMoveStack(&grid->moveHistory, 100);

    return grid;
}

void destroyGrid(Grid* grid) {
    if (grid) {
        // Free the grid's values array
        free(grid->values);
        grid->values = NULL;

        // Free the move history stack (if present)
        freeMoveStack(&grid->moveHistory);

        // Free the grid structure itself
        free(grid);
    }
}

char* gridToString(const Grid* grid) {
    // Estimate needed buffer size:
    // max 3 characters per int, one space, and one newline per row
    int estimatedSize = grid->width * grid->height * 5 + grid->height + 1;
    char* buffer = malloc(estimatedSize);
    if (!buffer) return NULL;

    char* ptr = buffer;
    int remaining = estimatedSize;

    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            int written = snprintf(ptr, remaining, "%d ", grid->values[y * grid->width + x]);
            ptr += written;
            remaining -= written;
        }
        *ptr++ = '\n';
        remaining--;
    }
    *ptr = '\0';  // Null-terminate the string

    return buffer;
}

Grid* gridFromString(const char* input) {
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

        // Trim the line (removes leading/trailing whitespace)
        char *trimmed = trim(line);

        // Tokenize the trimmed line
        char *token = strtok(trimmed, " ");
        while (token) {
            values[i++] = atoi(token);
            token = strtok(NULL, " ");
        }

        // Move the pointer to the next line
        ptr = line_end ? (line_end + 1) : NULL;
    }

    if (i != length) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    Grid *grid = initGrid(width, height, values);

    return grid;
}

static bool inBounds(const Grid* grid, int x, int y) {
    return x >= 0 && x < grid->width && y >= 0 && y < grid->height;
}

// Check if a move is valid
bool validateGridMove(const Grid* grid, Move* move) {
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

    // Check if the target position is within bounds
    if (!inBounds(grid, tx, ty)) {
        return false;
    }

    int tindex = ty * grid->width + tx;

    if (grid->values[tindex] == 0) {
        return false;
    }

    return true;
}

MoveResult executeGridMove(Grid* grid, Move* move) {
    if (!validateGridMove(grid, move)) {
        return (MoveResult){ -1, -1, 0, 0 };
    }
    
    // TODO: simplify peek/execute, maybe by adding a dryrun flag
    // TODO: maybe it's simpler if MoveResult tracks indices instead of x/y coord?

    // Check if the stack is full, and resize if necessary
    if (grid->moveHistory.size == grid->moveHistory.capacity) {
        resizeMoveStack(&grid->moveHistory);
    }

    int index = move->y * grid->width + move->x;
    int value = grid->values[index];

    // Calculate the target position based on the move
    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;
    int tindex = ty * grid->width + tx;
    int tvalue = grid->values[tindex];

    // Save move in the stack
    grid->moveHistory.moves[grid->moveHistory.size++] = *move;

    // determine addition or subtraction
    int change = move->add ? value : -value;

    // Apply the move
    grid->values[index] = 0;
    grid->values[tindex] = abs(tvalue + change);

    // Update score by subtracting original values and adding new value in target cell
    grid->score -= (value + tvalue);
    grid->score += grid->values[tindex];

    return (MoveResult){ tx, ty, grid->values[tindex], change };
}

MoveResult peekGridMove(const Grid* grid, Move* move) {
    if (!validateGridMove(grid, move)) {
        return (MoveResult){ -1, -1, 0, 0 };
    }

    int index = move->y * grid->width + move->x;
    int value = grid->values[index];

    // Calculate the target position based on the move
    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;
    int tindex = ty * grid->width + tx;
    int tvalue = grid->values[tindex];

    // determine addition or subtraction
    int change = move->add ? value : -value;

    return (MoveResult){ tx, ty, abs(tvalue + change), change };
}

bool isGridSolved(const Grid* grid) {
    return grid->score == 0;
}

int getGridValue(const Grid* grid, int x, int y) {
    if (!inBounds(grid, x, y)) {
        return -1;
    }

    int index = y * grid->width + x;
    return grid->values[index];    
}
