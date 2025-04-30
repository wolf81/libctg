#ifndef CTG_TYPES_H
#define CTG_TYPES_H

#include <stdbool.h>

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

#endif
