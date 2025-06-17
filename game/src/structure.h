#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "block.h"

#define STRUCTURE_SIZE(pattern) sizeof(pattern) / sizeof(structureBlock_t)

typedef struct chunkValue_t chunkValue_t;

typedef struct {
    /// The cache number
    int cacheN;
    /// The chunkValues in the cache
    chunkValue_t *cache[3][3][3];

    /// The origin chunk
    chunkValue_t *origin;
    /// The x,y and z coords of the origin
    int ox, oy, oz;
} decorator_t;

typedef struct {
    /// The type of the block
    block_t type;
    /// The offset of the block from the origin
    int x,y,z;
    /// The chance the block has to appear
    float chanceToAppear;
    /// Whether the block can appear if placing it would overlap the same type of block
    bool allowOverlap;
} structureBlock_t;

typedef struct {
    /// The decorator for the structure
    decorator_t decorator;
    /// How many blocks are in the structure
    int numBlocks;
    /// The array of blocks making up the structure
    structureBlock_t *blocks;
} structure_t;

static const structureBlock_t cactusPattern[] = {
    {BL_CACTUS, 0, 0, 0, 1.f, true},
    {BL_CACTUS, 0, 1, 0, 1.f, true},
    {BL_CACTUS, 0, 2, 0, 0.5f, true},
};

const structure_t cactusStructure = {
    .numBlocks = STRUCTURE_SIZE(cactusPattern),
    .blocks = cactusPattern,
};

static const structureBlock_t treePattern[] = {
    // Tree trunk
    {BL_LOG, 0,0,0, 1.f, false},
    {BL_LOG, 0,1,0, 1.f, false},
    {BL_LOG, 0,2,0, 1.f, false},
    {BL_LOG, 0,3,0, 1.f, false},
    {BL_LOG, 0,4,0, 1.f, false},

    // y=2 layer (5x5)
    {BL_LEAF, -2,2,-2, 1.f, true}, {BL_LEAF, -1,2,-2, 1.f, true}, {BL_LEAF, 0,2,-2, 1.f, true}, {BL_LEAF, 1,2,-2, 1.f, true}, {BL_LEAF, 2,2,-2, 1.f, true},
    {BL_LEAF, -2,2,-1, 1.f, true}, {BL_LEAF, -1,2,-1, 1.f, true}, {BL_LEAF, 0,2,-1, 1.f, true}, {BL_LEAF, 1,2,-1, 1.f, true}, {BL_LEAF, 2,2,-1, 1.f, true},
    {BL_LEAF, -2,2,0, 1.f, true},  {BL_LEAF, -1,2,0, 1.f, true},  {BL_LEAF, 1,2,0, 1.f, true},  {BL_LEAF, 2,2,0, 1.f, true},
    {BL_LEAF, -2,2,1, 1.f, true},  {BL_LEAF, -1,2,1, 1.f, true},  {BL_LEAF, 0,2,1, 1.f, true},  {BL_LEAF, 1,2,1, 1.f, true},  {BL_LEAF, 2,2,1, 1.f, true},
    {BL_LEAF, -2,2,2, 1.f, true},  {BL_LEAF, -1,2,2, 1.f, true},  {BL_LEAF, 0,2,2, 1.f, true},  {BL_LEAF, 1,2,2, 1.f, true},  {BL_LEAF, 2,2,2, 1.f, true},

    // y=3 layer (5x5)
    {BL_LEAF, -2,3,-2, 0.8f, true}, {BL_LEAF, -1,3,-2, 1.f, true}, {BL_LEAF, 0,3,-2, 1.f, true}, {BL_LEAF, 1,3,-2, 1.f, true}, {BL_LEAF, 2,3,-2, 0.8f},
    {BL_LEAF, -2,3,-1, 1.f, true}, {BL_LEAF, -1,3,-1, 1.f, true}, {BL_LEAF, 0,3,-1, 1.f, true}, {BL_LEAF, 1,3,-1, 1.f, true}, {BL_LEAF, 2,3,-1, 1.f, true},
    {BL_LEAF, -2,3,0, 1.f, true},  {BL_LEAF, -1,3,0, 1.f, true},  {BL_LEAF, 1,3,0, 1.f, true},  {BL_LEAF, 2,3,0, 1.f, true},
    {BL_LEAF, -2,3,1, 1.f, true},  {BL_LEAF, -1,3,1, 1.f, true},  {BL_LEAF, 0,3,1, 1.f, true},  {BL_LEAF, 1,3,1, 1.f, true},  {BL_LEAF, 2,3,1, 1.f, true},
    {BL_LEAF, -2,3,2, 0.8f, true},  {BL_LEAF, -1,3,2, 1.f, true},  {BL_LEAF, 0,3,2, 1.f, true},  {BL_LEAF, 1,3,2, 1.f, true},  {BL_LEAF, 2,3,2, 0.8f, true},

    // y=4 layer (diamond pattern)
    {BL_LEAF, -1,4,0, 1.f, true}, {BL_LEAF, 0,4,-1, 1.f, true}, {BL_LEAF, 0,4,0, 1.f, true}, {BL_LEAF, 0,4,1, 1.f, true}, {BL_LEAF, 1,4,0, 1.f, true},

    // y=5 layer (diamond pattern)
    {BL_LEAF, -1,5,0, 0.8f, true}, {BL_LEAF, 0,5,-1, 0.8f, true}, {BL_LEAF, 0,5,0, 1.f, true}, {BL_LEAF, 0,5,1, 1.f, true}, {BL_LEAF, 1,5,0, 1.f, true}
};

