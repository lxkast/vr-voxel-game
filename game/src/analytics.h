#pragma once

#define FPS_QUEUE_SIZE 1024

typedef struct {
    double currentTime;
    double previousTime;
    double dt;
    double fps;

    double fpsTimestamps[FPS_QUEUE_SIZE];
    int fpsTimestampsHead;
    int fpsTimestampsCount;
} analytics_t;

void analytics_init(analytics_t *a);

double analytics_fpsUpdate(analytics_t *a);

void analytics_startFrame(analytics_t *a);