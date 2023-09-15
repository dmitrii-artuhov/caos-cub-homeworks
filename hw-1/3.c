#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

typedef struct {
    int verbose;
    char* excluded_env_name;
} env_context;

extern char** environ;

void print_environment(const env_context* context) {
    if (context->verbose) {
        printf("Printing current environment\n");
    }

    for (char** env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}

void add_key_value_pair_to_environment(const env_context* context, char* key_value_pair) {
    if (context->verbose) {
        printf("Setting environment key-value pair: %s\n", key_value_pair);
    }

    // Split each entry into key and value
    char* key = key_value_pair;
    char* value = NULL;

    // Find the equals sign to split key and value
    while (*key_value_pair != '\0') {
        if (*key_value_pair == '=') {
            *key_value_pair = '\0';  // Null-terminate the key
            value = key_value_pair + 1;  // Set the value
            break;
        }
        key_value_pair++;
    }

    if (setenv(key, value, 1) != 0) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }
}

void update_environment(const env_context* context) {
    char* key_name = context->excluded_env_name;
    
    if (key_name == NULL) {
        return;
    }
    
    if (context->verbose) {
        printf("Removing environment variable with key: %s\n", key_name);
    }

    if (unsetenv(key_name) != 0) {
        perror("unsetenv");
        exit(EXIT_FAILURE);
    }
}

int is_key_value_pair(const char* string) {
    int before_equals = 0;
    int after_equals = 0;
    int had_equals = 0;

    while (*string != '\0') {
        if (*string == '=') {
            had_equals = 1;
            string++;
            continue;
        }

        if (!had_equals) {
            before_equals++;
        }
        else {
            after_equals++;
        }

        string++;
    }

    return (before_equals && after_equals && had_equals);
}


int main(int argc, char *argv[]) {
    int opt;
    env_context context = { 0, NULL };

    while ((opt = getopt(argc, argv, "+vu:")) != -1) {
        switch (opt) {
            case 'v':
                context.verbose = 1;
                break;
            case 'u':
                context.excluded_env_name = optarg;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-v]? [-u excluded env param name]? [name=value]... [command [arg]...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Process key value pairs (if any)
    int command_index = -1;

    for (int i = optind; i < argc; i++) {
        if (!is_key_value_pair(argv[i])) {
            command_index = i;
            break;
        }

        add_key_value_pair_to_environment(&context, argv[i]);
    }

    update_environment(&context);
    
    if (command_index != -1) {
        // run the command with modified environment command
        if (context.verbose) {
            printf("Running '%s' command with arguments: ", argv[command_index]);

            for (int i = command_index + 1; i < argc; i++) {
                if (i == argc - 1) {
                    printf("'%s'", argv[i]);
                }
                else {
                    printf("'%s', ", argv[i]);
                }
            }

            printf("\n");
        }

        execvp(argv[command_index], argv + command_index);
 
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else {
        print_environment(&context);
    }

    exit(EXIT_SUCCESS);
}