//
// Created by src24 on 27/05/25.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "datastructs.h"

#define ASSERT(predicate, msg) if (!(predicate)) { LOG_FATAL("Error: %s\n", msg); exit(1); }

void testInsertion() {
    hashmap data;
    hashmap_init(&data, 10);

    int x = 1, y = 2;
    hashmap_setElement(&data, "test", &x);
    hashmap_setElement(&data, "test2", &y);
    ASSERT(hashmap_getElement(&data, "test") == &x, "Failed to read back set element");
    ASSERT(hashmap_getElement(&data, "test2") == &y, "Failed to read back set element");
    LOG_INFO("Test hashmap insertion completed");

    hashmap_free(&data);
}

void testGetEmpty() {
    hashmap data;
    hashmap_init(&data, 10);

    ASSERT(hashmap_getElement(&data, "test") == NULL, "Empty hashmap doesn't return null");
    ASSERT(!hashmap_removeElement(&data, "test"), "Remove from empty should return false");
    LOG_INFO("Test hashmap getEmpty completed");
    hashmap_free(&data);
}

void testChange() {
    hashmap data;
    hashmap_init(&data, 10);

    int x = 1, y = 2;
    hashmap_setElement(&data, "test", &x);
    hashmap_setElement(&data, "test", &y);
    ASSERT(hashmap_getElement(&data, "test") == &y, "Reset failed");
    LOG_INFO("Test hashmap change completed");
    hashmap_free(&data);
}

void testRemove() {
    hashmap data;
    hashmap_init(&data, 10);

    int x = 1, y = 2;
    hashmap_setElement(&data, "test", &x);
    hashmap_setElement(&data, "test2", &y);
    ASSERT(hashmap_getElement(&data, "test") == &x, "Failed to read back set element");
    ASSERT(hashmap_getElement(&data, "test2") == &y, "Failed to read back set element");

    hashmap_removeElement(&data, "test");
    ASSERT(hashmap_getElement(&data, "test") == NULL, "Remove failed");
    ASSERT(hashmap_getElement(&data, "test2") == &y, "Removed the wrong thing");
    LOG_INFO("Test hashmap remove completed");

    hashmap_free(&data);
}

void stressTest() {
    hashmap map;
    hashmap_init(&map, 2);
    srand(0);
    char string[10];
    for (uint64_t i = 1; i < 1000; i++) {
        sprintf(string, "%ld", i);
        string[9] = '\0';
        hashmap_setElement(&map, string, (void*) i);
        ASSERT(map.size <= i, "Size mismatch");
    }
    srand(0);
    for (uint64_t i = 1; i < 1000; i++) {
        sprintf(string, "%ld", i);
        string[9] = '\0';
        uint64_t got = (uint64_t) hashmap_getElement(&map, string);
        ASSERT(got == i, "Stress test failed");
    }
    LOG_INFO("Hashmap Stress Test completed");
    hashmap_free(&map);
}


void arrListTestInsert() {
    resizingArrayList_t arrList;
    arraylist_init(&arrList, 10);
    int x = 1, y = 2, z = 3;
    arraylist_append(&arrList, &x);
    arraylist_append(&arrList, &y);
    arraylist_append(&arrList, &z);
    ASSERT(arraylist_size(&arrList) == 3, "Size mismatch");
    ASSERT(arraylist_get(&arrList, 0) == &x, "Readback mismatch");
    ASSERT(arraylist_get(&arrList, 1) == &y, "Readback mismatch");
    ASSERT(arraylist_get(&arrList, 2) == &z, "Readback mismatch");
    arraylist_free(&arrList);
    LOG_INFO("Test arraylist insertion completed");
}

void arrListTestDelete() {
    resizingArrayList_t arrList;
    arraylist_init(&arrList, 10);
    int x = 1, y = 2, z = 3;
    arraylist_append(&arrList, &x);
    arraylist_append(&arrList, &y);
    arraylist_append(&arrList, &z);
    ASSERT(arraylist_size(&arrList) == 3, "Size mismatch");
    arraylist_remove(&arrList, &y);
    ASSERT(arraylist_size(&arrList) == 2, "Size mismatch");

    ASSERT(arraylist_get(&arrList, 0) == &x, "Readback mismatch");
    ASSERT(arraylist_get(&arrList, 1) == &z, "Readback mismatch");
    arraylist_free(&arrList);
    LOG_INFO("Test arraylist deletion completed");
}


int main(void) {
    log_init(stdout);
    testInsertion();
    testGetEmpty();
    testRemove();
    testChange();
    stressTest();

    arrListTestInsert();
    arrListTestDelete();

    LOG_INFO("Tests OK");
}