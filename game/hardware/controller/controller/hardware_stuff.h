#ifndef HARDWARE_STUFF_H
#define HARDWARE_STUFF_H

#include <stdbool.h>
#include <stdint.h>

#define SWITCH_COUNT 5

typedef struct {
    float dx;
    float dy;
    bool buttons[SWITCH_COUNT];
} ReadData;

int hardware_init();


int read_data(ReadData* data);

#endif
