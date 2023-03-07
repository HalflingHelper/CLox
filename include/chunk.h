#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
} OpCode;


// Dynamic Array to hold line numbers using Run-Length Encoding
typedef struct {
    int count;
    int capacity;
    int* lines;
    uint8_t* frequency;
} LineTracker;

void initLineTracker(LineTracker* tracker);
void freeLineTracker(LineTracker* tracker);
void addLine(LineTracker* tracker, int line);

// The chunk struct
typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    LineTracker lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int index);

#endif
