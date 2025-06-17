#include "datastructs.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../logging/logging.h"

#define RESIZE_UP_THRESHOLD 0.75
#define RESIZE_DOWN_THRESHOLD 0.25
#define RESIZE_AMOUNT 2
#define MINIMUM_SIZE 16

static unsigned long hash_str(const char *str) {
    long hash = 5381;
    while (*str != '\0') {
        hash = ((hash << 5) + str[0]) ^ hash;
        str++;
    }
    return hash;
}

static void resize(hashmap *map, const int new_capacity) {
    hashmapElement **old = map->data;
    const int old_capacity = map->capacity;
    map->data = malloc(sizeof(hashmapElement *) * new_capacity);
    if (!map->data) {
        LOG_FATAL("Failed to malloc map data in resize");
    }
    memset(map->data, 0, sizeof(hashmapElement *) * new_capacity);
    map->capacity = new_capacity;

    for (int i = 0; i < old_capacity; i++) {
        hashmapElement *current = old[i];
        while (current) {
            hashmap_setElement(map, current->name, current->data);
            map->size--;    // keep size constant to avoid downsizing or upsizing when reinserting into the map.

            hashmapElement *next = current->next;
            free(current->name);
            free(current);
            current = next;
        }
    }
    free(old);
}

static void considerResize(hashmap *map) {
    if ((double)map->size / map->capacity > RESIZE_UP_THRESHOLD) {
        resize(map, RESIZE_AMOUNT * map->capacity);
    } else if (map->capacity > MINIMUM_SIZE && (double)map->size / map->capacity < RESIZE_DOWN_THRESHOLD) {
        resize(map, map->capacity / RESIZE_AMOUNT);
    }
}

/*
 * Get an element from the hashmap, if the element doesn't exist then this returns NULL
 */
void *hashmap_getElement(const hashmap *hashmap, const char *name) {
    const unsigned long hash = hash_str(name);

    if (hashmap->data[hash % hashmap->capacity] == NULL) {
        return NULL;
    }

    hashmapElement current = *hashmap->data[hash % hashmap->capacity];
    while (strcmp(current.name, name) != 0) {
        if (current.next == NULL) {
            return NULL;
        }
        current = *current.next;
    }
    return current.data;
}

bool hashmap_setElement(hashmap *hashmap, const char *name, void *data) {
    const unsigned long hash = hash_str(name);

    hashmapElement **current = &hashmap->data[hash % hashmap->capacity];
    while (*current) {
        if (strcmp(name, (*current)->name) == 0) {
            (*current)->data = data;
            return true;
        }
        current = &(*current)->next;
    }

    hashmapElement *newElement = malloc(sizeof(hashmapElement));
    if (!newElement) {
        LOG_FATAL("Failed to malloc newElement in hashmap_setElement");
    }
    newElement->name = strdup(name);
    newElement->data = data;
    newElement->next = NULL;
    *current = newElement;
    hashmap->size++;
    considerResize(hashmap);
    return false;
}

bool hashmap_removeElement(hashmap *hashmap, const char *name) {
    const unsigned long hash = hash_str(name);
    hashmapElement **current = &hashmap->data[hash % hashmap->capacity];
    while (*current) {
        if (strcmp((*current)->name, name) == 0) {
            free((*current)->name);
            free(*current);

            *current = NULL;
            hashmap->size--;
            considerResize(hashmap);
            return true;
        }
        current = &(*current)->next;
    }
    return false;
}

void hashmap_free(const hashmap *hashmap) {
    for (int i = 0; i < hashmap->capacity; i++) {
        hashmapElement *current = hashmap->data[i];
        while (current) {
            hashmapElement *prev = current;
            current = current->next;
            free(prev->name);
            free(prev);
        }
    }
    free(hashmap->data);
}

void hashmap_init(hashmap *hashmap, const int capacity) {
    hashmap->capacity = capacity;
    hashmap->size = 0;
    hashmap->data = malloc(sizeof(hashmapElement *) * capacity);
    if (!hashmap->data) {
        LOG_FATAL("Failed to mallloc hashmap data in hashmap_init");
    }
    memset(hashmap->data, 0, sizeof(hashmapElement *) * capacity);
}

void arraylist_init(resizingArrayList_t *arrList, const int capacity) {
    arrList->capacity = capacity;
    arrList->size = 0;
    arrList->data = malloc(sizeof(resizingArrayList_t *) * capacity);
    if (!arrList->data) {
        LOG_FATAL("Failed to mallloc arrayList data in arraylist_init");
    }
}

void arraylist_free(resizingArrayList_t *arrList) {
    free(arrList->data);
    arrList->capacity = 0;
}

static void arraylist_resize(resizingArrayList_t *arrList, const int newCapacity) {
    arrList->data = realloc(arrList->data, sizeof(resizingArrayList_t *) * newCapacity);
    if (!arrList->data) {
        LOG_FATAL("Failed to realloc arrayList data in arraylist_resize");
    }
    arrList->capacity = newCapacity;
}

void arraylist_append(resizingArrayList_t *arrList, void *element) {
    if (arrList->size == arrList->capacity) {
        arraylist_resize(arrList, arrList->capacity * RESIZE_AMOUNT);
    }
    arrList->data[arrList->size++] = element;
}

bool arraylist_remove(resizingArrayList_t *arrList, const void *element) {
    int i = 0;
    while (i < arrList->size && arrList->data[i] != element) {
        i++;
    }
    if (i == arrList->size) {
        return false;
    }
    for (; i < arrList->size - 1; i++) {
        arrList->data[i] = arrList->data[i + 1];
    }

    arrList->size--;
    return true;
}

void *arraylist_get(const resizingArrayList_t *arrList, const int index) {
    if (index >= arrList->size) {
        return NULL;
    }
    return arrList->data[index];
}

int arraylist_size(const resizingArrayList_t *arrList) {
    return arrList->size;
}
