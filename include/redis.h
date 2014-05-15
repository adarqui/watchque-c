#ifndef REDIS_H
#define REDIS_H

#include "watch.h"

ret_t r_sadd(redisContext *, watch_t *);
void r_multi(redisContext *);
void r_exec(redisContext *);
ret_t r_enqueue(redisContext *, watch_t *, struct inotify_event *);

#endif
