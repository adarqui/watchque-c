#include "inotify.h"


ret_t mask_to_event(int m, int n)  {
 RET_INIT;

 if(m & WATCH_MASK_CREATE) {
  RET_OK("CREATE");
 }
 if(m & WATCH_MASK_UPDATE) {
  RET_OK("UPDATE");
 }
 if(m & WATCH_MASK_DELETE) {
  RET_OK("DELETE");
 }
 if(m & WATCH_MASK_CLOSE_WRITE) {
  RET_OK("CLOSE_WRITE");
 }

 RET_ERROR("unknown");
}


ret_t event_to_json(watch_t *w, struct inotify_event *ie, char *mask_s) {
 static char buf[512];
 RET_INIT;
 if(!w || !ie) {
  RET_ERROR("event_to_json error");
 }

 snprintf(buf, sizeof(buf)-1, "{\"class\":\"%s\",\"args\":[{\"filePath\":\"%s/%s\",\"event\":\"%s\"}]}", w->class, w->sources, ie->name, mask_s);

 RET_OK(buf);
}



