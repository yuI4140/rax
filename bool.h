#include <stdio.h>
#include <stdlib.h>
struct bl {
  char value;
};
#ifdef BOOL_IMP
typedef struct bl bool;
bool newBool(char v) {
  if (v > 1 || v < 0) {
    printf("ERROR: passed value is not bool --➤  NULL \n");
    exit(1);
  }
  bool ret = {v};
  return ret;
}
bool checkBool(bool *bl) {
  if (bl->value > 1 || bl->value < 0) {
    printf("ERROR: passed value is not bool --➤  NULL\n");
    exit(1);
  }
  bool newBl = {bl->value};
  return newBl;
}
#endif
