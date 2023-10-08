bool createFile(const char *path);
bool appendToFile(const char *path, String *buffer);
void mvFIle(const char *dest, const char *src); String *extractLinef(String *path, int32_t line);
String *extractLine(String *content, int32_t line);
void readFile(const char *path, String *dest);
String *extractTwoPoints(int32_t line);
String *extractTwoPoints_ex(String *path, int32_t line);
String *extractTwoPoints_ex2(String *path);
#ifdef IO_STRING_IMP
bool createFile(const char *path) {
  FILE *fp = fopen(path, "w");
  if (fp != NULL) {
    fclose(fp);
    return true;
  }
  return false;
}
bool appendToFile(const char *path, String *buffer) {
  FILE *fp = fopen(path, "a");
  if (fp != NULL) {
    fprintf(fp, "%s", (char *)c_str(buffer));
    fclose(fp);
    return true;
  }
  return false;
}
void mvFIle(const char *dest, const char *src) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "copy_%s_to_%s", src, dest);
  mHandleIf(nob_copy_file(src, dest), buffer);
  remove(src);
}
String *extractLinef(String *path, int32_t line) {
  FILE *extract = fopen(c_str(path), "r");
  mHandleIf(extract != NULL, "file_extract_null_check");
  String *buffer = allocStr(256);
  String *ret = allocStr(256);
  int32_t cline = 0;
  while ((fgets((char *)buffer->value, buffer->size, extract)) != NULL) {
    if (cline == line) {
      mvStr(ret, buffer);
    }
    ++cline;
  }
  fclose(extract);
  return ret;
}
String *extractLine(String *content, int32_t line) {
  mHandleIf((char *)content->value != NULL, "content_null_check");
  mHandleIf(line != 0, "line_min_check");
  String *buffer = allocStr(256);
  int32_t cline = 0;
  int32_t end = 0;
  int32_t prev_end = 0;
  int32_t start = -1;
  (void)prev_end;
  (void)start;
  mvStr(buffer, content);
  fiterate_str(content) {
    if (cline == line) {
      if (cline > 1) {
        strCut(buffer, prev_end, end);
      } else
        strCut(buffer, start, end);
      break;
    } else if (atStr(content, it) == '\n') {
      prev_end = end;
      end = it;
      ++cline;
      ++start;
    }
  }
  return buffer;
}
void readFile(const char *path, String *dest) {
  FILE *fp = fopen(path, "r");
  if (fp==NULL) {
   fprintf(stderr,"cannot open %s for read\n",path); 
   exit(0);
  }
  fseek(fp, 0, SEEK_END);
  size_t size_f = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  (void)fread((char *)dest->value, dest->size, size_f, fp);
  fclose(fp);
  dest->size = size_f;
}
void copyFromTo(char *src, char *dest, int32_t start, int32_t end) {
  assert(dest != NULL && src != NULL);
  memset(dest, 0, strlen(src));
  memmove(dest, src + start, end);
  size_t len = strlen(dest);
  dest[len + 1] = '\0';
}
String *extractTwoPoints(int32_t line) {
  String *two_points = extractLinef(newStr("./hbuild/config.conf"), line);
  fiterate_str(two_points) {
    if (atStr(two_points, it) == ':' && atStr(two_points, it) != '\n') {
      strCut(two_points, it + 1, two_points->size);
    }
  }
  return two_points;
}
String *extractTwoPoints_ex(String *path, int32_t line) {
  String *two_points = extractLinef(path, line);
  fiterate_str(two_points) {
    if (atStr(two_points, it) == ':' && atStr(two_points, it) != '\n') {
      strCut(two_points, it + 1, two_points->size);
    }
  }
  return two_points;
}
String *extractTwoPoints_ex2(String *path) {
  String *two_points = path;
  fiterate_str(two_points) {
    if (atStr(two_points, it) == ':' && atStr(two_points, it) != '\n') {
      strCut(two_points, it + 1, two_points->size);
    }
  }
  return two_points;
}
#endif // IO_STRING_IMP
