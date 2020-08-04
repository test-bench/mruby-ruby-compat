#include <mruby.h>
#include <mruby/variable.h>

#include <stdio.h>

#include "debug.h"
#include "require.h"

void mrb_mruby_local_gem_init(mrb_state *mrb) {
  debug_printf("Initializing\n");

  mrb_require_init(mrb);

  return;
}

void mrb_mruby_local_gem_final(mrb_state *mrb) {
  mrb_require_final(mrb);

  debug_printf("Finished\n");

  return;
}
