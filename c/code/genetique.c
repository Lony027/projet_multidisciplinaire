#include "src/genetique.h"
#include <stdlib.h>
#include <stdio.h>
#include <MLV/MLV_text.h>
#include <MLV/MLV_time.h>
#include <MLV/MLV_window.h>

#include "src/graph.h"
#include "src/models.h"

void shuffle(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

int **first_models(List place) {
    int **model = malloc(sizeof(int *) * POP_INIT);
    for (int i = 0; i < POP_INIT; i++) {
        model[i] = malloc(sizeof(int) * (place.size - 1));
        for (int j = 1; j < place.size; j++) {
            model[i][j - 1] = j;
        }
        shuffle(model[i], place.size - 1);
    }
    return model;
}

double eval_fitness(int *individue, Matrix *distance, int size) {
    double dist_tot = 0.0;
    for (int i = 0; i < size - 1; i++)
    {
        dist_tot += distance->matrix[individue[i]][individue[i + 1]];
    }
    return dist_tot;
}

int *selection_tournoi(int **population, double *fitness, int tournament_size) {
    int meilleur_index = rand() % POP_INIT;

    for (int i = 1; i < tournament_size; i++) {
        int challenger = rand() % POP_INIT;
        if (fitness[challenger] > fitness[meilleur_index])
        {
            meilleur_index = challenger;
        }
    }

    return population[meilleur_index];
}

void mutation_swap(int *individu, int size) {
    int i = rand() % size;
    int j = rand() % size;
    int temp = individu[i];
    individu[i] = individu[j];
    individu[j] = temp;
}

void mutation_inversion(int *individu, int size) {
    int i = rand() % size;
    int j = rand() % size;
    if (i > j) {
        int tmp = i;
        i = j;
        j = tmp;
    }
    while (i < j) {
        int temp = individu[i];
        individu[i] = individu[j];
        individu[j] = temp;
        i++;
        j--;
    }
}

void crossover_pmx(int *parent1, int *parent2, int *enfant1, int *enfant2, int size) {
    int start = rand() % size;
    int end = rand() % size;
    if (start > end) {
        int temp = start;
        start = end;
        end = temp;
    }

    for (int i = 0; i < size; i++) {
        enfant1[i] = -1;
        enfant2[i] = -1;
    }

    for (int i = start; i <= end; i++) {
        enfant1[i] = parent2[i];
        enfant2[i] = parent1[i];
    }

    for (int i = 0; i < size; i++) {
        if (enfant1[i] == -1) {
            int val = parent1[i];
            while (1) {
                int conflict = 0;
                for (int j = start; j <= end; j++) {
                    if (val == enfant1[j]) {
                        val = parent1[j];
                        conflict = 1;
                        break;
                    }
                }
                if (!conflict) break;
            }
            enfant1[i] = val;
        }

        if (enfant2[i] == -1) {
            int val = parent2[i];
            while (1) {
                int conflict = 0;
                for (int j = start; j <= end; j++) {
                    if (val == enfant2[j]) {
                        val = parent2[j];
                        conflict = 1;
                        break;
                    }
                }
                if (!conflict) break;
            }
            enfant2[i] = val;
        }
    }
}

void eval_all_fitness(int **population, Matrix *distance, int size, double *fitness) {
    for (int i = 0; i < POP_INIT; i++) {
        fitness[i] = eval_fitness(population[i], distance, size);
    }
}

Models *genetique(Matrix *distance, List place, Matrix *time, int is_graphic) {
   int **population = first_models(place);
   int **new_pop = malloc(sizeof(int *) * POP_INIT);
   double *fitness = malloc(sizeof(double) * POP_INIT);

   int *best_solution = malloc(sizeof(int) * (place.size - 1));
   Models *best_model = NULL;

   int start_time = 0;
   int best_index = 0;
   double best_fitness = 0;

   for (int i = 0; i < POP_INIT; i++) {
       new_pop[i] = malloc(sizeof(int) * (place.size - 1));
   }

   if (is_graphic) {
       graph_init();
       appState = RUNNING;
       start_time = MLV_get_time();
   }

   for (int gen = 0; gen < GENERATION && (!is_graphic || appState != EXIT); gen++) {
       if (!is_graphic) {
           printf("gen: %d\n", gen);
       }

       eval_all_fitness(population, distance, place.size - 1, fitness);

       best_index = 0;
       best_fitness = fitness[0];
       for (int i = 1; i < POP_INIT; i++) {
           if (fitness[i] < best_fitness) {
               best_fitness = fitness[i];
               best_index = i;
           }
       }

       for (int i = 0; i < place.size - 1; i++) {
           best_solution[i] = population[best_index][i];
       }

       if (is_graphic) {
           if (best_model) {
               free_models(best_model);
           }
           best_model = list_to_models(time, place, best_solution, distance);
           draw(place, best_model->list_truck, best_model->size);
           draw_generation_info(gen, best_model->dist_tot, best_model->size);
           draw_timer(start_time);
           MLV_actualise_window();
       }

       for (int i = 0; i < POP_INIT; i += 2) {
           int *parent1 = selection_tournoi(population, fitness, SIZE_TOURNAMENT);
           int *parent2 = selection_tournoi(population, fitness, SIZE_TOURNAMENT);
           crossover_pmx(parent1, parent2, new_pop[i], new_pop[i + 1], place.size - 1);

           if ((double)rand() / RAND_MAX < MUTATION_PROBA) {
               mutation_swap(new_pop[i], place.size - 1);
           }
           if ((double)rand() / RAND_MAX < MUTATION_PROBA) {
               mutation_inversion(new_pop[i + 1], place.size - 1);
           }
       }

       for (int i = 0; i < POP_INIT; i++) {
           for (int j = 0; j < place.size - 1; j++) {
               population[i][j] = new_pop[i][j];
           }
       }

       if (is_graphic && check_escape_event()) {
           appState = EXIT;
       }
   }

   if (is_graphic) {
       if (best_model) {
           printf("\n=== SOLUTION FINALE ===\n");
           print_models(best_model);

           appState = FINISHED;
           while (appState != EXIT) {
               if (check_escape_event()) {
                   appState = EXIT;
               }
           }
       }
       graph_free();
   }

   for (int i = 0; i < POP_INIT; i++) {
       free(population[i]);
       free(new_pop[i]);
   }
   free(population);
   free(new_pop);
   free(fitness);
   free(best_solution);

   return best_model;
}
