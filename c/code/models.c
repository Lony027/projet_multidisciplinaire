#include "src/models.h"
#include <stdio.h>
#include <stdlib.h>

Models *init_models(int num_max_truck)
{
    Models *new = malloc(sizeof(Models));
    if (new)
    {
        new->dist_tot = 0;
        new->list_truck = malloc(sizeof(Queue *) * num_max_truck);
        new->size = 0;
    }
    return new;
}

void free_models(Models *models)
{
    if (models)
    {
        for (int i = 0; i < models->size; i++)
        {
            free_queue(models->list_truck[i]);
        }
        free(models->list_truck);
        models->list_truck = NULL;
        free(models);
        models = NULL;
    }
}

void print_models(Models *models)
{
    if (models)
    {
        printf("number_of_truck %d total distance %d\n", models->size, models->dist_tot);

        for (int i = 0; i < models->size; i++)
        {
            printf("truck %d\n", i);
            print_queue(models->list_truck[i]);
        }
        return;
    }
    printf("the queue is empty");
}

int add_truck(Queue *queue, Models *models)
{
    if (!queue || !models)
    {
        return 0;
    }
    models->list_truck[models->size] = queue;
    models->size += 1;
    models->dist_tot += queue->dist;
    return 1;
}

int delete_truck(Models *models, int num_of_truck)
{
    if (num_of_truck > models->size)
    {
        return 0;
    }
    Queue *delete = models->list_truck[num_of_truck];
    models->list_truck[num_of_truck] = NULL;
    for (int i = num_of_truck + 1; i < models->size; i++)
    {
        models->list_truck[i - 1] = models->list_truck[i];
    }
    models->list_truck[models->size - 1] = NULL;
    models->size -= 1;
    models->dist_tot -= delete->dist;
    free_queue(delete);
    return 1;
}