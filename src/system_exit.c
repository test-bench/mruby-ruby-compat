#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/ruby_compat/system_exit.h>

#include "debug_printf.h"

static mrb_value
mrb_system_exit_status(mrb_state* mrb, mrb_value self) {
  return mrb_attr_get(mrb, self, mrb_intern_lit(mrb, "status"));
}

static mrb_value
mrb_system_exit_initialize(mrb_state* mrb, mrb_value system_exit) {
  mrb_int status;
  mrb_value message;

  mrb_int argc;

  argc = mrb_get_args(mrb, "|iS", &status, &message);

  if(argc == 0) {
    status = 0;
  }
  mrb_iv_set(mrb, system_exit, mrb_intern_lit(mrb, "status"), mrb_fixnum_value(status));

  if(argc == 2) {
    mrb_iv_set(mrb, system_exit, mrb_intern_lit(mrb, "mesg"), message);
  } else {
    message = mrb_str_new_cstr(mrb, "(no message)");
  }

  debug_printf("Raising SystemExit (Status: %d, Message: %s)\n", status, RSTRING_CSTR(mrb, message));

  return system_exit;
}

void
mrb_ruby_compat_system_exit_init(mrb_state* mrb) {
  struct RClass* system_exit;

  system_exit = mrb_define_class(mrb, "SystemExit", mrb->eException_class);
  mrb_define_method(mrb, system_exit, "initialize", mrb_system_exit_initialize, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, system_exit, "status", mrb_system_exit_status, MRB_ARGS_NONE());

  return;
}

void
mrb_ruby_compat_system_exit_final(mrb_state* mrb) {
  return;
}
