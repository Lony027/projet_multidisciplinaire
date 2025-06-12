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
    new.name = malloc(sizeof(char) * strlen(tmp) + 1);
    strcpy(new.name, tmp);
    // create adress

    tmp = strtok(NULL, separator);
    new.address = malloc(sizeof(char) * strlen(tmp) + 1);
    strcpy(new.address, tmp);

    // create postal
    tmp = strtok(NULL, separator);
    new.postal = malloc(sizeof(char) * strlen(tmp) + 1);
    strcpy(new.postal, tmp);

    // create ville
    tmp = strtok(NULL, separator);
    new.city = malloc(sizeof(char) * strlen(tmp) + 1);
    strcpy(new.city, tmp);

    // create lat
    tmp = strtok(NULL, separator);
    new.coord.latitude = atof(tmp);

    // create lat
    tmp = strtok(NULL, separator);
    new.coord.longitude = atof(tmp);

    new.num = num;
    return new;
}

List open_place_csv(char *file_name)
{

    FILE *f = fopen(file_name, "r");
    if (!f)
    {
        exit(1);
    }
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
    free(l->address);
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
    printf("%d: %s %s %s %s %f %f\n", l.num, l.address, l.name, l.postal, l.city, l.coord.latitude, l.coord.longitude);
}

void print_list(List l)
{
    printf("size list: %d\n", l.size);
    for (int i = 0; i < l.size; i++)
    {
        print_place(l.lst[i]);
    }
}
/*
List* place_sort(Coord* a, List *toSort){
    List *sorted = malloc(sizeof(List));
    sorted->size = toSort->size;
    sorted->lst = malloc(sizeof(Place) * toSort->size);
    for (int i = 0; i < toSort->size; i++)
    {
        sorted->lst[i] = toSort->lst[i];
    }
    for (int i = 0; i < sorted->size; i++)
    {
        for (int j = 0; j < sorted->size - 1; j++)
        {
            if (distance(a, &sorted->lst[j].coord) > distance(a, &sorted->lst[j + 1].coord))
            {
                Place tmp = sorted->lst[j];
                sorted->lst[j] = sorted->lst[j + 1];
                sorted->lst[j + 1] = tmp;
            }
        }
    }
    return sorted;
}
*/