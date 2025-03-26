#ifndef _LIEU

#define _LIEU
#include "c/src/coord.h"
typedef struct lieu
{
    char *postal;
    char *adresse;
    char *ville;
    Coord coord;
    int num;
    char *name;
} Lieu;

typedef struct liste
{
    Lieu *lst;
    int size;
} Liste;
Lieu create_lieu(char *buffer, int num);

Liste *open_csv_lieu(char *file_name);

void free_lieu(Lieu *l);

void free_liste(Liste *l);
#endif