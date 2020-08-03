#include <mruby.h>
#include <mruby/variable.h>

#include <stdio.h>

#include "debug.h"
#include "require.h"

void mrb_mruby_local_gem_init(mrb_state *mrb) {
  debug_printf("Initializing\n");

  mrb_require_init(mrb);

  return;
}

void mrb_mruby_local_gem_final(mrb_state *mrb) {
  debug_printf("Finished\n");

  return;
}

/*
static mrb_value toplevel_binding_receiver(mrb_state *mrb, mrb_value module) {
  return mrb_top_self(mrb);
}

struct RClass *define_toplevel_module(mrb_state *mrb) {
  struct RClass *toplevel_binding_module;

  toplevel_binding_module = mrb_define_module(mrb, "TOPLEVEL_BINDING");

  mrb_define_singleton_method(mrb, (struct RObject *)toplevel_binding_module, "receiver", toplevel_binding_receiver, MRB_ARGS_NONE());

  return toplevel_binding_module;
}

void mrb_mruby_ruby_cross_compat_gem_init(mrb_state *mrb) {
  mrb_value load_paths;

  load_paths = init_load_paths(mrb);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$LOAD_PATH"), load_paths);

  define_toplevel_module(mrb);

  mrb_define_method(mrb, mrb->kernel_module, "load", mrb_f_load, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require_relative", mrb_f_require_relative, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require", mrb_f_require, MRB_ARGS_REQ(1));

  mrb_define_alias(mrb, mrb->array_class, "*", "join");

  return;
}

void mrb_mruby_ruby_cross_compat_gem_final(mrb_state *mrb) {
  return;
}
*/
