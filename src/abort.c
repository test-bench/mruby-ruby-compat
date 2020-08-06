#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby_local/abort.h>

#include <stdio.h>

#include "debug.h"


#define E_SYSTEM_EXIT (mrb_class_get(mrb, "SystemExit"))


static mrb_value mrb_system_exit_status(mrb_state* mrb, mrb_value self) {
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "status"));
}


mrb_value mrb_f_abort(mrb_state* mrb, mrb_value self) {
  mrb_value message;
  mrb_value exception;

  mrb_get_args(mrb, "|S", &message);

  if(mrb_string_p(message)) {
    fprintf(stderr, "%s\n", RSTRING_CSTR(mrb, message));
  } else {
    message = mrb_str_new_cstr(mrb, "exit");
  }

  exception = mrb_funcall(mrb, mrb_obj_value(E_SYSTEM_EXIT), "new", 1, message);

  mrb_iv_set(mrb, exception, mrb_intern_lit(mrb, "status"), mrb_fixnum_value(1));

  mrb_exc_raise(mrb, exception);
}


void mrb_abort_init(mrb_state* mrb) {
  struct RClass* system_exit;

  system_exit = mrb_define_class(mrb, "SystemExit", mrb->eException_class);
  mrb_define_method(mrb, system_exit, "status", mrb_system_exit_status, MRB_ARGS_NONE());

  mrb_define_method(mrb, mrb->kernel_module, "abort", mrb_f_abort, MRB_ARGS_OPT(1));
}

void mrb_abort_final(mrb_state* mrb) {
}
