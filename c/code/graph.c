#include "src/graph.h"

#include <math.h>

#include "MLV/MLV_all.h"
#include "src/queue.h"
#include "src/genetique.h"
#include "src/models.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void graph_init() {
    MLV_create_window("Simulation camions", "Simulation Camions", WINDOW_WIDTH, WINDOW_HEIGHT);
    MLV_clear_window(MLV_COLOR_BLACK);
}

/*
 *
 * Permet d'harmoniser le plan géographique en fonction des villes
 * pour éviter d'avoir des emplacements tous collé les uns aux autres
*/
void get_coord_limits(List places, double *min_lon, double *max_lon,
                      double *min_lat, double *max_lat) {
    int i;

    *min_lon = 180.0;
    *max_lon = -180.0;
    *min_lat = 90.0;
    *max_lat = -90.0;

    if (places.size <= 0) return;

    for (i = 0; i < places.size; i++) {
        if (places.lst[i].coord.longitude < *min_lon) *min_lon = places.lst[i].coord.longitude;
        if (places.lst[i].coord.longitude > *max_lon) *max_lon = places.lst[i].coord.longitude;
        if (places.lst[i].coord.latitude < *min_lat) *min_lat = places.lst[i].coord.latitude;
        if (places.lst[i].coord.latitude > *max_lat) *max_lat = places.lst[i].coord.latitude;
    }

    // marge de 10% pour éviter d'avoir des points aux bords de la zone de simulation
    // ainsi la simulation est plus centrée
    double lon_margin = (*max_lon - *min_lon) * 0.1;
    double lat_margin = (*max_lat - *min_lat) * 0.1;
    *min_lon -= lon_margin;
    *max_lon += lon_margin;
    *min_lat -= lat_margin;
    *max_lat += lat_margin;
}

void geo_to_screen(double lon, double lat, double min_lon, double max_lon,
                  double min_lat, double max_lat, int *x, int *y) {

    // marge 50px de chaque côté (pour timer, info)
    int margin = 50;
    int usable_width = WINDOW_WIDTH - 2 * margin;
    int usable_height = WINDOW_HEIGHT - 2 * margin;

    *x = margin + (int)((lon - min_lon) / (max_lon - min_lon) * usable_width);
    *y = margin + (int)((max_lat - lat) / (max_lat - min_lat) * usable_height);
}

void draw_places(List places) {
    int i;
    double min_lon, max_lon, min_lat, max_lat;
    int x, y;

    if (places.size <= 0) return;

    get_coord_limits(places, &min_lon, &max_lon, &min_lat, &max_lat);

    for (i = 0; i < places.size; i++) {
        geo_to_screen(places.lst[i].coord.longitude, places.lst[i].coord.latitude,
                     min_lon, max_lon, min_lat, max_lat, &x, &y);

        MLV_draw_filled_circle(x, y, 5, MLV_COLOR_RED);
        MLV_draw_text(x + 10, y - 5, places.lst[i].name, MLV_COLOR_WHITE);
    }
}

void draw_truck_path(Queue *queue, List places, MLV_Color color) {
    if (queue == NULL || is_empty(queue)) {
        return;
    }

    double min_lon, max_lon, min_lat, max_lat;
    int x1, y1, x2, y2;

    get_coord_limits(places, &min_lon, &max_lon, &min_lat, &max_lat);

    Appointment *current = queue->first;
    while (current && current->next) {
        geo_to_screen(current->place.coord.longitude, current->place.coord.latitude,
                      min_lon, max_lon, min_lat, max_lat, &x1, &y1);

        geo_to_screen(current->next->place.coord.longitude, current->next->place.coord.latitude,
                     min_lon, max_lon, min_lat, max_lat, &x2, &y2);

        MLV_draw_line(x1, y1, x2, y2, color);

        current = current->next;
    }
}

void draw(List places, Queue **queues, int num_queues) {
    MLV_clear_window(MLV_COLOR_BLACK);

    draw_places(places);

    MLV_Color truck_colors[] = {MLV_COLOR_BLUE, MLV_COLOR_GREEN, MLV_COLOR_YELLOW, MLV_COLOR_CYAN};

    for (int i = 0; i < num_queues; i++) {
        MLV_Color color = truck_colors[i % 4];
        draw_truck_path(queues[i], places, color);
    }

    MLV_actualise_window();
}

void draw_timer() {
    int current_time = MLV_get_time();
    int elapsed_time = (current_time - 0) / 1000;

    int minutes = elapsed_time / 60;
    int seconds = elapsed_time % 60;

    char timer_text[50];
    sprintf(timer_text, "Time: %02d:%02d", minutes, seconds);

    MLV_draw_text(
        20, WINDOW_HEIGHT - 50,
        timer_text,
        MLV_COLOR_WHITE
    );
}

