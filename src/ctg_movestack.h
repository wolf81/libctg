#ifndef CTG_MOVESTACK_H
#define CTG_MOVESTACK_H

#include "ctg_types.h"

void ctg_movestack_init(MoveStack* stack, int capacity);

void ctg_movestack_free(MoveStack* stack);

bool ctg_movestack_resize(MoveStack* stack);

#endif