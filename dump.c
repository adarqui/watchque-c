#include "watch.h"


void dump_stats(stats_t *s) {
 if(!s) {
  return;
 }

 printf(
  "\nDumping stats:\n"
  "\tgoodEvent: %llu\n"
  "\tbadEvent: %llu\n"
  "\tgoodFilter: %llu\n"
  "\tbadFilter: %llu\n"
  "\tredisReConnect: %llu\n"
  "\tdirAdded: %llu\n"
  "\tdirRemoved: %llu\n"
  "\tcbCalled: %llu\n"
  "\tgoodEvent / cbCalled: %f\n"
  "\tnoWatcher: %llu\n"
  "\tzeroMask: %llu\n"
  "\tgoodRead: %llu\n"
  "\tbadRead: %llu\n"
  "\tbadIoctl: %llu\n"
  "\tfionreadBytes: %llu\n"
  "\treadBytes: %llu\n"
  ,
  s->goodEvent,
  s->badEvent,
  s->goodFilter,
  s->badFilter,
  s->redisReConnect,
  s->dirAdded,
  s->dirRemoved,
  s->cbCalled,
  s->cbCalled == 0 ? 0 : (float)s->goodEvent / (float)s->cbCalled,
  s->noWatcher,
  s->zeroMask,
  s->goodRead,
  s->badRead,
  s->badIoctl,
  s->fionreadBytes,
  s->readBytes
 );

 return;
}


void dump_inotify_event(struct inotify_event *ie) {
 if(!ie) {
  return;
 }
 printf(
  "Dumping inotify_event:\n"
  "\twd: %i\n"
  "\tmask: %i\n"
  "\tcookie: %i\n"
  "\tlen: %i\n"
  "\tname: %s\n",
  ie->wd,
  ie->mask,
  ie->cookie,
  ie->len,
  ie->name
 );

 return;
}


void dump_watch(watch_t *w) {
 if(!w) {
  return;
 }
 printf(
  "Dumping watch:\n"
  "\tclass: %s\n"
  "\tqueue: %s\n"
  "\tqueue_pre_formatted: %s\n"
  "\tevents: %s\n"
  "\tsources: %s\n"
  "\tfilter: %s\n"
  "\tfilter_re: %p\n"
  "\tdest_type: %i\n"
  "\tmask: %i\n"
  "\tcitizen: %i\n"
  ,
  w->class,
  w->queue,
  w->queue_pre_formatted,
  w->events,
  w->sources,
  w->filter,
  w->filter_re,
  w->dest_type,
  w->mask,
  w->member
 );

 return;
}


void dump_list(list_t *ll) {
 list_elm_t *le;

 if(!ll) {
  return;
 }

 puts("Dumping list:");
 for(le = ll->head; le != NULL; le = le->next) {
  dump_watch((watch_t *)le->data);
 }

 return;
}
