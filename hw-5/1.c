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
#include <assert.h>

#include "circular_buffer.h"

enum { TRAVELER, DRIVER };
#define MAX_TIME_WAIT_IN_SECOND 10

typedef struct {
    int travelers;
    int drivers;
} context;

typedef struct {
    int type;
    int id;
} person;

void read_int(int* res, char* str) {
    char* endptr; // To check for conversion errors
    *res = strtol(str, &endptr, 10);
    
    if (*endptr != '\0') {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
}

void fill_array(int* array, int size, int val) {
    for (int i = 0; i < size; ++i) {
        array[i] = val;
    }
}

int is_empty_array(int* array, int size, int empty_val) {
    for (int i = 0; i < size; ++i) {
        if (array[i] != empty_val) return 0;
    }
    return 1;
}

context ctx = {
    .travelers = 1,
    .drivers = 1
};
pthread_mutex_t mutex;
pthread_cond_t tcv; // condition variable for travelers
pthread_cond_t dcv; // condition variable for drivers

int* travelers_present;
circular_buffer drivers_queue;
int* picked_traveler;
int* picked_driver;


void process_traveler(person* p) {
    printf("t%d entering\n", p->id);
    pthread_mutex_lock(&mutex);

    travelers_present[p->id] = 1;
    int slept = 0;

    while (circular_buffer_empty(&drivers_queue) && picked_traveler[p->id] == -1) {
        printf("t%d waiting...\n", p->id);
        slept = 1;
        pthread_cond_wait(&tcv, &mutex);
    }

    if (slept == 1) {
        printf("...t%d waking up\n", p->id);
    }

    if (picked_traveler[p->id] == -1) {
        // we pick the driver
        int driver_id = circular_buffer_pop(&drivers_queue);
        picked_driver[driver_id] = p->id;
        printf("t%d picking driver %d\n", p->id, driver_id);
        travelers_present[p->id] = 0;
        pthread_cond_broadcast(&dcv);
    }
    else {
        // we are managed by driver
        int driver_id = picked_traveler[p->id];
        picked_traveler[p->id] = -1;
        printf("t%d picked by driver %d\n", p->id, driver_id);
    }

    printf("t%d leaving\n", p->id);
    pthread_mutex_unlock(&mutex);
}

void process_driver(person* p) {
    printf("d%d entering\n", p->id);
    pthread_mutex_lock(&mutex);

    circular_buffer_push(&drivers_queue, p->id);
    int slept = 0;
    
    while (is_empty_array(travelers_present, ctx.travelers, 0) && picked_driver[p->id] == -1) {
        printf("d%d waiting...\n", p->id);
        slept = 1;
        pthread_cond_wait(&dcv, &mutex);
    }

    if (slept) {
        printf("...d%d waking up\n", p->id);
    }

    if (picked_driver[p->id] == -1) {
        // we pick all available travelers
        int driver_id_from_queue = circular_buffer_pop(&drivers_queue);
        assert(p->id == driver_id_from_queue);

        for (int traveler_id = 0; traveler_id < ctx.travelers; ++traveler_id) {
            if (!travelers_present[traveler_id]) continue;

            travelers_present[traveler_id] = 0;
            picked_traveler[traveler_id] = p->id;
            printf("d%d picking t%d\n", p->id, traveler_id);
        }

        pthread_cond_broadcast(&tcv);
    }
    else {
        // we are managed by traveler
        int traveler_id = picked_driver[p->id];
        picked_driver[p->id] = -1;
        printf("d%d picked by t%d\n", p->id, traveler_id);
    }

    printf("d%d leaving\n", p->id);
    pthread_mutex_unlock(&mutex);
}

void *threadfun(void *data) {
    person* p = (person*)(data);
    void (*func) (person*);

    switch (p->type) {
        case TRAVELER:
            func = process_traveler;
            break;
        case DRIVER:
            func = process_driver;
            break;
    }

    while (1) {
        func(p);
        long int time = random() % MAX_TIME_WAIT_IN_SECOND;
        if (p->type == TRAVELER) printf("t%d waits for %lds\n", p->id, time);
        else printf("d%d waits for %lds\n", p->id, time);
        sleep(time);
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "t:d:")) != -1) {
        switch (opt) {
            case 't':
                read_int(&ctx.travelers, optarg);
                break;
            case 'd':
                read_int(&ctx.drivers, optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-t travelers] [-d drivers]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("travelers: %d, drivers: %d\n", ctx.travelers, ctx.drivers);

    
    // init 
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&tcv, NULL);
    pthread_cond_init(&dcv, NULL);

    pthread_t travelers_threads[ctx.travelers];
    person travelers[ctx.travelers];
    travelers_present = calloc(ctx.travelers, sizeof(int));
    picked_traveler = calloc(ctx.travelers, sizeof(int));
    fill_array(picked_traveler, ctx.travelers, -1);
    
    pthread_t drivers_threads[ctx.drivers];
    person drivers[ctx.drivers];
    circular_buffer_init(&drivers_queue, ctx.drivers);
    picked_driver = calloc(ctx.drivers, sizeof(int));
    fill_array(picked_driver, ctx.drivers, -1);


    for (int i = 0; i < ctx.travelers; ++i) {
        travelers[i].type = TRAVELER;
        travelers[i].id = i;

        if (pthread_create(&travelers_threads[i], NULL, threadfun, &travelers[i]) < 0) {
            perror("traveler pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < ctx.drivers; ++i) {
        drivers[i].type = DRIVER;
        drivers[i].id = i;

        if (pthread_create(&drivers_threads[i], NULL, threadfun, &drivers[i]) < 0) {
            perror("driver pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < ctx.travelers; ++i) {
        pthread_join(travelers_threads[i], NULL);
    }

    for (int i = 0; i < ctx.drivers; ++i) {
        pthread_join(drivers_threads[i], NULL);
    }


    // destroy
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&tcv);
    pthread_cond_destroy(&dcv);

    free(travelers_present);
    free(picked_traveler);

    circular_buffer_destroy(&drivers_queue);
    free(picked_driver);

    return 0;
}