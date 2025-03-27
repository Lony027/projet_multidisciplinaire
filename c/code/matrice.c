#include "src/matrice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_matrice(Matrice* matrice, const int size) {
    matrice->size = size;
    matrice->matrice = (int **)malloc(size * sizeof(int *));
    int i, j;
    for (i = 0; i < size; i++) {
        matrice->matrice[i] = (int *)malloc(size * sizeof(int));
        for (j = 0; j < size; j++) {
            matrice->matrice[i][j] = 0;
        }
    }
}

void free_matrice(Matrice* matrice) {
    int i;
    for (i = 0; i < matrice->size; i++) {
        free(matrice->matrice[i]);
    }
    free(matrice->matrice);
}

int open_csv_matrice(char* file_name, Matrice* matrice) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", file_name);
        return 0;
    }
    char line[1000];
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, ",");
        int j = 0;
        while (token != NULL) {
            if (j == 5) { // TODO: En attente du mockup matrice distance
                const int value = atoi(token);
                matrice->matrice[i][0] = value;
                break;
            }
            token = strtok(NULL, ",");
            j++;
        }
        i++;
    }
    fclose(file);
    return 1;
}

void print_matrice(Matrice* matrice) {
    int i, j;
    for (i = 0; i < matrice->size; i++) {
        for (j = 0; j < matrice->size; j++) {
            printf("%d ", matrice->matrice[i][j]);
        }
        printf("\n");
    }
}
