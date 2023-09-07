/*
MIT License

Copyright (c) 2023 yuI4140

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
typedef struct String {
  size_t size;
  size_t cap;
  char value[];
} String;
void *ptrWrapper_s(void *ptr, int32_t line, const char *file);
#define ptrWrapper(ptr) (ptrWrapper_s(ptr, __LINE__, __FILE__))
// does a malloc but 'safe'
void *allocPtr(size_t size);
String *allocString(size_t size);
String *newString(const char string[]);
// do a string for a force move or copy to string->value
String *implicitString(String *str);
void appendString(String *str, const char string[]);
// actually you could do string->value and get the value from that, but
// I'm lazy so I made a func for do that and set a nul term to the end of the array
const char *c_str(String *str);
int stringCmp(String *str1, String *str2);
void clearString(String *str);
char atString(String *str, size_t index);
void putcharString(String *str, char ch);
void mvStrings(String *dest,String *src);
void mvCharpToString(String *dest,const char*src);
#ifdef STRING_IMP
void *ptrWrapper_s(void *ptr, int32_t line, const char *file) {
  if (ptr == NULL) {
    fprintf(stderr, "%s => ptr == NULL at Line:%d\n", file, line);
    exit(EXIT_FAILURE);
  }
  return ptr;
}
void *allocPtr(size_t size) {
  void *ptr = malloc(size);
  return ptrWrapper(ptr);
}
String *allocString(size_t size) {
  String *str = (String *)allocPtr(sizeof(String) + size);
  str->size = size;
  str->cap = size;
  return str;
}
String *newString(const char string[]) {
  size_t length = strlen(string);
  String *new = allocString(length);
  for (size_t i = 0; i < length; ++i) {
    new->value[i] = string[i]; 
  }
  return new;
}
String *implicitString(String *str){
  size_t length = strlen(str->value);
  String *imp=allocString(length);
  return imp;
}
void appendString(String *str, const char string[]) {
    size_t length = strlen(string);
  if (str->size + length > str->cap) {
    size_t new_cap = str->cap * 2;
    while (str->size + length > new_cap) {
      new_cap *= 2;
    }
    str = (String *)realloc(str, sizeof(String) + new_cap);
    str->cap = new_cap;
  }
  for (size_t i = 0; i < length; ++i) {
    str->value[str->size + i] = string[i];
  }
  str->size += length;
}
const char *c_str(String *str) {
    str->value[str->size+1]='\0';
    return str->value;
}
int stringCmp(String *str1, String *str2) {
  size_t min_size = (str1->size < str2->size) ? str1->size : str2->size;
  int cmp_result = strncmp(str1->value, str2->value, min_size);
  if (cmp_result == 0) {
    if (str1->size < str2->size) {
      return -1;
    } else if (str1->size > str2->size) {
      return 1;
    } else {
      return 0;
    }
  }
  return cmp_result;
}
void clearString(String *str) {
    str->size=0;
    str->cap=0; 
    memset(str->value, 0, str->size);
}
char atString(String *str, size_t index) {
  if (index >= str->size) {
    fprintf(stderr, "Index out of bounds\n");
    exit(EXIT_FAILURE);
  }
  return str->value[index];
}
void putcharString(String *str, char ch) {
    if (str->size + 1 > str->cap) {
        size_t new_cap = str->cap * 2;
        while (str->size + 1 > new_cap) {
            new_cap *= 2;
        }
        str = (String *)realloc(str, sizeof(String) + new_cap);
        str->cap = new_cap;
    }
    str->value[str->size] = ch;
    str->size++;
}
void mvStrings(String *dest,String *src){
    clearString(dest);
    dest->size=src->size; 
    memmove(dest->value,src->value,src->size);
    dest->cap=src->cap;
}
void mvCharpToString(String *dest,const char*src){
    clearString(dest);
    dest->size=strlen(src); 
    memmove(dest->value,src,strlen(src)-1);
    dest->cap=strlen(src)-1;
}
#endif // STRING_IMP
