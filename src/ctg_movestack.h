#ifndef CTG_MOVESTACK_H
#define CTG_MOVESTACK_H

void initMoveStack(MoveStack* stack, int capacity);

void freeMoveStack(MoveStack* stack);

bool resizeMoveStack(MoveStack* stack);

#endif