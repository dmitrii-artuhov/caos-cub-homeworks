#define _DEFAULT_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <getopt.h>
#include <float.h>


double execute_command(const char *command, char* args[], int *status) {
    struct timespec start, end;
    double elapsed_time;

    clock_gettime(CLOCK_MONOTONIC, &start);

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(command, args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // Parent process
        waitpid(pid, status, 0);

        if (WIFEXITED(*status) && WEXITSTATUS(*status) == EXIT_SUCCESS) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        }
        else {
            // Indicates a failed execution
            elapsed_time = -1.0; 
        }
    }
    else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return elapsed_time;
}


int main(int argc, char *argv[]) {
    int opt;
    double duration = 5.0; // Default duration in seconds
    int warmup_runs = 0; // Default number of warmup runs

    while ((opt = getopt(argc, argv, "+d:w:")) != -1) {
        switch (opt) {
            case 'd':
                duration = (double)atoi(optarg);
                break;
            case 'w':
                warmup_runs = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-d duration] [-w warmup_runs] command\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Command is missing.\n");
        exit(EXIT_FAILURE);
    }

    const char *command = argv[optind];
    int status;
    double min_time = DBL_MAX;
    double max_time = 0;
    double total_time = 0;
    int num_executions = 0;
    int num_failures = 0;

    // Warm-up runs
    for (int i = 0; i < warmup_runs; i++) {
        double elapsed_time = execute_command(command, argv + optind, &status);
        if (elapsed_time < 0) {
            num_failures++;
        }
    }

    // Measurement runs
    while (total_time < duration) {
        double elapsed_time = execute_command(command, argv + optind, &status);
        if (elapsed_time < 0) {
            num_failures++;
        } else {
            total_time += elapsed_time;
            num_executions++;

            if (elapsed_time < min_time) {
                min_time = elapsed_time;
            }

            if (elapsed_time > max_time) {
                max_time = elapsed_time;
            }
        }
    }

    printf("Min:   %.6f seconds \tWarmups: %d\n", min_time, warmup_runs);
    printf("Avg:   %.6f seconds \tRuns: %d\n", num_executions > 0 ? total_time / num_executions : 0, num_executions);
    printf("Max:   %.6f seconds \tFails: %d\n", max_time, num_failures);
    printf("Total: %.6f seconds\n", total_time);

    return 0;
}