#include "ctg_io.h"
#include "ctg_grid.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static char *trim(const char *str) {
    if (str == NULL) return NULL;

    // Skip leading whitespace
    while (isspace((unsigned char)*str)) str++;

    // Find the end of the string
    const char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Length of the trimmed string
    size_t len = end - str + 1;

    // Allocate new string
    char *trimmed = malloc(len + 1);
    if (!trimmed) return NULL;

    strncpy(trimmed, str, len);
    trimmed[len] = '\0';

    return trimmed;
}


char* ctg_io_grid_to_string(const Grid* grid) {
    int estimatedSize = grid->length * 4 + grid->height + 1;
    char* buffer = malloc(estimatedSize);
    if (!buffer) return NULL;

    char* ptr = buffer;
    int remaining = estimatedSize;

    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            int written = snprintf(ptr, remaining, "%d ", grid->values[y][x]);
            ptr += written;
            remaining -= written;
        }
        *ptr++ = '\n';
        remaining--;
    }

    *ptr = '\0';
    return buffer;
}

Grid* ctg_io_grid_from_string(const char* input) {
    char* input_copy = strdup(input);
    if (!input_copy) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    char* trimmed = trim(input_copy);
    size_t len = strlen(trimmed);

    char* adjusted_input = malloc(len + 2); // +1 for '\n', +1 for '\0'
    if (!adjusted_input) {
        free(input_copy);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    strcpy(adjusted_input, trimmed);
    adjusted_input[len] = '\n';
    adjusted_input[len + 1] = '\0';

    int width = 0, height = 0;
    if (sscanf(adjusted_input, "%d %d\n", &width, &height) != 2) {
        free(input_copy);
        free(adjusted_input);
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    int length = width * height;
    int *values = (int *)malloc(sizeof(int) * length);
    if (values == NULL) {
        free(input_copy);
        free(adjusted_input);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    const char *ptr = strchr(adjusted_input, '\n') + 1;

    int i = 0;
    char line[256];

    while (*ptr != '\0') {
        char *line_end = strchr(ptr, '\n');
        size_t line_len = (line_end) ? (line_end - ptr) : strlen(ptr);
        strncpy(line, ptr, line_len);
        line[line_len] = '\0';
        char *trimmed = trim(line);
        char *token = strtok(trimmed, " ");
        while (token) {
            values[i++] = atoi(token);
            token = strtok(NULL, " ");
        }
        ptr = line_end ? (line_end + 1) : NULL;
    }

    free(input_copy);
    free(adjusted_input);

    if (i != length) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    Grid *grid = ctg_grid_create(width, height, values);
    free(values);
    return grid;
}
