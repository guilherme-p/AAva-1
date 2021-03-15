#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling.h"

#define BUFSIZE 256

char *read_line() {
    size_t i = 0;
    size_t buffer_size = BUFSIZE;
    char *buffer = (char *) safe_malloc(BUFSIZE * sizeof(char));

    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        char c = (char) ch;
        buffer[i++] = c;

        if (i == buffer_size) {
            buffer_size *= 2;
            buffer = (char *) safe_realloc(buffer, buffer_size);
        }
    }

    if (ch == EOF) {
        fprintf(stderr, "EOF while reading. Exiting...\n");
        exit(1);
    }

    buffer[i] = '\0';
    return buffer;
}

char *read_query_str(char *input) {
    char *query_str = (char *) safe_malloc(strlen(input) + 1 * sizeof(char)); // Allocate 1 more byte for terminating string char
    strcpy(query_str, input);

    return query_str;
}

void naive_search(char *query_str, char *pattern) {
    size_t M = strlen(query_str);
    size_t N = strlen(pattern);

    for (size_t i = 0; i < M - N + 1; i++) {
        size_t j = 0;

        while (j < N && query_str[i + j] == pattern[j]) {
            j++;
        }

        if (j == N) {
            printf("%ld ", i);
        }
    }

    printf("\n");
}

void compute_lps(char *pattern, int N, size_t *lps) {
    NULL;
}

void knuth_morris_pratt(char *query_str, char *pattern) {
    size_t M = strlen(query_str);
    size_t N = strlen(pattern);

    size_t lps[N];
    compute_lps(pattern, N, lps);

    size_t i = 0, j = 0;
    size_t comps = 0;

    while (i < M) {
        if (query_str[i] == pattern[j]) {
            i++;
            j++;
        } else {
            if (j == 0)
                i++;
            else
                j = lps[j - 1];
        }

        comps++;

        if (j == N) {                           // Match
            printf("%ld ", i - j);
            j = lps[j - 1];
        }
    }

    printf("\n%ld \n", comps);
}

void boyer_moore(char *query_str, char *pattern) {
    NULL;
}

int main() {
    char *input = read_line();
    char *query_str = NULL;

    while (input && input[0] != '\0') {                    
        char command = input[0];
        char *arg = (strlen(input) > 1) ? input + 2 : // Skip input ptr over space
                                          NULL;

        switch (command) {
            case 'T':
                query_str = read_query_str(arg); 
                break;

            case 'N':
                naive_search(query_str, arg);
                break;

            case 'K':
                knuth_morris_pratt(query_str, arg);
                break;

            case 'B':
                boyer_moore(query_str, arg);
                break;

            case 'X':
                free(input);
    
                if (query_str)
                    free(query_str);
                
                return 0;

            default:
                fprintf(stderr, "Unknown/invalid command.\n");
        }

        free(input);
        input = read_line();
    }

    return 0;
}