#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <mruby/string.h>

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

extern mrb_value mrb_thread_migrate_value(mrb_state*, mrb_value const, mrb_state*);

static void
check_pthread_error(mrb_state *mrb, int res) {
  if (res == 0) { return; }
  mrb_raise(mrb, mrb_class_get(mrb, "ThreadError"), strerror(res));
}

typedef struct {
  pthread_mutex_t mutex;

  pthread_mutex_t read_lock;
  pthread_cond_t read_cond;

  pthread_mutex_t write_lock;
  pthread_cond_t write_cond;

  mrb_state* mrb;

  mrb_int reads_waiting;
  mrb_int writes_waiting;
  mrb_int size_limit;

  mrb_value queue;
} mrb_sized_queue_context;

static void
mrb_sized_queue_context_free(mrb_state *mrb, void *ptr) {
  if(ptr) {
    mrb_sized_queue_context* context = (mrb_sized_queue_context*) ptr;
    pthread_cond_destroy(&context->read_cond);
    pthread_cond_destroy(&context->write_cond);
    pthread_mutex_destroy(&context->mutex);
    pthread_mutex_destroy(&context->read_lock);
    pthread_mutex_destroy(&context->write_lock);
    free(ptr);
  }
}

static const struct mrb_data_type mrb_sized_queue_context_type = {
  "mrb_sized_queue_context", mrb_sized_queue_context_free,
};

static mrb_value
mrb_sized_queue_init(mrb_state* mrb, mrb_value self) {
  mrb_int size_limit;

  mrb_sized_queue_context* context = (mrb_sized_queue_context*) malloc(sizeof(mrb_sized_queue_context));

  mrb_get_args(mrb, "i", &size_limit);

  if(size_limit <= 0) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "queue size must be positive");
  }

  check_pthread_error(mrb, pthread_mutex_init(&context->mutex, NULL));
  check_pthread_error(mrb, pthread_cond_init(&context->read_cond, NULL));
  check_pthread_error(mrb, pthread_cond_init(&context->write_cond, NULL));
  check_pthread_error(mrb, pthread_mutex_init(&context->read_lock, NULL));
  check_pthread_error(mrb, pthread_mutex_init(&context->write_lock, NULL));

  context->mrb = mrb;
  context->reads_waiting = 0;
  context->writes_waiting = 0;
  context->size_limit = size_limit;
  context->queue = mrb_ary_new_capa(mrb, size_limit);

  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "queue"), context->queue);

  mrb_data_init(self, context, &mrb_sized_queue_context_type);
  fprintf(stderr, "******** Sized queue init: %s\n", RSTRING_PTR(mrb_inspect(mrb, self)));

  check_pthread_error(mrb, pthread_mutex_lock(&context->read_lock));

  return self;
}

static mrb_value
mrb_queue_lock(mrb_state* mrb, mrb_value self) {
  mrb_sized_queue_context* context = DATA_PTR(self);
  check_pthread_error(mrb, pthread_mutex_lock(&context->mutex));
  return mrb_nil_value();
}

static mrb_value
mrb_queue_unlock(mrb_state* mrb, mrb_value self) {
  mrb_sized_queue_context* context = DATA_PTR(self);
  check_pthread_error(mrb, pthread_mutex_unlock(&context->mutex));
  return mrb_nil_value();
}

static mrb_value
mrb_sized_queue_clear(mrb_state* mrb, mrb_value self) {
  mrb_sized_queue_context* context = DATA_PTR(self);

  mrb_queue_lock(mrb, self);
  mrb_ary_clear(mrb, context->queue);
  mrb_queue_unlock(mrb, self);

  return mrb_nil_value();
}

static mrb_value
mrb_sized_queue_push(mrb_state* mrb, mrb_value self) {
  mrb_value arg;
  mrb_bool nonblock;
  mrb_sized_queue_context* context = DATA_PTR(self);
  mrb_int len;

  mrb_get_args(mrb, "o|b", &arg, &nonblock);

  mrb_queue_lock(mrb, self);
  len = RARRAY_LEN(context->queue);
  context->writes_waiting++;
  mrb_queue_unlock(mrb, self);

  if(len >= context->size_limit) {
    if(nonblock) {
      mrb_raise(mrb, mrb_class_get(mrb, "ThreadError"), "queue full");
    } else {
      check_pthread_error(mrb, pthread_cond_wait(&context->write_cond, &context->write_lock));
    }
  }

  mrb_queue_lock(mrb, self);
  context->writes_waiting--;
  mrb_ary_push(context->mrb, context->queue, mrb_thread_migrate_value(mrb, arg, context->mrb));
  mrb_queue_unlock(mrb, self);

  check_pthread_error(mrb, pthread_cond_signal(&context->read_cond));

  return mrb_nil_value();
}

static mrb_value
mrb_sized_queue_unshift(mrb_state* mrb, mrb_value self) {
  mrb_value arg;
  mrb_bool nonblock;
  mrb_sized_queue_context* context = DATA_PTR(self);
  mrb_int len;

  mrb_get_args(mrb, "o|b", &arg, &nonblock);

  mrb_queue_lock(mrb, self);
  len = RARRAY_LEN(context->queue);
  context->writes_waiting++;
  mrb_queue_unlock(mrb, self);

  if(len >= context->size_limit) {
    if(nonblock) {
      mrb_raise(mrb, mrb_class_get(mrb, "ThreadError"), "queue full");
    } else {
      check_pthread_error(mrb, pthread_cond_wait(&context->write_cond, &context->write_lock));
    }
  }

  mrb_queue_lock(mrb, self);
  context->writes_waiting--;
  fprintf(stderr, "\t**** %s\n", RSTRING_PTR(mrb_inspect(mrb, arg)));
  mrb_ary_unshift(context->mrb, context->queue, mrb_thread_migrate_value(mrb, arg, context->mrb));
  mrb_queue_unlock(mrb, self);

  check_pthread_error(mrb, pthread_cond_signal(&context->read_cond));

  return mrb_nil_value();
}

