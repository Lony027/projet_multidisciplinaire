#ifndef _APP
#define _APP

#include "src/place.h"

typedef struct appointment
{
    struct appointment *next;
    struct appointment *prev;
    Place place;
} Appointment;

Appointment *create_appointment(Place place);
void print_appointment(Appointment *appointment);
void free_appointment(Appointment *appointment);
#endif