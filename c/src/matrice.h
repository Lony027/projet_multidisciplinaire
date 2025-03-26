#ifndef _MATRICE

#define _MATRICE

typedef struct matrice
{
    int **matrice;
    int size;
} Matrice;

void initMatrix(Matrice matrice, int size);
void freeMatrix(Matrice matrice);
void printMatrix(Matrice matrice);
int open_csv_matrice(char* fileName, Matrice matrice);


#endif
