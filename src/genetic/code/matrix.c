#include "src/matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Matrix *create_matrix(int size)
{
    Matrix *matrix = malloc(sizeof(Matrix));
    matrix->size = size;
    matrix->matrix = (int **)malloc(size * sizeof(int *));
    int i, j;
    for (i = 0; i < size; i++)
    {
        matrix->matrix[i] = (int *)malloc(size * sizeof(int));
        for (j = 0; j < size; j++)
        {
            matrix->matrix[i][j] = 0;
        }
    }
    return matrix;
}

void free_matrix(Matrix *matrix)
{
    int i;
    for (i = 0; i < matrix->size; i++)
    {
        free(matrix->matrix[i]);
    }
    free(matrix->matrix);
    free(matrix);
}

Matrix *open_csv_matrix(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", file_name);
        return NULL;
    }

    char line[1000];
    int size = 0;

    if (fgets(line, sizeof(line), file) != NULL)
    {
        char *token = strtok(line, ",");
        while (token != NULL)
        {
            size++;
            token = strtok(NULL, ",");
        }
        
    }

    Matrix *matrix = create_matrix(size);

    rewind(file);
    

    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL && i < size)
    {
        char *token = strtok(line, ",");
        int j = 0;
        while (token != NULL)
        {
            
            matrix->matrix[i][j] = atoi(token);
            
            token = strtok(NULL, ",");
            j++;
        }
        i++;
    }
    fclose(file);
    return matrix;
}

void print_matrix(Matrix *matrix)
{
    int i, j;
    for (i = 0; i < matrix->size; i++)
    {
        for (j = 0; j < matrix->size; j++)
        {
            printf("%d ", matrix->matrix[i][j]);
        }
        putchar('\n');
    }
}

int dist_from_to(int from, int to, Matrix *matrix)
{
    return matrix->matrix[from][to];
}