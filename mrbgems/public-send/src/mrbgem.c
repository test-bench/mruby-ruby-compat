#include <mruby.h>

void
mrb_mruby_public_send_gem_init(mrb_state* mrb) {
  mrb_define_alias(mrb, mrb->kernel_module, "public_send", "send");

  return;
}

void
mrb_mruby_public_send_gem_final(mrb_state* mrb) {
  return;
}
