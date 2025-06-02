#include "src/matrix.h"
#include "src/place.h"
#include "src/appointment.h"
#include "src/queue.h"
#include "src/models.h"
#include "src/genetique.h"
#include <stdio.h>
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

void test_appointment_module()
{
    List new = open_place_csv("mock.csv");
    Appointment *a = create_appointment(new.lst[0]);
    print_appointment(a);
    free_appointment(a);
    free_list(&new);
}
void test_models_module()
{
    List new = open_place_csv("mock.csv");
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    Appointment *a = create_appointment(new.lst[0]);
    Appointment *b = create_appointment(new.lst[1]);
    Appointment *c = create_appointment(new.lst[0]);
    Appointment *d = create_appointment(new.lst[1]);
    Queue *test = create_queue();
    Queue *test2 = create_queue();
    enqueue(a, test, m);
    enqueue(b, test, m);
    enqueue(c, test2, m);
    enqueue(d, test2, m);
    Models *tested = init_models(new.size);
    add_truck(test, tested);
    print_models(tested);
    add_truck(test2, tested);
    print_models(tested);
    delete_truck(tested, 0);
    print_models(tested);
    free_models(tested);
    free_list(&new);
    free_matrix(m);
}
void test_queue_module()
{
    List new = open_place_csv("mock.csv");
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    Appointment *a = create_appointment(new.lst[0]);
    Appointment *b = create_appointment(new.lst[1]);
    Queue *test = create_queue();
    printf("first insertion\n");
    enqueue(a, test, m);
    print_queue(test);
    printf("second insertion\n");
    enqueue(b, test, m);
    print_queue(test);
    printf("first deleting\n");
    Appointment *c = dequeue(test, m);
    print_queue(test);
    free_appointment(c);
    free_queue(test);
    free_list(&new);
    free_matrix(m);
}

int main()
{

    List new = open_place_csv("mock.csv");
    Matrix *m = open_csv_matrix("../distance_matrix.csv");
    int *result = genetique(m, new);
    for (int i = 0; i < new.size - 1; i++)
    {
        printf("%d", result[i]);
    }

    return 0;
}
