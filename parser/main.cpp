//
//  main.cpp
//  parser
//
//  Created by Chen Liu on 2018/11/8.
//  Copyright © 2018 Chen Liu. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hashtable.h"

typedef int (*on_csv_field_pt)(void *opaque, int row, int column, char *data, int len);
void parse_csv_line(void *opaque, int row, char *data, long length, on_csv_field_pt cb);

static char *read_file(const char *path, long *length)
{
    FILE *fp;
    long  file_size;
    char *content = NULL;
    int   rt = -1;
    
    do {
        fp = fopen(path, "rb");
        if (!fp) return NULL;
        
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        content = (char *)malloc(file_size + 1);
        if (!content) {
            break;
        }
        *(content + file_size) = 0;
        
        if (file_size != fread(content, 1, file_size, fp)) {
            break;
        }
        
        rt = 0;
    } while(0);

    fclose(fp);
    if (rt) {
        free(content);
        content = NULL;
        *length = 0;
    } else {
        *length = file_size;
    }
    
    return content;
}

typedef struct csv_field_s {
    char *data;
    int   length;
} csv_field_t;

void on_second_field_parse(void *opaque, int row, int column, char *data, int len)
{
  csv_field_t *f = (csv_field_t *)opaque;
  printf("%s\n", data);
}

void on_csv_field_found(void *opaque, int row, int column, char *data, int len)
{
  static csv_field_t first_fields[32] = {0};
  csv_field_t *field = &first_fields[row & 31];
  if (!column) {
    assert(!field->data);
    field->data = data;
    field->length = len;
  } else {
    assert(field->data);
    parse_csv_line(field, 0, data, len, (on_csv_field_pt)on_second_field_parse);
    field->data = NULL;
  }
}

void parse_csv_line(void *opaque, int row, char *start, long length, on_csv_field_pt cb)
{
    char *ptr = start;
    int field_index = 0;
    int len;
    while (*ptr) {
        switch (*ptr) {
            case ',':
              len = (int)(ptr - start);
              if (*start == '"' && *(ptr - 1) == '"') {
                  start++;
                  *(ptr - 1) = 0;
                  len -= 2;

              }
              *ptr = 0;
              cb(opaque, row, field_index++, start, len);
              start = ptr + 1;
              break;
            case '"':
                ptr++;
                while (*ptr) {
                    if (*ptr == '\\' && *(ptr + 1) == '"') {
                        ptr++;
                        continue;
                    }
                    
                    if (*ptr == '"') {
                        break;
                    }
                    ptr++;
                }
                break;
        }
        ptr++;
    }
    
    if (start < ptr) {
      len = (int)(ptr - start);
      if (*start == '"' && *(ptr - 1) == '"') {
        start++;
        *(ptr - 1) = 0;
        len -= 2;
      }
      cb(opaque, row, field_index++, start, len);
    }
}

int parse_csv(char *content, long length)
{
    char *start, *ptr, *end;
    int row = 0;
    
    start = ptr = content;
    end = start + length;
    
    while(*ptr) {
        if (*ptr == '\n') {
            *ptr = 0;
            parse_csv_line(NULL, row++, start, ptr - start, (on_csv_field_pt)on_csv_field_found);
            start = ptr + 1;
        }
        ptr++;
    }
    
    if (start < ptr) {
      parse_csv_line(NULL, row++, start, ptr - start, (on_csv_field_pt)on_csv_field_found);
    }
    return 0;
}

int main(int argc, const char * argv[]) {
   
    //const char *attr_file = "/Users/chenliu/Desktop/globalegrow/ner_goods_attribute/preprocess/data/mix_fixed_1108.csv";
    //const char *attr_file = "/Users/chenliu/Desktop/test.txt";
    //const char *attr_file = "I:/parser/data/mix_fixed_1108.csv";
    //long attr_file_size;
    //char *attr_content = read_file(attr_file, &attr_file_size);
    //parse_csv(attr_content, attr_file_size);
    //free(attr_content);

    hash_table_size_t size = get_hash_table_size(5);
    hash_table_t *hashtable = hash_table_new(size, (key_equal_func_pt)int_equal, NULL, NULL, (hash_func_pt)int_hash);
    int key = 1;
    int value = 2;
    hash_table_insert(hashtable, (void *)key, (void *)value);
    key = 2;
    value = 3;
    hash_table_insert(hashtable, (void *)key, (void *)value);
    key = 18;
    value = 30;
    hash_table_insert(hashtable, (void *)key, (void *)value);
    key = 4;
    value = 5;
    hash_table_insert(hashtable, (void *)key, (void *)value);

    char buf[1024];
    hash_table_dump_keys(hashtable, int_hash_key_format, buf, sizeof(buf));
    int ret = (int)hash_table_find(hashtable, (void *)key);
    ret = (int)hash_table_find(hashtable, (void *)18);
    hash_table_remove(hashtable, (void *)key);
    hash_table_remove(hashtable, (void *)18);
    hash_table_free(&hashtable);
    
    return 0;
}
