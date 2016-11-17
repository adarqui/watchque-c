#ifndef WATCH_H
#define WATCH_H

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ev.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ret.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <regex.h>
#include <hiredis/hiredis.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>

#define MAX_EVENTS 500000
#define MAX_BUCKETS MAX_EVENTS+1	// flaccid
#define BUF_LEN (sizeof(struct inotify_event)*MAX_EVENTS)

typedef enum {
	WATCH_MASK_CREATE = IN_CREATE | IN_MOVED_TO,
	WATCH_MASK_UPDATE = IN_MODIFY | IN_ATTRIB,
	WATCH_MASK_DELETE = IN_DELETE | IN_DELETE_SELF,
	WATCH_MASK_RENAME = IN_MOVE_SELF | IN_MOVED_FROM,
	WATCH_MASK_CLOSE_WRITE = IN_CLOSE_WRITE
} WATCH_MASK;

typedef enum {
	WATCH_CITIZEN_PARENT = 1,
	WATCH_CITIZEN_CHILD
} watch_citizen_t;

typedef struct watch {
	char *dest;
	char *class;
	char *queue;
	char *queue_pre_formatted;
	char *events;
	char *sources;
	char *filter;
	int dest_type;
	int mask;
	int wd;
	// space hack -> allocate this once, share it among all 'children' of the parent watch_t
	regex_t *filter_re;
	watch_citizen_t member;
	unsigned char recursive;
} watch_t;

typedef struct redis {
	char *host;
	int port;
	int db; // optional database
	// h = handle
	redisContext *h;
} redis_t;

typedef enum {
	POST_FLAG_RPUSH,
    POST_FLAG_LPUSH,
    POST_FLAG_PUBLISH
} POST_FLAG;

typedef struct flags {
	unsigned char d;
    unsigned char * meta;
    POST_FLAG post_method;
} flags_t;

typedef struct watch_lim {
	int x;
	int y;
	int n;
} watch_lim_t;

typedef struct stats {
	time_t t;
	unsigned long long goodEvent;
	unsigned long long badEvent;
	unsigned long long goodFilter;
	unsigned long long badFilter;
	unsigned long long redisReConnect;
	unsigned long long dirAdded;
	unsigned long long dirRemoved;
	unsigned long long cbCalled;
	unsigned long long noWatcher;
	unsigned long long zeroMask;
	unsigned long long goodRead;
	unsigned long long badRead;
	unsigned long long badIoctl;
	unsigned long long fionreadBytes;
	unsigned long long readBytes;
	unsigned long long watchers;
} stats_t;

typedef struct blob {
	int wfd;
	watch_t *w[MAX_BUCKETS + 1];
	watch_lim_t wl;
	redis_t *r;
	flags_t f;
	stats_t *s;
} blob_t;

typedef struct list_elm {
	struct list_elm *next;
	void *data;
} list_elm_t;

typedef struct list {
	list_elm_t *head;
	list_elm_t *tail;
	int size;
} list_t;

extern int errno;
stats_t global_stats;

ret_t watch_cb(blob_t *, char *, int);
ret_t watch_loop(blob_t *);
ret_t watch_open(blob_t *, watch_t *);
//ret_t watch_open_from_list(int, list_t *);
//ret_t watch_assign_to_blob_from_list(blob_t *, list_t *);
ret_t watch_init(watch_citizen_t, watch_t *, char *, char *, char *, char *,
		 char *, int, int);
ret_t watch_cloneRef(watch_t *, char *);
ret_t watch_fini(watch_t *);
ret_t watch_put(blob_t *, watch_t *);
ret_t watch_get(blob_t *, int);
ret_t watch_delete(blob_t *, watch_t *);
ret_t watch_add_directory(blob_t *, watch_t *, char *);

#define DEBUG(fmt, ...) \
    do { if (flags.d > 0) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#include "main.h"
#include "dump.h"
#include "parse.h"
#include "inotify.h"
#include "redis.h"
#include "list.h"
#include "stats.h"
#include "sig.h"

#endif
