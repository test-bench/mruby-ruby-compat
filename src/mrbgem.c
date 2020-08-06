#include <mruby.h>
#include <mruby/variable.h>

#include <mruby_local/abort.h>
#include <mruby_local/require.h>

#include <stdio.h>

#include "debug.h"

void mrb_mruby_local_gem_init(mrb_state *mrb) {
  mrb_abort_init(mrb);
  mrb_require_init(mrb);

  return;
}

void mrb_mruby_local_gem_final(mrb_state *mrb) {
  mrb_require_final(mrb);
  mrb_abort_final(mrb);

  debug_printf("Finished\n");

  return;
}
