#ifndef CTG_ITERATOR_H
#define CTG_ITERATOR_H

#include "ctg_types.h"

typedef struct {
    const Grid* grid;
    int index;
} GridIterator;

LIBCTG_API bool gridIteratorNext(GridIterator* iter, int* x, int* y, int* value);

LIBCTG_API void initGridIterator(GridIterator* it, const Grid* grid);

#endif
