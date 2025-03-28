#include <stdlib.h>

#include "src/matrice.h"
#include "src/place.h"

void test_lieu_module()
{
    List new = open_place_csv("mock.csv");
    print_list(new);
    free_list(&new);
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
    test_lieu_module();
    return 0;
}
