#include "watch.h"


ret_t r_sadd_from_list(blob_t *b, list_t *ll) {
 list_elm_t *le;
 watch_t *w;
 RET_INIT;

 for(le = ll->head; le != NULL; le = le->next) {
  w = le->data;
  r_sadd(b->r->h, w);
 }

 RET_OK(NULL);
}


void r_multi(redisContext *h) {
 freeReplyObject(redisCommand(h, "MULTI"));
 return;
}


void r_exec(redisContext *h) {
 freeReplyObject(redisCommand(h, "EXEC"));
 return;
}


ret_t r_sadd(redisContext *h, watch_t *w) {
 redisReply *rr;
 RET_INIT;

 rr = redisCommand(h, "SADD resque:queues %s", w->queue);
 if(rr) {
  freeReplyObject(rr);
 }

 RET_OK(NULL);
}


ret_t r_enqueue(redisContext *h, watch_t *w, struct inotify_event *ie) {
 redisReply *rr;
 RET_INIT;

 if(!h || !w || !ie) {
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

 rr = redisCommand(h, "RPUSH %s %s", w->queue_pre_formatted, (char *)RET_V);
 if(rr) {
  freeReplyObject(rr);
 }

 RET_OK(NULL);
}
