#include "src/matrix.h"

int main() {
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    print_matrix(m);

    free_matrix(m);

    return 0;
}

