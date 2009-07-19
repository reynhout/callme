#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ruby.h>

typedef struct watcher_data {
  char *path;
  void *cb;
  int s;
  } watcher_data;

static void cb_native(struct watcher_data *w)
  { printf("(C:%d)", w->s); fflush(stdout); }

static void cb_ruby(struct watcher_data *w)
  { rb_funcall((int)w->cb, rb_intern("call"), 1, INT2FIX(w->s)); }

static void do_watch(struct watcher_data *w)
  {
  struct stat statbuf;
  time_t time0 = time(NULL);

  while(1)
    {
    if (stat(w->path, &statbuf) == 0)
      {
      w->s = statbuf.st_mtime-time0;
      unlink(w->path);
      rb_thread_create((void *)cb_ruby, w);
      }
    sleep(1);
    }
  }

static void watch_path(VALUE self, VALUE v_path, void *cb_fp)
  {
  char *path = StringValuePtr(v_path);

  pthread_t watcher_thr;
  struct watcher_data *w;

  w = (watcher_data *)malloc(sizeof(watcher_data));
  w->path = strdup(path);
  w->cb = cb_fp;
  printf("-- Using callback at 0x%08x\n", (unsigned int)w->cb);
  printf("-- Trigger with:\n   touch %s/%s\n", getcwd(NULL, 0), w->path);
  pthread_create(&watcher_thr, NULL, (void *(*)(void *))do_watch, w);
  }

void Init_callme() {
  VALUE myModule = rb_define_module("CallMeModule");
  VALUE myClass = rb_define_class_under(myModule, "CallMe", rb_cObject); 
  rb_define_module_function(myClass, "watch_path", (void *)&watch_path, 2); 
  }

int main(void)
  {
  int i;
  char *path = "somefile";

  watch_path(0, (VALUE)path, (void *)cb_native);
  for ( i=0; i<=30; i++ )
    { printf("."); fflush(stdout); sleep(1); }
  printf("done\n");
  return EXIT_SUCCESS;
  }
