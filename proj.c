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

void knuth_morris_pratt(char *query_str, char *pattern) {
    NULL;
}

void boyer_moore(char *query_str, char *pattern) {
    NULL;
}

int main() {
    char *input = read_line();
    char *query_str = NULL;

    while (input[0] != '\0') {                     // input cant be null, check for empty input (\n or EOF in case tests dont always end on X)
        char command = input[0];
        char *arg = NULL;

        switch (command) {
            case 'T':
                arg = input + 2;

                query_str = read_query_str(arg); // Skip input ptr over space
                break;

            case 'N':
                arg = input + 2;

                naive_search(query_str, arg);
                break;

            case 'K':
                arg = input + 2;

                knuth_morris_pratt(query_str, arg);
                break;

            case 'B':
                arg = input + 2;

                boyer_moore(query_str, arg);
                break;

            case 'X':
                goto End; // Disgusting goto to reuse free-ing logic and return statement.

            default:
                fprintf(stderr, "Unknown/invalid command.\n");
        }

        free(input);
        input = read_line();
    }
    

End:
    free(input);
    
    if (query_str)
        free(query_str);

    return 0;
}