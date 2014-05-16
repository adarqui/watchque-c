#include "watch.h"


ret_t r_sadd_from_list(blob_t *b, list_t *ll) {
 list_elm_t *le;
 watch_t *w;
 RET_INIT;

 if(!b || !ll) {
  RET_ERROR("r_sadd_from_list invalid arguments");
 }

 for(le = ll->head; le != NULL; le = le->next) {
  w = le->data;
  r_sadd(b->r, w);
 }

 RET_OK(NULL);
}


ret_t r_multi(redis_t *r) {
 redisReply *rr;
 RET_INIT;

 if(!r) {
  RET_ERROR("r_multi invalid arguments");
 }

 while(!(rr = redisCommand(r->h, "MULTI"))) {
  _r = r_connect(r->host, r->port);
  if(RET_ISOK == RET_BOOL_TRUE) {
   r->h = (redisContext *) RET_V;
  }
 }
 if(rr) {
  freeReplyObject(rr);
 }
 RET_OK(NULL);
}


ret_t r_exec(redis_t *r) {
 redisReply *rr;
 RET_INIT;

 if(!r) {
  RET_ERROR("r_exec invalid arguments");
 }

 while(!(rr = redisCommand(r->h, "EXEC"))) {
  _r = r_connect(r->host, r->port);
  if(RET_ISOK == RET_BOOL_TRUE) {
   r->h = (redisContext *) RET_V;
  } 
 }
 if(rr) {
  freeReplyObject(rr);
 }
 RET_OK(NULL);
}


ret_t r_sadd(redis_t *r, watch_t *w) {
 redisReply *rr;
 RET_INIT;

 if(!r || !w) {
  RET_ERROR("sadd invalid arguments");
 }

 while(!(rr = redisCommand(r->h, "SADD resque:queues %s", w->queue))) {
  _r = r_connect(r->host, r->port);
  if(RET_ISOK == RET_BOOL_TRUE) {
   r->h = (redisContext *) RET_V;
  } 
 }
 if(rr) {
  freeReplyObject(rr);
 }

 RET_OK(NULL);
}


ret_t r_enqueue(redis_t *r, watch_t *w, struct inotify_event *ie) {
 redisReply *rr;
 RET_INIT;

 if(!r || !w || !ie) {
  RET_ERROR("enqueue error");
 }

 _r = mask_to_event(w->mask, ie->mask);
 if(RET_ISOK != RET_BOOL_TRUE) {
  RET_ERROR("mask_to_event");
 }

 _r = event_to_json(w, ie, (char *)RET_V);
 if(RET_ISOK != RET_BOOL_TRUE) {
  RET_ERROR("event_to_json error");
 }

 while(!(rr = redisCommand(r->h, "RPUSH %s %s", w->queue_pre_formatted, (char *)RET_V))) {
  _r = r_connect(r->host, r->port);
  if(RET_ISOK == RET_BOOL_TRUE) {
   r->h = (redisContext *) RET_V;
  } 
 }
 if(rr) {
  freeReplyObject(rr);
 }

 RET_OK(NULL);
}


ret_t r_connect(char * host, int port) {
 redisContext *h;
 stats_t *s;
 RET_INIT;

 if(!host || port < 0) {
  errx(1, "r_connect: host = %p, port = %i, fatal error\n", host, port);
 }

 while(1) {
  h = redisConnect(host, port);
  if(h) {
   RET_OK(h);
  }
  s = &global_stats;
  stat_inc_redisReConnect(s);
  sleep(1);
 }

 RET_ERROR("wtf?");
}
