#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void **buf;
    size_t mask;
    atomic_size_t head;
    atomic_size_t tail;
} spscRing_t;

bool spscRing_init(spscRing_t *r, size_t cap);

bool spscRing_offer(spscRing_t *r, void *item);

bool spscRing_poll(spscRing_t *r, void **item);

void spscRing_free(const spscRing_t *r);