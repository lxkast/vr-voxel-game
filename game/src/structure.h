#pragma once
#include "block.h"

typedef struct chunkValue_t chunkValue_t;

typedef struct {
    int cacheN;
    chunkValue_t *cache[3][3][3];

    chunkValue_t *origin;
    int ox, oy, oz;
} decorator_t;

typedef struct {
    block_t type;
    int x,y,z;
    float chanceToAppear;
} structureBlock_t;

typedef struct {
    decorator_t decorator;
    int numBlocks;
    structureBlock_t *blocks;
    block_t base;
} structure_t;

static const structureBlock_t cactusPattern[] = {
    {BL_CACTUS, 0, 0, 0, 1.f},
    {BL_CACTUS, 0, 1, 0, 1.f},
    {BL_CACTUS, 0, 2, 0, 0.5f},
};

const structure_t cactusStructure = {
    .numBlocks = 3,
    .blocks = cactusPattern,
    .base = BL_SAND,
};

static const structureBlock_t treePattern[] = {
    // Tree trunk
    {BL_LOG, 0,0,0, 1.f},
    {BL_LOG, 0,1,0, 1.f},
    {BL_LOG, 0,2,0, 1.f},
    {BL_LOG, 0,3,0, 1.f},
    {BL_LOG, 0,4,0, 1.f},

    // y=2 layer (5x5)
    {BL_LEAF, -2,2,-2, 1.f}, {BL_LEAF, -1,2,-2, 1.f}, {BL_LEAF, 0,2,-2, 1.f}, {BL_LEAF, 1,2,-2, 1.f}, {BL_LEAF, 2,2,-2, 1.f},
    {BL_LEAF, -2,2,-1, 1.f}, {BL_LEAF, -1,2,-1, 1.f}, {BL_LEAF, 0,2,-1, 1.f}, {BL_LEAF, 1,2,-1, 1.f}, {BL_LEAF, 2,2,-1, 1.f},
    {BL_LEAF, -2,2,0, 1.f},  {BL_LEAF, -1,2,0, 1.f},  {BL_LEAF, 1,2,0, 1.f},  {BL_LEAF, 2,2,0, 1.f},
    {BL_LEAF, -2,2,1, 1.f},  {BL_LEAF, -1,2,1, 1.f},  {BL_LEAF, 0,2,1, 1.f},  {BL_LEAF, 1,2,1, 1.f},  {BL_LEAF, 2,2,1, 1.f},
    {BL_LEAF, -2,2,2, 1.f},  {BL_LEAF, -1,2,2, 1.f},  {BL_LEAF, 0,2,2, 1.f},  {BL_LEAF, 1,2,2, 1.f},  {BL_LEAF, 2,2,2, 1.f},

    // y=3 layer (5x5)
    {BL_LEAF, -2,3,-2, 0.8f}, {BL_LEAF, -1,3,-2, 1.f}, {BL_LEAF, 0,3,-2, 1.f}, {BL_LEAF, 1,3,-2, 1.f}, {BL_LEAF, 2,3,-2, 0.8f},
    {BL_LEAF, -2,3,-1, 1.f}, {BL_LEAF, -1,3,-1, 1.f}, {BL_LEAF, 0,3,-1, 1.f}, {BL_LEAF, 1,3,-1, 1.f}, {BL_LEAF, 2,3,-1, 1.f},
    {BL_LEAF, -2,3,0, 1.f},  {BL_LEAF, -1,3,0, 1.f},  {BL_LEAF, 1,3,0, 1.f},  {BL_LEAF, 2,3,0, 1.f},
    {BL_LEAF, -2,3,1, 1.f},  {BL_LEAF, -1,3,1, 1.f},  {BL_LEAF, 0,3,1, 1.f},  {BL_LEAF, 1,3,1, 1.f},  {BL_LEAF, 2,3,1, 1.f},
    {BL_LEAF, -2,3,2, 0.8f},  {BL_LEAF, -1,3,2, 1.f},  {BL_LEAF, 0,3,2, 1.f},  {BL_LEAF, 1,3,2, 1.f},  {BL_LEAF, 2,3,2, 0.8f},

    // y=4 layer (diamond pattern)
    {BL_LEAF, -1,4,0, 1.f}, {BL_LEAF, 0,4,-1, 1.f}, {BL_LEAF, 0,4,0, 1.f}, {BL_LEAF, 0,4,1, 1.f}, {BL_LEAF, 1,4,0, 1.f},

    // y=5 layer (diamond pattern)
    {BL_LEAF, -1,5,0, 1.f}, {BL_LEAF, 0,5,-1, 1.f}, {BL_LEAF, 0,5,0, 1.f}, {BL_LEAF, 0,5,1, 1.f}, {BL_LEAF, 1,5,0, 1.f}
};

const structure_t treeStructure = {
    .numBlocks = 63,
    .blocks = treePattern,
    .base = BL_GRASS,
};

// second structure for testing purposes
static const structureBlock_t stoneT[] = {
    {BL_STONE, 0, 0, 0, 1.f},
    {BL_STONE, 0, 1, 0, 1.f},
    {BL_STONE, 0, 2, 0, 1.f},
    {BL_STONE, -1, 2, 0, 1.f},
    {BL_STONE, 1, 2, 0, 1.f},
};

const structure_t stoneTStructure = {
    .numBlocks = 5,
    .blocks = stoneT,
    .base = BL_GRASS,
};

static const structureBlock_t woodenHouse[] = {
    {5, -1, 0, -2, 1.f},
    {5, -1, 0, 2, 1.f},
    {5, -1, 1, -2, 1.f},
    {5, -1, 1, 2, 1.f},
    {5, -1, 2, -2, 1.f},
    {5, -1, 2, -1, 1.f},
    {5, -1, 2, 1, 1.f},
    {5, -1, 2, 2, 1.f},
    {5, -1, 3, -1, 1.f},
    {5, -1, 3, 0, 1.f},
    {5, -1, 3, 1, 1.f},
    {5, 0, 0, -2, 1.f},
    {5, 0, 0, 2, 1.f},
    {5, 0, 2, -2, 1.f},
    {5, 0, 2, 2, 1.f},
    {5, 0, 3, -1, 1.f},
    {5, 0, 3, 0, 1.f},
    {5, 0, 3, 1, 1.f},
    {5, 1, 0, -2, 1.f},
    {5, 1, 0, 2, 1.f},
    {5, 1, 1, -2, 1.f},
    {5, 1, 1, 2, 1.f},
    {5, 1, 2, -2, 1.f},
    {5, 1, 2, 2, 1.f},
    {5, 1, 3, -1, 1.f},
    {5, 1, 3, 0, 1.f},
    {5, 1, 3, 1, 1.f},
    {5, 2, 0, -1, 1.f},
    {5, 2, 0, 0, 1.f},
    {5, 2, 0, 1, 1.f},
    {5, 2, 1, -1, 1.f},
    {5, 2, 1, 1, 1.f},
    {5, 2, 2, -1, 1.f},
    {5, 2, 2, 0, 1.f},
    {5, 2, 2, 1, 1.f},
};

const structure_t woodenHouseStructure = {
    .numBlocks = sizeof(woodenHouse)/sizeof(structureBlock_t),
    .blocks = woodenHouse,
    .base = BL_SAND,
};

structure_t structures[] = {
    treeStructure,
    stoneTStructure,
    woodenHouseStructure,
};

const int numStructures = sizeof(structures)/sizeof(structure_t);
