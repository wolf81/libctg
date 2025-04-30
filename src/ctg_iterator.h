#ifndef CTG_ITERATOR_H
#define CTG_ITERATOR_H

#include "ctg_types.h"

typedef struct {
    const Grid* grid;
    int index;
} GridIterator;

LIBCTG_API void ctg_iterator_init(GridIterator* it, const Grid* grid);

LIBCTG_API bool ctg_iterator_next(GridIterator* iter, int* x, int* y, int* value);

#endif
