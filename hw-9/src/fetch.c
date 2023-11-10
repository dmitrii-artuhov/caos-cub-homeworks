#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define JSON_BUFFER_SIZE 1024

char* fetch(char *url) {
    
    int fd[2];

    if (pipe(fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int pid = fork();

    if (pid < 0) {
        // parent
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // child

        // close read end
        if (close(fd[0]) < 0) {
            perror("child close fd[0]");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // close write end (copy)
        if (close(fd[1]) < 0) {
            perror("child close fd[1]");
            exit(EXIT_FAILURE);
        }

        execlp("curl", "curl", "-s", url, NULL);

        perror("execlp");
        exit(EXIT_FAILURE);
    }

    int status;
    
    // printf("parent waiting for json from pid %d...\n", pid);
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    // printf("pid %d finished with status code %d\n", pid, status);

    // close write end
    close(fd[1]);

    if (status != 0) {
        // close read
        close(fd[0]);
        return NULL;
    }

    char json_data[JSON_BUFFER_SIZE] = {0};

    if (read(fd[0], json_data, JSON_BUFFER_SIZE) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // close read
    close(fd[0]);

    int data_length = strlen(json_data);
    char* result = calloc(data_length + 1, sizeof(char));

    for (int i = 0; i < data_length; ++i) {
        result[i] = json_data[i];
    }
    
    return result;
}