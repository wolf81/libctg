#include <stdbool.h>

#ifndef LIBCTG_H
#define LIBCTG_H

#ifdef _WIN32
  #define LIBCTG_API __declspec(dllexport)
#else
  #define LIBCTG_API __attribute__((visibility("default")))
#endif

typedef enum {
    SUCCESS = 0,
    ERR_MEMORY_ALLOCATION = 1,
    ERR_INVALID_INPUT = 2,
} ErrorCode;

LIBCTG_API ErrorCode last_error;

typedef struct {
    int width;
    int height;
    int length;
    int* values;
} Grid;

typedef struct {
    int dx;
    int dy;
} Direction;

typedef struct {
    int x;
    int y;
    Direction dir;
    bool add;
} Move;

LIBCTG_API Grid* parseGrid(const char* input);

LIBCTG_API void printGrid(Grid* grid);

LIBCTG_API void freeGrid(Grid* grid);

LIBCTG_API int getValue(Grid* grid, int x, int y);

LIBCTG_API bool isValidMove(Grid* grid, Move* move);

#endif
