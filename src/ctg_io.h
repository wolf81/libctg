#ifndef CTG_IO_H
#define CTG_IO_H

#include "ctg_types.h"

LIBCTG_API Grid* gridFromString(const char* input);

LIBCTG_API char* gridToString(const Grid* grid);

#endif