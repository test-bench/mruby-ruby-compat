#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/variable.h>

#include "../../include/mruby/require/require.h"

int
main(int argc, char *argv[]) {
  int index;
  mrb_state* mrb;
  mrb_value argv_constant;
  mrb_value program_name;

  if(argc <= 1) {
    fprintf(stderr, "No input file\n");
    return 1;
  }

  mrb = mrb_open();
  if (!mrb) {
    fprintf(stderr, "Error: could not initialize mruby\n");
    return 1;
  }

  argv_constant = mrb_ary_new_capa(mrb, argc - 2);

  for(index = 2; index < argc; index++) {
    char* arg = mrb_utf8_from_locale(argv[index], -1);

    mrb_ary_push(mrb, argv_constant, mrb_str_new_cstr(mrb, arg));

    mrb_utf8_free(arg);
  }

  mrb_define_global_const(mrb, "ARGV", argv_constant);

  program_name = mrb_str_new_cstr(mrb, argv[1]);
  mrb_obj_freeze(mrb, program_name);
  mrb_gv_set(mrb, mrb_intern_cstr(mrb, "$PROGRAM_NAME"), program_name);

  mrb_load(mrb, argv[1]);

  if(mrb->exc) {
    if(!mrb_obj_is_kind_of(mrb, mrb_obj_value(mrb->exc), mrb_class_get(mrb, "SystemExit"))) {
      mrb_print_error(mrb);
    }
  }

  mrb_close(mrb);

  return mrb->exc != NULL;
}
