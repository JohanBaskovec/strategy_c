#ifndef ST_CHARACTER_H
#define ST_CHARACTER_H

typedef struct Character {
    bool alive;
    enum Texture texture;
    int spriteIndex;
    Box3f box;
    Vec3f velocity;
    bool hovered;
    bool selected;

    bool hasTarget;
    Vec3f target;
    bool isMoving;
    Vec3f movingTo;
    Vec3fArray path;
    int pathIndex;
    float movementSpeed;
    float hunger;
} Character;

typedef struct CharacterArray {
    int length;
    int allocated;
    Character *data;
}CharacterArray;
#endif
