#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/ruby_compat/abort.h>
#include <mruby/ruby_compat/load_error.h>
#include <mruby/ruby_compat/require.h>
#include <mruby/ruby_compat/require_compiled_feature.h>
#include <mruby/ruby_compat/system_exit.h>

#include <stdio.h>

static mrb_value mrb_f_exception_cause(mrb_state* mrb, mrb_value self) {
  return mrb_nil_value();
}

void
mrb_mruby_ruby_compat_gem_init(mrb_state* mrb) {
  mrb_ruby_compat_abort_init(mrb);
  mrb_ruby_compat_system_exit_init(mrb);
  mrb_ruby_compat_load_error_init(mrb);
  mrb_ruby_compat_require_compiled_feature_init(mrb);
  mrb_ruby_compat_require_init(mrb);

  mrb_define_method(mrb, mrb->eException_class, "cause", mrb_f_exception_cause, MRB_ARGS_NONE());

  mrb_define_alias(mrb, mrb->kernel_module, "public_send", "send");

  mrb_define_global_const(mrb, "RUBY_DESCRIPTION", mrb_str_new_lit(mrb, MRUBY_DESCRIPTION));

  return;
}

void
mrb_mruby_ruby_compat_gem_final(mrb_state* mrb) {
  mrb_ruby_compat_require_final(mrb);
  mrb_ruby_compat_require_compiled_feature_final(mrb);
  mrb_ruby_compat_load_error_final(mrb);
  mrb_ruby_compat_system_exit_final(mrb);
  mrb_ruby_compat_abort_final(mrb);

  return;
}
