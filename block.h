#ifndef ST_BLOCK_H
#define ST_BLOCK_H

typedef struct Block {
    enum Texture texture;
    int spriteIndex;
    Box3f box;
} Block;

typedef struct BlockArray {
    int length;
    int allocated;
    Block *data;
}BlockArray;

#endif
