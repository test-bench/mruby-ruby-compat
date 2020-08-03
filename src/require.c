#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "require.h"
#include "debug.h"


#define E_LOAD_ERROR (mrb_class_get(mrb, "LoadError"))


static mrb_value mrb_init_load_paths(mrb_state* mrb) {
  mrb_value load_paths;

  load_paths = mrb_ary_new(mrb);

  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$:"), load_paths);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$LOAD_PATH"), load_paths);

  return load_paths;
}

/*
static mrb_value mrb_get_load_paths(mrb_state* mrb) {
  mrb_value load_paths;

  load_paths = mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$:"));

  return load_paths;
}
*/


typedef struct load_stack_entry_struct {
  struct load_stack_entry_struct *parent;
  int depth;
  const char *path;
} load_stack_entry;

load_stack_entry *load_stack_top = NULL;

static const int load_stack_depth(void) {
  if(load_stack_top == NULL) {
    return 0;
  } else {
    return load_stack_top->depth;
  }
}

static load_stack_entry* push_load_stack(const char* const path) {
  load_stack_entry *entry;

  entry = malloc(sizeof(load_stack_entry));

  entry->parent = load_stack_top;
  entry->path = path;

  if(entry->parent == NULL) {
    entry->depth = 1;
  } else {
    entry->depth = entry->parent->depth + 1;
  }

  load_stack_top = entry;

  return entry;
}

const char* const pop_load_stack(void) {
  const char* path;
  load_stack_entry* entry;

  entry = load_stack_top;

  if(entry == NULL) {
    fprintf(stderr, "Error: require stack is empty!\n");
    return NULL;
  }

  load_stack_top = entry->parent;

  path = entry->path;

  free(entry);

  return path;
}


static mrb_value mrb_load_error_path(mrb_state* mrb, mrb_value self) {
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "path"));
}

static mrb_noreturn void mrb_raise_load_error(mrb_state* mrb, const char* const path) {
  mrb_value message;
  mrb_value exception;

  message = mrb_str_new_cstr(mrb, "cannot load such file");
  mrb_str_cat_lit(mrb, message, " -- ");
  mrb_str_cat_cstr(mrb, message, path);

  exception = mrb_funcall(mrb, mrb_obj_value(E_LOAD_ERROR), "new", 1, message);

  mrb_iv_set(mrb, exception, mrb_intern_lit(mrb, "path"), mrb_str_new_cstr(mrb, path));

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

  if(pop_load_stack() == NULL) {
    mrb_raise_load_error(mrb, path);
    debug_printf("Could not load file %s: load stack empty\n", path);
    return FALSE;
  }

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


const char* const resolve_require_relative_path(const char* relative_path) {
  if(load_stack_top == NULL) {
    fprintf(stderr, "No files have been loaded!\n");
    return NULL;
  }

  return resolve_path(relative_path, load_stack_top->path);
}

mrb_bool mrb_require_relative(mrb_state* mrb, const char* const relative_path) {
  char* absolute_path;
  mrb_bool success;

  absolute_path = resolve_require_relative_path(relative_path);

  if(debug_mode()) {
    fprintf(stderr, "Require relative: %s\n", absolute_path);
  }

  success = mrb_load_raw(mrb, absolute_path);

  free(absolute_path);

  return success;
}

mrb_value mrb_f_require_relative(mrb_state* mrb, mrb_value self) {
  mrb_value relative_path;

  mrb_get_args(mrb, "o", &relative_path);

  if(mrb_type(relative_path) != MRB_TT_STRING) {
    mrb_raisef(mrb, E_TYPE_ERROR, "can't convert %S into String", relative_path);
    return mrb_nil_value();
  }

  return mrb_require_relative(mrb, RSTRING_CSTR(mrb, relative_path));
}


void mrb_require_init(mrb_state* mrb) {
  struct RClass* load_error;

  mrb_init_load_paths(mrb);

  load_error = mrb_define_class(mrb, "LoadError", E_SCRIPT_ERROR);
  mrb_define_method(mrb, load_error, "path", mrb_load_error_path, MRB_ARGS_NONE());
  debug_printf("Defined LoadError\n");

  mrb_define_method(mrb, mrb->kernel_module, "load", mrb_f_load, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->kernel_module, "require_relative", mrb_f_require_relative, MRB_ARGS_REQ(1));
}
