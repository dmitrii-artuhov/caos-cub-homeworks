#include "quiz.h"
#include "codes.h"

#include <string.h>
#include <jansson.h>
#include <time.h>


// Function to swap two strings
void swap(char **a, char **b) {
    char *temp = *a;
    *a = *b;
    *b = temp;
}

// Function to shuffle an array of strings
void shuffle(char *strs[], int n) {
    if (n > 1) {
        srand(time(NULL)); // Initialize random seed
        
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            if (i != j) {
                swap(&strs[i], &strs[j]);
            }
        }
    }
}


int parse(quiz_t* quiz, char *msg) {
    json_error_t error;
    json_t* json_data = json_loads(msg, 0, &error);

    if (!json_data) {
        fprintf(stderr, "Parsing error: invalid JSON at line %d: %s\n", error.line, error.text);
        return ERROR_CODE;
    }

    int response_code;
    json_t *results;

    if (json_unpack(json_data, "{s:i, s:o}", "response_code", &response_code, "results", &results) == -1) {
        fprintf(stderr, "Parsing error: unable to unpack JSON data\n");
        json_decref(json_data);
        return ERROR_CODE;
    }

    // non-zero response code
    if (response_code != 0) {
        fprintf(stderr, "Network error: non-zero response code %d\n", response_code);
        json_decref(json_data);
        return ERROR_CODE;
    }

    // checking whether results is an array
    if (!json_is_array(results)) {
        fprintf(stderr, "Parsing error: 'results' is not an array\n");
        json_decref(json_data);
        return ERROR_CODE;
    }

    // retrieve question and correct answer
    json_t *quiz_data = json_array_get(results, 0);

    char *category;
    char *question;
    char *correct_answer;

    if (json_unpack(quiz_data, "{s:s, s:s, s:s}",
                    "category", &category,
                    "question", &question,
                    "correct_answer", &correct_answer) == -1) {
        fprintf(stderr, "Parsing error: unable to unpack values from the first result\n");
        json_decref(json_data);
        return ERROR_CODE;
    }

    // retrieve incorrect_answers array
    json_t *incorrect_answers;
    if (json_unpack(quiz_data, "{s:o}", "incorrect_answers", &incorrect_answers) == -1) {
        fprintf(stderr, "Parsing error: unable to unpack 'incorrect_answers' array\n");
        json_decref(json_data);
        return ERROR_CODE;
    }

    // check whether incorrect_answers is an array
    if (!json_is_array(incorrect_answers)) {
        fprintf(stderr, "Parsing error: 'incorrect_answers' is not an array\n");
        json_decref(json_data);
        return ERROR_CODE;
    }

    // updating quiz state
    quiz->question = calloc(strlen(question) + 1, sizeof(char));
    strcpy(quiz->question, question);

    quiz->answer = calloc(strlen(correct_answer) + 1, sizeof(char));
    strcpy(quiz->answer, correct_answer);

    size_t choices_size = sizeof(quiz->choices) / sizeof(quiz->choices[0]);

    for (size_t i = 0; i < choices_size && i < json_array_size(incorrect_answers); ++i) {
        const char *incorrect_answer = json_string_value(json_array_get(incorrect_answers, i));
        quiz->choices[i] = calloc(strlen(incorrect_answer) + 1, sizeof(char));
        strcpy(quiz->choices[i], incorrect_answer);
    }

    quiz->choices[choices_size - 1] = calloc(strlen(correct_answer) + 1, sizeof(char));
    strcpy(quiz->choices[choices_size - 1], correct_answer);


    shuffle(quiz->choices, choices_size);

    json_decref(json_data);

    return 0;
}