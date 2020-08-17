#include <mruby.h>

mrb_value json_key_to_ruby_symbol(mrb_state*, const char* const);
mrb_value ruby_symbol_to_json_key(mrb_state*, mrb_sym);
