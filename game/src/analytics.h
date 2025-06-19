#ifndef ANALYTICS_H
#define ANALYTICS_H

#define FPS_QUEUE_SIZE 1024

typedef struct {
    /// The time at the current frame
    double currentTime;
    /// The time at the previous frame
    double previousTime;
    /// The time since the last frame
    double dt;
    /// The FPS
    double fps;

    double fpsTimestamps[FPS_QUEUE_SIZE];
    int fpsTimestampsHead;
    int fpsTimestampsCount;
} analytics_t;

void analytics_init(analytics_t *a);

void analytics_startFrame(analytics_t *a);

#endif