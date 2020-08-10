#ifndef MRUBY_RUBY_COMPAT_ABORT_H
#define MRUBY_RUBY_COMPAT_ABORT_H

#include <mruby.h>

mrb_value mrb_abort(mrb_state*, mrb_value);

void mrb_ruby_compat_abort_init(mrb_state*);
void mrb_ruby_compat_abort_final(mrb_state*);

#endif /* MRUBY_RUBY_COMPAT_ABORT_H */
