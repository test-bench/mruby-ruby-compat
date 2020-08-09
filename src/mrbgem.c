#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <stdio.h>

#define E_SYSTEM_EXIT (mrb_class_get(mrb, "SystemExit"))

static mrb_value
mrb_system_exit_status(mrb_state* mrb, mrb_value self) {
  return mrb_attr_get(mrb, self, mrb_intern_lit(mrb, "status"));
}

static mrb_value
mrb_system_exit_initialize(mrb_state* mrb, mrb_value system_exit) {
  mrb_int status;
  mrb_value message;

  mrb_int argc;

  argc = mrb_get_args(mrb, "|iS", &status, &message);

  if(argc == 0) {
    status = 0;
  }
  mrb_iv_set(mrb, system_exit, mrb_intern_lit(mrb, "status"), mrb_fixnum_value(status));

  if(argc == 2) {
    mrb_iv_set(mrb, system_exit, mrb_intern_lit(mrb, "mesg"), message);
  } else {
    message = mrb_str_new_cstr(mrb, "(no message)");
  }

  return system_exit;
}

mrb_value
mrb_abort(mrb_state* mrb, mrb_value message) {
  mrb_value system_exit;

  if(mrb_string_p(message)) {
    fprintf(stderr, "%s\n", RSTRING_CSTR(mrb, message));
  } else {
    message = mrb_str_new_cstr(mrb, "SystemExit");
  }

  system_exit = mrb_funcall(mrb, mrb_obj_value(E_SYSTEM_EXIT), "new", 2, mrb_fixnum_value(1), message);

  mrb_exc_raise(mrb, system_exit);
}

static mrb_value
mrb_f_abort(mrb_state* mrb, mrb_value self) {
  mrb_value message;

  mrb_get_args(mrb, "|S", &message);

  return mrb_abort(mrb, message);
}


static mrb_value mrb_f_exception_cause(mrb_state* mrb, mrb_value self) {
  return mrb_nil_value();
}


static mrb_value
mrb_f_require_inert(mrb_state* mrb, mrb_value self) {
  mrb_value path;

  mrb_get_args(mrb, "o", &path);

  if(mrb_type(path) != MRB_TT_STRING) {
    mrb_raisef(mrb, E_TYPE_ERROR, "can't convert %S into String", path);
    return mrb_nil_value();
  }

  return mrb_false_value();
}


void
mrb_mruby_ruby_compat_gem_init(mrb_state* mrb) {
  struct RClass* system_exit;

  system_exit = mrb_define_class(mrb, "SystemExit", mrb->eException_class);
  mrb_define_method(mrb, system_exit, "initialize", mrb_system_exit_initialize, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, system_exit, "status", mrb_system_exit_status, MRB_ARGS_NONE());

  mrb_define_method(mrb, mrb->kernel_module, "abort", mrb_f_abort, MRB_ARGS_OPT(1));

  mrb_define_method(mrb, mrb->eException_class, "cause", mrb_f_exception_cause, MRB_ARGS_NONE());

  mrb_define_alias(mrb, mrb->kernel_module, "public_send", "send");

  mrb_define_method(mrb, mrb->kernel_module, "require_inert", mrb_f_require_inert, MRB_ARGS_REQ(1));
  mrb_define_alias(mrb, mrb->kernel_module, "require", "require_inert");

  mrb_define_alias(mrb, mrb->array_class, "*", "join");

  mrb_define_global_const(mrb, "RUBY_DESCRIPTION", mrb_str_new_lit(mrb, MRUBY_DESCRIPTION));

  return;
}

void
mrb_mruby_ruby_compat_gem_final(mrb_state* mrb) {
  return;
}
