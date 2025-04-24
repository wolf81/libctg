#include <string.h>
#include <ctype.h>

char* trim(char *str) {
    // trim leading whitespace
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }

    // trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    // null terminate
    *(end + 1) = '\0';

    return str;
}