void draw_generation_info(int generation, double best_fitness, int num_trucks) {
    char info_text[200];
    sprintf(info_text, "Generation: %d | Best Distance: %.2f km | Trucks: %d",
            generation, best_fitness / 1000.0, num_trucks);

    MLV_draw_filled_rectangle(10, 10, 400, 30, MLV_COLOR_BLACK);
    MLV_draw_text(10, 15, info_text, MLV_COLOR_WHITE);
}

Queue* create_random_truck_path(List places, Matrix *matrix, int start_place_idx) {
    Queue *queue = create_queue();
    int visited[places.size];

    for (int i = 0; i < places.size; i++) {
        visited[i] = 0;
    }

    Appointment *first_app = create_appointment(places.lst[start_place_idx]);
    enqueue(first_app, queue, matrix, matrix);
    visited[start_place_idx] = 1;

    int places_added = 1;
    int max_places = places.size;

    while (places_added < max_places) {
        int next_place = rand() % places.size;

        if (!visited[next_place]) {
            Appointment *app = create_appointment(places.lst[next_place]);
            enqueue(app, queue, matrix, matrix);
            visited[next_place] = 1;
            places_added++;
        }
    }
    return queue;
}

// TODO: NEED REFACTO : utiliser genetiquer function plusieurs fois pour éviter la ducplication
void draw_genetic_evolution(List places, Matrix *dist, Matrix *time) {
    int **population = first_models(places);
    int **new_pop = malloc(sizeof(int *) * POP_INIT);
    double *fitness = malloc(sizeof(double) * POP_INIT);

    for (int i = 0; i < POP_INIT; i++) {
        new_pop[i] = malloc(sizeof(int) * (places.size - 1));
    }

    int *best_solution = malloc(sizeof(int) * (places.size - 1));
    Models *best_model = NULL;

    for (int gen = 0; gen < GENERATION && appState != EXIT; gen++) {

        eval_all_fitness(population, dist, places.size - 1, fitness);

        int best_index = 0;
        double best_fitness = fitness[0];
        for (int i = 1; i < POP_INIT; i++) {
            // TODO: A voir avec paul, car on doit trouver la distance la plus courte??
            if (fitness[i] < best_fitness) {
                best_fitness = fitness[i];
                best_index = i;
            }
        }

        for (int i = 0; i < places.size - 1; i++) {
            best_solution[i] = population[best_index][i];
        }

        if (best_model) {
            free_models(best_model);
        }

        best_model = list_to_models(time, places, best_solution, dist);

        draw(places, best_model->list_truck, best_model->size);
        draw_generation_info(gen, best_model->dist_tot, best_model->size);
        draw_timer();
        MLV_actualise_window();

        printf("Generation %d: Best fitness = %.2f km, Trucks = %d\n",
               gen, best_model->dist_tot / 1000.0, best_model->size);

        for (int i = 0; i < POP_INIT; i += 2) {
            int *parent1 = selection_tournoi(population, fitness, SIZE_TOURNAMENT);
            int *parent2 = selection_tournoi(population, fitness, SIZE_TOURNAMENT);

            crossover_pmx(parent1, parent2, new_pop[i], new_pop[i + 1], places.size - 1);

            if ((double)rand() / RAND_MAX < MUTATION_PROBA) {
                mutation_swap(new_pop[i], places.size - 1);
            }
            if ((double)rand() / RAND_MAX < MUTATION_PROBA) {
                mutation_inversion(new_pop[i + 1], places.size - 1);
            }
        }

        for (int i = 0; i < POP_INIT; i++) {
            for (int j = 0; j < places.size - 1; j++) {
                population[i][j] = new_pop[i][j];
            }
        }

        MLV_Event event;
        int mouse_x, mouse_y;
        event = MLV_get_event(NULL, NULL, NULL, NULL, NULL,
                             &mouse_x, &mouse_y, NULL, NULL);

        if (event == MLV_KEY) {
            MLV_Keyboard_button key;
            MLV_get_event(&key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            if (key == MLV_KEYBOARD_ESCAPE) {
                appState = EXIT;
            }
        }
    }

    if (best_model) {
        printf("\n=== SOLUTION FINALE ===\n");
        print_models(best_model);

        appState = FINISHED;
        while (appState != EXIT) {
            MLV_Event event;
            event = MLV_get_event(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            if (event == MLV_KEY) {
                MLV_Keyboard_button key;
                MLV_get_event(&key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                if (key == MLV_KEYBOARD_ESCAPE) {
                    appState = EXIT;
                }
            }
        }
    }

    for (int i = 0; i < POP_INIT; i++) {
        free(population[i]);
        free(new_pop[i]);
    }
    free(population);
    free(new_pop);
    free(fitness);
    free(best_solution);
    if (best_model) {
        free_models(best_model);
    }
}

void graph_free() {
    MLV_free_window();
}
