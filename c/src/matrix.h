#ifndef _MATRIX

#define _MATRIX

typedef struct matrix
{
    int **matrix;
    int size;
} Matrix;

Matrix *create_matrix(int size);
void free_matrix(Matrix *matrix);
void print_matrix(Matrix *matrix);
Matrix *open_csv_matrix(char *file_name);
int dist_from_to(int from, int to, Matrix *matrix);

#endif
