#include <mruby.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/ruby_compat/load_error.h>
#include <mruby/ruby_compat/require_compiled_feature.h>

#include <string.h>

#include "debug_printf.h"

#define MRUBY_COMPILED_FEATURES(mrb) (mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$MRUBY_COMPILED_FEATURES")))

mrb_bool
mrb_require_compiled_feature(mrb_state* mrb, const char* const feature) {
  mrb_value compiled_features;
  mrb_value mrb_feature;
  mrb_bool compiled;
  char* extension;

  extension = strrchr(feature, '.');
  if(extension != NULL && strcmp(extension, ".rb") == 0) {
    *extension = '\0';

    compiled = mrb_require_compiled_feature(mrb, feature);

    *extension = '.';

    return compiled;
  }

  compiled_features = MRUBY_COMPILED_FEATURES(mrb);

  if(mrb_nil_p(compiled_features)) {
    debug_printf("$MRUBY_COMPILED_FEATURES not set (Feature: %s)\n", feature);
    return FALSE;
  }

  mrb_feature = mrb_str_new_cstr(mrb, feature);
  mrb_obj_freeze(mrb, mrb_feature);

  compiled = mrb_hash_key_p(mrb, compiled_features, mrb_feature);

  if(compiled == TRUE) {
    debug_printf("Required compiled feature (Feature: %s)\n", feature);
  }

  return compiled;
}

mrb_value
mrb_record_compiled_feature(mrb_state* mrb, const char * const feature) {
  mrb_value mrb_feature;
  mrb_value compiled_features;

  mrb_feature = mrb_str_new_cstr(mrb, feature);
  mrb_obj_freeze(mrb, mrb_feature);

  compiled_features = MRUBY_COMPILED_FEATURES(mrb);

  if(mrb_nil_p(compiled_features)) {
    return mrb_nil_value();
  }

  mrb_hash_set(mrb, compiled_features, mrb_feature, mrb_true_value());

  debug_printf("Recorded compiled feature (Feature: %s)\n", feature);

  return mrb_feature;
}

mrb_int
mrb_record_compiled_features(mrb_state* mrb, const char * const features[], const int features_count) {
  int index;

  for(index = 0; features[index] != NULL && index < features_count; index++) {
    mrb_record_compiled_feature(mrb, features[index]);
  }

  return index;
}

void
mrb_ruby_compat_require_compiled_feature_init(mrb_state* mrb) {
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$MRUBY_COMPILED_FEATURES"), mrb_hash_new(mrb));

  return;
}

void
mrb_ruby_compat_require_compiled_feature_final(mrb_state* mrb) {
  return;
}
