#ifndef _GRAPH

#define _GRAPH

#include <MLV/MLV_color.h>

#include "place.h"
#include "queue.h"
#include "matrix.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

typedef enum {
    RUNNING,
    FINISHED,
    EXIT
} AppState;

extern AppState appState;

void graph_init();
void draw_truck_path(Queue *queue, List places, MLV_Color color);
void draw(List places, Queue **queues, int num_queues);
void draw_timer();
void draw_generation_info(int generation, double best_fitness, int num_trucks);
void draw_genetic_evolution(List places, Matrix *dist, Matrix *time);
Queue* create_random_truck_path(List places, Matrix *matrix, int start_place_idx);
void graph_free();

#endif
