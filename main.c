#include "watch.h"

int main(int argc, char *argv[], char *env[]) {
 blob_t b;
 redis_t *redp;
 int i;
 RET_INIT;

 if(argc < 3) {
  usage();
 }

 /* some maintenance, initialization */
 signal(SIGINT, sigint_handler);

/* old, not needed, but leaving commented
 _r = set_max_queued_events(MAX_EVENTS - 1);
 if(RET_ISOK != RET_BOOL_TRUE) {
   errx(1, "main: set_max_queued_events: %s\n", (char *)RET_V);
 }
*/
 memset(&b, 0, sizeof(b));
 b.s = &global_stats;
 b.wfd = inotify_init();

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

   /* it's safe to assume that, if parse_watch_multi succeeded, we can sadd that queue */
// FIXME
//   r_sadd(b.r, w);
   _r = parse_watch_multi(&b, argv[i]);
   if(RET_ISOK != RET_BOOL_TRUE) {
    errx(1, "main: parse_watch_multi: error: %s\n", (char *)RET_V);
   }
  }
 }

 for(i=0;i<MAX_BUCKETS;i++) {
  if(b.w[i] != NULL) {
   dump_watch(b.w[i]);
  }
 }

 _r = watch_loop(&b);

 return EXIT_SUCCESS;
}
