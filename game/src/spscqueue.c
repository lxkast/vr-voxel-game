#include "spscqueue.h"
#include <stdlib.h>

bool spscRing_init(spscRing_t *r, const size_t cap) {
    if ((cap & (cap - 1)) != 0) {
        return false;
    }

    r->buf = calloc(cap, sizeof(chunkValue_t *));
    if (!r->buf) {
        return false;
    }

    r->mask = cap - 1;
    atomic_init(&r->head, 0);
    atomic_init(&r->head, 0);

    return true;
}

bool spscRing_offer(spscRing_t *r, chunkValue_t *item) {
    size_t tail = atomic_load_explicit(&r->tail, memory_order_relaxed);
    size_t next = (tail + 1) % r->mask;

    if (next == atomic_load_explicit(&r->head, memory_order_acquire)) {
        return false;
    }
    r->buf[tail] = item;
    atomic_store_explicit(&r->tail, next, memory_order_release);
    return true;
}

bool spscRing_poll(spscRing_t *r, chunkValue_t **item) {
    size_t h = atomic_load_explicit(&r->head, memory_order_relaxed);

    if (h == atomic_load_explicit(&r->tail, memory_order_acquire)) {
        return false;
    }

    *item = r->buf[h];
    atomic_store_explicit(&r->head, (h + 1) & r->mask, memory_order_release);

    return true;
}

void spscRing_free(const spscRing_t *r) {
    free(r->buf);
}
