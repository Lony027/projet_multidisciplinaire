#include "c/src/lieu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Lieu create_lieu(char *buffer, int num)
{
    Lieu new = malloc(sizeof(Lieu));
    char *separator = ",";
    // create name

    char *tmp = strtok(buffer, separator);
    new.name = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.name, tmp);
    // create adresse

    tmp = strtok(NULL, separator);
    new.adresse = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.adresse, tmp);

    // create postal
    tmp = strtok(NULL, separator);
    new.postal = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.postal, tmp);

    // create ville
    tmp = strtok(NULL, separator);
    new.ville = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.ville, tmp);

    // create lat
    tmp = strtok(NULL, separator);
    new.coord.lat = atof(tmp);

    // create lat
    tmp = strtok(NULL, separator);
    new.coord.longi = atof(tmp);

    new.num = num;
    return new;
}

Liste *open_csv_lieu(char *file_name)
{
    FILE *f = fopen(file_name, "r");
    int size = 0;
    char *buffer[1000];
    while (fgets(buffer, 1000, f))
    {
        size++;
    }
    lseek(f, 0, SEEK_SET);
    Liste new = malloc(sizeof(liste));
    new.size = size;
    new.lst = malloc(sizeof(lieu) * size);
    for (int i = 0; i < size; i++)
    {
        fgets(buffer, 1000, f);
        new.lst[i] = create_lieu(buffer, i);
    }
    fclose(f);
}

void free_lieu(Lieu *l)
{
    free(l->adresse);
    free(l->name);
    free(l->postal);
    free(l->ville);
    free(l);
    l = NULL;
}

void free_liste(Liste *l)
{
    for (int i = 0; i < l->size; i++)
    {
        free_lieu(l->lst[i]);
    }
    free(l);
    l = NULL;
}

void print_lieu(Lieu l)
{
    printf("%d: %s %s %s %s %d %d\n", l.num, l.adresse, l.name, l.postal, l.ville, l.coord.lat, l.coord.longi);
}

void print_liste(Liste l)
{
    printf("size list: %d\n", l.size);
    for (int i = 0; i < l.size; i++)
    {
        print_lieu(l.lst[i]);
    }
}