#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct map_item {
    char* key;
    int val;
    struct map_item* next;
};

struct hashmap {
    struct map_item* data;
    int capacity;
};

struct hashmap* new_map(int capacity);
void del_map(struct hashmap* map);

struct map_item* map_set(struct hashmap* map, char* new_key, int new_val);
int map_get(struct hashmap* map, char* key);
int map_rem(struct hashmap* map, char* key);

int djb2(struct hashmap* map, char* key);

#endif