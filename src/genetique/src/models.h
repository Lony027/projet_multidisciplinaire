#ifndef _MODELS

#define _MODELS

#include "src/queue.h"
#include "src/place.h"
#include "src/matrix.h"
typedef struct models
{
    int dist_tot;
    int size; // number of truck
    int time;
    Queue **list_truck;
} Models;

Models *init_models(int num_max_truck);

void free_models(Models *models);

void print_models(Models *models);

int add_truck(Queue *queue, Models *models);

int delete_truck(Models *models, int num_of_truck);
Models *list_to_models(Matrix *time, List place, int *parkour, Matrix *dist);
#endif