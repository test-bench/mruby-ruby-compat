#include <mruby.h>
#include <mruby/variable.h>

#include <mruby_local/abort.h>
#include <mruby_local/require.h>
#include <mruby_local/toplevel_binding.h>

#include <stdio.h>

#include "debug.h"

static mrb_value mrb_f_exception_cause(mrb_state* mrb, mrb_value self) {
  return mrb_nil_value();
}

static mrb_value mrb_dir_brackets(mrb_state* mrb, mrb_value self) {
  mrb_int argc;
  mrb_value* argv;
  mrb_value block;

  mrb_get_args(mrb, "*!&", &argv, &argc, &block);

  return mrb_funcall_with_block(mrb, self, mrb_intern_lit(mrb, "glob"), argc, argv, block);
}

void mrb_test_bench_mruby_gem_init(mrb_state* mrb) {
  struct RClass* dir_class;

  mrb_abort_init(mrb);
  mrb_require_init(mrb);
  mrb_toplevel_binding_init(mrb);

  mrb_define_method(mrb, mrb->eException_class, "cause", mrb_f_exception_cause, MRB_ARGS_NONE());

  mrb_define_alias(mrb, mrb->kernel_module, "public_send", "send");

  mrb_define_alias(mrb, mrb->array_class, "*", "join");

  dir_class = mrb_class_get(mrb, "Dir");
  mrb_define_class_method(mrb, dir_class, "[]", mrb_dir_brackets, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());

  return;
}

void mrb_test_bench_mruby_gem_final(mrb_state* mrb) {
  mrb_toplevel_binding_final(mrb);
  mrb_require_final(mrb);
  mrb_abort_final(mrb);

  debug_printf("Finished\n");

  return;
}
