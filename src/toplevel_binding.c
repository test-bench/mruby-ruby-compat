#include <mruby.h>

#include "toplevel_binding.h"

static mrb_value toplevel_binding_receiver(mrb_state *mrb, mrb_value module) {
  return mrb_top_self(mrb);
}

void mrb_toplevel_binding_init(mrb_state *mrb) {
  struct RClass* toplevel_binding_module;

  toplevel_binding_module = mrb_define_module(mrb, "TOPLEVEL_BINDING");

  mrb_define_singleton_method(
    mrb,
    (struct RObject *)toplevel_binding_module,
    "receiver",
    toplevel_binding_receiver,
    MRB_ARGS_NONE()
  );

  return;
}

void mrb_toplevel_binding_final(mrb_state *mrb) {
  return;
}
