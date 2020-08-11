#include <mruby.h>

static
mrb_value mrb_exception_cause(mrb_state* mrb, mrb_value self) {
  return mrb_nil_value();
}

void
mrb_mruby_exception_cause_gem_init(mrb_state* mrb) {
  mrb_define_method(mrb, mrb->eException_class, "cause", mrb_exception_cause, MRB_ARGS_NONE());

  return;
}

void
mrb_mruby_exception_cause_gem_final(mrb_state* mrb)
{
  return;
}
