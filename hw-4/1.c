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

#define COINS_COUNT 20

typedef struct {
    uint64_t flips;
    int thread_count;
} context;

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


context ctx = {
    .flips = 10000,
    .thread_count = 100
};


void run_threads(int n, void* (*strategy)(void *)) {
    pthread_t threads[n];

    for (int i = 0; i < n; ++i) {
        if (pthread_create(&threads[i], NULL, strategy, NULL) < 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }
}

static double timeit(int n, void* (*proc)(void *)) {
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc);
    t2 = clock();
    return ((double) t2 - (double) t1) / CLOCKS_PER_SEC * 1000;
}


static char coins[COINS_COUNT + 1] = {0};
static pthread_mutex_t single_mutex;
static pthread_mutex_t *multiple_mutexes;

void flip_coin(int i) {
    if (i < 0 || i >= COINS_COUNT) {
        fprintf(stderr, "flip_coin: %d is out of coins bounds: [%d, %d]", i, 0, COINS_COUNT);
        exit(EXIT_FAILURE);
    }

    // random coin toss
    if (rand() & 1) {
        coins[i] = 'O';
    }
    else {
        coins[i] = 'X';
    }
}

void reset_coins() {
    for (int i = 0; i < COINS_COUNT; ++i) {
        coins[i] = 'O';
    }
}


void *global_lock_strategy(void *) {
    pthread_mutex_lock(&single_mutex);
    
    for (size_t i = 0; i < ctx.flips; ++i) {
        for (int j = 0; j < COINS_COUNT; ++j) {
            flip_coin(j);
        }
    }

    pthread_mutex_unlock(&single_mutex);
    pthread_exit(NULL);
}

void *iteration_lock_strategy(void*) {
    for (size_t i = 0; i < ctx.flips; ++i) {
        pthread_mutex_lock(&single_mutex);

        for (int j = 0; j < COINS_COUNT; ++j) {
            flip_coin(j);
        }

        pthread_mutex_unlock(&single_mutex);
    }

    pthread_exit(NULL);
}

void *coin_lock_strategy(void*) {
    for (size_t i = 0; i < ctx.flips; ++i) {
        for (int j = 0; j < COINS_COUNT; ++j) {
            pthread_mutex_lock(&multiple_mutexes[j]);
            
            flip_coin(j);
            
            pthread_mutex_unlock(&multiple_mutexes[j]);
        }
    }

    pthread_exit(NULL);
}


void get_coins_state(char* state) {
    for (int i = 0; i < COINS_COUNT; ++i) {
        state[i] = coins[i];
    }
}

void print_results(double elapsed_time, char* initial_state, char* label) {
    printf("coins: %s (state - %s)\n", initial_state, label);
    printf("coins: %s (end - %s)\n", coins, label);
    printf("%d threads x %ld flips: %0.3lfms\n", ctx.thread_count, ctx.flips, elapsed_time);
}


int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
            case 'n':
                read_uint64(&ctx.flips, optarg);
                break;
            case 'p':
                read_int(&ctx.thread_count, optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-n flips] [-p people]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("threads: %d, flips: %ld\n", ctx.thread_count, ctx.flips);

    // init mutexes
    pthread_mutex_init(&single_mutex, NULL); // single

    pthread_mutex_t mutexes[ctx.thread_count]; // multiple
    for (int i = 0; i < ctx.thread_count; ++i) {
        pthread_mutex_init(&mutexes[i], NULL);
    }
    multiple_mutexes = mutexes;


    char initial_state[COINS_COUNT + 1] = {0};


    // global lock strategy
    reset_coins();
    get_coins_state(initial_state);
    double measure1 = timeit(ctx.thread_count, global_lock_strategy);
    print_results(measure1, initial_state, "global lock");
    
    // scope lock strategy
    reset_coins();
    get_coins_state(initial_state);
    double measure2 = timeit(ctx.thread_count, iteration_lock_strategy);
    print_results(measure2, initial_state, "iteration lock");

    
    // multiple locks strategy
    reset_coins();
    get_coins_state(initial_state);
    double measure3 = timeit(ctx.thread_count, coin_lock_strategy);
    print_results(measure3, initial_state, "coin lock");


    // destroy mutexes
    pthread_mutex_destroy(&single_mutex); //  single

    for (int i = 0; i < ctx.thread_count; ++i) { // multiple
        pthread_mutex_destroy(&mutexes[i]);
    }

    return 0;
}