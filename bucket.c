#include "watch.h"

ret_t bucket_init(int n)
{
    watch_t **w;
    RET_INIT;
    if (n <= 0) {
	RET_ERROR("invalid");
    }

    w = (watch_t **) calloc(n + 1, sizeof(watch_t *));
    RET_OK(w);
}

/*
ret_t bucket_add_watcher(int wd) {

}
*/
