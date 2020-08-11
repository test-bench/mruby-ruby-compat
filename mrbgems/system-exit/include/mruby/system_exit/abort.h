#ifndef MRUBY_SYSTEM_EXIT_ABORT_H
#define MRUBY_SYSTEM_EXIT_ABORT_H

#include <mruby.h>

void mrb_noreturn mrb_abort(mrb_state* mrb, mrb_value message);

#endif /* MRUBY_SYSTEM_EXIT_ABORT_H */
