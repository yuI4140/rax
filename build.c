#include <stdio.h>
#include <string.h>
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
typedef enum { TARGET_POSIX, TARGET_WIN64_MINGW, COUNT_TARGETS } Target;
typedef struct {
  Target target;
  String *path;
} Config;
const char *target_str[] = {
    [TARGET_POSIX] = "POSIX", [TARGET_WIN64_MINGW] = "WIN64_MINGW"};
void handleIf(bool condition, const char *condition_name, int line) {
  if (condition) {
    nob_log(NOB_INFO, "OK=>%s", condition_name);
  } else {
    nob_log(NOB_ERROR, "=>%s fail! At line:%d", condition_name, line);
    exit(0);
  }
}
bool createFile(const char *path) {
  FILE *fp = fopen(path, "a");
  if (fp != NULL) {
    fclose(fp);
    return true;
  }
  fclose(fp);
  return false;
}
bool appendToFile(const char *path, String *buffer) {
  FILE *fp = fopen(path, "a");
  if (fp != NULL) {
    fprintf(fp, "%s", c_str(buffer));
    fclose(fp);
    return true;
  }
  fclose(fp);
  return false;
}
#define mHandleIf(expr, expr_name) (handleIf(expr, expr_name, __LINE__))
#define str_shift_args(argc, argv) newStr(nob_shift_args(argc, argv))
void mvFIle(const char *dest, const char *src) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "copy_%s_to_%s", src, dest);
  mHandleIf(nob_copy_file(src, dest), buffer);
  remove(src);
}
String *extractLine(String *path, int32_t line) {
  char buffer[256];
  String *str_line = allocStr(256);
  FILE *config = fopen("./hbuild/config.conf", "r");
  mHandleIf(config != NULL, "handle_path_of_config_file");
  int32_t cline = 0;
  while ((fgets(buffer, sizeof(buffer) - 1, config)) != NULL) {
    if (cline == line) {
      mvCharpToStr(str_line, buffer);
    }
    ++cline;
  }
  fclose(config);
  return str_line;
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
  size_t bytes_r = fread((char *)dest->value, dest->size, size_f, fp);
  fclose(fp);
  dest->size = size_f;
}
bool newConfigExists(Config *conf) {
  nob_log(NOB_INFO, "Reading file...");
  String *buffer=extractLine(conf->path,0);
  fiterate_str(buffer) {
    if (atStr(buffer, it) == ':' && atStr(buffer, it) != '\n') {
      if (atStr(buffer, it + 1) == 'P') {
        conf->target = TARGET_POSIX;
        return true;
      } else if (atStr(buffer, it + 1) == 'W') {
        conf->target = TARGET_WIN64_MINGW;
        return true;
      }
    }
  }
  return false;
}
bool newConfig(Config *conf) {
  bool config_not_exists = nob_file_exists(c_str(conf->path)) == 0;
  if (conf->path == NULL)
    nob_log(NOB_ERROR, "There is no path for config file");
  if (config_not_exists) {
#ifdef WIN32
    conf->target = TARGET_WIN64_MINGW;
#else
    conf->target = TARGET_POSIX;
#endif
    FILE *config = fopen(c_str(conf->path), "w");
    ptrWrapper(config);
    fprintf(config, "target:%s\n", target_str[conf->target]);
    fclose(config);
    return true;
  } else if (!config_not_exists) {
    return newConfigExists(conf);
  }
  return false;
}
void startConfig(Config conf) {
  bool ok = newConfig(&conf);
  if (ok) {
    nob_log(NOB_INFO, "OK=>Config\n");
  } else {
    nob_log(NOB_ERROR, "couldn't create Config\n");
  }
}
String *extractTwoPoints(int32_t line) {
  String * two_points= extractLine(newStr("../hbuild/config.conf"),line);
  fiterate_str(two_points) {
    if (atStr(two_points, it) == ':' && atStr(two_points, it) != '\n') {
      strCut(two_points,it+1,two_points->size);
    }
  }
  return two_points;
}
String *extractTwoPoints_ex(String *path) {
  String * two_points= path;
  fiterate_str(two_points) {
    if (atStr(two_points, it) == ':' && atStr(two_points, it) != '\n') {
      strCut(two_points,it+1,two_points->size);
    }
  }
  return two_points;
}
String *getExecutable(Config *conf) {
  String *project_name = extractTwoPoints(1);
  String *exe_path = allocStr(256);
  if (conf->target == TARGET_WIN64_MINGW) {
    str_snprintf(exe_path, "./hbuild/%s.exe", c_str(project_name));
  } else {
    str_snprintf(exe_path, "./hbuild/%s", c_str(project_name));
  }
  return exe_path;
}
void buildProject() {
  nob_log(NOB_INFO, "building project...");
  Nob_Cmd cmd = {0};
  Config conf = {0};
  newConfigExists(&conf);
  String *exe_path = getExecutable(&conf);
  nob_cmd_append(&cmd, "clang", "-Wall", "-Wextra", "-Wpedantic", "-o",
                 c_str(exe_path), "./src/main.c");
  nob_cmd_run_sync(cmd);
  nob_log(NOB_INFO, "project build successfully!");
}
bool subcommands(int argc, char *argv[]) {
  Nob_Cmd command = {0};
  if (argc <= 1) {
    nob_log(NOB_ERROR, "No args given!\n Use -h for more help");
    exit(0);
  }
  String *program = str_shift_args(&argc, &argv);
  String *cmd = str_shift_args(&argc, &argv);
  if (strEq(cmd, newStr("-h"))) {
    nob_log(NOB_INFO, "subcommands:");
    nob_log(NOB_INFO, "-h \n for help menu ");
    nob_log(NOB_INFO, "-load_config \n for read | create a config file ");
    nob_log(NOB_INFO, "create [project_name] \n for create a proyect");
    nob_log(NOB_INFO, "run \n run the proyect");
    nob_log(NOB_INFO, "build \n build the proyect");
    nob_log(NOB_INFO, "crun -d/-r\n run a file without project\n d=debug | r=release flags");
  }
  if (strEq(cmd, newStr("-load_config"))) {
    Config conf = {0};
    startConfig(conf);
  }
  if (strEq(cmd, newStr("create"))) { 
    Config conf = {0};
    String *project_name = str_shift_args(&argc, &argv);
    nob_log(NOB_INFO, "building proyect..");
    mHandleIf(nob_mkdir_if_not_exists("./src"), "dir_src");
    mHandleIf(nob_mkdir_if_not_exists("./hbuild"), "dir_build");
    mHandleIf(nob_mkdir_if_not_exists("./lib"), "dir_lib");
    mHandleIf(nob_mkdir_if_not_exists("./headers"), "dir_headers");
    mHandleIf(createFile("./src/main.c"), "create_main_file");
    String *main_file =
        newStr("#include<stdio.h>\n"
               "int main(void){\nprintf(\"Hello world!%c\",10);\n}");
    mHandleIf(appendToFile("./src/main.c", main_file), "append_main_file");
    conf.path = newStr("./hbuild/config.conf");
    startConfig(conf);
    FILE *config = fopen("./hbuild/config.conf", "a");
    fprintf(config, "project_name:%s\n", c_str(project_name));
    fclose(config);
    nob_log(NOB_INFO, "project created successfully!");
  }
  if (strEq(cmd, newStr("build"))) {
    buildProject();
  }
  if (strEq(cmd, newStr("run"))) {
    Nob_Cmd cmd = {0};
    Config conf = {0};
    newConfig(&conf);
    String *exe_path = getExecutable(&conf);
    if (!nob_file_exists(c_str(exe_path))) {
      nob_log(NOB_INFO, "executable not found");
      buildProject();
    }
    nob_cmd_append(&cmd, c_str(exe_path));
    nob_cmd_run_async(cmd);
    exit(0);
  }
  return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) { 
    subcommands(argc,argv);
}
