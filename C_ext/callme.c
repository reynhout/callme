#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ruby.h>

//typedef void* fp(unsigned int);
typedef void (*fp)(int);

typedef struct watcher_struct {
  char* path;
  fp callback;
  } watcher_struct;

static void cb_internal(int x)
  { printf("(C:%d)",x); fflush(stdout); }

static void do_watch(struct watcher_struct* w)
  {
  struct stat statbuf;
  time_t time0;
  int tmp_x;

  time0 = time(NULL);
  printf("(watching)"); fflush(stdout);
  while(1)
    {
    if (stat(w->path,&statbuf) == 0)
      {
      tmp_x = statbuf.st_mtime-time0;
      unlink(w->path);
      printf("(firing callback 0x%08x)", (unsigned int)w->callback);
      fflush(stdout);
      printf("(tmp_x is %d)",tmp_x); fflush(stdout);
      //w->callback(tmp_x);
      rb_thread_blocking_region((rb_blocking_function_t *)w->callback, &tmp_x, RUBY_UBF_IO, 0);
      }
    sleep(1);
    }
  }

VALUE watch_path(VALUE self, VALUE v_path, fp cb_fp, unsigned char use_pthreads)
  {
  char* path = StringValuePtr(v_path);

  pthread_t watcher_thr;
  struct watcher_struct* w;

  w = (watcher_struct *)malloc(sizeof(watcher_struct));
  w->path = strdup(path);
  w->callback = !NIL_P(cb_fp) ? cb_fp : (fp)cb_internal;
  printf("-- Trigger callback with:\n   touch %s/%s\n",getcwd(NULL,0),w->path);
  printf("-- Will use callback at address 0x%08x\n",(unsigned int)w->callback);
  printf("-- use_pthreads is %d\n", use_pthreads);
  fflush(stdout);
  if ( use_pthreads )
    pthread_create(&watcher_thr,NULL,(void*(*)(void*))do_watch,(void*)w);
  else
    do_watch(w);

  return self;
  }

void Init_callme() {
  VALUE myModule = rb_define_module("CallMeModule");
  VALUE myClass = rb_define_class_under(myModule, "CallMe", rb_cObject); 
  rb_define_module_function(myClass, "watch_path", (void *)&watch_path, 3); 
  }

int main(void)
  {
  int i;
  char* path = "somefile";

  printf("-- C callback cb_internal 0x%08x\n", (unsigned int)&cb_internal);
  fflush(stdout);
  watch_path(0,(VALUE)path,(fp)cb_internal,1);
  for ( i=0; i<=30; i++ )
    { printf("."); fflush(stdout); sleep(1); }
  printf("done\n");
  return EXIT_SUCCESS;
  }
