#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256
#define ALPHABET_SIZE 4          /* A C T G */

/* ---------------- */
/* ------MISC------ */
/* ---------------- */

void *safe_malloc(size_t size) {
    void *ptr = (void *) malloc(size);

    if (ptr == NULL) {
        perror("malloc error");
        exit(1);
    }

    return ptr;
}

void *safe_realloc(void *ptr, size_t size) {
    void *new_ptr = (void *) realloc(ptr, size);

    if (new_ptr == NULL) {
        perror("realloc error");
        exit(1);
    }
    
    return new_ptr;
}

void print_table(size_t *table, size_t N) {
    size_t i;
    for (i = 0; i < N; i++) {
        printf("%ld ", table[i]);
    }

    printf("\n");
}

long int max(long int s1, long int s2) {
    return (s1 > s2) ? s1 : s2;
}

/* --------------- */
/* -----INPUT----- */
/* --------------- */

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
    char *query_str = (char *) safe_malloc(strlen(input) + 1 * sizeof(char));   /* Allocate 1 more byte for terminating string char */
    strcpy(query_str, input);

    return query_str;
}


/* --------------- */
/* -----NAIVE----- */
/* --------------- */

void naive_search(char *query_str, char *pattern) {
    size_t M, N, i;

    M = strlen(query_str);
    N = strlen(pattern);
    
    for (i = 0; i < M - N + 1; i++) {
        size_t j;
        j = 0;

        while (j < N && query_str[i + j] == pattern[j]) {
            j++;
        }

        if (j == N) {
            printf("%ld ", i);
        }
    }

    printf("\n");
}


/* --------------- */
/* ------KMP------ */
/* --------------- */

void compute_lps(char *pattern, size_t N, size_t *lps) {  /* https://www.youtube.com/watch?v=tWDUjkMv6Lc */
    size_t prev, i;

    prev = 0;
    lps[0] = 0;
    i = 1;

    while (i < N) {
        if (pattern[prev] == pattern[i]) {
            prev++;
            lps[i] = prev;

            i++;

        } else {
            if (prev == 0) {
                lps[i] = 0;
                i++;
            } else {
                prev = lps[prev - 1];
            }
        }
    }
}

void knuth_morris_pratt(char *query_str, char *pattern) { /* https://youtu.be/BXCEFAzhxGY?t=290 */
    size_t M, N, q, comps, i;
    size_t *lps;

    M = strlen(query_str);
    N = strlen(pattern);

    lps = safe_malloc(sizeof(size_t) * N);
    compute_lps(pattern, N, lps);

    q = comps = 0;

    for (i = 0; i < M; i++) {
        while (q > 0 && pattern[q] != query_str[i]) {
            q = lps[q - 1];
            comps++;
        }
        
        if (pattern[q] == query_str[i])
            q++;
        
        if (q == N) {
            printf("%ld ", i + 1 - N);
            q = lps[q - 1];
            
            if (i < M - 1)
                comps++;
        }

        comps++;
    }

    printf("\n%ld \n", comps);
    free(lps);
}


/* ------------------- */
/* ----BOYER-MOORE---- */
/* ------------------- */

size_t get_alphabet_index(char c) {
    switch (c) {
        case 'A':
            return 0;
        case 'C':
            return 1;
        case 'G':
            return 2;
        case 'T':
            return 3;
        default:
            return 4;
    }
}

void swap(char *s1, char *s2) {
    char temp;

    temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

void compute_bad_match(char *pattern, size_t N, size_t *bad_match) {
    size_t idx, i;

    for (i = 0; i < ALPHABET_SIZE + 1; i++) {
        bad_match[i] = -1;
    }

    for (i = 0; i < N; i++) {
        idx = get_alphabet_index(pattern[i]);
        bad_match[idx] = i;
    }
}

void compute_strong_suffix(size_t *strong_suffix, size_t *L, char *pattern, size_t N) {
    size_t i, j;

    for (i = 0; i < N + 1; i++) {
        strong_suffix[i] = 0;
    }

    i = N;
    j = N + 1;

    L[i] = j;
  
    while (i > 0) {
        while (j <= N && pattern[i - 1] != pattern[j - 1]) {
            if (strong_suffix[j] == 0) {
                strong_suffix[j] = j - i;
            }
  
            j = L[j];
        }

        i--;
        j--;
        L[i] = j; 
    }

    j = L[0];

    for (i = 0; i <= N; i++) {
        if (strong_suffix[i] == 0) {
            strong_suffix[i] = j;
        }
  
        if (i == j) {
            j = L[j];
        }
    }
}

void boyer_moore(char *query_str, char *pattern) {
    size_t M, N, i, s1, s2, comps;
    size_t *bad_match, *strong_suffix, *L;
    int j;

    M = strlen(query_str);
    N = strlen(pattern);
    
    bad_match = safe_malloc(sizeof(size_t) * (ALPHABET_SIZE + 1));
    compute_bad_match(pattern, N, bad_match);

    L = safe_malloc(sizeof(size_t) * (N + 1));
    strong_suffix = safe_malloc(sizeof(size_t) * (N + 1));
    compute_strong_suffix(strong_suffix, L, pattern, N);

    comps = 0;
    i = 0;

    while (i <= (M - N)) {
        j = N - 1;

        while (j >= 0 && pattern[j] == query_str[i + j]) {
            j--;
            comps++;
        }

        if (j < 0) {
            printf("%ld ", i);

            i += strong_suffix[0];
        } else {
            s1 = max(1, j - bad_match[get_alphabet_index(query_str[i + j])]);
            s2 = strong_suffix[j + 1];

            i += max(s1, s2);
            comps++;
        }
    }

    printf("\n%ld \n", comps);

    free(bad_match);
    free(L);
    free(strong_suffix);
}


/* -------------- */
/* -----MAIN----- */
/* -------------- */

int main() {
    char *input = read_line();
    char *query_str = NULL;

    while (input && input[0] != '\0') {                    
        char command = input[0];
        char *arg = (strlen(input) > 1) ? input + 2 : /* Skip input ptr over space */
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