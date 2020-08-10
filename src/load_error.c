#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/ruby_compat/load_error.h>

#include "debug_printf.h"

static mrb_value
mrb_load_error_path(mrb_state* mrb, mrb_value self) {
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "path"));
}

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

void
mrb_ruby_compat_load_error_init(mrb_state* mrb) {
  struct RClass* load_error;

  load_error = mrb_define_class(mrb, "LoadError", mrb_class_get(mrb, "ScriptError"));

  mrb_define_method(mrb, load_error, "path", mrb_load_error_path, MRB_ARGS_NONE());

  return;
}

void
mrb_ruby_compat_load_error_final(mrb_state* mrb) {
  return;
}
