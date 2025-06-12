#include "src/matrix.h"
#include "src/place.h"
#include "src/appointment.h"
#include "src/queue.h"
#include "src/models.h"
#include "src/graph.h"
#include "src/output.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <MLV/MLV_window.h>
#include "src/genetique.h"

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
    enqueue(a, test, m, m);
    enqueue(b, test, m, m);
    enqueue(c, test2, m, m);
    enqueue(d, test2, m, m);
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
    List new = open_place_csv("../mock.csv");
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    Appointment *a = create_appointment(new.lst[0]);
    Appointment *b = create_appointment(new.lst[1]);
    Queue *test = create_queue();
    printf("first insertion\n");
    enqueue(a, test, m, m);
    print_queue(test);
    printf("second insertion\n");
    enqueue(b, test, m, m);
    print_queue(test);
    printf("first deleting\n");
    Appointment *c = dequeue(test, m, m);
    print_queue(test);
    free_appointment(c);
    free_queue(test);
    free_list(&new);
    free_matrix(m);
}


void test_output_module()
{
    List new = open_place_csv("mock.csv");
    Matrix *m = open_csv_matrix("../mock_distance.csv");
    Appointment *a = create_appointment(new.lst[0]);
    Appointment *b = create_appointment(new.lst[1]);
    Appointment *c = create_appointment(new.lst[2]);
    Appointment *d = create_appointment(new.lst[3]);
    Appointment *e = create_appointment(new.lst[4]);
    Appointment *f = create_appointment(new.lst[5]);
    Appointment *g = create_appointment(new.lst[6]);
    Queue *test = create_queue();
    Queue *test2 = create_queue();
    enqueue(a, test, m);
    enqueue(b, test, m);
    enqueue(c, test, m);
    enqueue(d, test, m);
    enqueue(e, test2, m);
    enqueue(f, test2, m);
    enqueue(g, test2, m);
    Models *tested = init_models(new.size);
    add_truck(test, tested);
    add_truck(test2, tested);
    create_output_csv(tested);
    free_models(tested);
    free_list(&new);
    free_matrix(m);
}

AppState appState;

int main()
{
    srand(time(NULL));

    appState = RUNNING;

    List places = open_place_csv("../mock.csv");
    Matrix *dist = open_csv_matrix("../distance_matrix.csv");
    Matrix *time = open_csv_matrix("../duration_matrix.csv");

    genetique(dist, places, time, 1);

    // best = genetique(dist, places, time, 1);
    // create_output(best);

    free_list(&places);
    free_matrix(dist);
    free_matrix(time);

    return EXIT_SUCCESS;
}
