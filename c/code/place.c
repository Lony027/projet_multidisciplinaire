#include "src/place.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Place create_place(char *buffer, int num)
{
    Place new;
    char *separator = ",";
    // create name

    char *tmp = strtok(buffer, separator);
    new.name = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.name, tmp);
    // create adress

    tmp = strtok(NULL, separator);
    new.adress = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.adress, tmp);

    // create postal
    tmp = strtok(NULL, separator);
    new.postal = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.postal, tmp);

    // create ville
    tmp = strtok(NULL, separator);
    new.city = malloc(sizeof(char) * strlen(tmp));
    strcpy(new.city, tmp);

    // create lat
    tmp = strtok(NULL, separator);
    new.coord.lat = atof(tmp);

    // create lat
    tmp = strtok(NULL, separator);
    new.coord.longi = atof(tmp);

    new.num = num;
    return new;
}

List open_csv_place(char *file_name)
{
    FILE *f = fopen(file_name, "r");
    int size = 0;
    char buffer[1000];
    while (fgets(buffer, 1000, f))
    {
        size++;
    }
    fseek(f, 0, SEEK_SET);
    List new;
    new.size = size;
    new.lst = malloc(sizeof(Place) * size);
    for (int i = 0; i < size; i++)
    {
        fgets(buffer, 1000, f);
        new.lst[i] = create_place(buffer, i);
    }
    fclose(f);
    return new;
}

void free_place(Place *l)
{
    free(l->adress);
    free(l->name);
    free(l->postal);
    free(l->city);
}

void free_list(List *l)
{
    for (int i = 0; i < l->size; i++)
    {
        free_place(&(l->lst[i]));
    }
    free(l->lst);
    l->lst = NULL;
}

void print_place(Place l)
{
    printf("%d: %s %s %s %s %f %f\n", l.num, l.adress, l.name, l.postal, l.city, l.coord.lat, l.coord.longi);
}

void print_list(List l)
{
    printf("size list: %d\n", l.size);
    for (int i = 0; i < l.size; i++)
    {
        print_place(l.lst[i]);
    }
}
