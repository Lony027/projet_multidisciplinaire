#ifndef _MATRICE

#define _MATRICE

typedef struct matrice
{
    int **matrice;
    int size;
} Matrice;

void create_matrice(Matrice* matrice, int size);
void free_matrice(Matrice* matrice);
void print_matrice(Matrice* matrice);
int open_csv_matrice(char* file_name, Matrice* matrice);


#endif
