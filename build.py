import os
os.system("mkdir build bin src lib headers third-party")
with open("src/main.c", "w") as file:
    file.write("#include <stdio.h>\n\nint main() {\n")
    file.write("\tprintf(\"Hello, World!\\n\");\n")
    file.write("\treturn 0;\n}")
defines="""
#pragma once
// Unsigned int types.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long _u64;
// Signed int types.
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef signed long _s64;
// Regular int types.
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef long _i64;
// Floating point types
typedef float f32;
typedef double f64;
// Boolean types
typedef u8 b8;
typedef u32 b32;
// stirng types
// char*
typedef i8 *str;
// const char*
typedef const str cstr;
// void
typedef void vd;
// void Func
typedef void vFn(void);
// void Func pointer
typedef void (*vFnp)(void);
#define true 1
#define false 0
#define null 0
#define WCHAR_MAX 0xffff
#define nullptr ((void *)0)
#define UNUSED(var) ((void)(var))
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#ifdef DEF_ARRAY
#define Slice_Prototype(type)                                                  \
  typedef struct type##_slice {                                                \
    type *elems;                                                               \
    u32 len;                                                                   \
  } type##_slice;
#define slice(type) type##_slice
#define Iterate(array, var) for (int var = 0; var < array.len; var++)
#define IteratePtr(array, var) for (int var = 0; var < array->len; var++)
// SizeofArray macro to get the number of elements in an array
#define SizeofArray(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif /*DEF_ARRAY*/
#ifdef DEF_STR
// Stringification macros
#define Stringify(x) #x
#define StringifyMacro(x) Stringify(x)
#endif /*DEF_STR*/
#ifdef DEF_DIR
#include <dirent.h>
#include <stdio.h>

// DIr pointer
typedef DIR *Drp;
// file pointer
typedef FILE *Fsp;
// dir entity pointer
typedef struct dirent *Drep;
#endif
#ifdef DEF_MATH
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define RoundUp(value, alignment) (((value) + (alignment)-1) & ~((alignment)-1))
// RoundDown macro to round a value down to the nearest multiple of a specified
// alignment
#define RoundDown(value, alignment) ((value) & ~((alignment)-1))
#define u32_max 4294967295
#define PATH_MAX 4096
#define Clamp(a, x, b) (((x) < (a)) ? (a) : ((b) < (x)) ? (b) : (x))
#define ClampTop(a, b) Min(a, b)
#define ClampBot(a, b) Max(a, b)
#define ReverseClamp(a, x, b) (((x) < (a)) ? (b) : ((b) < (x)) ? (a) : (x))
#define Wrap(a, x, b) ReverseClamp(a, x, b)
#define Gigabytes(count) (u64)(count * 1024 * 1024 * 1024)
#define Megabytes(count) (u64)(count * 1024 * 1024)
#define Kilobytes(count) (u64)(count * 1024)
#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Max(a, b) (((a) > (b)) ? (a) : (b))
// Alignment macros
#define AlignDown(x, alignment) ((x) & ~((alignment)-1))
#define AlignUp(x, alignment) (((x) + (alignment)-1) & ~((alignment)-1))
// only it support numerical types
#define Swap(a, b)                                                             \
  do {                                                                         \
    a ^= b;                                                                    \
    b ^= a;                                                                    \
    a ^= b;                                                                    \
  } while (0)
// OffsetOf macro to get the offset of a member in a struct
#define OffsetOf(type, member) ((size_t)(&((type *)0)->member))
// ContainerOf macro to get the pointer to the parent struct from a member
#define ContainerOf(ptr, type, member)                                         \
  ((type *)((char *)(ptr)-OffsetOf(type, member)))
#define MemoryCopy(d, s, z) memmove((d), (s), (z))
#define MemoryCopyStruct(d, s)                                                 \
  MemoryCopy((d), (s), Min(sizeof(*(d)), sizeof(*(s))))
#define MemoryZero(d, z) memset((d), 0, (z))
#define MemoryZeroStruct(d, s) MemoryZero((d), sizeof(s))
#endif /*DEF_MATH*/

#ifdef DEF_MEM
#define Sawp_s(a, b, size)                                                     \
  do {                                                                         \
    if ((a) != NULL && (b) != NULL && (a) != (b)) {                            \
      if ((size) > 0) {                                                        \
        union {                                                                \
          char c[size];                                                        \
          short s[size / sizeof(short)];                                       \
          int i[size / sizeof(int)];                                           \
          long l[size / sizeof(long)];                                         \
          float f[size / sizeof(float)];                                       \
          double d[size / sizeof(double)];                                     \
          char *cp[size / sizeof(char *)];                                     \
          const char *ccp[size / sizeof(const char *)];                        \
          wchar_t wc[size / sizeof(wchar_t)];                                  \
          unsigned long long ull[size / sizeof(unsigned long long)];           \
          /* Add additional data types as needed */                            \
        } u;                                                                   \
        memcpy(&u, (a), (size));                                               \
        memcpy((a), (b), (size));                                              \
        memcpy((b), &u, (size));                                               \
      }                                                                        \
    }                                                                          \
  } while (0)
#include "Err.c"
#define defmemcpy(dest, src, n)                                                \
  do {                                                                         \
    CHECK_MSG(dest == nullptr || src == NULL, "Null pointer detected");        \
    CHECK_MSG(n == 0, "Invalid argument");                                     \
    memcpy(dest, src, n);                                                      \
  } while (0)

#define defmemmove(dest, src, n)                                               \
  do {                                                                         \
    CHECK_MSG(dest == nullptr || src == NULL, "Null pointer detected");        \
    CHECK_MSG(n == 0, "Invalid argument");                                     \
    memmove(dest, src, n);                                                     \
  } while (0)

#define defmemset(ptr, value, n)                                               \
  do {                                                                         \
    CHECK_MSG(ptr == NULL, "Null pointer detected");                           \
    CHECK_MSG(n == 0, "Invalid argument");                                     \
    memset(ptr, value, n);                                                     \
  } while (0)
#endif /*DEF_MEM*/

#ifdef DEF_BIT
#define XOR(a, b) ((a) ^ (b))
#define AND(a, b) ((a) & (b))
#define OR(a, b) ((a) | (b))
#define NOT(x) (~(x))
#define BitSet(x, n) ((x) |= (1ULL << (n)))
#define BitClear(x, n) ((x) &= ~(1ULL << (n)))
#define BitToggle(x, n) ((x) ^= (1ULL << (n)))
#define BitCheck(x, n) ((x) & (1ULL << (n)))
// Byte order macros
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&             \
    (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) &&              \
    (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif /* DEF_BIT*/
"""
win = """
gcc -Wall -Wextra -Werror -I./headers -o ./bin/main ./src/main.c
"""
helper="""
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void printc(int endl, const char *fmt);
char *format(const char *format, ...);
char *readFile(const char *filename);
void writeFile(const char *filename, const char *format, ...);
#ifdef HLP_IMP
char *readFile(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Failed to open file: %s\n", filename);
    return NULL;
  }
  // Determine the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  // Allocate memory for the file contents
  char *buffer = (char *)malloc((file_size + 1) * sizeof(char));
  if (buffer == NULL) {
    fclose(file);
    printf("Failed to allocate memory for file contents.\n");
    return NULL;
  }
  // Read the file contents into the buffer
  size_t result = fread(buffer, sizeof(char), file_size, file);
  if (result != file_size) {
    fclose(file);
    free(buffer);
    printf("Failed to read file: %s\n", filename);
    return NULL;
  }
  buffer[file_size] = '\0'; // Null-terminate the buffer
  fclose(file);
  return buffer;
}
void writeFile(const char *filename, const char *format, ...) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Failed to open file: %s\n", filename);
    return;
  }
  va_list args;
  va_start(args, format);

  vfprintf(file, format, args);

  va_end(args);

  fclose(file);
}
char *format(const char *format, ...) {
  va_list args;
  va_start(args, format);
  // Determine the length of the formatted string
  va_list args_copy;
  va_copy(args_copy, args);
  int len = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);
  // Allocate memory for the formatted string
  char *buffer = (char *)malloc((len + 1) * sizeof(char));
  // Format the string
  vsprintf(buffer, format, args);
  va_end(args);
  return buffer;
}
void printc(int endl, const char *fmt) {
  int i = 0;
  while (fmt[i] != '\0') {
    putchar(fmt[i]);
    i++;
  }
  if (endl == 1) {
    putchar('\n');
  }
}
#endif
"""
linux = """    
gcc -Wall -Wextra -Werror -I./headers -o ./bin/main ./src/main.c
x86_64-w64-mingw32-gcc -Wall -Wextra -Werror -I./headers -o ./bin/main ./src/main.c
"""

with open("build/build.sh", "w") as sh_file:
    sh_file.write(linux)
with open("headers/helper.h", "w") as file:
    file.write(helper)
with open("headers/defines.h", "w") as file:
    file.write(defines)
with open("build/build.bat", "w") as bat_file:
    bat_file.write(win)
