#include "watch.h"

ret_t watch_fini(watch_t * w)
{
	RET_INIT;

	if (!w) {
		RET_ERROR("watch_fini invalid");
	}

	if (w->member == WATCH_CITIZEN_PARENT) {
		if (w->class) {
			free(w->class);
		}
		if (w->queue) {
			free(w->queue);
		}
		if (w->events) {
			free(w->events);
		}
/*
  if(w->sources) {
   free(w->sources);
  }
*/
		if (w->filter) {
			free(w->filter);
		}
		if (w->filter_re) {
			regfree(w->filter_re);
		}
		if (w->queue_pre_formatted) {
			free(w->queue_pre_formatted);
		}
	}

	/* sources is allocated for both child and parent */
	if (w->sources) {
		free(w->sources);
	}

	free(w);
	// NO LONGER SAFE..

	RET_OK(NULL);
}

ret_t watch_cloneRef(watch_t * wp, char *source)
{
	watch_t *wc;
	RET_INIT;

	if (!wp || !source) {
		RET_ERROR("watch_cloneRef invalid arguments");
	}

	wc = (watch_t *) calloc(1, sizeof(watch_t));
	if (!wc) {
		errx(1, "watch_cloneRef: calloc failed");
	}

	wc->dest = wp->dest;
	wc->class = wp->class;
	wc->queue = wp->queue;
	wc->queue_pre_formatted = wp->queue_pre_formatted;
	wc->events = wp->events;
	wc->sources = strdup(source);
	wc->filter = wp->filter;
	wc->filter_re = wp->filter_re;
	wc->dest_type = wp->dest_type;
	wc->mask = wp->mask;
	wc->wd = 0;
	wc->member = WATCH_CITIZEN_CHILD;
	wc->recursive = wp->recursive;

	RET_OK(wc);
}

ret_t watch_init(watch_citizen_t citizen, watch_t * wp, char *class,
		 char *queue, char *events, char *source, char *filter,
		 int mask, int recursive)
{
	watch_t *w;
	RET_INIT;

	if (!class || !queue || !events || !source) {
		RET_ERROR("watch_init invalid");
	}

	if (citizen == WATCH_CITIZEN_CHILD) {
		return watch_cloneRef(wp, source);
	}

	w = (watch_t *) calloc(1, sizeof(watch_t));
	if (!w) {
		RET_ERROR("calloc");
	}

	w->class = strdup(class);
	w->queue = strdup(queue);
	w->events = strdup(events);
	w->sources = strdup(source);
	if (filter) {
		regex_t *filter_re;
		int rgx;
		w->filter = strdup(filter);
		filter_re = (regex_t *) calloc(1, sizeof(regex_t));
		/*
		 * just consider these fatal immediately, for now
		 */
		if (!filter_re) {
			errx(1, "watch_init: calloc failed");
		}
		rgx = regcomp(filter_re, w->filter, 0);
		if (rgx) {
			errx(1, "watch_init: regcomp failed");
		}
		w->filter_re = filter_re;
	}

	do {
		char buf[strlen("resque:queue") + 1 + strlen(w->sources) + 1];
		int n;
		n = snprintf(buf, sizeof(buf) - 1, "%s:%s", "resque:queue",
			     w->queue);
		buf[n] = '\0';
		w->queue_pre_formatted = strdup(buf);
	} while (0);

	w->mask = mask;
	w->member = WATCH_CITIZEN_PARENT;
	w->recursive = recursive;

	RET_OK(w);
}

