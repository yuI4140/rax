#define NOB_IMP
#include "./nob.h"
#define STRING_IMP
#define DEF_BIT
#include "string/string_t.h"
#define witerate() while (true)
#define UNUSED_ARGS(argc, argv)                                                \
  do {                                                                         \
    (void)argc;                                                                \
    (void)argv;                                                                \
  } while (0)
void removeNewlines(char *str) {
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    if (str[i] == '\n') {
      memmove(&str[i], &str[i + 1], len - i);
      len--;
      i--;
    }
  }
}
typedef enum { POSIX, _WIN64_ } Target;
typedef struct {
  Target target;
  String *path;
} Config;
const char *target_str[] = {[POSIX] = "POSIX", [_WIN64_] = "_WIN64_"};
bool detectOsConfig() {
#ifdef WIN32
  return _WIN64_;
#else
  return POSIX;
#endif
}
void handleIf(bool condition, const char *condition_name, int line) {
  if (condition) {
    nob_log(NOB_INFO, "OK=>%s", condition_name);
  } else {
    nob_log(NOB_ERROR, "=>%s fail! At line:%d", condition_name, line);
    exit(0);
  }
}
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
#define mHandleIf(expr, expr_name) (handleIf(expr, expr_name, __LINE__))
#define str_shift_args(argc, argv) newStr(nob_shift_args(argc, argv))
#define v_shift_args(argc, argv) (void)nob_shift_args(argc, argv)
void mvFIle(const char *dest, const char *src) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "copy_%s_to_%s", src, dest);
  mHandleIf(nob_copy_file(src, dest), buffer);
  remove(src);
}
String *extractLine(String *path, int32_t line) {
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
void checkConfigFile(Config conf) {
  bool exists = nob_file_exists(c_str(conf.path)) == 0;
  if (!exists) {
    nob_log(NOB_WARNING, "Config not exists!");
  }
}
Nob_String_Builder *convertStringToStringBuilder(String *str) {
  Nob_String_Builder *ret = allocPtr(sizeof(String));
  ret->capacity = str->size;
  ret->count = str->size;
  ret->items = (char *)c_str(str);
  return ret;
}
void readFile(const char *path, String *dest) {
  FILE *fp = fopen(path, "r");
  fseek(fp, 0, SEEK_END);
  size_t size_f = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  (void)fread((char *)dest->value, dest->size, size_f, fp);
  fclose(fp);
  dest->size = size_f;
}
Config *newConfigExists(const char *path) {
  Config conf = {0};
  nob_log(NOB_INFO, "Reading file...");
  String *content_conf = allocStr(256);
  readFile(path, content_conf);
  String *buffer = extractLine(content_conf, 0);
  fiterate_str(buffer) {
    if (atStr(buffer, it) == ':' && atStr(buffer, it) != '\n') {
      if (atStr(buffer, it + 1) == 'P') {
        conf.target = POSIX;
        return &(Config){.path = conf.path, .target = conf.target};
      } else if (atStr(buffer, it + 1) == 'W') {
        conf.target = _WIN64_;
        return &(Config){.path = conf.path, .target = conf.target};
      }
    }
  }
  return NULL;
}
void appendFileEx(const char *path, const char *text) {
  FILE *fp = fopen(path, "a");
  if (fp == NULL) {
    nob_log(NOB_WARNING, "file cannot be append! => file = NULL");
    exit(1);
  }
  fprintf(fp, "%s", text);
  fclose(fp);
}
Config *createConfig(const char *path) {

  if (nob_file_exists(path) != 0) {
    newConfigExists(path);
  } else {
    Config conf = {0};
    String *buffer = allocStr(256);
    nob_log(NOB_INFO, "Creating config file");
    bool create_conf = createFile(path);
    mHandleIf(create_conf, "create_config");
    nob_log(NOB_INFO, "OS detected:%s", target_str[detectOsConfig()]);
    str_snprintf(buffer, "platform:%s\n", target_str[detectOsConfig()]);
    bool append_conf = appendToFile(path, buffer);
    mHandleIf(append_conf, "append_created_config");
    conf.path = newStr(path);
    conf.target = detectOsConfig();
    return &(Config){.path = conf.path, .target = conf.target};
  }
  return NULL;
}
Config *initConfig(const char *path) {
  mHandleIf(nob_mkdir_if_not_exists("./hbuild"), "dir_build");
  if (nob_file_exists(path) != 0) {
    newConfigExists(path);
  } else {
    Config conf = {0};
    String *buffer = allocStr(256);
    nob_log(NOB_INFO, "Creating config file");
    bool create_conf = createFile(path);
    mHandleIf(create_conf, "create_config");
    nob_log(NOB_INFO, "OS detected:%s", target_str[detectOsConfig()]);
    str_snprintf(buffer, "platform:%s\n", target_str[detectOsConfig()]);
    bool append_conf = appendToFile(path, buffer);
    mHandleIf(append_conf, "append_created_config");
    conf.path = newStr(path);
    conf.target = detectOsConfig();
    return &(Config){.path = conf.path, .target = conf.target};
  }
  return NULL;
}
Config *startConfig(String *path) {
  Config *conf = createConfig(c_str(path));
  if (conf != NULL) {
    nob_log(NOB_INFO, "Config created succesfully!");
    return conf;
  } else {
    nob_log(NOB_ERROR, "Config fail to be createte");
    exit(1);
  }
}
String *extractTwoPoints(int32_t line) {
  String *two_points = extractLine(newStr("./hbuild/config.conf"), line);
  fiterate_str(two_points) {
    if (atStr(two_points, it) == ':' && atStr(two_points, it) != '\n') {
      strCut(two_points, it + 1, two_points->size);
    }
  }
  return two_points;
}
String *extractTwoPoints_ex(String *path, int32_t line) {
  String *two_points = extractLine(path, line);
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
size_t findCharStr(String *str, char ch) {
  fiterate_str(str) {
    if (atStr(str, it) == ch) {
      return it;
      break;
    }
  }
  return 0;
}
String *getExecutable() {
  String *proj_name_line = extractLine(newStr("./hbuild/build.conf"), 1);
  size_t two_p = findCharStr(proj_name_line, ':');
  String *proj_name = allocStr(256);
  mvStr(proj_name, proj_name_line);
  strCut(proj_name, two_p + 1, two_p + 2);
  String *exe_path = allocStr(256);
  str_snprintf(exe_path, "./hbuild/%s", c_str(proj_name));
  removeNewlines((char *)exe_path->value);
  implicitStr(exe_path);
  return exe_path;
}
void buildProject() {
#ifdef WIN32
#define COMPILER "gcc"
#else
#define COMPILER "clang"
#endif
  nob_log(NOB_INFO, "building project...");
  Nob_Cmd cmd = {0};
  String *exe_path = getExecutable();
  nob_cmd_append(&cmd, COMPILER, "-Wall", "-Wextra", "-Wpedantic", "-o",
                 (char *)exe_path->value, "./src/main.c");
  nob_cmd_run_sync(cmd);
  nob_log(NOB_INFO, "project build successfully!");
}
bool subcommands(int argc, char *argv[]) {
  String *config_path = newStr("./hbuild/build.conf");
  if (argc <= 1) {
    nob_log(NOB_ERROR, "No args given!\n Use -h for more help");
    exit(0);
  }
  v_shift_args(&argc, &argv);
  String *cmd = str_shift_args(&argc, &argv);
  if (strEq(cmd, newStr("-h"))) {
    nob_log(NOB_INFO, "subcommands:");
    nob_log(NOB_INFO, "-h \n for help menu ");
    nob_log(NOB_INFO,
            "load \n for read | create a config file for a non-project");
    nob_log(NOB_INFO, "create [project_name] \n for create a proyect");
    nob_log(NOB_INFO, "run \n run the proyect");
    nob_log(NOB_INFO, "build \n build the proyect");
    nob_log(
        NOB_INFO,
        "crun -d/-r\n run a file without project\n d=debug | r=release flags");
  } else if (strEq(cmd, newStr("load"))) {
    nob_log(NOB_INFO, "Loading config...");
    (void)initConfig(c_str(config_path));
  } else if (strEq(cmd, newStr("create"))) {
    const char *main_path = "./src/main.c";
    String *project_name = str_shift_args(&argc, &argv);
    nob_log(NOB_INFO, "building project..");
    mHandleIf(nob_mkdir_if_not_exists("./src"), "dir_src");
    mHandleIf(nob_mkdir_if_not_exists("./hbuild"), "dir_build");
    mHandleIf(nob_mkdir_if_not_exists("./lib"), "dir_lib");
    mHandleIf(nob_mkdir_if_not_exists("./headers"), "dir_headers");
    String *main_text =
        newStr("#include<stdio.h>\n"
               "int main(void){\nprintf(\"Hello world!%c\",10);\n}");
    char buffer[256];
    mHandleIf(appendToFile(main_path, main_text), "create_append_main_file");
    snprintf(buffer, sizeof(buffer), "project_name:%s\n", c_str(project_name));
    (void)startConfig(config_path);
    mHandleIf(appendToFile(c_str(config_path), newStr(buffer)),
              "append_name_to_config");
    nob_log(NOB_INFO, "project created successfully!");
  } else if (strEq(cmd, newStr("build"))) {
    buildProject();
  } else if (strEq(cmd, newStr("run"))) {
    Nob_Cmd cmd = {0};
    if (nob_file_exists(c_str(config_path)) == 0) {
      nob_log(NOB_WARNING, "config not initialize!");
      exit(0);
    }
    String *exe_path = getExecutable();
    if (!nob_file_exists(c_str(exe_path)) == 0) {
      buildProject();
    }
    nob_cmd_append(&cmd, (char *)exe_path->value);
    nob_cmd_run_sync(cmd);
    exit(0);
  }
  return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) {
    subcommands(argc, argv); 
}
