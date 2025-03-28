#include "src/matrix.h"

int test_matrice_module() {
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    print_matrix(m);

    free_matrix(m);

    return 1;
}

