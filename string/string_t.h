/*MIT License

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
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct String {
  size_t size;
  char value[];
} String;
void *ptrWrapper_s(void *ptr, int32_t line, const char *file);
#define ptrWrapper(ptr) (ptrWrapper_s(ptr, __LINE__, __FILE__))
// a memalloc but 'safe'
void *allocPtr(size_t size);
#define STR_MALLOC allocPtr
String *allocStr(size_t size);
void clearStr(String *str);
char atStr(String *str, size_t index);
void mvStr(String *dest, String *src);
void implicitStr(String *str);
String *newStr(const char arr[]);
// compare two sets of strings
bool strEq(String *a, String *b);
// is the same as strsub of C++
void strCut(String *src, uint32_t start, uint32_t end);
const char *c_str(String *str);
void strConcat(String *dest, String *src);
void pushCharStr(String *dest, char ch);
void pushCharpStr(String *dest, const char *chp);
void popStr(String *src);
void popCountStr(String *src, uint32_t count);
String *intToString(int num);
String *floatToString(float num);
#define fiterate_str(str) for (size_t it = 0; it < str->size; ++it)
#define witerate_str(str)                                                      \
  size_t it = 0;                                                               \
  while (str->value[it] != '\0')
void str_snprintf(String *buffer, const char *fmt, ...);
void replaceCharStr(String *str, char old, char _new);
#define MV_CHAR(dest, src)                                                     \
  do {                                                                         \
    if (dest != src) {                                                         \
      memcpy(&dest, &src, sizeof(char));                                       \
      src = 0;                                                                 \
    }                                                                          \
  } while (0)
#define CP_CHAR(dest, src)                                                     \
  do {                                                                         \
    if (dest != src) {                                                         \
      memcpy(&dest, &src, sizeof(char));                                       \
    }                                                                          \
  } while (0)
size_t findCharStr(String *str, char ch);
void rmNewLineStr(String *str);
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#include <errno.h>
#include <math.h>
int32_t strToInt(String *str);
float strToFloatPointer(String *str);
#define HANDLER_CONVER_ERR(result)                                             \
  do {                                                                         \
    if ((errno == ERANGE &&                                                    \
         ((result) == HUGE_VAL || (result) == -HUGE_VAL)) ||                   \
        (errno != 0 && (result) == 0)) {                                       \
      perror("Conversion error");                                              \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)
#define NEW_STR(x)                                                             \
  _Generic((x),                                                                \
      int: intToString(x),                                                     \
      float: floatToString(x),                                                 \
      double: floatToString(x),                                                \
      default: NULL)
#endif
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
String *allocStr(size_t size) {
  String *str = (String *)STR_MALLOC(sizeof(String) + size);
  str->size = size;
  return str;
}
void clearStr(String *str) {
  str->size = 0;
  memset(str->value, 0, str->size);
}
char atStr(String *str, size_t index) {
  if (index >= str->size) {
    fprintf(stderr, "Index out of bounds\n");
    exit(EXIT_FAILURE);
  }
  return str->value[index];
}
void mvStr(String *dest, String *src) {
  clearStr(dest);
  dest->size = src->size;
  memmove(dest->value, src->value, src->size);
}
void mvCharpToStr(String *dest, const char *src) {
  clearStr(dest);
  dest->size = strlen(src);
  memmove(dest->value, src, strlen(src) - 1);
  implicitStr(dest);
}
void implicitStr(String *str) {
  size_t lengh = strlen(str->value);
  str->size = lengh;
}
void strConcat(String *dest, String *src) {
  memmove((char *)dest->value + dest->size, src->value, src->size);
  implicitStr(dest);
}
String *newStr(const char arr[]) {
  size_t size = strlen(arr);
  String *str = allocStr(size);
  memcpy(str->value, arr, size);
  return str;
}
bool strEq(String *a, String *b) {
  return memcmp(a->value, b->value, a->size) != 0 ? false : true;
}
void strCut(String *src, uint32_t start, uint32_t end) {
  ptrWrapper(src);
  assert(start < end && end <= src->size);
  memmove(src->value, (char *)src->value + start, end);
  implicitStr(src);
}

const char *c_str(String *str) {
  str->value[str->size] = '\0';
  return str->value;
}
void pushCharStr(String *dest, char ch) {
  memcpy((char *)dest->value, &ch, sizeof(char));
  implicitStr(dest);
}
void pushCharpStr(String *dest, const char *chp) {
  memcpy((char *)dest->value + dest->size, chp, strlen(chp));
  implicitStr(dest);
}
void popStr(String *src) {
  src->value[src->size - 1] = '\0';
  implicitStr(src);
}
void popCountStr(String *src, uint32_t count) {
  while (count != 0) {
    popStr(src);
    --count;
  }
}

void handlerResultConvertErr(String *str, String *endptr) {
  if (strEq(str, endptr)) {
    fprintf(stderr, "No digits found.\n");
    exit(EXIT_FAILURE);
  }
}
int32_t strToInt(String *str) {
  String *handler = allocStr(256);
  int32_t buffer = strtol(str->value, (char **)handler->value, 10);
  HANDLER_CONVER_ERR(buffer);
  handlerResultConvertErr(str, handler);
  return buffer;
}
float strToFloatPointer(String *str) {
  String *endptr = allocStr(256);
  double float_num = strtod(str->value, (char **)endptr->value);
  HANDLER_CONVER_ERR(float_num);
  handlerResultConvertErr(str, endptr);
  return float_num;
}
String *intToString(int num) {
  String *str = allocStr(sizeof(int32_t));
  snprintf((char *)str->value, str->size, "%d", num);
  implicitStr(str);
  return str;
}
String *floatToString(float num) {
  String *str = allocStr(sizeof(double));
  snprintf(str->value, str->size, "%.2f", num);
  implicitStr(str);
  return str;
}
void str_snprintf(String *buffer, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)vsnprintf((char *)buffer->value, buffer->size, fmt, args);
  va_end(args);
}
void replaceCharStr(String *str, char old, char _new) {
  fiterate_str(str) {
    if (atStr(str, it) == old) {
      MV_CHAR(str->value[it], _new);
    }
  }
}
size_t findCharStr(String *str, char ch) {
  fiterate_str(str) {
    if (atStr(str, it) == ch) {
      return it;
      break;
    }
  }
  return 0;
}
void rmNewLineStr(String *str) {
  fiterate_str(str) {
    if (atStr(str, it) == '\n') {
      char p_str = atStr(str, it + 1);
      memmove(&str[it], &p_str, str->size - it);
      --str->size;
      --it;
    }
  }
}

#endif // END OF STRING_IMP
