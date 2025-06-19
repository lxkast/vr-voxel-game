#ifndef ARC_H
#define ARC_H

#include <stdatomic.h>
#include <stdlib.h>

/**
 * @brief A type for a function pointer to a destructor function for the arc
 */
typedef void (*arc_destructor)(void *p);

/**
 * @brief An atomic reference counter struct
 */
typedef struct {
    /// The reference count of the arc
    atomic_int refCount;
    /// The inner pointer to the data
    void *p;
    /// The destructor function to call when p should be freed
    arc_destructor d;
} arc_t;

/**
 * @brief Creates a new arc
 * @param p A pointer to the data
 * @param d A destructor function pointer
 * @return
 */
arc_t *arc_new(void *p, arc_destructor d);

/**
 * @brief Clones an arc (increases the reference count)
 * @param a A pointer to an arc
 * @return arc_t A pointer to the arc
 * @note The pointer returned is actually just the same
 */
arc_t *arc_clone(arc_t *a);

/**
 * @brief Drops a reference to the arc, frees the data if the new number of references is 0
 * @param a A pointer to an arc
 */
void arc_drop(arc_t *a);

#endif
