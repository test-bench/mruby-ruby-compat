#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby/require/dir.h>
#include <mruby/require/load_error.h>
#include <mruby/require/require.h>

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "debug_printf.h"

typedef struct load_stack_entry_struct {
  struct load_stack_entry_struct* parent;
  int depth;
  char directory[];
} load_stack_entry;

load_stack_entry* load_stack_top = NULL;

static const int
load_stack_depth(void) {
  if(load_stack_top == NULL) {
    return 0;
  } else {
    return load_stack_top->depth;
  }
}

static char*
load_stack_current_directory(void) {
  if(load_stack_top == NULL) {
    return NULL;
  } else {
    return load_stack_top->directory;
  }
}

static load_stack_entry*
push_load_stack(const char* const path) {
  load_stack_entry* entry;

  entry = malloc(sizeof(load_stack_entry) + strlen(path) + 1);

  entry->parent = load_stack_top;

  strcpy(entry->directory, path);

  if(strcmp(dirname(entry->directory), ".") == 0) {
    sprintf(entry->directory, ".");
  }

  if(entry->parent == NULL) {
    entry->depth = 1;
  } else {
    entry->depth = entry->parent->depth + 1;
  }

  load_stack_top = entry;

  return entry;
}

static void
pop_load_stack(void) {
  load_stack_entry* entry;

  entry = load_stack_top;

  if(entry == NULL) {
    fprintf(stderr, "Error: require stack is empty!\n");
    return;
  }

  load_stack_top = entry->parent;

  free(entry);

  return;
}

mrb_bool
mrb_load(mrb_state* mrb, const char* const path) {
  FILE* file;
  mrbc_context* mrbc_ctx;

  debug_printf("Loading file: %s (Depth: %d)\n", path, load_stack_depth());

  file = fopen(path, "rb");

  if(file == NULL) {
    debug_printf("Could not read file file %s: %s\n", path, strerror(errno));
    mrb_raise_load_error(mrb, path);
  }

  mrbc_ctx = mrbc_context_new(mrb);
  mrbc_filename(mrb, mrbc_ctx, path);

  push_load_stack(path);

  mrb_load_file_cxt(mrb, file, mrbc_ctx);

  mrbc_context_free(mrb, mrbc_ctx);

  pop_load_stack();

  if(mrb->exc) {
    return FALSE;
  } else {
    return TRUE;
  }
}

static char*
resolve_ruby_file(const char* const relative_path, const char* const root) {
  char* extension;
  char* path;
  char joined_path[PATH_MAX] = {0};
  struct stat stat_buf;

  debug_printf("Resolving ruby file (Relative Path: %s, Root: %s)\n", relative_path, root);

  if(stat(root, &stat_buf) != 0) {
    goto failure;
  }

  extension = strrchr(relative_path, '.');
  if(extension == NULL || strcmp(extension, ".rb") != 0) {
    extension = (char*) ".rb";
  } else {
    extension = (char*) "";
  }

  sprintf(joined_path, "%s/%s%s", root, relative_path, extension);

  if(stat(joined_path, &stat_buf) != 0) {
    goto failure;
  }

  path = realpath(joined_path, NULL);

  if(path == NULL) {
    goto failure;
  }

  debug_printf("Resolved ruby file (Relative Path: %s, Root: %s, Path: %s)\n", relative_path, root, path);

  return path;

failure:
  debug_printf("Failed to resolve ruby file (Relative Path: %s, Root: %s)\n", relative_path, root);

  return NULL;
}

static mrb_bool
mrb_require_absolute(mrb_state* mrb, const char* const path) {
  mrb_value mrb_path;
  mrb_value required_files_hash;
  mrb_value required_files;

  debug_printf("Require absolute begin (Absolute Path: %s)\n", path);

  mrb_path = mrb_str_new_cstr(mrb, path);
  mrb_obj_freeze(mrb, mrb_path);

  required_files_hash = MRUBY_REQUIRED_FILES(mrb);

  if(mrb_hash_key_p(mrb, required_files_hash, mrb_path)) {
    debug_printf("Require absolute done (Absolute Path: %s, Required: false)\n", path);
    return FALSE;
  }

  mrb_hash_set(mrb, required_files_hash, mrb_path, mrb_true_value());

  required_files = LOADED_FEATURES(mrb);
  mrb_ary_push(mrb, required_files, mrb_path);

  mrb_load(mrb, path);

  debug_printf("Require absolute done (Absolute Path: %s, Required: true)\n", path);
  return TRUE;
}

mrb_bool
mrb_require_relative(mrb_state* mrb, const char* const relative_path) {
  mrb_bool success;
  char* path;

  debug_printf("Require relative begin (Relative Path: %s)\n", relative_path);

  path = resolve_ruby_file(relative_path, load_stack_top->directory);

  if(path == NULL) {
    debug_printf("Require relative failure (Relative Path: %s)\n", relative_path);
    mrb_raise_load_error(mrb, relative_path);
  }

  success = mrb_require_absolute(mrb, path);

  debug_printf("Require relative finished (Relative Path: %s, Path: %s, Success: %s)\n", relative_path, path, success ? "true" : "false");

  free(path);

  return success;
}

mrb_bool
mrb_require(mrb_state* mrb, const char* const path) {
  int index;
  mrb_value require_search_paths;
  mrb_bool success;

  const char* require_search_path;
  char* absolute_path;

  debug_printf("Require begin (Path: %s)\n", path);

  if(mrb_require_compiled_feature(mrb, path) == TRUE) {
    return FALSE;
  }

  require_search_paths = LOAD_PATH(mrb);

  for(index = 0; index < RARRAY_LEN(require_search_paths); index++) {
    require_search_path = RSTRING_CSTR(mrb, mrb_ary_entry(require_search_paths, index));

    debug_printf("Searching path (Path: %s, Search Path: %s)\n", path, require_search_path);

    absolute_path = resolve_ruby_file(path, require_search_path);

    if(absolute_path != NULL) {
      success = mrb_require_absolute(mrb, absolute_path);

      free(absolute_path);

      debug_printf("Require finish (Path: %s, Search Path: %s, Success: %s)\n", path, require_search_path, success ? "true" : "false");

      return success;
    }

    free(absolute_path);
  }

  mrb_raise_load_error(mrb, path);
}

mrb_value
mrb_register_compiled_feature(mrb_state* mrb, const char * const feature) {
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
mrb_register_compiled_features(mrb_state* mrb, const char * const features[], const int features_count) {
  int index;

  for(index = 0; features[index] != NULL && index < features_count; index++) {
    mrb_register_compiled_feature(mrb, features[index]);
  }

  return index;
}

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
mrb_f___dir__(mrb_state* mrb, mrb_value self) {
  char* dir = load_stack_current_directory();
  mrb_value str;

  str = mrb_str_new_cstr(mrb, dir);
  mrb_obj_freeze(mrb, str);

  return str;
}
