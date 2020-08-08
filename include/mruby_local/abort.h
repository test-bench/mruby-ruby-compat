#include <mruby.h>


#define E_SYSTEM_EXIT (mrb_class_get(mrb, "SystemExit"))


mrb_value mrb_f_abort(mrb_state* mrb, mrb_value self);

void mrb_abort_init(mrb_state* mrb);
void mrb_abort_final(mrb_state* mrb);
