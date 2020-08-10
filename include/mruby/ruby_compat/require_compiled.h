#ifndef MRUBY_RUBY_COMPAT_REQUIRE_COMPILED_H
#define MRUBY_RUBY_COMPAT_REQUIRE_COMPILED_H

#include <mruby.h>

mrb_bool mrb_require_compiled(mrb_state*, const char * const);

mrb_value mrb_record_compiled_library(mrb_state*, const char * const);

void mrb_ruby_compat_require_compiled_init(mrb_state*);
void mrb_ruby_compat_require_compiled_final(mrb_state*);

#endif /* MRUBY_RUBY_COMPAT_REQUIRE_COMPILED_H */
