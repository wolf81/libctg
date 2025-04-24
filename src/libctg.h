typedef enum {
    SUCCESS = 0,
    ERR_MEMORY_ALLOCATION = 1,
    ERR_INVALID_INPUT = 2,
} ErrorCode;

ErrorCode last_error;

typedef struct
{
    int width;
    int height;
    int* values;
} Grid;

Grid* parseGrid(const char* input);

void printGrid(Grid* grid);
