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
    int dx;
    int dy;
} Direction;

typedef struct {
    int x;
    int y;
    Direction dir;
    bool add;
} Move;

typedef struct {
    Move* moves;
    int size;
    int capacity;
} MoveStack;

typedef struct {
    int width;
    int height;
    int length;
    int* values;
    int score;

    MoveStack moveHistory;
} Grid;

typedef struct {
    int x;
    int y;
    int value;
    int change;
} MoveResult;

LIBCTG_API Grid* gridFromString(const char* input);

LIBCTG_API char* gridToString(const Grid* grid);

LIBCTG_API void destroyGrid(Grid* grid);

LIBCTG_API int getGridValue(const Grid* grid, int x, int y);

LIBCTG_API bool validateGridMove(const Grid* grid, Move* move);

LIBCTG_API MoveResult executeGridMove(Grid* grid, Move* move);

LIBCTG_API MoveResult peekGridMove(const Grid* grid, Move* move);

LIBCTG_API bool isGridSolved(const Grid* grid);

#endif
