#ifndef MRUBY_RUBY_COMPAT_REQUIRE_H
#define MRUBY_RUBY_COMPAT_REQUIRE_H

mrb_bool mrb_load(mrb_state*, const char* const);
mrb_bool mrb_require_relative(mrb_state*, const char* const);
mrb_bool mrb_require(mrb_state*, const char* const);

void mrb_ruby_compat_require_init(mrb_state*);
void mrb_ruby_compat_require_final(mrb_state*);

#endif /* MRUBY_RUBY_COMPAT_REQUIRE_H */
