#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/require/load_error.h>

#include "debug_printf.h"

mrb_noreturn void
mrb_raise_load_error(mrb_state* mrb, const char* const path) {
  mrb_value message;
  mrb_value mrb_path;
  mrb_value load_error;

  message = mrb_str_new_cstr(mrb, "cannot load such file");
  mrb_str_cat_lit(mrb, message, " -- ");
  mrb_str_cat_cstr(mrb, message, path);
  mrb_obj_freeze(mrb, message);

  mrb_path = mrb_str_new_cstr(mrb, path);
  mrb_obj_freeze(mrb, mrb_path);

  debug_printf("Raising LoadError (Path: %s)\n", path);

  load_error = mrb_funcall(mrb, mrb_obj_value(E_LOAD_ERROR), "new", 1, message);
  mrb_iv_set(mrb, load_error, mrb_intern_lit(mrb, "path"), mrb_path);
  mrb_exc_raise(mrb, load_error);
}