const structure_t treeStructure = {
    .numBlocks = STRUCTURE_SIZE(treePattern),
    .blocks = treePattern,
};

static const structureBlock_t jungleTreePattern[] = {
    // Tree trunk
    {BL_JUNGLE_LOG, 0,0,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,1,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,2,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,3,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,4,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,5,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,6,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,7,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,8,0, 1.f, false},
    {BL_JUNGLE_LOG, 0,9,0, 1.f, false},

    // y=2 layer (5x5)
    {BL_JUNGLE_LEAF, -2,2,-2, 1.f, true}, {BL_JUNGLE_LEAF, -1,2,-2, 1.f, true}, {BL_JUNGLE_LEAF, 0,2,-2, 1.f, true}, {BL_JUNGLE_LEAF, 1,2,-2, 1.f, true}, {BL_JUNGLE_LEAF, 2,2,-2, 1.f, true},
    {BL_JUNGLE_LEAF, -2,2,-1, 1.f, true}, {BL_JUNGLE_LEAF, -1,2,-1, 1.f, true}, {BL_JUNGLE_LEAF, 0,2,-1, 1.f, true}, {BL_JUNGLE_LEAF, 1,2,-1, 1.f, true}, {BL_JUNGLE_LEAF, 2,2,-1, 1.f, true},
    {BL_JUNGLE_LEAF, -2,2,0, 1.f, true},  {BL_JUNGLE_LEAF, -1,2,0, 1.f, true},  {BL_JUNGLE_LEAF, 1,2,0, 1.f, true},  {BL_JUNGLE_LEAF, 2,2,0, 1.f, true},
    {BL_JUNGLE_LEAF, -2,2,1, 1.f, true},  {BL_JUNGLE_LEAF, -1,2,1, 1.f, true},  {BL_JUNGLE_LEAF, 0,2,1, 1.f, true},  {BL_JUNGLE_LEAF, 1,2,1, 1.f, true},  {BL_JUNGLE_LEAF, 2,2,1, 1.f, true},
    {BL_JUNGLE_LEAF, -2,2,2, 1.f, true},  {BL_JUNGLE_LEAF, -1,2,2, 1.f, true},  {BL_JUNGLE_LEAF, 0,2,2, 1.f, true},  {BL_JUNGLE_LEAF, 1,2,2, 1.f, true},  {BL_JUNGLE_LEAF, 2,2,2, 1.f, true},

    // y=7 layer (5x5)
    {BL_JUNGLE_LEAF, -2,7,-2, 0.8f, true}, {BL_JUNGLE_LEAF, -1,7,-2, 1.f, true}, {BL_JUNGLE_LEAF, 0,7,-2, 1.f, true}, {BL_JUNGLE_LEAF, 1,7,-2, 1.f, true}, {BL_JUNGLE_LEAF, 2,7,-2, 0.8f, true},
    {BL_JUNGLE_LEAF, -2,7,-1, 1.f, true}, {BL_JUNGLE_LEAF, -1,7,-1, 1.f, true}, {BL_JUNGLE_LEAF, 0,7,-1, 1.f, true}, {BL_JUNGLE_LEAF, 1,7,-1, 1.f, true}, {BL_JUNGLE_LEAF, 2,7,-1, 1.f, true},
    {BL_JUNGLE_LEAF, -2,7,0, 1.f, true},  {BL_JUNGLE_LEAF, -1,7,0, 1.f, true},  {BL_JUNGLE_LEAF, 1,7,0, 1.f, true},  {BL_JUNGLE_LEAF, 2,7,0, 1.f, true},
    {BL_JUNGLE_LEAF, -2,7,1, 1.f, true},  {BL_JUNGLE_LEAF, -1,7,1, 1.f, true},  {BL_JUNGLE_LEAF, 0,7,1, 1.f, true},  {BL_JUNGLE_LEAF, 1,7,1, 1.f, true},  {BL_JUNGLE_LEAF, 2,7,1, 1.f, true},
    {BL_JUNGLE_LEAF, -2,7,2, 0.8f, true},  {BL_JUNGLE_LEAF, -1,7,2, 1.f, true},  {BL_JUNGLE_LEAF, 0,7,2, 1.f, true},  {BL_JUNGLE_LEAF, 1,7,2, 1.f, true},  {BL_JUNGLE_LEAF, 2,7,2, 0.8f, true},

    // y=8 layer (diamond pattern)
    {BL_JUNGLE_LEAF, -1,8,0, 1.f, true}, {BL_JUNGLE_LEAF, 0,8,-1, 1.f, true}, {BL_JUNGLE_LEAF, 0,8,0, 1.f, true}, {BL_JUNGLE_LEAF, 0,8,1, 1.f, true}, {BL_JUNGLE_LEAF, 1,8,0, 1.f, true},

    // y=9 layer (diamond pattern)
    {BL_JUNGLE_LEAF, -1,9,0, 1.f, true}, {BL_JUNGLE_LEAF, 0,9,-1, 1.f, true}, {BL_JUNGLE_LEAF, 0,9,0, 1.f, true}, {BL_JUNGLE_LEAF, 0,9,1, 1.f, true}, {BL_JUNGLE_LEAF, 1,9,0, 1.f, true}
};

