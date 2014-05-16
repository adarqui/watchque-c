#include "watch.h"


ret_t watch_fini(watch_t *w) {
 RET_INIT;

 if(!w) {
  RET_ERROR("watch_fini invalid");
 }

 if(w->member == WATCH_CITIZEN_PARENT) {
  if(w->class) {
   free(w->class);
  }
  if(w->queue) {
   free(w->queue);
  }
  if(w->events) {
   free(w->events);
  }
/*
  if(w->sources) {
   free(w->sources);
  }
*/
  if(w->filter) {
   free(w->filter);
  }
  if(w->filter_re) {
   regfree(w->filter_re);
  }
  if(w->queue_pre_formatted) {
   free(w->queue_pre_formatted);
  }
 }

 /* sources is allocated for both child and parent */
 if(w->sources) {
  free(w->sources);
 }

 free(w);
 // NO LONGER SAFE..

 RET_OK(NULL);
}



ret_t watch_cloneRef(watch_t *wp, char *source) {
 watch_t *wc;
 RET_INIT;

 if(!wp || !source) {
  RET_ERROR("watch_cloneRef invalid arguments");
 }

 wc = (watch_t *) calloc(1, sizeof(watch_t));
 if(!wc) {
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

 RET_OK(wc);
}



ret_t watch_init(watch_citizen_t citizen, watch_t *wp, char *class, char *queue, char *events, char *source, char *filter, int mask) {
 watch_t *w;
 RET_INIT;

 if(!class || !queue || !events || !source) {
  RET_ERROR("watch_init invalid");
 }

 if(citizen == WATCH_CITIZEN_CHILD) {
  return watch_cloneRef(wp, source);
 }

 w = (watch_t *) calloc(1, sizeof(watch_t));
 if(!w) {
  RET_ERROR("calloc");
 }

 w->class = strdup(class);
 w->queue = strdup(queue);
 w->events = strdup(events);
 w->sources = strdup(source);
 if(filter) {
  regex_t *filter_re;
  int rgx;
  w->filter = strdup(filter);
  filter_re = (regex_t *) calloc(1, sizeof(regex_t));
  /*
   * just consider these fatal immediately, for now
   */
  if(!filter_re) {
   errx(1, "watch_init: calloc failed");
  }
  rgx = regcomp(filter_re, w->filter, 0);
  if(rgx) {
   errx(1, "watch_init: regcomp failed");
  } 
  w->filter_re = filter_re;
 }

 do {
  char buf[strlen("resque:queue")+1+strlen(w->sources)+1];
  snprintf(buf, sizeof(buf)-1, "%s:%s", "resque:queue", w->queue);
  w->queue_pre_formatted = strdup(buf);
 } while(0);

 w->mask = mask;
 w->member = WATCH_CITIZEN_PARENT;

 RET_OK(w);
}


ret_t watch_cb(blob_t *b, char *buf, int n) {
 struct inotify_event *ie;
 watch_t *w;
 int ni = 0, ri;
 RET_INIT;

 stat_inc_cbCalled(b->s);

 // initiate multi
 r_multi(b->r);

 while(ni < n) {
  ie = (buf)+ni;

//dump_inotify_event(ie);

  w = b->w[ie->wd];
  ni += sizeof(struct inotify_event) + ie->len;

  if(ie->wd < 0) {
   dump_inotify_event(ie);
   stat_inc_badEvent(b->s);
   continue;
  }

  if(ie->len <= 0) {
   dump_inotify_event(ie);
   stat_inc_badEvent(b->s);
   continue;
  } else {

   if(!w) {
    stat_inc_noWatcher(b->s);
    continue;
   }

   if(w->mask == 0) {
    stat_inc_zeroMask(b->s);
    continue;
   }
  
   stat_inc_goodEvent(b->s);

   if(w->filter_re) {
    char full_path[strlen(w->sources)+1+strlen(ie->name)+1+2];
    snprintf(full_path, sizeof(full_path)-1, "%s/%s", w->sources, ie->name);
    ri = regexec(w->filter_re, full_path, 0, NULL, 0);
    if(!ri) {
     /*
      * Match
      */
     stat_inc_goodFilter(b->s);
    } else {
     stat_inc_badFilter(b->s);
     continue;
    }
   }

   _r = r_enqueue(b->r, w, ie);
  }
 }

 // exec
 r_exec(b->r);

 RET_OK(NULL);
}


ret_t watch_loop(blob_t *b) {
 static fd_set rfds;
 int n, nie, rc = 0;
 RET_INIT;

 FD_ZERO(&rfds);
 FD_SET(b->wfd, &rfds);
 while(1) {
  rc = select(b->wfd+1, &rfds, NULL, NULL, NULL);
  rc = ioctl(b->wfd, FIONREAD, &n);
  if(rc < 0) {
   stat_inc_badIoctl(b->s);
  }
  stat_inc_fionreadBytes(b->s, n);
  do {
   char buf[n+1];
   nie = read(b->wfd, buf, n);
   if(nie < 0) {
    stat_inc_badRead(b->s);
    continue;
   }
   stat_inc_goodRead(b->s);
   stat_inc_readBytes(b->s, nie);
   watch_cb(b, buf, nie);
  } while(0);
 }

 RET_OK(NULL);
}


ret_t watch_open(int wfd, watch_t *w) {
 RET_INIT;
 if(!w) {
  RET_ERROR("w = null");
 }
 w->wd = inotify_add_watch(wfd, w->sources, w->mask);
 RET_OK(NULL);
}



ret_t watch_open_from_list(int wfd, list_t *ll) {
 watch_t *w;
 list_elm_t *le;
 RET_INIT;

 if(!ll) {
  RET_ERROR("watch_open_from_list invalid");
 }

 for(le = ll->head; le != NULL; le = le->next) {
  w = (watch_t *) le->data;
  _r = watch_open(wfd, w);
 }

 RET_OK(NULL);
}


ret_t watch_assign_to_blob_from_list(blob_t *b, list_t *ll) {
 watch_t *w;
 list_elm_t *le;
 RET_INIT;

 if(!b || !ll) {
  RET_ERROR("watch_assign_to_blob_from_list invalid");
 }
 for(le = ll->head; le != NULL; le = le->next) {
  w = (watch_t *) le->data;
  b->w[w->wd] = w;  
 }

 RET_OK(NULL);
}
