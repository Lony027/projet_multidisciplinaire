#include "src/appointment.h"
#include <stdlib.h>
#include <stdio.h>

Appointment *create_appointment(Place place)
{
    Appointment *new = malloc(sizeof(Appointment));
    new->next = NULL;
    new->prev = NULL;
    new->place = place;
    return new;
}

void print_appointment(Appointment *appointment)
{
    print_place(appointment->place);
}

void free_appointment(Appointment *appointment)
{
    appointment->next = NULL;
    appointment->prev = NULL;
    free(appointment);
    appointment = NULL;
}