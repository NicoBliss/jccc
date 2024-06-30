#include "hash.h"
#include "out.h"
#include <stdlib.h>
#include <string.h>
#include <testing/tassert.h>
#include <testing/test_utils.h>

const uint64_t OFFSET = 14695981039346656037;
const uint64_t FNV_PRIME = 1099511628211;

HashMap *hm_init(int num_buckets) {
    HashMap *hm = malloc(sizeof(HashMap));

    // bounce if out of memory
    if (hm == NULL) {
        PRINT_WARNING("out of memory");
        return NULL;
    }

    hm->stored = 0;
    hm->capacity = num_buckets;

    hm->entries = calloc(hm->capacity, sizeof(HashNode));

    // bounce if out of memory
    if (hm->entries == NULL) {
        PRINT_WARNING("out of memory, could not allocate space for %d entries",
                      hm->capacity);
        free(hm->entries);
        return NULL;
    }

    return hm;
}

void hm_free(HashMap *hm) {
    for (int i = 0; i < hm->capacity; i++) {
        // free all the chars allocated as keys
        free(hm->entries[i].key);
    }
    free(hm->entries);
    free(hm);
}

// follows FNV-1a hashing
uint64_t hash(const char *key) {
    uint64_t hash = OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (uint64_t)(char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void *hm_lookup(HashMap *hm, const char *key) {
    uint64_t ind = hash(key) % hm->capacity;

    for (int i = 0; i < hm->capacity - 1; i++) {
        if (strcmp(key, hm->entries[ind].key) == 0) {
            return hm->entries[ind].value;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    return NULL;
}

int hm_set(HashMap *hm, const char *key, void *value) {
    uint64_t ind = hash(key) % hm->capacity;

    for (int i = 0; i < hm->capacity - 1; i++) {
        if (hm->entries[ind].key == NULL) {
            hm->entries[ind].value = value;
            hm->stored++;
            return 1;
        }

        ind++;
        if (ind >= hm->capacity) {
            ind = 0;
        }
    }

    return -1;
}

int test_hash_init() {
    testing_func_setup();
    HashMap *h = hm_init(100);

    tassert(h->stored == 0);
    tassert(h->capacity == 100);
}

int test_hash_init_and_store() {
    testing_func_setup();
    HashMap *h = hm_init(100);

    tassert(h->stored == 0);
    tassert(h->capacity == 100);

    char name[100] = "jake";

    char key[10] = "test";
    int ret = hm_set(h, key, name);
    tassert(ret != -1);

    /*uint64_t ind = hash(key) % h->capacity;*/
    /*HashNode hn = h->entries[ind];*/
    /*tassert(strcmp(hn.key, key) == 0);*/

    tassert(h->stored == 1);
    tassert(h->capacity == 100);
}

int test_hash_set_and_get() {
    testing_func_setup();
    HashMap *h = hm_init(100);

    char name[100] = "jake";
    char key[10] = "test";

    int ret = hm_set(h, key, name);
    tassert(ret != -1);

    /*char *got = hm_lookup(h, "test");*/
    /**/
    /*tassert(strcmp(got, "jake") == 0);*/
    /**/
    return 0;
}
