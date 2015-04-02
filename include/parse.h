#ifndef PARSE_H
#define PARSE_H

#include "watch.h"

ret_t parse_redis(char *);
ret_t parse_event_masks(char *, int *);
ret_t parse_watch(char *);
ret_t parse_watch_multi(blob_t *, char *);
ret_t parse_watch_single(blob_t *, watch_citizen_t, watch_t *, char *, char *,
			 char *, char *, char *, regex_t *, int, int, int);
ret_t parse_flag(flags_t *, char *);

#endif
