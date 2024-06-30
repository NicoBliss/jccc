#pragma once

#include <stdint.h>

typedef struct Node {
    void *value;
    // the key is NULL if the node is empty
    char *key;
} HashNode;

typedef struct {
    int stored;        // # of items in hashtable
    int capacity;      // # of entries that can fit in the table
    HashNode *entries; // collection of entries
} HashMap;

// initialize. returns pointer to the hashMap
HashMap *hm_init(int num_buckets);

// free memory
void hm_free(HashMap *hm);

// find element
void *hm_lookup(HashMap *hm, const char *key);

// add element
int hm_set(HashMap *hm, const char *key, void *value);

// remove element (unimplemented)
void hm_remove(HashMap *hm, const char *key);

// hashes the keys
uint64_t hash(const char *key);

int test_hash_init();

int test_hash_init_and_store();

int test_hash_set_and_get();
