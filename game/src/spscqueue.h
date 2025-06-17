#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct chunkValue chunkValue_t;

typedef struct {
    chunkValue_t **buf;
    size_t mask;
    atomic_size_t head;
    atomic_size_t tail;
} spscRing_t;

bool spscRing_init(spscRing_t *r, size_t cap);

bool spscRing_offer(spscRing_t *r, chunkValue_t *item);

bool spscRing_poll(spscRing_t *r, chunkValue_t **item);

void spscRing_free(const spscRing_t *r);