#pragma once
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