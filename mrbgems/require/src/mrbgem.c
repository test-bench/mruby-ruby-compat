#include <mruby.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/require/dir.h>
#include <mruby/require/require.h>

#include <stdlib.h>
#include <string.h>

static mrb_value
mrb_f_load(mrb_state* mrb, mrb_value self) {
  mrb_value path;

  mrb_get_args(mrb, "o", &path);

  if(mrb_type(path) != MRB_TT_STRING) {
    mrb_raisef(mrb, E_TYPE_ERROR, "can't convert %S into String", path);
    return mrb_nil_value();
  }

  if(mrb_load(mrb, RSTRING_CSTR(mrb, path)) == TRUE) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value
mrb_f_require_relative(mrb_state* mrb, mrb_value self) {
  mrb_value relative_path;

  mrb_get_args(mrb, "o", &relative_path);

  if(mrb_type(relative_path) != MRB_TT_STRING) {
    mrb_raisef(mrb, E_TYPE_ERROR, "can't convert %S into String", relative_path);
    return mrb_nil_value();
  }

  if(mrb_require_relative(mrb, RSTRING_CSTR(mrb, relative_path))) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value
mrb_f_require(mrb_state* mrb, mrb_value self) {
  mrb_value path;

  mrb_get_args(mrb, "o", &path);

  if(mrb_type(path) != MRB_TT_STRING) {
    mrb_raisef(mrb, E_TYPE_ERROR, "can't convert %S into String", path);
    return mrb_nil_value();
  }

  if(mrb_require(mrb, RSTRING_CSTR(mrb, path))) {
    return mrb_true_value();
  } else {
    return mrb_false_value();
  }
}

static mrb_value
mrb_load_error_path(mrb_state* mrb, mrb_value self) {
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "path"));
}

#ifndef MRUBY_REQUIRE_COMPILED_FEATURES
#define MRUBY_REQUIRE_COMPILED_FEATURES ""
#endif /* MRUBY_REQUIRE_COMPILED_FEATURES */

static const char* const mruby_require_compiled_features = MRUBY_REQUIRE_COMPILED_FEATURES;

static void
register_initial_compiled_features(mrb_state* mrb) {
  char* initial_compiled_features;
  char* strtok_r_saveptr;
  char* compiled_feature;
  int len;

  len = strlen(mruby_require_compiled_features);

  initial_compiled_features = malloc(len + 1);

  strncpy(initial_compiled_features, mruby_require_compiled_features, len);

  compiled_feature = initial_compiled_features;

  compiled_feature = strtok_r(compiled_feature, ",", &strtok_r_saveptr);
  while(compiled_feature != NULL) {
    mrb_register_compiled_feature(mrb, compiled_feature);
    compiled_feature = strtok_r(NULL, ",", &strtok_r_saveptr);
  }
  mrb_register_compiled_feature(mrb, compiled_feature);

  free(initial_compiled_features);

  return;
}

void
mrb_mruby_require_gem_init(mrb_state* mrb) {
  mrb_value require_search_paths;
  mrb_value compiled_features_hash;
  struct RClass* load_error;

  mrb_require_init(mrb);

  mrb_define_method(mrb, mrb->kernel_module, "load", mrb_f_load, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require_relative", mrb_f_require_relative, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require", mrb_f_require, MRB_ARGS_REQ(1));

  load_error = mrb_define_class(mrb, "LoadError", mrb_class_get(mrb, "ScriptError"));
  mrb_define_method(mrb, load_error, "path", mrb_load_error_path, MRB_ARGS_NONE());

  mrb_define_method(mrb, mrb->kernel_module, "__dir__", mrb_f___dir__, MRB_ARGS_NONE());

  register_initial_compiled_features(mrb);

  return;
}

void
mrb_mruby_require_gem_final(mrb_state* mrb) {
  return;
}
