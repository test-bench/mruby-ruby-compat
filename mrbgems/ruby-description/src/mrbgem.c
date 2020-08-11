#include <mruby.h>

void
mrb_mruby_ruby_description_gem_init(mrb_state* mrb) {
  mrb_define_global_const(mrb, "RUBY_DESCRIPTION", mrb_str_new_lit(mrb, MRUBY_DESCRIPTION));

  return;
}

void
mrb_mruby_ruby_description_gem_final(mrb_state* mrb)
{
  return;
}
