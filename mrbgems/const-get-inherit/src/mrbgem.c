#include <mruby.h>
#include <mruby/class.h>

mrb_value
mrb_mod_const_get_inherit(mrb_state* mrb, mrb_value mod) {
  mrb_sym id;
  mrb_bool inherit;
  mrb_value cls;

  inherit = TRUE;

  mrb_get_args(mrb, "n|b", &id, &inherit);

  if(!inherit) {
    goto const_get_internal;
  }

  if(mrb_class_ptr(mod)->tt == MRB_TT_MODULE) {
    goto const_get_internal;
  }

  if(mrb_bool(mrb_funcall(mrb, mod, "const_defined?", 2, mrb_symbol_value(id), mrb_true_value())) == FALSE) {
    goto raise_name_error;
  }

  cls = mod;

  while(mrb_bool(mrb_funcall(mrb, cls, "const_defined?", 2, mrb_symbol_value(id), mrb_false_value())) == FALSE) {
    cls = mrb_funcall(mrb, cls, "superclass", 0);
  }

raise_name_error:

const_get_internal:

  return mrb_funcall(mrb, mod, "const_get_internal", 1, mrb_symbol_value(id));
}

void
mrb_mruby_const_get_inherit_gem_init(mrb_state* mrb) {
  mrb_define_method(mrb, mrb->module_class, "const_get_inherit", mrb_mod_const_get_inherit, MRB_ARGS_ARG(1, 1));

  mrb_define_alias(mrb, mrb->module_class, "const_get_internal", "const_get");
  mrb_define_alias(mrb, mrb->module_class, "const_get", "const_get_inherit");

  return;
}

void
mrb_mruby_const_get_inherit_gem_final(mrb_state* mrb) {
  return;
}
