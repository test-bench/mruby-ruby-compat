#include <mruby.h>
#include <mruby/string.h>

#include <mruby/ruby_compat/abort.h>
#include <mruby/ruby_compat/system_exit.h>

mrb_value
mrb_abort(mrb_state* mrb, mrb_value message) {
  mrb_value system_exit;

  if(mrb_string_p(message)) {
    fprintf(stderr, "%s\n", RSTRING_CSTR(mrb, message));
  } else {
    message = mrb_str_new_cstr(mrb, "SystemExit");
  }

  system_exit = mrb_funcall(mrb, mrb_obj_value(E_SYSTEM_EXIT), "new", 2, mrb_fixnum_value(1), message);

  mrb_exc_raise(mrb, system_exit);
}

static mrb_value
mrb_f_abort(mrb_state* mrb, mrb_value self) {
  mrb_value message;

  mrb_get_args(mrb, "|S", &message);

  return mrb_abort(mrb, message);
}

void
mrb_ruby_compat_abort_init(mrb_state* mrb) {
  mrb_define_method(mrb, mrb->kernel_module, "abort", mrb_f_abort, MRB_ARGS_OPT(1));

  return;
}

void
mrb_ruby_compat_abort_final(mrb_state* mrb) {
  return;
}
