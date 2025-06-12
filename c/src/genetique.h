#ifndef _GEN
#define _GEN

#include "src/place.h"
#include "src/matrix.h"

#define POP_INIT 200
#define GENERATION 150
#define SIZE_TOURNAMENT 4
#define MUTATION_PROBA 0.1

int *genetique(Matrix *distance, List place, Matrix *time, int is_graphic);
int **first_models(List place);
double eval_fitness(int *individue, Matrix *distance, int size);
void eval_all_fitness(int **population, Matrix *distance, int size, double *fitness);

int *selection_tournoi(int **population, double *fitness, int tournament_size);
void mutation_swap(int *individu, int size);
void mutation_inversion(int *individu, int size);
void crossover_pmx(int *parent1, int *parent2, int *enfant1, int *enfant2, int size);

void shuffle(int *array, int n);

#endif