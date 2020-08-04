#include <mruby.h>

mrb_bool mrb_load(mrb_state* mrb, const char* const path);
mrb_value mrb_f_load(mrb_state* mrb, mrb_value self);

mrb_bool mrb_require_relative(mrb_state* mrb, const char* const relative_path);
mrb_value mrb_f_require_relative(mrb_state* mrb, mrb_value self);

void mrb_require_init(mrb_state* mrb);
