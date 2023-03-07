#include <stdlib.h>
#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initLineTracker(&chunk->lines);
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeLineTracker(&chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    addLine(&chunk->lines, line);
    chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1; //Return the index where the constant was appended
}

/*
    Line Tracker Stuff
*/
void initLineTracker(LineTracker* tracker) {
    tracker->count = 0;
    tracker->capacity = 0;
    tracker->lines = NULL;
    tracker->frequency = NULL;
}

void freeLineTracker(LineTracker* tracker) {
    FREE_ARRAY(int, tracker->lines, tracker->capacity);
    FREE_ARRAY(uint8_t, tracker->frequency, tracker->capacity);
}

// Itsy bitsy issue is that we might add space when we really just want to add to the curretn last value
// Also need to make sure that frequency doesn't overflow
void addLine(LineTracker* tracker, int line) {
    // See if we need to add a new line
    if (tracker->capacity < tracker->count + 1) {
        int oldCapacity = tracker->capacity;
        tracker->capacity = GROW_CAPACITY(oldCapacity);
        tracker->lines = GROW_ARRAY(int, tracker->lines, oldCapacity, tracker->capacity);
        tracker->frequency = GROW_ARRAY(uint8_t, tracker->frequency, oldCapacity, tracker->capacity);
    }

    // The line is the same as a repeat line
    if (line == tracker->lines[tracker->count-1] && tracker->frequency[tracker->count - 1] < 255) {
        tracker->frequency[tracker->count-1]++;
        return;
    }

    tracker->lines[tracker->count] = line;
    tracker->frequency[tracker->count] = 1;

    tracker->count++;
}

/*
    Given the Index of an Instruction, determines the line where this instruction occurs
    This is very inneficient!
    TODO: Need to fully test this method, I'm not sure if always accurate
*/
int getLine(Chunk* chunk, int index) {
    // Making sure the index is within bounds
    if (index < 0 || index >= chunk->count) {
        fprintf(stderr, "Index invalid.\n");
        return -1;
    }

    int line_index = 0;
    while (index > 0) {
        index -= chunk->lines.frequency[line_index];
        line_index++;

    }

    //We exactly use up
    if (index == 0)
        return chunk->lines.lines[line_index];
    
    //We go over
    return chunk->lines.lines[line_index-1];
}
