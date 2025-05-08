#include "src/matrix.h"
#include "src/place.h"
#include "src/appointment.h"
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

    List new = open_place_csv("mock.csv");
    Appointment *a = create_appointment(new.lst[0]);
    print_appointment(a);
    free_appointment(a);
    free_list(&new);

    return 0;
}