const structure_t jungleTreeStructure = {
    .numBlocks = STRUCTURE_SIZE(jungleTreePattern),
    .blocks = jungleTreePattern,
};

// second structure for testing purposes
static const structureBlock_t stoneTPattern[] = {
    {BL_STONE, 0, 0, 0, 1.f, false},
    {BL_STONE, 0, 1, 0, 1.f, false},
    {BL_STONE, 0, 2, 0, 1.f, false},
    {BL_STONE, -1, 2, 0, 1.f, false},
    {BL_STONE, 1, 2, 0, 1.f, false},
};

const structure_t stoneTStructure = {
    .numBlocks = STRUCTURE_SIZE(stoneTPattern),
    .blocks = stoneTPattern,
};

static const structureBlock_t woodenHousePattern[] = {
    {5, -1, 0, -2, 1.f, false},
    {5, -1, 0, 2, 1.f, false},
    {5, -1, 1, -2, 1.f, false},
    {5, -1, 1, 2, 1.f, false},
    {5, -1, 2, -2, 1.f, false},
    {5, -1, 2, -1, 1.f, false},
    {5, -1, 2, 1, 1.f, false},
    {5, -1, 2, 2, 1.f, false},
    {5, -1, 3, -1, 1.f, false},
    {5, -1, 3, 0, 1.f, false},
    {5, -1, 3, 1, 1.f, false},
    {5, 0, 0, -2, 1.f, false},
    {5, 0, 0, 2, 1.f, false},
    {5, 0, 2, -2, 1.f, false},
    {5, 0, 2, 2, 1.f, false},
    {5, 0, 3, -1, 1.f, false},
    {5, 0, 3, 0, 1.f, false},
    {5, 0, 3, 1, 1.f, false},
    {5, 1, 0, -2, 1.f, false},
    {5, 1, 0, 2, 1.f, false},
    {5, 1, 1, -2, 1.f, false},
    {5, 1, 1, 2, 1.f, false},
    {5, 1, 2, -2, 1.f, false},
    {5, 1, 2, 2, 1.f, false},
    {5, 1, 3, -1, 1.f, false},
    {5, 1, 3, 0, 1.f, false},
    {5, 1, 3, 1, 1.f, false},
    {5, 2, 0, -1, 1.f, false},
    {5, 2, 0, 0, 1.f, false},
    {5, 2, 0, 1, 1.f, false},
    {5, 2, 1, -1, 1.f, false},
    {5, 2, 1, 1, 1.f, false},
    {5, 2, 2, -1, 1.f, false},
    {5, 2, 2, 0, 1.f, false},
    {5, 2, 2, 1, 1.f, false},
};

