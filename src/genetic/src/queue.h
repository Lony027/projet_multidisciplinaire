#ifndef _QUEUE

#define _QUEUE
#include "src/appointment.h"
#include "src/matrix.h"

typedef struct queue
{
    int dist;
    Appointment *first;
    Appointment *last;
    int size;
    float time;
} Queue;

void recalculate_fitness(Queue *queue, Matrix *dista, Matrix *time);

Queue *create_queue();
int is_empty(Queue *queue);
void free_queue(Queue *queue);
Appointment *dequeue(Queue *queue, Matrix *matrix, Matrix *time);

int enqueue(Appointment *new, Queue *queue, Matrix *matrix, Matrix *time);
void print_queue(Queue *queue);

#endif
