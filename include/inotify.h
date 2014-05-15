#ifndef INOTIFY_H
#define INOTIFY_H

#include "watch.h"

ret_t mask_to_event(int, int);
ret_t event_to_json(watch_t *, struct inotify_event *, char *);
ret_t set_max_queued_events(int);

#endif
