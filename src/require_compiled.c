#include <mruby.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/ruby_compat/load_error.h>
#include <mruby/ruby_compat/require_compiled.h>

#include <string.h>

#include "debug_printf.h"

#define MRUBY_COMPILED_LIBRARIES(mrb) (mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$MRUBY_COMPILED_LIBRARIES")))

mrb_bool
mrb_require_compiled(mrb_state* mrb, const char* const path) {
  mrb_value compiled_libraries;
  mrb_value mrb_path;
  mrb_bool compiled;
  char* extension;

  debug_printf("Extension\n");
  extension = strrchr(path, '.');
  if(extension != NULL && strcmp(extension, ".rb") == 0) {
    debug_printf("Extension: %s\n", extension);
    *extension = '\0';

    compiled = mrb_require_compiled(mrb, path);

    *extension = '.';

    return compiled;
  }

  compiled_libraries = MRUBY_COMPILED_LIBRARIES(mrb);

  if(mrb_nil_p(compiled_libraries)) {
    debug_printf("$MRUBY_COMPILED_LIBRARIES not set (Path: %s)\n", path);
    return FALSE;
  }

  mrb_path = mrb_str_new_cstr(mrb, path);
  mrb_obj_freeze(mrb, mrb_path);

  compiled = mrb_hash_key_p(mrb, compiled_libraries, mrb_path);

  if(compiled == TRUE) {
    debug_printf("Required compiled library (Path: %s)\n", path);
  }

  return compiled;
}

mrb_value
mrb_record_compiled_library(mrb_state* mrb, const char * const path) {
  mrb_value mrb_path;
  mrb_value compiled_libraries;

  debug_printf("Recording compiled library (Path: %s)\n", path);

  mrb_path = mrb_str_new_cstr(mrb, path);
  mrb_obj_freeze(mrb, mrb_path);

  compiled_libraries = MRUBY_COMPILED_LIBRARIES(mrb);

  if(mrb_nil_p(compiled_libraries)) {
    return mrb_nil_value();
  }

  mrb_hash_set(mrb, compiled_libraries, mrb_path, mrb_true_value());

  return mrb_path;
}

void
mrb_ruby_compat_require_compiled_init(mrb_state* mrb) {
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$MRUBY_COMPILED_LIBRARIES"), mrb_hash_new(mrb));

  return;
}

void
mrb_ruby_compat_require_compiled_final(mrb_state* mrb) {
  return;
}
