#include <mruby.h>
#include <mruby/time.h>

#include <time.h>

static mrb_int
utc_offset(mrb_state* mrb, mrb_value time) {
  time_t local_epoch;
  time_t utc_epoch;

  struct tm time_data;

  time_data.tm_year = mrb_fixnum(mrb_funcall(mrb, time, "year", 0));
  time_data.tm_mon = mrb_fixnum(mrb_funcall(mrb, time, "month", 0));
  time_data.tm_mday = mrb_fixnum(mrb_funcall(mrb, time, "day", 0));
  time_data.tm_hour = mrb_fixnum(mrb_funcall(mrb, time, "hour", 0));
  time_data.tm_min = mrb_fixnum(mrb_funcall(mrb, time, "min", 0));
  time_data.tm_sec = mrb_fixnum(mrb_funcall(mrb, time, "sec", 0));
  time_data.tm_isdst = mrb_bool(mrb_funcall(mrb, time, "dst?", 0)) == TRUE ? 1 : 0;

  local_epoch = timelocal(&time_data);
  utc_epoch = timegm(&time_data);

  return utc_epoch - local_epoch;
}

static mrb_value
mrb_time_utc_offset(mrb_state* mrb, mrb_value time) {
  mrb_value utc;

  utc = mrb_funcall(mrb, time, "utc?", 0);

  if(mrb_bool(utc) == TRUE) {
    return mrb_fixnum_value(0);
  } else {
    return mrb_fixnum_value(utc_offset(mrb, time));
  }
}

void
mrb_mruby_time_iso8601_gem_init(mrb_state* mrb) {
  struct RClass* time_class;

  time_class = mrb_class_get(mrb, "Time");

  mrb_define_method(mrb, time_class, "utc_offset", mrb_time_utc_offset, MRB_ARGS_NONE());
  mrb_define_alias(mrb, time_class, "gmt_offset", "utc_offset");

  return;
}

void
mrb_mruby_time_iso8601_gem_final(mrb_state* mrb) {
  return;
}
