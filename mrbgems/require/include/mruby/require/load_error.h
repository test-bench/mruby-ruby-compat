#ifndef MRUBY_REQUIRE_LOAD_ERROR_H
#define MRUBY_REQUIRE_LOAD_ERROR_H

#include <mruby.h>

#define E_LOAD_ERROR (mrb_class_get(mrb, "LoadError"))

mrb_noreturn void mrb_raise_load_error(mrb_state*, const char* const);

#endif /* MRUBY_REQUIRE_LOAD_ERROR_H */
