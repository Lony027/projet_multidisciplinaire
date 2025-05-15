#include "src/output.h"

#include <stdio.h>
#include <stdlib.h>

int create_output_csv(Models* model){
    FILE* output = fopen("output.csv", "w+");
    if (output == NULL)
    {
        perror("Unable to open the file");
        return 1;
    }
    
    for(int i=0; i < model->size; i++) {
        Queue* queue = model->list_truck[i];
        Appointment *apt = queue->first;
        while (apt != NULL)
        {
            fprintf(output,"%d", apt->place.num);
            if (apt->next != NULL) {
                fprintf(output, ",");
            }
            apt = apt->next;
        }
        fprintf(output, "\n");
    }
    fclose(output);
    return 0;
}