const structure_t woodenHouseStructure = {
    .numBlocks = STRUCTURE_SIZE(woodenHousePattern),
    .blocks = woodenHousePattern,
};

static const structureBlock_t iglooPattern[] = {
    {8, -3, 0, -1, 1.f, false},
    {8, -3, 0, 1, 1.f, false},
    {8, -3, 1, -1, 1.f, false},
    {8, -3, 1, 1, 1.f, false},
    {8, -3, 2, 0, 1.f, false},
    {8, -2, 0, -1, 1.f, false},
    {8, -2, 0, 1, 1.f, false},
    {8, -2, 1, -1, 1.f, false},
    {8, -2, 1, 1, 1.f, false},
    {8, -2, 2, 0, 1.f, false},
    {8, -1, 0, -2, 1.f, false},
    {8, -1, 0, 2, 1.f, false},
    {8, -1, 1, -2, 1.f, false},
    {8, -1, 1, 2, 1.f, false},
    {8, -1, 2, -1, 1.f, false},
    {8, -1, 2, 1, 1.f, false},
    {8, -1, 3, 0, 1.f, false},
    {8, 0, 0, -2, 1.f, false},
    {8, 0, 0, 2, 1.f, false},
    {8, 0, 1, -2, 1.f, false},
    {8, 0, 1, 2, 1.f, false},
    {8, 0, 2, -1, 1.f, false},
    {8, 0, 2, 1, 1.f, false},
    {8, 0, 3, 0, 1.f, false},
    {8, 1, 0, -2, 1.f, false},
    {8, 1, 0, 2, 1.f, false},
    {8, 1, 1, -2, 1.f, false},
    {8, 1, 1, 2, 1.f, false},
    {8, 1, 2, -1, 1.f, false},
    {8, 1, 2, 1, 1.f, false},
    {8, 1, 3, 0, 1.f, false},
    {8, 2, 0, -1, 1.f, false},
    {8, 2, 0, 1, 1.f, false},
    {8, 2, 1, -1, 1.f, false},
    {8, 2, 1, 1, 1.f, false},
    {8, 2, 2, 0, 1.f, false},
    {8, 3, 0, 0, 1.f, false},
    {8, 3, 1, 0, 1.f, false},
};

const structure_t iglooStructure = {
    .numBlocks = STRUCTURE_SIZE(iglooPattern),
    .blocks = iglooPattern,
};

structure_t structures[] = {
    treeStructure,
    stoneTStructure,
    woodenHouseStructure,
};

const int numStructures = sizeof(structures)/sizeof(structure_t);

#endif
