#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t (*key_equal_func_pt)(const void *data1, const void *data2);
typedef uint32_t (*hash_func_pt)(const void *data);
typedef void (*free_pt)(void *data);
typedef uint32_t (*hash_key_format)(char *buf, uint32_t remain_len, const void *key);

typedef struct hash_table_size_s {
  int power;
} hash_table_size_t;

typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s {
  hash_table_node_t **buckets;
  uint32_t            capacity;
  uint32_t            size;
  int                 mask;
  key_equal_func_pt   key_equal_func;
  hash_func_pt        hash_func;
  free_pt             key_free_func;
  free_pt             value_free_func;
} hash_table_t;

hash_table_size_t get_hash_table_size(int size);

hash_table_t *hash_table_new(hash_table_size_t size,
                             key_equal_func_pt key_equal_func,
                             free_pt key_free_func,
                             free_pt value_free_func,
                             hash_func_pt hash_func);

void hash_table_free(hash_table_t **t);
void *hash_table_find(hash_table_t *t, const void *key);
int32_t hash_table_insert(hash_table_t *t, void *key, void *value);
int32_t hash_table_remove(hash_table_t *t, const void *key);
uint32_t hash_table_capacity(hash_table_t *t);
uint32_t hash_table_size(hash_table_t *t);
uint32_t hash_table_dump_keys(hash_table_t *t, hash_key_format key_format, char *outputbuf, uint32_t maxsize);

uint32_t string_hash(const char *str);
uint32_t int_hash(const int i);
int32_t  string_equal(const char *s1, const char *s2);
int32_t  int_equal(const int i1, const int i2);
uint32_t string_hash_key_format(char *buf, uint32_t reamin_len, const void *key);
uint32_t int_hash_key_format(char *buf, uint32_t reamin_len, const void *key);

#ifdef __cplusplus
}
#endif

#endif
