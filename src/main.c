#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 100
#define ASCII_LOWER 97 // a
#define ASCII_UPPER 122 // z
#define NAME_START_NUM 26 // '$'
#define NAME_END_NUM 27 // '.'
#define ROWS 28
#define COLS 28

struct Bigram {
    char a;
    char b;
};

// Function to read names from a file and store them in an array
void readNamesFromFile(const char *filename, char ***names, int *numNames) {
    FILE *file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0) {
        perror("Error opening file");
        exit(1);
    }

    *numNames = 0;
    *names = NULL;
    char line[MAX_NAME_LENGTH];

    // Read names from the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove the newline character at the end of the line
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Allocate memory for the new name
        char *newName = _strdup(line);
        if (newName == NULL) {
            perror("Memory allocation error");
            exit(1);
        }

        // Resize the array to store the new name
        *names = realloc(*names, ((*numNames) + 1) * sizeof(char *));
        if (*names == NULL) {
            perror("Memory allocation error");
            exit(1);
        }

        // Store the new name in the array
        (*names)[*numNames] = newName;
        (*numNames)++;
    }

    fclose(file);
}

void printNames(char **names, int numNames){
    printf("Names read from the file:\n");
    for (int i = 0; i < numNames; i++) {
        printf("%d: %s\n", i + 1, names[i]);
    }
}

void printBigrams(char **names, int numNames, int numPrints){
    struct Bigram bigram;
    for (int i = 0; i < numNames; i++) {
        char* name = names[i];
        printf("%s: \n", name);
        size_t len = strlen(name);
        size_t pos = 0;
        while (pos < len) {
            if (pos == 0){
                bigram.a = '$';
                bigram.b = name[pos];
            }
            else if (pos == len - 1){
                bigram.b = '.';
                bigram.a = name[pos];
            }
            else {
                bigram.a = name[pos];
                bigram.b = name[pos + 1];
            }
            pos++;
            printf("%c %c\n", bigram.a, bigram.b);
        }
        if (i == numPrints){
            break;
        }
    }
}

void countBigrams(char **names, int numNames, int **counts){
    for (int i = 0; i < numNames; i++) {
        char* name = names[i];
        size_t len = strlen(name);
        size_t pos = 0;
        int a, b = 0;
        while (pos < len) {
            if (pos == 0){
                a = NAME_START_NUM;
                b = (int)name[pos] - ASCII_LOWER;
            }
            else if (pos == len - 1){
                b = NAME_END_NUM;
                a = (int)name[pos] - ASCII_LOWER;
            }
            else {
                a = name[pos] - ASCII_LOWER;
                b = name[pos + 1] - ASCII_LOWER;
            }
            pos++;
            counts[a][b]++;
        }
    }
}

int mostFrequentBigram(int rows, int cols, int** counts){
    int max_count = -1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int val = counts[i][j];
            if (val > max_count){
                max_count = val;
            }
        }
    }
    return max_count;
}

void norm_probs(int rows, int cols, int** counts, float** probs){
    for (int i = 0; i < rows; i++) {
        int c = 0;
        for (int j = 0; j < cols; j++) {
            c += (float)counts[i][j];
        }
        for (int j = 0; j < cols; j++) {
            probs[i][j] = (float)counts[i][j] / (float)c;
        }
    }

}

void freeNames(char **names, int numNames){
    for (int i = 0; i < numNames; i++) {
        free(names[i]); // Free memory for each name
    }
    free(names);
}

void alloc_arrayf(float*** a){
    *a = (float**)malloc( ROWS * sizeof(float*));
    for (int row = 0; row < ROWS; row++){
        (*a)[row] = (float*)malloc( COLS * sizeof(float*));
    }
    for (int row = 0; row < ROWS; row++){
        for (int col = 0; col < ROWS; col++){
            (*a)[row][col] = 0;
        }
    }

}

void alloc_array(int*** a){
    *a = (int**)malloc( ROWS * sizeof(int*));
    for (int row = 0; row < ROWS; row++){
        (*a)[row] = (int*)malloc( COLS * sizeof(int*));
    }
    for (int row = 0; row < ROWS; row++){
        for (int col = 0; col < ROWS; col++){
            (*a)[row][col] = 0;
        }
    }
}

int main() {
    char **names = NULL;
    int numNames = 0;
    int** bigram_counts = NULL;
    float** bigram_probs = NULL;
    const char *filename = "names.txt"; // Change this to your file's name

    printf("Allocating Arrays\n");

    alloc_array(&bigram_counts);
    alloc_arrayf(&bigram_probs);

    printf("Read names from file\n");
    readNamesFromFile(filename, &names, &numNames);

    printf("Counting\n");
    countBigrams(names, numNames, bigram_counts);

    printf("Normalizing \n");
    norm_probs(ROWS, COLS, bigram_counts, bigram_probs);

    const int name_len = 9;
    char name[name_len + 1];
    name[0] = 'c';
    name[name_len] = '\0';
    int prev_ord = 0;

    printf("Start eval\n");
    for (int n = 1; n < name_len; n++) {
        if (name[n-1] == '%') {
            prev_ord = NAME_START_NUM;
        }
        else if (name[n-1] == '.') {
            prev_ord = NAME_END_NUM;
        }
        else {
            prev_ord = (int)name[n - 1] - ASCII_LOWER;
        }
        printf("%d, %c\n", prev_ord, name[n-1]);

        int max_prob_i = -1;
        float max_prob = -1;
        for (int j = 0; j < COLS; j++) {
            if (bigram_probs[prev_ord][j] > max_prob){
                max_prob = bigram_probs[prev_ord][j];
                max_prob_i = j;
                printf("%f %c\n", max_prob, (char)(max_prob_i + ASCII_LOWER));
            }
        }
        if (max_prob_i == NAME_START_NUM) {
            name[n] = '$';
        }
        if (max_prob_i == NAME_END_NUM) {
            name[n] = '.';
        }
        else {
            name[n] = (char)(max_prob_i + ASCII_LOWER);
        }
    }
    // printf("Generated name: %s", name);

    freeNames(names, numNames); // Free the array of names
    free(bigram_probs);
    free(bigram_counts);
    while (1) {};
    return 0;
}

