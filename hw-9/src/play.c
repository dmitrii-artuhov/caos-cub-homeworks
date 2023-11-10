#include "quiz.h"
#include "codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_quiz(quiz_t* quiz) {
    printf(
        "Quiz: \n"
        "n=%d \n"
        "score=%d \n"
        "max=%d \n"
        "question=%s \n"
        "answer=%s \n"
        "choices[0]=%s \n"
        "choices[1]=%s \n"
        "choices[2]=%s \n"
        "choices[3]=%s \n",
        quiz->n,
        quiz->score,
        quiz->max,
        quiz->question,
        quiz->answer,
        quiz->choices[0],
        quiz->choices[1],
        quiz->choices[2],
        quiz->choices[3]
    );
}

void free_quiz(quiz_t* quiz) {
    free(quiz->question);
    free(quiz->answer);
    free(quiz->choices[0]);
    free(quiz->choices[1]);
    free(quiz->choices[2]);
    free(quiz->choices[3]);
}


int get_correct_choice(char* correct, char* choices[], int n) {
    for (int i = 0; i < n; i++) {
        if (strcmp(choices[i], correct) == 0) {
            return i;
        }
    }

    return -1;
}

int play(quiz_t *quiz) {
    int question_cost = 8;

    // fetch
    char* QUIZ_URL = "https://opentdb.com/api.php?amount=1&category=18&type=multiple";

    printf("Retrieving new question...\n");
    char* json = fetch(QUIZ_URL);

    if (json == NULL) {
        fprintf(stderr, "Error while retriving JSON quiz data");
        exit(EXIT_FAILURE);
    }
    // printf("json: %s\n", json);

    // parse
    if (parse(quiz, json) < 0) {
        fprintf(stderr, "Error while parsing JSON quiz data");
        exit(EXIT_FAILURE);
    }

    // play
    quiz->n += 1;
    quiz->max += question_cost;
    int question_score = 8;
    int choices_count = sizeof(quiz->choices) / sizeof(char*);
    int correct_choice = get_correct_choice(quiz->answer, quiz->choices, choices_count);

    // print_quiz(quiz);
    printf("Answer: %s\n", quiz->answer);

    printf("%s\n\n", quiz->question);
    // printf("Choices: %d\n", choices_count);

    if (correct_choice < 0) {
        printf("Play error: invalid correct choice: %d (correct index) / %d (all choices)", correct_choice, choices_count);
        return ERROR_CODE;
    }

    for (int i = 0; i < choices_count; i++) {
        printf("[%c] %s\n", (char)('a' + i), quiz->choices[i]);
    }

    while (question_score > 0) {
        char c;
        printf("(%dpt) > ", question_score);
        
        int read_entries = scanf(" %c", &c);
        if (read_entries == EOF) {
            // free memory
            free(json);
            free_quiz(quiz);
            return EOF_CODE;
        }
        else if (read_entries != 1) {
            // free memory
            free(json);
            free_quiz(quiz);
            return ERROR_CODE;
        }

        int choice = c - 'a';
        
        if (choice < 0 || choice >= choices_count) {
            printf("Answer [%c] is not valid. Pick correct variant.\n", c);
            continue;
        }
        
        if (choice == correct_choice) {
            quiz->score += question_score;
            printf("Congratulation, answer [%c] is correct. ", c);
            break;
        }
        
        // printf("Answer [%c] is wrong, ", c);
        question_score /= 2;

        if (question_score == 0) {
            printf("try next question. ");
        }
        else {
            printf("try again.\n");
        }
    }

    printf("Your current score is %d/%d points.\n\n", quiz->score, quiz->max);
    
    // free memory
    free(json);
    free_quiz(quiz);
    
    return 0;
}