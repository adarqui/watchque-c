#ifndef DUMP_H
#define DUMP_H

#include "watch.h"

void dump_inotify_event(struct inotify_event *);
void dump_watch(watch_t *);
void dump_list(list_t *);

#endif
