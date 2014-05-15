#ifndef REDIS_H
#define REDIS_H

#include "watch.h"

ret_t sadd(redisContext *, watch_t *);
ret_t enqueue(redisContext *, watch_t *, struct inotify_event *);

#endif
