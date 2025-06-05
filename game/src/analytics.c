#include "analytics.h"
#include "GLFW/glfw3.h"

void analytics_init(analytics_t *a) {
    a->fpsTimestampsCount = 0;
    a->fpsTimestampsHead = 0;
}

double analytics_fpsUpdate(analytics_t *a) {
    double current = glfwGetTime();

    a->fpsTimestampsHead = (a->fpsTimestampsHead + 1) % FPS_QUEUE_SIZE;
    a->fpsTimestamps[a->fpsTimestampsHead] = current;
    if (a->fpsTimestampsCount < FPS_QUEUE_SIZE) {
        a->fpsTimestampsCount++;
    }

    int removed = 0;
    while (a->fpsTimestampsCount > 0) {
        int iOldest = (a->fpsTimestampsHead - a->fpsTimestampsCount + 1 + FPS_QUEUE_SIZE) % FPS_QUEUE_SIZE;
        if (current - a->fpsTimestamps[iOldest] > 1.0) {
            a->fpsTimestampsCount--;
            removed++;
        } else {
            break;
        }
    }

    return (double)a->fpsTimestampsCount;
}
