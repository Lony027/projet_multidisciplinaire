#ifndef _GEN
#define _GEN
#include "src/place.h"
#include "src/matrix.h"
#define POP_INIT 200
#define GENERATION 150
#define SIZE_TOURNAMENT 4
#define MUTATION_PROBA 0.1
int *genetique(Matrix *distance, List place);
#endif