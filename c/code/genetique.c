#include "src/genetique.h"

Models *first_model(List list, Matrix *m)
{
    Models *new = init_models(list.size);

    for (int i = 1; i < list.size; i++)
    {
        Appointment *start = create_appointment(list.lst[0]);
        Appointment *tmp = create_appointment(list.lst[i]);
        Queue *queue = create_queue();
        enqueue(start, queue, m);
        enqueue(tmp, queue, m);
        add_truck(queue, new);
    }
    return new;
}

int is_better(Models *last, Models *new)
{
    for (int i = 0; i < new->size; i++)
    {
        if (new->list_truck[i]->time > 3 * 60 * 60)
        {
            return 0;
        }
    }
    if (last->dist_tot > new->dist_tot)
    {
        return 1;
    }
    return 0;
}