static mrb_value
mrb_sized_queue_pop(mrb_state* mrb, mrb_value self) {
  mrb_value ret;
  mrb_sized_queue_context* context = DATA_PTR(self);
  mrb_bool nonblock;
  int len;

  mrb_get_args(mrb, "|b", &nonblock);

  mrb_queue_lock(mrb, self);
  len = RARRAY_LEN(context->queue);
  context->reads_waiting++;
  mrb_queue_unlock(mrb, self);

  if (len == 0) {
    if(nonblock) {
      mrb_raise(mrb, mrb_class_get(mrb, "ThreadError"), "queue empty");
    } else {
      check_pthread_error(mrb, pthread_cond_wait(&context->read_cond, &context->read_lock));
    }
  }

  mrb_queue_lock(mrb, self);
  context->reads_waiting--;
  ret = mrb_thread_migrate_value(context->mrb, mrb_ary_pop(context->mrb, context->queue), mrb);
  mrb_queue_unlock(mrb, self);

  check_pthread_error(mrb, pthread_cond_signal(&context->write_cond));

  return ret;
}

static mrb_value
mrb_sized_queue_shift(mrb_state* mrb, mrb_value self) {
  mrb_value ret;
  mrb_sized_queue_context* context = DATA_PTR(self);
  mrb_bool nonblock;
  int len;

  mrb_get_args(mrb, "|b", &nonblock);

  mrb_queue_lock(mrb, self);
  len = RARRAY_LEN(context->queue);
  context->reads_waiting++;
  mrb_queue_unlock(mrb, self);

  if (len == 0) {
    if(nonblock) {
      mrb_raise(mrb, mrb_class_get(mrb, "ThreadError"), "queue empty");
    } else {
      check_pthread_error(mrb, pthread_cond_wait(&context->read_cond, &context->read_lock));
    }
  }

  mrb_queue_lock(mrb, self);
  context->reads_waiting--;
  ret = mrb_thread_migrate_value(context->mrb, mrb_ary_shift(context->mrb, context->queue), mrb);
  mrb_queue_unlock(mrb, self);

  check_pthread_error(mrb, pthread_cond_signal(&context->write_cond));

  return ret;
}

static mrb_value
mrb_sized_queue_limit(mrb_state* mrb, mrb_value self) {
  mrb_int ret;
  mrb_sized_queue_context* context = DATA_PTR(self);

  ret = context->size_limit;

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_sized_queue_size(mrb_state* mrb, mrb_value self) {
  mrb_int ret;
  mrb_sized_queue_context* context = DATA_PTR(self);

  mrb_queue_lock(mrb, self);
  ret = RARRAY_LEN(context->queue);
  mrb_queue_unlock(mrb, self);

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_sized_queue_num_waiting(mrb_state* mrb, mrb_value self) {
  mrb_value num_waiting;
  mrb_sized_queue_context* context = DATA_PTR(self);

  mrb_queue_lock(mrb, self);
  num_waiting = mrb_fixnum_value(context->reads_waiting + context->writes_waiting);
  mrb_queue_unlock(mrb, self);

  return num_waiting;
}

static mrb_value
mrb_sized_queue_empty_p(mrb_state* mrb, mrb_value self) {
  mrb_bool ret;
  mrb_sized_queue_context* context = DATA_PTR(self);

  mrb_queue_lock(mrb, self);
  ret = RARRAY_LEN(context->queue) == 0;
  mrb_queue_unlock(mrb, self);

  return mrb_bool_value(ret);
}

void
mrb_mruby_thread_sized_queue_init(mrb_state* mrb) {
  struct RClass* sized_queue;

  sized_queue = mrb_define_class(mrb, "SizedQueue", mrb->object_class);
  MRB_SET_INSTANCE_TT(sized_queue, MRB_TT_DATA);
  mrb_define_method(mrb, sized_queue, "initialize", mrb_sized_queue_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, sized_queue, "clear", mrb_sized_queue_clear, MRB_ARGS_NONE());

  mrb_define_method(mrb, sized_queue, "push", mrb_sized_queue_push, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, sized_queue, "unshift", mrb_sized_queue_unshift, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_alias(mrb, sized_queue, "<<", "push");
  mrb_define_alias(mrb, sized_queue, "enq", "unshift");

  mrb_define_method(mrb, sized_queue, "pop", mrb_sized_queue_pop, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, sized_queue, "shift", mrb_sized_queue_shift, MRB_ARGS_OPT(1));
  mrb_define_alias(mrb, sized_queue, "deq", "pop");

  mrb_define_method(mrb, sized_queue, "limit", mrb_sized_queue_limit, MRB_ARGS_NONE());
  mrb_define_alias(mrb, sized_queue, "max", "limit");

  mrb_define_method(mrb, sized_queue, "size", mrb_sized_queue_size, MRB_ARGS_NONE());
  mrb_define_method(mrb, sized_queue, "num_waiting", mrb_sized_queue_num_waiting, MRB_ARGS_NONE());
  mrb_define_method(mrb, sized_queue, "empty?", mrb_sized_queue_empty_p, MRB_ARGS_NONE());
}
