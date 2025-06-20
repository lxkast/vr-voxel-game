#ifndef SPSCQUEUE_H
#define SPSCQUEUE_H

#include <stdatomic.h>
#include <stdbool.h>

/**
 * @brief A struct representing an spscRing
 */
typedef struct {
    /// The heap-allocated array of data
    void **buf;
    /// The mask that contains data about how to access and add to the ring
    size_t mask;
    /// The value representing the head of the queue
    atomic_size_t head;
    /// The value representing the tail of the queue
    atomic_size_t tail;
} spscRing_t;

/**
 * @brief Initialises a spscRing
 * @param r A pointer to a spscRing
 * @param cap The max capacity
 * @return bool If initialisation was succesful
 */
bool spscRing_init(spscRing_t *r, size_t cap);

/**
 * @brief Attempts to queue an item to the ring
 * @param r A pointer to an spscRing
 * @param item The item
 * @return bool Whether the operation was successful
 */
bool spscRing_offer(spscRing_t *r, void *item);

/**
 * @brief Attempts to dequeue an item to the ring
 * @param r A pointer to an spscRing
 * @param item A location to store the item
 * @return bool Whether the operation was successful
 */
bool spscRing_poll(spscRing_t *r, void **item);

/**
 * @brief Frees a spscRing
 * @param r A pointer to a spscRing
 */
void spscRing_free(const spscRing_t *r);

#endif
