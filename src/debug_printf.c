#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int
debug_printf(const char* const format, ...) {
  va_list args;
  int args_printed;

  const char* const debug_env_var = getenv("MRUBY_RUBY_COMPAT_DEBUG");

  if(debug_env_var != NULL && strncmp(debug_env_var, "on", 3) == 0) {
    va_start(args, format);

    fprintf(stderr, "[mruby-require] ");

    args_printed = vfprintf(stderr, format, args);

    va_end(args);

    return args_printed;
  } else {
    return 0;
  }
}
