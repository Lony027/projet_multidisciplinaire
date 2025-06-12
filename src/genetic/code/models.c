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
        new->time = 0;
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
    models->time += queue->time;
    return 1;
}
void total_fit(Models *models, Matrix *dista, Matrix *time)
{
    models->dist_tot = 0;
    models->time = 0;
    for (int i = 0; i < models->size; i++)
    {
        recalculate_fitness(models->list_truck[i], dista, time);
        models->dist_tot += models->list_truck[i]->dist;
        models->time += models->list_truck[i]->time;
    }
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
    models->time -= delete->time;
    free_queue(delete);
    return 1;
}

Models *list_to_models(Matrix *time, List place, int *parkour, Matrix *dist)
{
    Models *model = init_models(place.size);
    int new_truck = 1;
    for (int i = 0; i < place.size - 1; i++)
    {
        int indice = parkour[i];
        if (model->size == 0 || new_truck)
        {
            Queue *new = create_queue();
            Appointment *base = create_appointment(place.lst[0]);
            Appointment *step = create_appointment(place.lst[indice]);
            Appointment *end = create_appointment(place.lst[0]);
            enqueue(base, new, dist, time);
            new->time += 3 * 60;
            enqueue(step, new, dist, time);
            enqueue(end, new, dist, time);
            add_truck(new, model);
            new_truck = 0;
        }
        else
        {
            int current_truck = model->size - 1;
            Appointment *end = dequeue(model->list_truck[current_truck], dist, time);
            Appointment *step = create_appointment(place.lst[indice]);
            enqueue(step, model->list_truck[current_truck], dist, time);
            enqueue(end, model->list_truck[current_truck], dist, time);
            model->list_truck[current_truck]->time += 3 * 60;
            if (model->list_truck[current_truck]->time > 3600 * 3)
            {
                end = dequeue(model->list_truck[current_truck], dist, time);

                step = dequeue(model->list_truck[current_truck], dist, time);
                free_appointment(step);
                enqueue(end, model->list_truck[current_truck], dist, time);
                i -= 1;
                model->list_truck[current_truck]->time -= 3 * 60;
                new_truck = 1;
            }
        }
    }
    total_fit(model, dist, time);
    return model;
}