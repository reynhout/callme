#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ruby.h>

typedef struct stf_data {
  void *cb;
  int s;
  } stf_data;

void fire_cb(struct stf_data *stf)
  {
  rb_funcall((int)stf->cb, rb_intern("call"), 1, INT2FIX(stf->s));
  }

static void sleep_then_fire(struct stf_data *stf)
  {
  printf("(stf:%d)", stf->s); fflush(stdout);
  sleep(stf->s);
  rb_thread_create((void *)fire_cb, stf);
  }

static void start_stf(VALUE self, void *cb, VALUE s)
  {
  pthread_t a_thr;
  struct stf_data *stf;

  stf = (stf_data *)malloc(sizeof(stf_data));

  stf->cb = cb;
  stf->s = FIX2INT(s);

  pthread_create(&a_thr, NULL, (void *(*)(void *))sleep_then_fire, stf);
  }

void Init_callme() {
  VALUE myModule = rb_define_module("CallMeModule");
  VALUE myClass = rb_define_class_under(myModule, "CallMe", rb_cObject); 
  rb_define_module_function(myClass, "start_stf", (void *)&start_stf, 2); 
  }

