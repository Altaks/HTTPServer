#include "hashmap.h"

struct hashmap* new_map(int capacity) {
    struct hashmap* map = (struct hashmap*)malloc(sizeof(struct hashmap));
    map->capacity = capacity;
    map->data = (struct map_item*)malloc(sizeof(struct map_item) * map->capacity);
    memset(map->data, 0, sizeof(struct map_item) * capacity);
    return map;
}

__attribute__((unused)) void del_map(struct hashmap* map) {
    for(int i = 0; i < map->capacity; i++) {
        struct map_item* crnt = &map->data[i];
        while(crnt->next != NULL) {
            struct map_item* newcrnt = crnt->next;
            free(crnt);
            crnt = newcrnt;
        }
        free(crnt);
    }
    free(map->data);
    free(map);
}

struct map_item* map_set(struct hashmap* map, char* new_key, int new_val) {
    struct map_item* map_ind = &map->data[djb2(map, new_key)];

    while(map_ind->key != NULL && strcmp(map_ind->key, new_key) != 0) {
        if(map_ind->next != NULL) {
            map_ind = map_ind->next;
        } else {
            map_ind->next = (struct map_item*)malloc(sizeof(struct map_item));
            map_ind = map_ind->next;
            break;
        }
    }

    *map_ind = (struct map_item) {.key = new_key, .val = new_val};
    return map_ind;
}

int map_get(struct hashmap* map, char* key) {
    struct map_item* map_ind = &map->data[djb2(map, key)];
    while(map_ind->next != NULL && map_ind->key != NULL && strcmp(map_ind->key, key) != 0) {
        map_ind = map_ind->next;
    }

    return map_ind->val;
}

__attribute__((unused)) void map_rem(struct hashmap* map, char* key) {
    struct map_item* map_ind = &map->data[djb2(map, key)];
    struct map_item* prev_map_ind;
    bool isBase = true;
    while(map_ind->next != NULL && map_ind->key != NULL && strcmp(map_ind->key, key) != 0) {
        prev_map_ind = map_ind;
        isBase = false;
        map_ind = map_ind->next;
    }

    if(map_ind->next == NULL) {
        memset(map_ind, 0, sizeof(struct map_item));
        free(map_ind);
        if(!isBase) {
            prev_map_ind->next = NULL;
        }
    } else {
        struct map_item* copied = map_ind->next;
        map_ind->key = copied->key;
        map_ind->val = copied->val;
        if(copied->next != NULL)
            map_ind->next = copied->next;
        memset(copied, 0, sizeof(struct map_item));
        free(copied);
    }

}

int djb2(struct hashmap* map, const char* key) {
    int hash = 5381;
    int i = 0;
    while(key[i] != '\0') {
        hash = ((hash << 5) + hash) + key[i];
        i++;
    }
    return hash % map->capacity;
}