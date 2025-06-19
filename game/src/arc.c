#include "arc.h"

arc_t *arc_new(void *p, arc_destructor d) {
    arc_t *a = (arc_t *)malloc(sizeof(arc_t));
    if (!a) {
        return NULL;
    }

    atomic_init(&a->refCount, 1);
    a->p = p;
    a->d = d;

    return a;
}

arc_t *arc_clone(arc_t *a) {
    atomic_fetch_add_explicit(&a->refCount, 1, memory_order_relaxed);
    return a;
}

void arc_drop(arc_t *a) {
    int old = atomic_fetch_sub_explicit(&a->refCount, 1, memory_order_acq_rel);

    if (old == 1) {
        atomic_thread_fence(memory_order_acquire);
        a->d(a->p);
        free(a);
    }
}