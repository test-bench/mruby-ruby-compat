#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <mruby_local/require.h>

#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "debug.h"


typedef struct load_stack_entry_struct {
  struct load_stack_entry_struct *parent;
  int depth;
  char directory[];
} load_stack_entry;

load_stack_entry *load_stack_top = NULL;

static const int load_stack_depth(void) {
  if(load_stack_top == NULL) {
    return 0;
  } else {
    return load_stack_top->depth;
  }
}

static char* load_stack_current_directory(void) {
  if(load_stack_top == NULL) {
    return NULL;
  } else {
    return load_stack_top->directory;
  }
}

static load_stack_entry* push_load_stack(const char* const path) {
  load_stack_entry *entry;

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

static void pop_load_stack(void) {
  load_stack_entry* entry;

  entry = load_stack_top;

  if(entry == NULL) {
    fprintf(stderr, "Error: require stack is empty!\n");
    return;
  }

  load_stack_top = entry->parent;

  free(entry);
}


static mrb_value mrb_load_error_path(mrb_state* mrb, mrb_value self) {
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "path"));
}

static mrb_noreturn void mrb_raise_load_error(mrb_state* mrb, const char* const path) {
  mrb_value message;
  mrb_value exception;
  mrb_value mrb_path;

  message = mrb_str_new_cstr(mrb, "cannot load such file");
  mrb_str_cat_lit(mrb, message, " -- ");
  mrb_str_cat_cstr(mrb, message, path);
  mrb_obj_freeze(mrb, message);

  exception = mrb_funcall(mrb, mrb_obj_value(E_LOAD_ERROR), "new", 1, message);

  mrb_path = mrb_str_new_cstr(mrb, path);
  mrb_obj_freeze(mrb, mrb_path);

  mrb_iv_set(mrb, exception, mrb_intern_lit(mrb, "path"), mrb_path);

  mrb_exc_raise(mrb, exception);
}

mrb_bool mrb_load(mrb_state* mrb, const char* const path) {
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

mrb_value mrb_f_load(mrb_state* mrb, mrb_value self) {
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


static char* resolve_ruby_file(const char* const relative_path, const char* const root) {
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


static mrb_bool mrb_require_absolute(mrb_state* mrb, const char* const path) {
  mrb_value mrb_path;
  mrb_value required_files_hash;
  mrb_value required_files;

  debug_printf("Require absolute begin (Absolute Path: %s)\n", path);

  mrb_path = mrb_str_new_cstr(mrb, path);
  mrb_obj_freeze(mrb, mrb_path);

  required_files_hash = mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$LOADED_FEATURES_HASH"));

  if(mrb_hash_key_p(mrb, required_files_hash, mrb_path)) {
    debug_printf("Require absolute done (Absolute Path: %s, Required: false)\n", path);
    return FALSE;
  }

  debug_printf("mrb_hash_set begin\n");
  debug_printf("  Size: %d, Path: %s\n", mrb_hash_size(mrb, required_files_hash), RSTRING_CSTR(mrb, mrb_path));
  mrb_hash_set(mrb, required_files_hash, mrb_path, mrb_true_value());

  debug_printf("mrb_hash_set end\n");

  required_files = mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$LOADED_FEATURES"));
  mrb_ary_push(mrb, required_files, mrb_path);

  mrb_load(mrb, path);

  debug_printf("Require absolute done (Absolute Path: %s, Required: true)\n", path);
  return TRUE;
}

mrb_bool mrb_require_relative(mrb_state* mrb, const char* const relative_path) {
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

mrb_value mrb_f_require_relative(mrb_state* mrb, mrb_value self) {
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


mrb_bool mrb_require(mrb_state* mrb, const char* const path) {
  int index;
  mrb_value require_search_paths;
  mrb_bool success;

  debug_printf("Require begin (Path: %s)\n", path);

  require_search_paths = mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$LOAD_PATH"));

  for(index = 0; index < RARRAY_LEN(require_search_paths); index++) {
    const char* require_search_path = RSTRING_CSTR(mrb, mrb_ary_entry(require_search_paths, index));
    char* absolute_path;

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

mrb_value mrb_f_require(mrb_state* mrb, mrb_value self) {
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


mrb_value mrb_f___dir__(mrb_state* mrb, mrb_value self) {
  char* dir = load_stack_current_directory();
  mrb_value str;

  str = mrb_str_new_cstr(mrb, dir);
  mrb_obj_freeze(mrb, str);

  return str;
}


void mrb_require_init(mrb_state* mrb) {
  struct RClass* load_error;
  mrb_value require_search_paths;
  mrb_value required_files;
  mrb_value required_files_hash;

  require_search_paths = mrb_ary_new(mrb);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$:"), require_search_paths);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$LOAD_PATH"), require_search_paths);

  required_files = mrb_ary_new(mrb);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$\""), required_files);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$LOADED_FEATURES"), required_files);

  required_files_hash = mrb_ary_new(mrb);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$LOADED_FEATURES_HASH"), required_files_hash);

  load_error = mrb_define_class(mrb, "LoadError", E_SCRIPT_ERROR);
  mrb_define_method(mrb, load_error, "path", mrb_load_error_path, MRB_ARGS_NONE());

  mrb_define_method(mrb, mrb->kernel_module, "load", mrb_f_load, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require_relative", mrb_f_require_relative, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require", mrb_f_require, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, mrb->kernel_module, "__dir__", mrb_f___dir__, MRB_ARGS_NONE());
}

void mrb_require_final(mrb_state* mrb) {
  mrb_int required_files_count;
  mrb_value required_files_hash;

  required_files_hash = mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$LOADED_FEATURES_HASH"));

  required_files_count = mrb_hash_size(mrb, required_files_hash);

  debug_printf("Requiring done\n\tRequired files: %d\n\tLoad stack depth: %d\n", required_files_count, load_stack_depth());
}
