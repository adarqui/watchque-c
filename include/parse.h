#ifndef PARSE_H
#define PARSE_H

#include "watch.h"

ret_t parse_redis(char *);
ret_t parse_event_masks(char *, int *);
ret_t parse_watch(char *);
ret_t parse_watch_multi(list_t *, char *);
ret_t parse_watch_single(list_t *, char *, char *, char *, char *, char *, int, int);
ret_t parse_flag(flags_t *, char *);

#endif
