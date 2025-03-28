#ifndef _Place

#define _Place
#include "src/coord.h"
typedef struct place
{
    char *postal;
    char *adress;
    char *city;
    Coord coord;
    int num;
    char *name;
} Place;

typedef struct list
{
    Place *lst;
    int size;
} List;
Place create_place(char *buffer, int num);

List open_csv_place(char *file_name);

void free_place(Place *l);

void free_list(List *l);

void print_list(List l);

void print_place(Place l);
#endif