#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Represents a hashmap pair with ptr to next pair in case of collision
 */
struct map_item {
    char* key;
    int val;
    struct map_item* next;
};

/**
 * Represents a hashmap with a ptr to the data allocated memory and the capacity
 */
struct hashmap {
    struct map_item* data;
    int capacity;
};

/**
 * Creates a new hashmap with the given capacity
 * @param capacity the capacity of the hashmap
 * @return the new hashmap
 */
struct hashmap* new_map(int capacity);

/**
 * Deletes the given hashmap
 * @param map the hashmap to delete
 */
__attribute__((unused)) void del_map(struct hashmap* map);

/**
 * Sets the value of the given key in the given hashmap
 * @param map the hashmap to set the value in
 * @param new_key the key to set the value of
 * @param new_val the value to set
 * @return the map item corresponding to the key
 */
struct map_item* map_set(struct hashmap* map, char* new_key, int new_val);

/**
 * Gets the value of the given key in the given hashmap
 * @param map the hashmap to get the value from
 * @param key the key to get the value of
 * @return the value corresponding to the key
 */
int map_get(struct hashmap* map, char* key);

/**
 * Removes the given key from the given hashmap
 * @param map the hashmap to remove the key from
 * @param key the key to remove
 * @return the value corresponding to the key
 */
__attribute__((unused)) void map_rem(struct hashmap* map, char* key);

/**
 * Hash function for the hashmap
 * @param map the hashmap to hash the key for
 * @param key the key to hash
 * @return the hash of the key
 */
int djb2(struct hashmap* map, const char* key);

#endif