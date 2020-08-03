#include <mruby.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"

int debug_printf(const char* const format, ...) {
  va_list args;
  int args_printed;

  if(debug_mode()) {
    va_start(args, format);

    fprintf(stderr, "[mruby-local] ");

    args_printed = vfprintf(stderr, format, args);

    va_end(args);

    return args_printed;
  } else {
    return 0;
  }
}

mrb_bool debug_mode(void) {
  const char* const debug = getenv("MRUBY_LOCAL_DEBUG");

  if(debug != NULL && strncmp(debug, "on", 3) == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
