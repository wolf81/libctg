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
    Move move;
    int svalue; // value at source before move
    int tvalue; // value at target before move
    int scoreChange;
} MoveRecord;

typedef struct {
    MoveRecord* moves;
    int size;
    int capacity;
} MoveStack;

typedef struct {
    int width;
    int height;
    int length;
    int** values;
    int score;

    MoveStack moveHistory;
} Grid;

typedef struct {
    int x;
    int y;
    int value;
    int change;
} MoveResult;

typedef struct {
    const Grid* grid;
    int index;
} GridIterator;

LIBCTG_API Grid* gridFromString(const char* input);

LIBCTG_API char* gridToString(const Grid* grid);

LIBCTG_API void destroyGrid(Grid* grid);

LIBCTG_API int getGridValue(const Grid* grid, int x, int y);

LIBCTG_API bool validateGridMove(const Grid* grid, Move* move);

LIBCTG_API MoveResult executeGridMove(Grid* grid, Move* move);

LIBCTG_API MoveResult peekGridMove(const Grid* grid, Move* move);

LIBCTG_API bool isGridSolved(const Grid* grid);

LIBCTG_API bool gridIteratorNext(GridIterator* iter, int* x, int* y, int* value);

LIBCTG_API void initGridIterator(GridIterator* it, const Grid* grid);

LIBCTG_API bool revertGridMove(Grid* grid);

#endif
