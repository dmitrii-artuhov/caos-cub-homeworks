#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *strndup(const char *s, size_t n) {
    char *p = NULL;

    if (s) {
        size_t len = strlen(s);
        if (n < len) {
            len = n;
        }
        p = (char *)malloc(len + 1);
        if (p) {
            strncpy(p, s, len);
            // p[len] = 0;
        }
    }
    return p;
}

int main(void) {
    static char m[] = "Hello World!";
    size_t len = strlen(m);

    for (size_t n = 1; n <= len; n++) {
        char *p = strndup(m, n);
        if (!p) {
            perror("strndup");
            return EXIT_FAILURE;
        }
        if (puts(p) == EOF) {
            perror("puts");
            return EXIT_FAILURE;
        }
        free(p);
    }

    if (fflush(stdout) == EOF) {
        perror("fflush");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}