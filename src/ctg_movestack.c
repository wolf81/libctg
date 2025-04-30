#include <stdlib.h>
#include "ctg_movestack.h"

void initMoveStack(MoveStack* stack, int capacity) {
    stack->moves = malloc(sizeof(MoveRecord) * capacity);
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

bool resizeMoveStack(MoveStack* stack) {
    int new_capacity = stack->capacity * 2;
    MoveRecord* new_moves = realloc(stack->moves, sizeof(MoveRecord) * new_capacity);
    if (new_moves == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return false;
    }
    stack->moves = new_moves;
    stack->capacity = new_capacity;
    return true;
}
