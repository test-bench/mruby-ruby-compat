#include <mruby.h>

#include "debug.h"
#include "require.h"

int main(int argc, char *argv[]) {
  int index;
  mrb_state *mrb;

  if(argc == 1) {
    fprintf(stderr, "Error: no input files\n");
    return 1;
  }

  mrb = mrb_open();
  if (!mrb) {
    fprintf(stderr, "Error: could not initialize mruby\n");
    return 2;
  }

  for(index = 1; index < argc; index++) {
    mrb_load(mrb, argv[index]);

    if(mrb->exc) {
      mrb_print_error(mrb);
      break;
    }
  }

  mrb_close(mrb);

  return mrb->exc != NULL;
}