ret_t watch_cb(blob_t * b, char *buf, int n)
{
	struct inotify_event *ie;
	struct stat st;
	watch_t *w;
	int ni = 0, ri;
	RET_INIT;

	stat_inc_cbCalled(b->s);

	// initiate multi
	r_multi(b->r);

	while (ni < n) {

		ie = (struct inotify_event *) (buf + ni);
		ni += sizeof(struct inotify_event) + ie->len;

//dump_inotify_event(ie);

		_r = watch_get(b, ie->wd);
		if (RET_ISOK != RET_BOOL_TRUE) {
			stat_inc_noWatcher(b->s);
			continue;
		}

		w = (watch_t *) RET_V;
		if (!w) {
			stat_inc_noWatcher(b->s);
			continue;
		}
//dump_watch(w);
//dump_inotify_event(ie);

		if (ie->wd < 0) {
			dump_inotify_event(ie);
			stat_inc_badEvent(b->s);
			continue;
		}

		if (ie->len <= 0) {
			dump_inotify_event(ie);
			stat_inc_badEvent(b->s);
			continue;
		} else {

			if (!w) {
				stat_inc_noWatcher(b->s);
				continue;
			}

			if (w->mask == 0) {
				stat_inc_zeroMask(b->s);
				continue;
			}

			stat_inc_goodEvent(b->s);

			if (1) {
				char full_path[strlen(w->sources) + 1 +
					       strlen(ie->name) + 1 + 2];
				int n;
				n = snprintf(full_path, sizeof(full_path) - 1,
					     "%s/%s", w->sources, ie->name);
				full_path[n] = '\0';

				if (w->recursive) {
					/* Manage the dynamic creation/deletion of directories */
					int suc;
					suc = stat(full_path, &st);
					if (ie->mask & WATCH_MASK_DELETE) {
						/* A folder we no longer need to watch */
						if (suc <= 0) {
							if (!strcmp
							    (w->sources,
							     full_path)) {
								watch_delete(b,
									     w);
								watch_fini(w);
								stat_inc_dirRemoved
								    (b->s);
								continue;
							}
						}
					} else if (ie->mask & WATCH_MASK_CREATE) {
						/* A new folder ot watch */
						if (suc >= 0) {
							_r = watch_add_directory
							    (b, w, full_path);
							if (RET_ISOK !=
							    RET_BOOL_TRUE) {
								printf
								    ("watch_cb: watch_add_directory: error: %s\n",
								     (char *)
								     RET_V);
							}
						}
					}
				}

				if (w->filter_re) {
					ri = regexec(w->filter_re, full_path, 0,
						     NULL, 0);
					if (!ri) {
						/*
						 * Match
						 */
						stat_inc_goodFilter(b->s);
					} else {
						stat_inc_badFilter(b->s);
						continue;
					}
				}

				if (ie->mask & w->mask) {
					_r = r_enqueue(b->r, w, ie, b->f);
				}

			}
		}
	}

	// exec
	r_exec(b->r);

	RET_OK(NULL);
}

ret_t watch_loop(blob_t * b)
{
	static fd_set rfds;
	int n, nie, rc = 0;
	RET_INIT;

	FD_ZERO(&rfds);
	FD_SET(b->wfd, &rfds);
	while (1) {
		rc = select(b->wfd + 1, &rfds, NULL, NULL, NULL);
		rc = ioctl(b->wfd, FIONREAD, &n);
		if (rc < 0) {
			stat_inc_badIoctl(b->s);
		}
		stat_inc_fionreadBytes(b->s, n);
		do {
			char buf[n + 1];
			nie = read(b->wfd, buf, n);

			if (nie < 0) {
				stat_inc_badRead(b->s);
				continue;
			}
			stat_inc_goodRead(b->s);
			stat_inc_readBytes(b->s, nie);
			watch_cb(b, buf, nie);
		} while (0);
	}

	RET_OK(NULL);
}

ret_t watch_open(blob_t * b, watch_t * w)
{
	int mask_wrapped;
	RET_INIT;
	if (!b || !w) {
		RET_ERROR("w = null");
	}
	mask_wrapped = w->mask;
	if (w->recursive) {
		mask_wrapped =
		    mask_wrapped | WATCH_MASK_CREATE | WATCH_MASK_DELETE;
	}
	w->wd = inotify_add_watch(b->wfd, w->sources, mask_wrapped);
	RET_OK(NULL);
}

ret_t watch_get(blob_t * b, int wd)
{
	/* returns a watch_t * */
	watch_t *w;
	RET_INIT;

	if (!b || wd < 0) {
		RET_ERROR("watch_get: invalid arguments");
	}

	w = b->w[wd];
	if (!w) {
		RET_ERROR("watch_get: no watchers found");
	}

	RET_OK(w);
}

ret_t watch_put(blob_t * b, watch_t * w)
{
	watch_t *wn;
	RET_INIT;

	if (!b || !w) {
		RET_ERROR("watch_put: invalid arguments");
	}

	wn = (watch_t *) b->w[w->wd];
	if (wn) {
		RET_ERROR("watch_put: watcher already exists");
	}

	b->w[w->wd] = w;
	stat_inc_watchers(b->s);

	RET_OK(NULL);
}

ret_t watch_delete(blob_t * b, watch_t * w)
{
	RET_INIT;

	if (!b || !w) {
		RET_ERROR("watch_delete: invalid arguments");
	}

	b->w[w->wd] = NULL;
	inotify_rm_watch(b->wfd, w->wd);

	stat_dec_watchers(b->s);

	RET_OK(NULL);
}

ret_t watch_add_directory(blob_t * b, watch_t * w, char *path)
{
	RET_INIT;

	if (!b || !path) {
		RET_ERROR("watch_add_directory: invalid arguments");
	}

	_r = parse_watch_single(b, w->member, w, w->class, w->queue, w->events,
				path, w->filter, w->filter_re, w->mask, 0,
				w->recursive);
	if (RET_ISOK != RET_BOOL_TRUE) {
		RET_ERROR("watch_add_directory: parse_watch_single: error");
	}

	RET_OK(NULL);
}
