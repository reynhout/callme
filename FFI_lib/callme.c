#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

typedef void (*sig_t) (int);
typedef struct watcher_struct {
  char *path;
  sig_t callback;
  } watcher_struct;

static void *do_watch(struct watcher_struct *w)
  {
  struct stat statbuf;
  time_t time0;

  time0 = time(NULL);
  printf("(watching)"); fflush(stdout);
  while(1)
    {
    if (stat(w->path,&statbuf) == 0)
      {
      unlink(w->path);
      w->callback(statbuf.st_mtime-time0);
      }
    sleep(1);
    }
  }

void watch_path(char *path, sig_t cbfp, int use_pthreads)
  {
  pthread_t watcher_thr;
  struct watcher_struct *w;
  char cwd[1024];

  w = (watcher_struct *) malloc(sizeof(watcher_struct));
  w->path = strdup(path);
  w->callback = cbfp;

  printf("trigger callback with:\n  touch %s/%s\n",getcwd(cwd,sizeof(cwd)),w->path);
  if ( use_pthreads == 1 )
    {
    printf("creating watcher thread\n");
    pthread_create(&watcher_thr,NULL,(void *(*)(void*))do_watch,(void *)w);
    }
  else
    do_watch(w);
  }

void cb_internal(x)
  { printf("(C:%d)",x); fflush(stdout); }

int main(void)
  {
  int i;

  printf("using C callback cb_internal\n");
  watch_path("somefile",&cb_internal,1);
  for ( i=0; i<=30; i++ )
    { printf("."); fflush(stdout); sleep(1); }
  printf("done\n");
  return EXIT_SUCCESS;
  }
