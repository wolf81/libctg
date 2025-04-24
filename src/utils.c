#include <string.h>
#include <ctype.h>

void trim(char *str) {
    // trim leading whitespace
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }

    // trim trailing whitespace
    if (*str) {
        char *end = str + strlen(str) - 1;
        while (end > str && isspace((unsigned char)*end)) {
            end--;
        }
        *(end + 1) = '\0';
    }
}
