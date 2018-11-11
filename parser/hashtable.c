#include "hashtable.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct hash_table_node_s {
  struct hash_table_node_s *next;
  void *value;
  void *key;
} hash_table_node_t;

uint32_t string_hash(const char *v) {
  register const char *p;
  register uint32_t h = 5381;

  for (p = v; *p ; p++)
    h = (h << 5) + h + *p;

  return h;
}

uint32_t int_hash(const int i) {
  return i;
}

int32_t string_equal(const char *s1, const char *s2)
{
  return !strcmp(s1, s2);
}

int32_t int_equal(const int i1, const int i2)
{
  return i1 == i2;
}


//static uint32_t hash(char *str)
//{
//  register uint32_t h;
//  register uint8_t *p;
//
//  for (h = 0, p = (uint8_t *)str; *p; p++)
//    h = 31 * h + *p;
//
//  return h;
//}

hash_table_size_t get_hash_table_size(int size)
{
  hash_table_size_t s;
  s.power = (int)ceil(log2(size));
  return s;
}

hash_table_t *hash_table_new(hash_table_size_t size, 
                             key_equal_func_pt key_equal_func, 
                             free_pt key_free_func, 
                             free_pt value_free_func, 
                             hash_func_pt hash_func
)
{
  hash_table_t *table;
  assert(size.power && key_equal_func && hash_func);
  if (!size.power || !key_equal_func || !hash_func) return NULL;
  table = (hash_table_t *)malloc(sizeof(hash_table_t));
  if (!table) return NULL;
  memset(table, 0, sizeof(hash_table_t));
  size_t buckets_size;
  table->key_equal_func = key_equal_func;
  table->hash_func = hash_func;
  table->key_free_func = key_free_func;
  table->value_free_func = value_free_func;
  table->capacity = 1 << size.power;
  table->mask = table->capacity - 1;
  buckets_size = table->capacity * sizeof(hash_table_node_t);
  table->buckets = (hash_table_node_t **)malloc(buckets_size);
  memset(table->buckets, 0, buckets_size);
  return table;
}

void hash_table_free(hash_table_t **t)
{
  hash_table_t *table = *t;
  int i;
  *t = NULL;
  for (i = 0; i < table->capacity; ++i) {
      hash_table_node_t *node = table->buckets[i];
      while (node) {
        hash_table_node_t *tmp = node;
        node = tmp->next;
        if (table->value_free_func && tmp->value) 
          table->value_free_func(tmp->value);
        if (table->key_free_func && tmp->key)
          table->key_free_func(tmp->key);
        free(tmp);
      }
  }

  free(table->buckets);
  free(table);
}

void *hash_table_find(hash_table_t *t, const void *key)
{
  uint32_t hash, index;
  hash_table_node_t *node;
  if (!t || !key) return NULL;
  hash = t->hash_func(key);
  index = hash & t->mask;
  node = t->buckets[index];
  if (!node) return NULL;
  while (node) {
    if (t->key_equal_func(node->key, key)) {
       return node->value;
    }
    node = node->next;
  }
  return NULL;
}

int32_t hash_table_insert(hash_table_t *t, void *key, void *value)
{ 
  uint32_t hash, index;
  hash_table_node_t *node, **n;

  if (!t || !key) return -1;
  hash = t->hash_func(key);
  index = hash & t->mask;
  n = &t->buckets[index];
  node = t->buckets[index];
  while (node) {
    if (t->key_equal_func(node->key, key)) {
      if (t->value_free_func && node->value)
        t->value_free_func(node->value);
      node->value = value;
      return 0;
    }
    n = &node->next;
    node = node->next;
  }

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  node->value = value;
  node->key = key;
  node->next = NULL;
  *n = node;
  return 0;
}

int32_t hash_table_remove(hash_table_t *t, const void *key)
{
  uint32_t hash, index;
  hash_table_node_t *node, **pre;

  if (!t || !key) return -1;
  hash = t->hash_func(key);
  index = hash & t->mask;

  pre = &t->buckets[index];
  node = t->buckets[index];
  while (node) {
    if (t->key_equal_func(node->key, key)) {
      break;
    }
    pre = &node->next;
    node = node->next;
  }

  if (node) {
    *pre = node->next;
    if (t->value_free_func && node->value) 
       t->value_free_func(node->value);
    if (t->key_free_func && node->key)
       t->key_free_func(node->key);
    free(node);
    return 0;
  }

  return -1;
}

uint32_t hash_table_capacity(hash_table_t *t)
{
  if (!t) return 0;
  return t->capacity;
}

uint32_t hash_table_size(hash_table_t *t)
{
  if (!t) return 0;
  return t->size;
}

uint32_t string_hash_key_format(char *buf, uint32_t remain_len, const void *key)
{
  return snprintf(buf, remain_len, "->[%s]", key);
}

uint32_t int_hash_key_format(char *buf, uint32_t remain_len, const void *key)
{
  return snprintf(buf, remain_len, "->[%d]", key);
}

uint32_t hash_table_dump_keys(hash_table_t *t, hash_key_format key_format, char *outputbuf, uint32_t maxsize)
{
  uint32_t idx;
  hash_table_node_t *node;
  char *ptr = outputbuf;
  int32_t remain_len = maxsize, len;
  for (idx = 0; idx < t->capacity && remain_len; ++idx) {
    node = t->buckets[idx];
    len = snprintf(ptr, remain_len, "(%d)", idx);
    remain_len -=len;
    ptr += len;
    while (node) {
      if (remain_len <= 0) break;
      len = key_format(ptr, remain_len, node->key);
      remain_len -=len;
      ptr += len;
      node = node->next;
    }

    if (remain_len > 0) {
      *ptr = '\n';
       ptr++;
       remain_len--;
    }
  }

  if (remain_len > 0) {
    *ptr = '\0';
     remain_len--;
  }

  return maxsize - remain_len;
}