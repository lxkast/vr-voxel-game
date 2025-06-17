#include "queue.h"
#include <logging.h>
#include <string.h>

void queue_initQueue(lightQueue_t *queue) {
    queue->size = 0;
    queue->capacity = 16;
    queue->data = malloc(queue->capacity * sizeof(lightQueueItem_t));
    if (!queue->data) {
        LOG_FATAL("queue_init malloc failed");
    }
    queue->head = 0;
    queue->tail = 0;
}

void queue_freeQueue(lightQueue_t *queue) {
    free(queue->data);
    queue->data = NULL;
}

static void queue_resize(lightQueue_t *queue) {
    size_t oldCapacity = queue->capacity;
    queue->capacity *= 2;
    lightQueueItem_t *newQueue = malloc(queue->capacity * sizeof(lightQueueItem_t));
    if (!newQueue) {
        LOG_FATAL("queue_resize malloc failed");
    }
    for (size_t i = 0; i < oldCapacity; ++i) {
        newQueue[i] = queue->data[(queue->head + i) % oldCapacity];
    }
    free(queue->data);
    queue->data = newQueue;
    queue->head = 0;
    queue->tail = queue->size;
}

void queue_push(lightQueue_t *queue, lightQueueItem_t item) {
    if (queue->size == queue->capacity) {
        queue_resize(queue);
    }
    queue->data[queue->tail] = item;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
}

lightQueueItem_t queue_pop(lightQueue_t *queue) {
    if (queue->size == 0) {
        LOG_FATAL("Cannot pop from empty queue");
    }
    lightQueueItem_t item = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    return item;
}
