#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <stdbool.h>

typedef struct hashmapElement {
    char *name;
    void *data;
    struct hashmapElement *next;
} hashmapElement;

typedef struct {
    int size;
    int capacity;
    hashmapElement **data;
} hashmap;

typedef struct {
    int size;
    int capacity;
    void **data;
} resizingArrayList_t;

extern void *hashmap_getElement(const hashmap *hashmap, const char *name);
extern bool hashmap_setElement(hashmap *hashmap, const char *name, void *data);
extern bool hashmap_removeElement(hashmap *hashmap, const char *name);

extern void hashmap_free(const hashmap *hashmap);
extern void hashmap_init(hashmap *hashmap, int capacity);

extern void arraylist_init(resizingArrayList_t *arrList, int capacity);
extern void arraylist_free(resizingArrayList_t *arrList);
extern void arraylist_append(resizingArrayList_t *arrList, void *element);
extern bool arraylist_remove(resizingArrayList_t *arrList, const void *element);
extern void *arraylist_get(const resizingArrayList_t *arrList, int index);
extern int arraylist_size(const resizingArrayList_t *arrList);

#endif
