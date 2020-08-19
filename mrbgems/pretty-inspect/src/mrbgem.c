#include <mruby.h>

void
mrb_mruby_pretty_inspect_gem_init(mrb_state* mrb) {
  mrb_define_alias(mrb, mrb->kernel_module, "pretty_inspect", "inspect");
  return;
}

void
mrb_mruby_pretty_inspect_gem_final(mrb_state* mrb) {
  return;
}
