#include "src/matrice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initMatrix(Matrice matrice, int size) {
    matrice.size = size;
    matrice.matrice = (int **)malloc(size * sizeof(int *));
    int i, j;
    for (i = 0; i < size; i++) {
        matrice.matrice[i] = (int *)malloc(size * sizeof(int));
        for (j = 0; j < size; j++) {
            matrice.matrice[i][j] = 0;
        }
    }
}


void freeMatrix(Matrice matrice) {
    for (int i = 0; i < matrice.size; i++) {
        free(matrice.matrice[i]);
    }
    free(matrice.matrice);
}

int open_csv_matrice(char* fileName, Matrice matrice) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", fileName);
        return 0;
    }
    int buffer = 1000;
    char line[buffer];
    int i;
    for (i = 0; fgets(line, buffer, file) != NULL; i++) {
        if (i==0) {
            continue;
        }
        const char *dist = strtok(line, ",");
        int j;
        for (j = 0; dist != NULL; j++) {
            if (j == 4) {
                printf("%s\n", dist);
                break;
            }
            dist = strtok(NULL, ",");
        }

    }

    fclose(file);
    return 1;
}
