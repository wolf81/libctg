#ifndef CTG_IO_H
#define CTG_IO_H

#include "ctg_types.h"

LIBCTG_API Grid* ctg_io_grid_from_string(const char* input);

LIBCTG_API char* ctg_io_grid_to_string(const Grid* grid);

#endif