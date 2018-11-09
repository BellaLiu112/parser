//
//  main.cpp
//  parser
//
//  Created by Chen Liu on 2018/11/8.
//  Copyright © 2018 Chen Liu. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <stdio.h>

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


void on_field_found(csv_field_t *fields, int count)
{
    //handle field
    assert(count == 2);
    
}

void on_parse_line(char *start, long length)
{
    char *ptr = start;
    csv_field_t field[32] = {0};
    int field_index = 0;
    while (*ptr) {
        switch (*ptr) {
            case ',':
                field[field_index].data = start;
                field[field_index++].length = (int)(ptr - start);
                *ptr = 0;
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
        field[field_index].data = start;
        field[field_index++].length = (int)(ptr - start);
    }
    
    on_field_found(field, field_index);
}

int32_t parse_csv(char *content, long length)
{
    char *start, *ptr, *end;
    
    start = ptr = content;
    end = start + length;
    
    while(*ptr) {
        if (*ptr == '\n') {
            *ptr = 0;
            on_parse_line(start, ptr - start);
            start = ptr + 1;
        }
        ptr++;
    }
    
    if (start < ptr) {
        on_parse_line(start, ptr - start);
    }
    return 0;
}

int main(int argc, const char * argv[]) {
   
    //const char *attr_file = "/Users/chenliu/Desktop/globalegrow/ner_goods_attribute/preprocess/data/mix_fixed_1108.csv";
    const char *attr_file = "/Users/chenliu/Desktop/test.txt";
    long attr_file_size;
    char *attr_content = read_file(attr_file, &attr_file_size);
    parse_csv(attr_content, attr_file_size);
    free(attr_content);
    
    return 0;
}
