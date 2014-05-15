#include "watch.h"

ret_t list_init(void) {
 list_t *ll;
 RET_INIT;
 ll = (list_t *) calloc(1, sizeof(list_t));
 RET_OK(ll);
}

ret_t list_insert(list_t *ll, void *d) {
 list_elm_t *le, *lp;
 RET_INIT;
 if(!ll || !d) {
  RET_ERROR("list_insert");
 }
 le = (list_t *) calloc(1, sizeof(list_t *));
 if(!le) {
  RET_ERROR("calloc");
 }

 le->data = d;
 
 if(!ll->head) {
  ll->head = ll->tail = le;
 } else {
  lp = ll->tail;
  ll->tail = le;
  lp->next = le;
 }

 RET_OK(le);
}
