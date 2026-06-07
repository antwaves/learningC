#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "hash_table.h"
#include "prime.c"

static const int HT_PRIME_1 = 139;
static const int HT_PRIME_2 = 229;
static ht_item HT_DELETED_ITEM = {NULL, NULL};
static const int HT_BASE_SIZE = 53;

static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key  = _strdup(k);
    i->value = _strdup(v);
    return i;
}

ht_hash_table* ht_new_sized(int n) {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->base_size = n;
    ht->size = next_prime(ht->base_size);
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}


ht_hash_table* ht_new() {
    return ht_new_sized(HT_BASE_SIZE);
}


static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}


void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}


static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);

    for (int i = 0; i < ht->size; i++) {
        ht_item* cur_item = ht->items[i];
        if (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, cur_item->key, cur_item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // swap attributes
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}


static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}


long hash(const char* k, const int a, const int n_buckets) {
    long hash = 0;
    const int str_len = strlen(k);
    for (int i = 0; i < str_len; i++) {
        hash += (long)(pow(a, str_len - (i + 1))) * (int)k[i];
    }
    hash = hash % n_buckets;
    return hash;
}


static int ht_get_hash(const char* k, const int n_buckets, const int attempt) {
    const int hash_a = hash(k, HT_PRIME_1, n_buckets);
    const int hash_b = hash(k, HT_PRIME_2, n_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % n_buckets;
}


void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }
    
    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;

    while (cur_item != NULL) {
        if (!strcmp(cur_item->key, key)) {
            ht_del_item(cur_item); 
            ht->items[index] = item;
            return;
        }

        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }

    ht->items[index] = item;
    ht->count++;
}


char* ht_search(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* cur_item = ht->items[index];


    int i = 1;
    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        if (!strcmp(cur_item->key, key)) {
            return cur_item->value;
        }

        index = ht_get_hash(cur_item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }
    return NULL;
} 


void ht_delete(ht_hash_table* ht, const char* key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize_down(ht);
    }
   
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    
    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        if (!strcmp(cur_item->key, key)) {
            ht_del_item(cur_item);
            ht->items[index] = &HT_DELETED_ITEM;
            break;
        }

        index = ht_get_hash(cur_item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    } 
    ht->count--;
}
