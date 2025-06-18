#ifndef QUEUE_H
#define QUEUE_H

#include <cglm/cglm.h>
#include <stdlib.h>

typedef struct {
    ivec3 pos;
    unsigned char lightValue;
} lightQueueItem_t;

// a resizing circular queue for light values
typedef struct {
    // capacity of the queue
    size_t capacity;
    // current size of the queue
    size_t size;
    // heap allocated array of items
    lightQueueItem_t *data;
    // index of the first element in the queue (returned when popped)
    size_t head;
    // index for the next element to be added
    size_t tail;
} lightQueue_t;

extern void queue_initQueue(lightQueue_t *queue);
extern void queue_freeQueue(lightQueue_t *queue);
extern void queue_push(lightQueue_t *queue, lightQueueItem_t item);
extern lightQueueItem_t queue_pop(lightQueue_t *queue);
extern int queue_tests();

#endif
