#ifndef _QUEUE

#define _QUEUE
#include "src/appointment.h"
#include "src/matrix.h"

typedef struct queue
{
    int dist;
    Appointment *first;
    Appointment *last;
    float time;
} Queue;

Queue *create_queue();
int is_empty(Queue *queue);
void free_queue(Queue *queue);
Appointment *dequeue(Queue *queue, Matrix *matrix);
void enqueue(Appointment *new, Queue *queue, Matrix *matrix);
void print_queue(Queue *queue);

#endif
