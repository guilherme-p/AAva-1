#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256
#define ALPHABET_SIZE 4          /* A C T G */

#define MAX(a,b) (((a)>(b))?(a):(b))

/* ---------------- */
/* -ERROR-HANDLING- */
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

char *reverse_str(char *str) {
    size_t N, i;
    char *new;

    N = strlen(str);
    new = safe_malloc(sizeof(char) * (N + 1));
    strcpy(new, str);
  
    for (i = 0; i < N / 2; i++)
        swap(&new[i], &new[N - i - 1]);
    
    return new;
}

void compute_Z(char *str, size_t Z[]) {
    size_t N, L, R, k, i;

    N = strlen(str);
    
    L = R = 0;
    for (i = 1; i < N; i++) {
        if (i > R) {
            L = R = i;

            while (R < N && str[R-L] == str[R])
                R++;

            Z[i] = R-L;
            R--;
        }

        else {
            k = i - L;

            if (Z[k] < R - i + 1)
                Z[i] = Z[k];
  
            else {
                L = i;

                while (R < N && str[R-L] == str[R])
                    R++;

                Z[i] = R - L;
                R--;
            }
        }
    }
}

void compute_N(size_t Z[], size_t N, size_t N_table[]) {
    size_t j;
    for (j = 0; j < N; j++) {
        N_table[j] = Z[N - j - 1];
    }
}

void compute_L(size_t N_table[], size_t N, size_t L_table[]) {
    size_t i, j;
    for (j = 0; j < N; j++) {
        L_table[j] = 0;
    }

    for (j = 0; j < N - 1; j++) {
        i = N - N_table[j] + 1;
        /* printf("i: %ld j: %ld N_table[j]: %ld \n", i, j, N_table[j]); */

        if (1 <= i && i <= N) {
            L_table[i - 1] = j + 1;
        }
    }
}

void compute_l(size_t N_table[], size_t N, size_t l_table[]) {
    size_t i, j;
    size_t *largest_j = safe_malloc(sizeof(size_t) * (N + 1));

    for (j = 0; j < N; j++) {
        largest_j[j] = 0;
        l_table[j] = 0;
    }

    for (j = 1; j <= N; j++) {
        /* printf("j: %ld N_table[j - 1]: %ld\n", j, N_table[j - 1]); */
        largest_j[j] = (N_table[j - 1] == j) ? j : largest_j[j - 1];
    }

    for (i = 0; i < N; i++) {
        j = N - i;
        l_table[i] = largest_j[j];
    }

    free(largest_j);
}

void compute_bad_match(char *pattern, size_t N, size_t *bad_match) {
    size_t idx, i;

    for (i = 0; i < ALPHABET_SIZE + 1; i++) {
        bad_match[i] = 0;
    }

    for (i = 0; i < N; i++) {
        idx = get_alphabet_index(pattern[i]);
        bad_match[idx] = i;
    }

    bad_match[ALPHABET_SIZE] = -1;
}

void boyer_moore(char *query_str, char *pattern) {
    size_t M, N, i, t, k, h, s1, s2, comps, flag;
    size_t *bad_match, *Z, *N_table, *L_table, *l_table;

    long int sub;

    char *reverse_pattern;

    M = strlen(query_str);
    N = strlen(pattern);
    
    bad_match = safe_malloc(sizeof(size_t) * (ALPHABET_SIZE + 1));
    compute_bad_match(pattern, N, bad_match);

    reverse_pattern = reverse_str(pattern);
    Z = safe_malloc(sizeof(size_t) * N);
    Z[0] = N;
    
    compute_Z(reverse_pattern, Z);
    free(reverse_pattern);

    N_table = safe_malloc(sizeof(size_t) * N);
    compute_N(Z, N, N_table);

    L_table = safe_malloc(sizeof(size_t) * N);
    compute_L(N_table, N, L_table);

    l_table = safe_malloc(sizeof(size_t) * (N + 1));
    compute_l(N_table, N, l_table);

    print_table(Z, N);
    print_table(N_table, N);
    print_table(L_table, N);
    print_table(l_table, N);
    printf("bad match:\n");
    print_table(bad_match, ALPHABET_SIZE + 1);

    comps = 0;
    k = N;

    while (k <= M) {
        i = N;
        h = k;

        flag = 0;

        while (i > 0 && pattern[i - 1] == query_str[h - 1]) {
            i--;
            h--;
            comps++;
            printf("\ncomps1: %ld \n", comps);
            flag = 1;
        }

        if (!flag) {
            k += 1;
            continue;
        }

        if (i == 0) {
            printf("%ld ", k - N);
            k = k + N - l_table[1];
            /* printf("k1: %ld \n", k); */
        } else {
            t = bad_match[get_alphabet_index(query_str[h - 1])];
            sub = i - t;

            s1 = MAX(1, sub);
            s2 = (L_table[i] > 0) ? N - L_table[i] : N - l_table[i];

            k = k + MAX(s1, s2);
            printf("\ns1: %ld s2: %ld L_table[i]: %ld\n", s1, s2, L_table[i]);
            comps++;
            printf("\ncomps3: %ld \n", comps);
        }
    }

    printf("\ncomps: %ld \n", comps);

    free(bad_match);
    free(Z);
    free(N_table);
    free(L_table);
    free(l_table);
    /* free(shift); */
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