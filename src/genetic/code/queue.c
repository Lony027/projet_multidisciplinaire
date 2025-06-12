#include "src/queue.h"
#include <stdio.h>
#include <stdlib.h>

Queue *create_queue()
{
    Queue *new = malloc(sizeof(Queue));
    new->dist = -1;
    new->last = NULL;
    new->first = NULL;
    new->time = 0;
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
    printf("dist %d time %f size %d \n", queue->dist, queue->time / 3600, queue->size);
    Appointment *tmp = queue->first;
    for (; tmp; tmp = tmp->next)
    {
        print_appointment(tmp);
    }
}

int enqueue(Appointment *new, Queue *queue, Matrix *matrix, Matrix *time)
{
    if (!new || !queue || !matrix)
    {
        return 0;
    }

    if (is_empty(queue))
    {

        queue->first = new;
        queue->last = new;
        queue->dist = 0;
        queue->time = 0;
        queue->size = 1;
        return 1;
    }
    queue->last->next = new;
    new->prev = queue->last;
    queue->last = new;
    queue->dist += dist_from_to(new->prev->place.num, new->place.num, matrix);
    queue->time += dist_from_to(new->prev->place.num, new->place.num, time);
    queue->time += 1;
    return 1;
}

void recalculate_fitness(Queue *queue, Matrix *dista, Matrix *time)
{
    queue->dist = 0;
    queue->time = 0;
    if (queue->first == queue->last)
    {
        queue->dist = 0;
        queue->time = 0;
        return;
    }
    Appointment *tmp = queue->first;
    for (; tmp->next; tmp = tmp->next)
    {
        queue->dist += dist_from_to(tmp->place.num, tmp->next->place.num, dista);
        queue->time += dist_from_to(tmp->place.num, tmp->next->place.num, time);
    }
}

Appointment *dequeue(Queue *queue, Matrix *matrix, Matrix *time)
{
    if (is_empty(queue) || !queue)
    {
        return NULL;
    }
    if (queue->first == queue->last)
    {
        Appointment *tmp = queue->first;
        queue->first = NULL;
        queue->last = NULL;
        queue->dist = -1;
        tmp->prev = NULL;
        queue->time = -1;
        queue->size = 0;
        return tmp;
    }
    Appointment *tmp = queue->last;
    queue->last = tmp->prev;
    queue->last->next = NULL;
    tmp->prev = NULL;
    queue->dist -= dist_from_to(queue->last->place.num, tmp->place.num, matrix);
    queue->time -= dist_from_to(queue->last->place.num, tmp->place.num, time);
    queue->size -= 1;
    return tmp;
}

void free_queue(Queue *queue)
{
    for (Appointment *iterator = queue->first; iterator;)
    {
        Appointment *tmp = iterator;
        iterator = iterator->next;
        free_appointment(tmp);
    }

    free(queue);
    queue = NULL;
}