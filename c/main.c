#include <stdlib.h>

#include "src/matrice.h"

int main() {
    Matrice *m = malloc(sizeof(Matrice));
    create_matrice(m, 10);

    open_csv_matrice("../mock.csv", m);
    print_matrice(m);

    free_matrice(m);

    return 0;
}