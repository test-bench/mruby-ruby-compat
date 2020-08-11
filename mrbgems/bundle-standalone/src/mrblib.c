#include <mruby.h>

#include <mruby/require/require.h>

void
mrb_mruby_bundle_standalone_gem_init(mrb_state* mrb) {
  mrb_register_compiled_feature(mrb, "rbconfig");

  return;
}

void
mrb_mruby_bundle_standalone_gem_final(mrb_state* mrb)
{
  return;
}
