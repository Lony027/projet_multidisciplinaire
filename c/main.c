#include "src/matrix.h"
#include "src/place.h"

void test_matrice_module()
{  
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    print_matrix(m);

    free_matrix(m);
}

void test_lieu_module()
{
    List new = open_place_csv("mock.csv");
    print_list(new);
    free_list(&new);
}

int main()
{
    test_lieu_module();
    test_matrice_module();
    return 0;
}

