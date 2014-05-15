#include "parse.h"


ret_t parse_redis(char *s) {
 redis_t *r;
 char *p;
 RET_INIT;

 if(!s) {
  RET_ERROR("null argument");
 }

 r = (redis_t *) calloc(1, sizeof(redis_t));
 if(!r) {
  RET_ERROR("calloc error");
 }

 p = strtok(s, ":"); 
 if(!p) {
  RET_ERROR("missing host:port tuple");
 }

 r->host = strdup(p);

 p = strtok(NULL, "");
 if(!p) {
  RET_ERROR("missing port");
 }
 
 r->port = atoi(p);

 r->h = redisConnect(r->host, r->port);
 if(!r->h) {
  RET_ERROR("redisConnect failed");
 }

 RET_OK(r);
}


ret_t parse_event_masks(char *s, int *m) {
 RET_INIT;

 if(!s || !m) {
  RET_ERROR("!s || !m");
 }

 *m = 0;
 while(*s) {
  switch(*s) {
   case 'c': {
    *m |= WATCH_MASK_CREATE;
    break;
   }
   case 'C': {
    *m |= WATCH_MASK_CLOSE_WRITE;
    break;
   }
   case 'u': {
    *m |= WATCH_MASK_UPDATE;
    break;
   }
   case 'd': {
    *m |= WATCH_MASK_DELETE;
    break;
   }
   case 'r': {
    *m |= WATCH_MASK_RENAME;
    break;
   }
   default: {
    RET_ERROR("unknown event to mask");
   }
  }
  s++;
 }

 RET_OK(NULL);
}


/*
 * class, queue, events, source, filter
 */
ret_t parse_watch_single(list_t *ll, char *class, char *queue, char *events, char *source, char *filter, int mask, int depth) {
 watch_t *w;
 list_t *l;
 struct stat st;
 DIR *dir;
 RET_INIT;

 if(!ll || !class || !queue || !events || !source) {
  RET_ERROR("parse_watch_single invalid");
 }

 if(stat(source, &st) < 0) {
  RET_ERROR("stat failed");
 }

 if(!S_ISDIR(st.st_mode) && depth != 0) {
  /*
   * Don't add this, it's a file >= 1 levels deep
   */
  RET_OK(NULL);
 }

 _r = watch_init(class, queue, events, source, filter, mask);
 if(RET_ISOK != RET_BOOL_TRUE) {
  RET_ERROR("watch_init failed");
 }

 w = RET_V;
 if(!w) {
  RET_ERROR("w == null");
 }
//dump_watch(w);

 _r = list_insert(ll, w);
 if(RET_ISOK != RET_BOOL_TRUE) {
  RET_ERROR("list_insert: failed");
 }

 l = (list_t *)RET_V;

 if(S_ISDIR(st.st_mode)) {
  dir = opendir(source);
  if(dir) {
   struct dirent *dp;
   while((dp = readdir(dir)) != NULL) {
    int len = strlen(dp->d_name);
    if(DT_DIR != dp->d_type) {
     continue;
    }
    if((len == 1 && dp->d_name[0] == '.') || (len == 2 && dp->d_name[0] == '.' && dp->d_name[1] == '.')) {
     continue;
    }
    do {
     char buf[strlen(source) + 1 + strlen(dp->d_name) + 2];
     snprintf(buf, sizeof(buf)-1, "%s/%s", source, dp->d_name);
     _r = parse_watch_single(ll, class, queue, events, buf, filter, mask, depth+1);
    } while(0);
   }
  }
 }
 RET_OK(NULL);
}


ret_t parse_watch_multi(list_t *ll, char *s) {
 int wmax, mask;
 char *p, *class, *queue, *events, *sources, *filter;
 RET_INIT;

 if(!s) {
  RET_ERROR("null argument");
 } 

/*
 w = (watch_t *) calloc(1, sizeof(watch_t)); 
 if(!w) {
  RET_ERROR("calloc error");
 }
*/

 p = strtok(s, ":");
 if(!p) {
  RET_ERROR("invalid watch syntax (class)");
 }
 class = strdup(p);

 p = strtok(NULL, ":");
 if(!p) {
  RET_ERROR("invalid watch syntax (queue)");
 }
 queue = strdup(p);

 p = strtok(NULL, ":");
 if(!p) {
  RET_ERROR("invalid watch syntax (events)");
 }
 events = strdup(p);
 
 p = strtok(NULL, ":");
 if(!p) {
  RET_ERROR("invalid watch syntax (sources)");
 }
 sources = strdup(p);

 /* optional */
 p = strtok(NULL, ":");
 if(p) {
  filter = strdup(p);
 } else {
  filter = NULL;
 }

 _r = parse_event_masks(events, &mask);
 if(RET_ISOK != RET_BOOL_TRUE) {
  RET_ERROR("invalid event masks");
 }

 p = strtok(sources, ",");
 if(!p) {
  RET_ERROR("sources error");
 }

 while(p) {
  _r = parse_watch_single(ll, class, queue, events, p, filter, mask, 0);
  if(RET_ISOK != RET_BOOL_TRUE) {
   printf("parse_watch_multi: error adding: %s\n", p);
  }
  p = strtok(NULL, ",");
 }

 if(class) {
  free(class);
 }
 if(queue) {
  free(queue);
 }
 if(events) {
  free(events);
 }
 if(sources) {
  free(sources);
 }
 if(filter) {
  free(filter);
 }

 RET_OK(NULL);
}


ret_t parse_flag(flags_t *f, char *s) {
 RET_INIT;

 if(!f || !s) {
  RET_ERROR("flag argument is null");
 }

 if(!strncasecmp(s, "debug=", strlen("debug="))) {
  char *p = &s[strlen("debug=")];
  int n;

  n = atoi(p);
  f->d = n;
 } else {
  RET_ERROR("unknown option");
 }

 RET_OK(NULL);
}
