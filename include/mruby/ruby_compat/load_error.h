#ifndef MRUBY_RUBY_COMPAT_LOAD_ERROR_H
#define MRUBY_RUBY_COMPAT_LOAD_ERROR_H

#include <mruby.h>

#define E_LOAD_ERROR (mrb_class_get(mrb, "LoadError"))

mrb_noreturn void mrb_raise_load_error(mrb_state*, const char* const);

void mrb_ruby_compat_load_error_init(mrb_state*);
void mrb_ruby_compat_load_error_final(mrb_state*);

#endif /* MRUBY_RUBY_COMPAT_LOAD_ERROR_H */
