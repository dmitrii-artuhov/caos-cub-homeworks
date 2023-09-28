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
#include <stdint.h>
#include <pthread.h>

typedef struct {
    uint64_t left;
    uint64_t right;
    int verbose;
    int thread_count;
} context;

typedef struct {
    uint64_t left;
    uint64_t right;
} thread_data;

static int is_perfect(uint64_t num) {
    uint64_t i, sum;

    if (num < 2) {
        return 0;
    }

    for (i = 2, sum = 1; i * i <= num; i++) {
        if (num % i == 0) {
            sum += (i * i == num) ? i : i + num / i;
        }
    }

    return (sum == num);
}

void read_uint64(uint64_t* res, char* str) {
    char* endptr; // To check for conversion errors
    *res = strtoull(str, &endptr, 10);
    
    if (*endptr != '\0') {
        perror("strtoull");
        exit(EXIT_FAILURE);
    }
}

void read_int(int* res, char* str) {
    char* endptr; // To check for conversion errors
    *res = strtol(str, &endptr, 10);
    
    if (*endptr != '\0') {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
}

uint64_t min(uint64_t a, uint64_t b) {
    if (a < b) return a;
    return b;
}

uint64_t max(uint64_t a, uint64_t b) {
    if (a > b) return a;
    return b;
}



char* program_name;
context ctx = {
    .left = 1,
    .right = 10000,
    .verbose = 0,
    .thread_count = 1
};

void *threadfun(void *args) {
    thread_data data = *((thread_data *)args);
    
    for (uint64_t i = data.left; i < data.right; ++i) {
        if (is_perfect(i) && ctx.verbose != -1) {
            printf("%ld\n", i);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int opt;
    program_name = argv[0];

    while ((opt = getopt(argc, argv, "s:e:t:vq")) != -1) {
        switch (opt) {
            case 's':
                read_uint64(&ctx.left, optarg);
                break;
            case 'e':
                read_uint64(&ctx.right, optarg);
                break;
            case 't':
                read_int(&ctx.thread_count, optarg);
                break;
            case 'v':
                ctx.verbose = 1;
                break;
            case 'q':
                ctx.verbose = -1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-s left] [-e right] [-t threads count] [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    ctx.right++;

    // printf("Segment: [%ld, %ld)\n", ctx.left, ctx.right);
    // printf("Threads count: %d\n", ctx.thread_count);
    // printf("Verbose: %d\n", ctx.verbose);

    pthread_t threads[ctx.thread_count];
    thread_data data[ctx.thread_count];

    uint64_t chunk_size = max(1, (ctx.right - ctx.left) / ctx.thread_count);
    // printf("Chunk size: %ld\n", chunk_size);

    for (int i = 0; i < ctx.thread_count; ++i) {
        // [left, right)
        data[i].left = min(ctx.right, chunk_size * i + ctx.left);
        data[i].right = min(ctx.right, chunk_size * (i + 1) + ctx.left);

        if (i == ctx.thread_count - 1) {
            data[i].right = ctx.right;
        }

        if (ctx.verbose == 1) {
            printf("%s:  t%d searching [%ld, %ld)\n", program_name, i, data[i].left, data[i].right);
        }
    }

    for (int i = 0; i < ctx.thread_count; ++i) {
        if (pthread_create(&threads[i], NULL, threadfun, &data[i]) < 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < ctx.thread_count; ++i) {
        if (ctx.verbose == 1) {
            printf("%s: t%d finishing\n", program_name, i);
        }
        pthread_join(threads[i], NULL);
    }

    return 0;
}