#ifndef SPSCQUEUE_H
#define SPSCQUEUE_H

#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    void **buf;
    size_t mask;
    atomic_size_t head;
    atomic_size_t tail;
} spscRing_t;

/**
 * @brief Initialises a spscRing
 * @param r A pointer to a spscRing
 * @param cap The max capacity
 * @return If initialisation was succesful
 */
bool spscRing_init(spscRing_t *r, size_t cap);

bool spscRing_offer(spscRing_t *r, void *item);

bool spscRing_poll(spscRing_t *r, void **item);

/**
 * @brief Frees a spscRing
 * @param r A pointer to a spscRing
 */
void spscRing_free(const spscRing_t *r);

#endif
