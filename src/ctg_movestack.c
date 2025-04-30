#include <stdlib.h>
#include "ctg_movestack.h"
#include "ctg_grid.h"

void ctg_movestack_init(MoveStack* stack, int capacity) {
    stack->moves = malloc(sizeof(MoveRecord) * capacity);
    stack->size = 0;
    stack->capacity = capacity;
}

void ctg_movestack_free(MoveStack* stack) {
    if (stack) {
        free(stack->moves);
        stack->moves = NULL;
        stack->size = stack->capacity = 0;
    }
}

bool ctg_movestack_resize(MoveStack* stack) {
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
