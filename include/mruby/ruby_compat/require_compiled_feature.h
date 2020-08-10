#ifndef MRUBY_RUBY_COMPAT_REQUIRE_COMPILED_FEATURE_H
#define MRUBY_RUBY_COMPAT_REQUIRE_COMPILED_FEATURE_H

#include <mruby.h>

mrb_bool mrb_require_compiled_feature(mrb_state*, const char * const);

mrb_int mrb_record_compiled_features(mrb_state* mrb, const char * const features[], const int features_count);
mrb_value mrb_record_compiled_feature(mrb_state*, const char * const);

void mrb_ruby_compat_require_compiled_feature_init(mrb_state*);
void mrb_ruby_compat_require_compiled_feature_final(mrb_state*);

#endif /* MRUBY_RUBY_COMPAT_REQUIRE_COMPILED_FEATURE_H */
