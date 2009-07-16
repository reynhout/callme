#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ruby.h>

typedef struct stf_data {
  void* cb;
  int s;
  } stf_data;

static void sleep_then_fire(struct stf_data* stf)
  {
  int x = 99;

  printf("(%d:sleeping)",stf->s); fflush(stdout);
  sleep(stf->s);
  printf("(%d:funcalling cb)",stf->s), fflush(stdout);
  rb_funcall((int)stf->cb,rb_intern("call"),1,INT2FIX(x));
  printf("(%d:cb returned)",stf->s), fflush(stdout);
  rb_thread_sleep_forever();
  //pthread_exit(NULL);
  }

static void start_stf(VALUE self, void(*cb)(void), VALUE s)
  {
  pthread_t watcher_thr;
  struct stf_data* stf;

  stf = (stf_data *)malloc(sizeof(stf_data));

  stf->cb = cb;
  stf->s = FIX2INT(s);

  printf("(%d:creating thread)",stf->s), fflush(stdout);
  pthread_create(&watcher_thr, NULL, (void*(*)(void*))sleep_then_fire, stf);
  printf("(%d:thread created)",stf->s), fflush(stdout);
  }

void Init_callme() {
  VALUE myModule = rb_define_module("CallMeModule");
  VALUE myClass = rb_define_class_under(myModule, "CallMe", rb_cObject); 
  rb_define_module_function(myClass, "start_stf", (void *)&start_stf, 2); 
  }

