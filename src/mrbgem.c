#include <mruby.h>
#include <mruby/variable.h>

#include <mruby_local/abort.h>
#include <mruby_local/require.h>
#include <mruby_local/toplevel_binding.h>

#include <stdio.h>

#include "debug.h"

static mrb_value mrb_f_exception(mrb_state* mrb, mrb_value self) {
  return mrb_nil_value();
}

void mrb_test_bench_mruby_gem_init(mrb_state* mrb) {
  mrb_abort_init(mrb);
  mrb_require_init(mrb);
  mrb_toplevel_binding_init(mrb);

  mrb_define_method(mrb, mrb->eException_class, "cause", mrb_f_exception, MRB_ARGS_NONE());

  mrb_define_alias(mrb, mrb->kernel_module, "public_send", "send");

  mrb_define_alias(mrb, mrb->array_class, "*", "join");

  return;
}

void mrb_test_bench_mruby_gem_final(mrb_state* mrb) {
  mrb_toplevel_binding_final(mrb);
  mrb_require_final(mrb);
  mrb_abort_final(mrb);

  debug_printf("Finished\n");

  return;
}
