#include "src/matrix.h"
#include "src/place.h"
#include "src/appointment.h"
#include "src/queue.h"
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

    return 0;
}
