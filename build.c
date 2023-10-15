#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOB_IMP
#include "../nob.h"
#define STRING_IMP
#include "../string/string_t.h"
#include "macros.h"
#define IO_STRING_IMP
#include "./io_str.h"
typedef enum { LINUX, _WIN64_ } Target;
typedef struct {
  Target target;
  String *path;
} Config;
#define CHARP_STR(str_value) ((char *)str_value)
const char *target_str[] = {[LINUX] = "LINUX", [_WIN64_] = "_WIN64_"};
static inline bool detectOsConfig(void) {
#ifdef WIN32
  return _WIN64_;
#else
  return LINUX;
#endif
}
Nob_String_Builder *convertStringToStringBuilder(String *str) {
  Nob_String_Builder *ret = allocPtr(sizeof(String));
  ret->capacity = str->size;
  ret->count = str->size;
  ret->items = (char *)c_str(str);
  return ret;
}
void testStr(String *str) {
  printf("%s\n", c_str(str));
  exit(0);
}
String *trackCharAndCut(String *text, int32_t line) {
  int32_t temp_l = -1;
  String *track = allocStr(256);
  testStr(text);
  fiterate_str(text) {
    pushCharStr(track, atStr(text, it));
    if (atStr(text, it) == '\n') {
      ++temp_l;
      if (temp_l == line) {
        return track;
        break;
      } else {
        clearStr(track);
        continue;
      }
    }
  }
  return NULL;
}
void cutLineStr(String *text, size_t dest[2], int32_t line) {
  int32_t cline = -1, start = 0, end = 0;
  fiterate_str(text) {
    if (cline == line) {
      break;
    } else if (atStr(text, it) == '\n') {
      if (line == 0) {
        end = it;
      } else {
        start = end;
        end = it;
      }
      ++cline;
    }
  }
  dest[0] = start;
  dest[1] = end;
}
void cutFromLineStr(String *text, int32_t desired_line) {
  size_t dest[2] = {0};
  cutLineStr(text, dest, desired_line);
  int32_t dif = text->size - dest[1];
  popCountStr(text, dif);
  strCut(text, dest[0], dest[1]);
  if (atStr(text, 0) == '\n') {
    char *buffer = allocPtr(256);
    copyFromTo((char *)text->value, buffer, 1, text->size);
    buffer[strlen(buffer) + 1] = '\0';
    clearStr(text);
    memmove((char *)text->value, buffer, strlen(buffer));
    implicitStr(text);
    popStr(text);
  }
}
String *findInConfig(const char *file_path, String *target) {
  nob_log(NOB_INFO, "finding %s in Config file...", c_str(target));
  String *ftext = allocStr(256);
  readFile(file_path, ftext);
  size_t cline = 0;
  while (cline < ftext->size) {
    String *cftext = allocStr(ftext->size);
    mvStr(cftext, ftext);
    cutFromLineStr(cftext, cline);
    String **tokens = tokStr(cftext, ":");
    if (tokens[1] != NULL && strEq(tokens[0], target)) {
      return tokens[1];
    }
    ++cline;
    clearStr(cftext);
  }

  nob_log(NOB_WARNING, "Target %s not found in the config file.",
          c_str(target));
  return NULL;
}
Config *newConfigExists(const char *path) {
  Config conf = {0};
  nob_log(NOB_INFO, "Reading file...");
  String *target = ptrWrapper(findInConfig(path, newStr("platform")));
  String *_path = newStr(path);
  conf.path = _path;
  if (strEq(target, newStr("LINUX"))) {
    conf.target = LINUX;
  } else {
    conf.target = _WIN64_;
  }
  return &(Config){.target = conf.target, .path = conf.path};
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
String *getExecutable(void) {
  String *proj_name =
      findInConfig("./hbuild/build.conf", newStr("project_name"));
  if (proj_name == NULL) {
    nob_log(NOB_ERROR, "proj_name couldn't be found in Config file");
    exit(0);
  }
  String *exe_path = allocStr(256);
  str_snprintf(exe_path, "./hbuild/%s", c_str(proj_name));
  rmNewLineStr(exe_path);
  implicitStr(exe_path);
  return exe_path;
}
void nonBuildProject(String *mode) {
  nob_log(NOB_INFO, "%s mode is building..", c_str(mode));
  nob_log(NOB_INFO, "building project...");
  String *build_path = newStr("./hbuild/build.conf");
  String *compiler = findInConfig(c_str(build_path), newStr("compiler"));
  if (compiler == NULL) {
    nob_log(NOB_WARNING, "The default compiler was set to gcc");
    nob_log(NOB_INFO, "for know how works config see hbuild/example");
    appendToFile(c_str(build_path), newStr("compiler:gcc\n"));
    compiler = findInConfig(c_str(build_path), newStr("compiler"));
  }
  String *build_mode = findInConfig(c_str(build_path), mode);
  String *linker_op = findInConfig(c_str(build_path), newStr("link"));
  String *include_op = findInConfig(c_str(build_path), newStr("include"));
  if (build_mode == NULL) {
    nob_log(NOB_WARNING, "%s not found in Config file", c_str(mode));
    nob_log(NOB_WARNING, "Autogenerated Debug mode in Config file");
    String *default_debug = newStr("debug:-Wall -Wextra -Wpedantic\n");
    appendToFile(c_str(build_path), default_debug);
    build_mode = findInConfig(c_str(build_path), newStr("debug"));
  }
  Nob_Cmd cmd = {0};
  String *exe_path = findInConfig(c_str(build_path), newStr("exe_path"));
  String *src_file = findInConfig(c_str(build_path), newStr("src_file"));
  if (exe_path == NULL) {
    nob_log(NOB_ERROR, "Add exe_path label in Config file!");
    exit(0);
  }
  if (src_file == NULL) {
    nob_log(NOB_ERROR, "Add src_file label in Config file!");
    exit(0);
  }
  nob_cmd_append(&cmd, c_str(compiler));
  if (build_mode != NULL) {
    String **build_flags = tokStr(build_mode, " ");
    for (int i = 0; build_flags[i] != NULL; ++i) {
      nob_cmd_append(&cmd, c_str(build_flags[i]));
    }
  }
  if (linker_op != NULL)
    nob_cmd_append(&cmd, c_str(linker_op));
  if (include_op != NULL)
    nob_cmd_append(&cmd, c_str(include_op));
  nob_cmd_append(&cmd, "-o");
  nob_cmd_append(&cmd, (char *)exe_path->value);
  nob_cmd_append(&cmd, c_str(src_file));

  nob_cmd_run_sync(cmd);
  nob_log(NOB_INFO, "project build successfully!");
}

void buildProject(String *mode) {
  nob_log(NOB_INFO, "%s mode is building..", c_str(mode));
  nob_log(NOB_INFO, "building project...");
  String *build_path = newStr("./hbuild/build.conf");
  String *compiler = findInConfig(c_str(build_path), newStr("compiler"));
  String *project_name =
      findInConfig(c_str(build_path), newStr("project_name"));
  if (compiler == NULL) {
    nob_log(NOB_WARNING, "The default compiler was set to gcc");
    nob_log(NOB_INFO, "for know how works config see hbuild/example");
    appendToFile(c_str(build_path), newStr("compiler:gcc\n"));
    compiler = findInConfig(c_str(build_path), newStr("compiler"));
  }
  String *build_mode = findInConfig(c_str(build_path), mode);
  String *linker_op = findInConfig(c_str(build_path), newStr("link"));
  String *include_op = findInConfig(c_str(build_path), newStr("include"));
  if (build_mode == NULL) {
    nob_log(NOB_WARNING, "%s not found in Config file", c_str(mode));
    nob_log(NOB_WARNING, "Autogenerated Debug mode in Config file");
    String *default_debug = newStr("debug:-Wall -Wextra -Wpedantic\n");
    appendToFile(c_str(build_path), default_debug);
    build_mode = findInConfig(c_str(build_path), newStr("debug"));
  }
  if (include_op == NULL) {
    nob_log(NOB_INFO, "not found include options");
    nob_log(NOB_INFO, "setting default include folder...");
    appendToFile(c_str(build_path), newStr("include:-I./headers\n"));
    include_op = findInConfig(c_str(build_path), newStr("include"));
  }
  Nob_Cmd cmd = {0};
  String *exe_path = getExecutable();
  nob_cmd_append(&cmd, c_str(compiler)); // Compiler name as the first argument
  if (build_mode != NULL) {
    String **build_flags = tokStr(build_mode, " ");
    for (int i = 0; build_flags[i] != NULL; ++i) {
      nob_cmd_append(&cmd, c_str(build_flags[i]));
    }
  }
  String *main_path = allocStr(256);
  str_snprintf(main_path,"./src/%s.c",c_str(project_name));
  if (linker_op != NULL)
    nob_cmd_append(&cmd, c_str(linker_op));
  nob_cmd_append(&cmd, c_str(include_op));
  nob_cmd_append(&cmd, "-o");
  nob_cmd_append(&cmd, (char *)exe_path->value);
  nob_cmd_append(&cmd, (char *)main_path->value);

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
    nob_log(NOB_INFO, "build [mode]\n build the proyect");
    nob_log(NOB_INFO, "nbuild [mode]\n build for non-proyect");
  } else if (strEq(cmd, newStr("load"))) {
    nob_log(NOB_INFO, "Loading config...");
    (void)initConfig(c_str(config_path));
  } else if (strEq(cmd, newStr("create"))) {
    String *project_name = str_shift_args(&argc, &argv);
    nob_log(NOB_INFO, "building project..");
    mHandleIf(nob_mkdir_if_not_exists("./src"), "dir_src");
    mHandleIf(nob_mkdir_if_not_exists("./hbuild"), "dir_build");
    mHandleIf(nob_mkdir_if_not_exists("./lib"), "dir_lib");
    mHandleIf(nob_mkdir_if_not_exists("./headers"), "dir_headers");
    String *main_text =
        newStr("#include<stdio.h>\n"
               "int main(void){\nprintf(\"Hello world!%c\",10);\n}\n");
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "project_name:%s\n", c_str(project_name));
    String *main_path = allocStr(256);
    str_snprintf(main_path, "./src/%s.c", c_str(project_name));
    mHandleIf(appendToFile((char *)main_path->value, main_text),
              "create_append_main_file");
    (void)startConfig(config_path);
    mHandleIf(appendToFile(c_str(config_path), newStr(buffer)),
              "append_name_to_config");
    nob_log(NOB_INFO, "project created successfully!");
  } else if (strEq(cmd, newStr("build"))) {
    String *mode = str_shift_args(&argc, &argv);
    buildProject(mode);
  } else if (strEq(cmd, newStr("nbuild"))) {
    String *mode = str_shift_args(&argc, &argv);
    nonBuildProject(mode);
  } else if (strEq(cmd, newStr("run"))) {
    Nob_Cmd cmd = {0};
    if (nob_file_exists(c_str(config_path)) == 0) {
      nob_log(NOB_WARNING, "config not initialize!");
      exit(0);
    }
    String *exe_path = getExecutable();
    if (!nob_file_exists((char *)exe_path->value)) {
      nob_log(NOB_WARNING, "not found executable begining to build...");
      buildProject(newStr("debug"));
    }
    nob_cmd_append(&cmd, (char *)exe_path->value);
    nob_cmd_run_sync(cmd);
    exit(0);
  }
  return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) { subcommands(argc, argv); }
