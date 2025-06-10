#include "analytics.h"
#include "GLFW/glfw3.h"

static void fpsUpdate(analytics_t *a) {
    a->fpsTimestampsHead = (a->fpsTimestampsHead + 1) % FPS_QUEUE_SIZE;
    a->fpsTimestamps[a->fpsTimestampsHead] = a->currentTime;
    if (a->fpsTimestampsCount < FPS_QUEUE_SIZE) {
        a->fpsTimestampsCount++;
    }

    int removed = 0;
    while (a->fpsTimestampsCount > 0) {
        int iOldest = (a->fpsTimestampsHead - a->fpsTimestampsCount + 1 + FPS_QUEUE_SIZE) % FPS_QUEUE_SIZE;
        if (a->currentTime - a->fpsTimestamps[iOldest] > 1.0) {
            a->fpsTimestampsCount--;
            removed++;
        } else {
            break;
        }
    }

    a->fps = a->fpsTimestampsCount;
}

void analytics_init(analytics_t *a) {
    a->previousTime = glfwGetTime();
    a->currentTime = a->previousTime;

    a->fpsTimestampsCount = 0;
    a->fpsTimestampsHead = 0;
}

void analytics_startFrame(analytics_t *a) {
    a->previousTime = a->currentTime;
    a->currentTime = glfwGetTime();
    a->dt = a->currentTime - a->previousTime;
    fpsUpdate(a);
}