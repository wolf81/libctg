#include "ctg_iterator.h"

void initGridIterator(GridIterator* iter, const Grid* grid) {
    iter->grid = grid;
    iter->index = 0;
}

bool gridIteratorNext(GridIterator* iter, int* out_x, int* out_y, int* out_value) {
    if (iter->index >= iter->grid->width * iter->grid->height) {
        return false;
    }
    *out_x = iter->index % iter->grid->width;
    *out_y = iter->index / iter->grid->width;
    *out_value = iter->grid->values[*out_y][*out_x];

    iter->index++;
    return true;
}
