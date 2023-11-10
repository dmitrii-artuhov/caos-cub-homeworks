#define _POSIX_C_SOURCE 200809L

#include "quiz.h"
#include "codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int is_playing = 1;

// Signal handler for SIGINT
void sigint_handler(int) {
    is_playing = 0;
}

int main(void) {
    quiz_t quiz = {
        .n = 0,
        .score = 0,
        .max = 0,
        .question = NULL,
        .answer = NULL,
        .choices = { NULL, NULL, NULL, NULL }
    };

    printf(
        "Answer multiple choice questions about computer science.\n"
        "You score points for each correctly answered question.\n"
        "If you need multiple attempts to answer a question, the\n"
        "points you score for a correct answer go down.\n"
        "\n"
    );

    // setup signal handlers
    // Set up the handler
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // Register the signal handler
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (is_playing) {
        int status = play(&quiz);

        if (status == ERROR_CODE || status == EOF_CODE) {
            break;
        }
    }

    printf("Thanks for playing today.\n");
    printf("Your final score is %d/%d points.\n", quiz.score, quiz.max);

    return 0;
}