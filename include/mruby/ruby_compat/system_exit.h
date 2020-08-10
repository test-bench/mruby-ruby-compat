#ifndef MRUBY_RUBY_COMPAT_SYSTEM_EXIT_H
#define MRUBY_RUBY_COMPAT_SYSTEM_EXIT_H

#include <mruby.h>

#define E_SYSTEM_EXIT (mrb_class_get(mrb, "SystemExit"))

void mrb_ruby_compat_system_exit_init(mrb_state*);
void mrb_ruby_compat_system_exit_final(mrb_state*);

#endif /* MRUBY_RUBY_COMPAT_SYSTEM_EXIT_H */
