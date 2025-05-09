#include "src/queue.h"
#include <stdio.h>
#include <stdlib.h>

Queue *create_queue()
{
    Queue *new = malloc(sizeof(Queue));
    new->dist = -1;
    new->last = NULL;
    new->first = NULL;
    new->time = -1;
    return new;
}

int is_empty(Queue *queue)
{
    if (!queue->first)
    {
        return 1;
    }
    return 0;
}

void print_queue(Queue *queue)
{
    if (is_empty(queue))
    {
        printf("the queue is empty\n");
        return;
    }
    Appointment *tmp = queue->first;
    for (; tmp; tmp = tmp->next)
    {
        print_appointment(tmp);
    }
}

void enqueue(Appointment *new, Queue *queue, Matrix *matrix)
{
    if (is_empty(queue))
    {

        queue->first = new;
        queue->last = new;
        queue->dist = 0;
        queue->time = 0;
        return;
    }
    queue->last->next = new;
    new->prev = queue->last;
    queue->last = new;
    queue->dist += dist_from_to(new->prev->place.num, new->place.num, matrix);
}

Appointment *dequeue(Queue *queue, Matrix *matrix)
{
    if (queue->first == queue->last)
    {
        Appointment *tmp = queue->first;
        queue->first = NULL;
        queue->last = NULL;
        queue->dist = -1;
        tmp->prev = NULL;
        return tmp;
    }
    Appointment *tmp = queue->last;
    queue->last = tmp->prev;
    queue->last->next = NULL;
    tmp->prev = NULL;
    queue->dist -= dist_from_to(queue->last->place.num, tmp->place.num, matrix);
    return tmp;
}

void free_queue(Queue *queue)
{
    for (Appointment *iterator = queue->first; iterator;)
    {
        Appointment *tmp = iterator;
        iterator = iterator->next;
        free(tmp);
    }

    free(queue);
    queue = NULL;
}