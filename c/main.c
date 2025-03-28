#include <stdlib.h>

#include "src/matrice.h"
#include "src/lieu.h"

int test_lieu_module()
{
    Liste new = open_csv_lieu("mock.csv");
    print_liste(new);
    free_liste(&new);
}

int main()
{

    /*
    Matrice m = (Matrice)malloc(sizeof(Matrice));
    initMatrix(m, 10);


    open_csv_matrice("../mock.csv", m);
    printMatrix(m);

    freeMatrix(m);
    */
}
