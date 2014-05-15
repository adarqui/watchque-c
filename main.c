#include "watch.h"

/*
"{\"class\":\"class\",\"args\":[{\"filePath\":\"/tmp/k\",\"event\":\"CLOSE_WRITE\"}]}"
*/



int main(int argc, char *argv[], char *env[]) {
// struct ev_loop *loop = ev_default_loop(0);
// ev_io watch_ev;
 list_t *ll;
 blob_t b;
 watch_t *w;
 redis_t *redp;
// flags_t flags;
 int i;
 RET_INIT;

 if(argc < 3) {
  usage();
 }

 /* some maintenance, initialization */
// memset(&flags, 0, sizeof(flags));
 signal(SIGINT, sigint_handler);
 _r = set_max_queued_events(MAX_EVENTS - 1);
 if(RET_ISOK != RET_BOOL_TRUE) {
   errx(1, "main: set_max_queued_events: %s\n", (char *)RET_V);
 }
 memset(&b, 0, sizeof(b));
 b.s = &global_stats;
 _r = list_init();
 ll = RET_V;
// ev_init(&watch_ev, watch_cb);
 b.wfd = inotify_init();
// ev_io_set(&watch_ev, wfd, EV_READ);

 _r = parse_redis(argv[1]);
 if(RET_ISOK != RET_BOOL_TRUE) {
  errx(1, "parse_redis: error: %s\n", (char *)RET_V);
 }

 redp = RET_V;
 b.r = redp;

 for(i=2; i < argc; i++) {
  printf("arg=%s\n", argv[i]);
  if(!strncmp(argv[i], "--", strlen("--"))) {
   _r = parse_flag(&b.f, &argv[i][strlen("--")]);
   if(RET_ISOK != RET_BOOL_TRUE) {
    errx(1, "parse_flag: error: %s\n", (char *)RET_V);
   }
  } else {
/*
   _r = parse_watch(argv[i]);
   if(RET_ISOK != RET_BOOL_TRUE) {
    errx(1, "parse_watch: error: %s\n", (char *)RET_V);
   }
   w = RET_V;
   dump_watch(w);
   _r = watch_open(b.wfd, w);
   if(RET_ISOK != RET_BOOL_TRUE) {
    errx(1, "watch_open: error: %s\n", (char *)RET_V);
   }
//   b.w[w->wd] = w;
   printf("w->wd %i\n", w->wd);
   //sadd(b.r->h, w);
*/
   _r = parse_watch_multi(ll, argv[i]);
   if(RET_ISOK != RET_BOOL_TRUE) {
    errx(1, "parse_watch_multi: error: %s\n", (char *)RET_V);
   }

   w = ll->tail->data;
   /* it's safe to assume that, if parse_watch_multi succeeded, we can sadd that queue */
   r_sadd(b.r->h, w);
  }
 }

 dump_list(ll);

 _r = watch_open_from_list(b.wfd, ll);
 if(RET_ISOK != RET_BOOL_TRUE) {
  errx(1, "watch_open_from_list: error: %s\n", (char *)RET_V);
 }

 _r = watch_assign_to_blob_from_list(&b, ll);
 if(RET_ISOK != RET_BOOL_TRUE) {
  errx(1, "watch_assign_to_blob_from_list: error: %s\n", (char *)RET_V);
 }

 _r = watch_loop(&b);

// ev_io_start(loop, &watch_ev);
// ev_loop(loop, 0);
 return EXIT_SUCCESS;
}
