#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *trim(const char *str) {
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
