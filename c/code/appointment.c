#include "src/appointment.h"
#include <stdlib.h>
#include <stdio.h>

Appointment *create_appointment(Place place)
{
    Appointment *new = malloc(sizeof(Appointment));
    new->next = NULL;
    new->place = place;
    return new;
}

void print_appointment(Appointment *appointment)
{
    print_place(appointment->place);
}

void free_appointment(Appointment *appointment)
{
    free(appointment);
    appointment = NULL;
}