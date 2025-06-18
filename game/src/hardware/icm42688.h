#pragma once

#include <stdbool.h>

typedef struct {
    bool gyroValid;
    double gyro[3];
    bool accelValid;
    double accel[3];
    double temp;
    double timestamp;
} fifodata_t;

void init_imu(void);

uint16_t readFIFOLength();

bool readFIFOData(fifodata_t *res);
