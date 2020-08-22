#ifndef MRUBY_REQUIRE_REQUIRE_H
#define MRUBY_REQUIRE_REQUIRE_H

#include <mruby.h>

#define LOAD_PATH(mrb) (mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$LOAD_PATH")))
#define LOADED_FEATURES(mrb) (mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$LOADED_FEATURES")))
#define MRUBY_REQUIRED_FILES(mrb) (mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$MRUBY_REQUIRED_FILES")))
#define MRUBY_COMPILED_FEATURES(mrb) (mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$MRUBY_COMPILED_FEATURES")))

mrb_bool mrb_load(mrb_state*, const char* const);
mrb_bool mrb_require_relative(mrb_state*, const char* const);
mrb_bool mrb_require(mrb_state*, const char* const);
mrb_bool mrb_require_absolute(mrb_state* mrb, const char* const path);

mrb_int mrb_register_compiled_features(mrb_state* mrb, const char * const[], const int);
mrb_value mrb_register_compiled_feature(mrb_state*, const char * const);
mrb_bool mrb_require_compiled_feature(mrb_state*, const char * const);

#endif /* MRUBY_REQUIRE_REQUIRE_H */
