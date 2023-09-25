#include <stdio.h>
#include <string.h>
#define NOB_IMP
#include "./nob.h"
#define STRING_IMP
#define DEF_BIT
#include "string/string_t.h"
#define witerate() while (true)
void test(int argc,char *argv[]);
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
  String *buffer = allocStr(1024);
  readFile(c_str(conf->path), buffer);
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
String *extractProjectName(){
    char buffer[256];
    String *project_name = allocStr(256);
    FILE *config = fopen("./hbuild/config.conf", "r");
    mHandleIf(config != NULL, "handle_path_of_config_file");
    int32_t line = 0;
    while ((fgets(buffer, sizeof(buffer) - 1, config)) != NULL) {
      if (line == 1) {
          mvCharpToStr(project_name,buffer);
      }
      ++line;
    }
    fclose(config);
    int32_t pline=0;
    fiterate_str(project_name) {
      if (atStr(project_name, it) == ':' && buffer[it] != '\n') {
          strCut(project_name,13,14);
      }
    } 
    return project_name;
}
String *getExecutable(){
    String *project_name=extractProjectName();
    String *exe_path=allocStr(256);
    str_snprintf(exe_path,"./hbuild/%s",c_str(project_name)); 
    return exe_path;
}
bool diff(const char* path,const char*path2){
    String *buffer=allocStr(256);
    String *buffer2=allocStr(256);
    readFile(path,buffer);
    readFile(path2,buffer2);
    if (strEq(buffer,buffer2)) {
        return false; 
    }
    return true;
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
  }
  if (strEq(cmd, newStr("-load_config"))) {
    Config conf = {0};
    startConfig(conf);
  }
  if (strEq(cmd, newStr("dev_test"))) {
      test(argc,argv);
  }
  if (strEq(cmd, newStr("create"))) {
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
    Config conf = {0};
    conf.path = newStr("./hbuild/config.conf");
    startConfig(conf);
    FILE *config = fopen("./hbuild/config.conf", "a");
    fprintf(config, "project_name:%s\n", c_str(project_name));
    fclose(config);
    nob_log(NOB_INFO,"project created successfully!");
  }
  if (strEq(cmd, newStr("build"))) {
    nob_log(NOB_INFO, "building project...");
    Nob_Cmd cmd = {0}; 
    String *exe_path=getExecutable();
    nob_cmd_append(&cmd,"clang","-Wall","-Wextra","-Wpedantic","-o",c_str(exe_path),"./src/main.c");
    nob_cmd_run_sync(cmd); 
    nob_log(NOB_INFO,"project build successfully!");
  }
  if (strEq(cmd, newStr("run"))) {
     Nob_Cmd cmd={0}; 
     String *exe_path=getExecutable();
     if (!nob_file_exists(c_str(exe_path))) {
        nob_log(NOB_INFO,"executable not found");
        nob_log(NOB_INFO, "building project...");
        Nob_Cmd scmd = {0}; 
       String *exe_path=getExecutable();
       nob_cmd_append(&scmd,"clang","-Wall","-Wextra","-Wpedantic","-o",c_str(exe_path),"./src/main.c");
       nob_cmd_run_sync(scmd); 
       nob_log(NOB_INFO,"project build successfully!"); 
     } 
     nob_cmd_append(&cmd,c_str(exe_path));
     nob_cmd_run_async(cmd);
     exit(0);
  }   
    return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) {
    subcommands(argc,argv);
}
void test(int argc,char *argv[]) {
   String *f1=str_shift_args(&argc,&argv); 
   String *f2=str_shift_args(&argc,&argv); 
   if (!diff(c_str(f1),c_str(f2))) {
        printf("files are not equal!\n");  
   }
        printf("files are equal!\n");  
        exit(0);
}
