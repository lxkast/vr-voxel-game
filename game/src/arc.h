#ifndef ARC_H
#define ARC_H

#include <stdatomic.h>
#include <stdlib.h>

typedef void (*arc_destructor)(void *p);

typedef struct {
    atomic_int refCount;
    void *p;
    arc_destructor d;
} arc_t;

arc_t *arc_new(void *p, arc_destructor d);

arc_t *arc_clone(arc_t *a);

void arc_drop(arc_t *a);

#endif
