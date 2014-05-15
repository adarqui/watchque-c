#include "watch.h"


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
  "\tdest_type: %i\n"
  "\tmask: %i\n"
  ,
  w->class,
  w->queue,
  w->queue_pre_formatted,
  w->events,
  w->sources,
  w->filter,
  w->dest_type,
  w->mask
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
}
