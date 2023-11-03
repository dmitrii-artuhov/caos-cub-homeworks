#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <time.h>
#include <wait.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

enum {
    CHUNK_SIZE = 4096
};

typedef struct FileStat {
    size_t lines;
    size_t words;
    size_t bytes;
} FileStat;

FileStat get_stats_from_buffer(char* buffer, size_t size) {
    FileStat stats = {
        .lines = 0,
        .words = 0,
        .bytes = size
    };
    
    size_t curr_line_len = 0;
    size_t curr_word_len = 0;

    for (size_t i = 0; i < size; ++i) {
        char c = buffer[i];

        curr_line_len++;

        if (isspace(c)) {
            if (c == '\n') {
                curr_line_len = 0;
                stats.lines++;
            }

            if (curr_word_len) {
                curr_word_len = 0;
                stats.words++;
            }
        }
        else {
            curr_word_len++;
        }
    }

    stats.lines += (curr_line_len > 0);
    stats.words += (curr_word_len > 0);

    return stats;
}

FileStat get_stats_from_file_descriptor(int fd) {
    char buffer[CHUNK_SIZE];
    FileStat stats = {0};

    ssize_t read_bytes = 0;

    while ((read_bytes = read(fd, buffer, CHUNK_SIZE)) > 0) {
        FileStat additional_stats = get_stats_from_buffer(buffer, read_bytes);
        stats.lines += additional_stats.lines;
        stats.words += additional_stats.words;
        stats.bytes += additional_stats.bytes;
    }

    if (read_bytes < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    return stats;
}

void print_stats(FileStat stats) {
    printf("%ld\t%ld\t%ld\n", stats.lines, stats.words, stats.bytes);
}

void print_stats_with_path(FileStat stats, const char* path) {
    printf("%ld\t%ld\t%ld\t%s\n", stats.lines, stats.words, stats.bytes, path);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        int fd = dup(STDIN_FILENO);
        if (fd < 0) {
            perror("dup");
            exit(EXIT_FAILURE);
        }

        print_stats(get_stats_from_file_descriptor(fd));
        close(fd);
    }
    else {
        for (int i = 1; i < argc; ++i) {
            const char* path = argv[i];

            int fd = open(path, O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            struct stat file_stat;
            if (lstat(path, &file_stat) == -1) {
                perror("stat");
                exit(EXIT_FAILURE);
            }

            if (S_ISREG(file_stat.st_mode)) {
                // mmap
                void* addr = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (addr == MAP_FAILED) {
                    perror("mmap");
                    exit(EXIT_FAILURE);
                }

                print_stats_with_path(get_stats_from_buffer(addr, file_stat.st_size), path);
                munmap(addr, file_stat.st_size);
            }
            else {
                print_stats_with_path(get_stats_from_file_descriptor(fd), path);
            }

            close(fd);
        }
    }

    return 0;
}
