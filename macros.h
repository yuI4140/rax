#define witerate() while (true)
void handleIf(bool condition, const char *condition_name, int line);
#define mHandleIf(expr, expr_name) (handleIf(expr, expr_name, __LINE__))
#define str_shift_args(argc, argv) newStr(nob_shift_args(argc, argv))
#define v_shift_args(argc, argv) (void)nob_shift_args(argc, argv)
void *mHandleIfReturn_s(void *expr,int32_t line,char* file);
#define mHandleIfReturn(expr) (mHandleIfReturn_s(expr,__LINE__,__FILE__))
#define UNUSED_ARGS(argc, argv)                                                \
  do {                                                                         \
    (void)argc;                                                                \
    (void)argv;                                                                \
} while (0)
void handleIf(bool condition, const char *condition_name, int line) {
  if (condition) {
    nob_log(NOB_INFO, "OK=>%s", condition_name);
  } else {
    nob_log(NOB_ERROR, "=>%s fail! At line:%d", condition_name, line);
    exit(0);
  }
}
void *mHandleIfRetrn_s(void *expr,int32_t line,char* file){
    if (expr==NULL) {
        nob_log(NOB_ERROR,"expr was NULL pointer | L:%d | FILE: %s",line,file);
        exit(1);
    } 
    return expr;
}
