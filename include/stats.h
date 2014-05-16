#ifndef STATS_H
#define STATS_H

#include "watch.h"

#define stat_inc_goodEvent(s) s->goodEvent++
#define stat_inc_badEvent(s) s->badEvent++
#define stat_inc_goodFilter(s) s->goodFilter++
#define stat_inc_badFilter(s) s->badFilter++
#define stat_inc_redisReConnect(s) s->redisReConnect++
#define stat_inc_dirAdded(s) s->dirAdded++
#define stat_inc_dirRemoved(s) s->dirRemoved++
#define stat_inc_cbCalled(s) s->cbCalled++
#define stat_inc_noWatcher(s) s->noWatcher++
#define stat_inc_zeroMask(s) s->zeroMask++
#define stat_inc_goodRead(s) s->goodRead++
#define stat_inc_badRead(s) s->badRead++
#define stat_inc_badIoctl(s) s->badIoctl++
#define stat_inc_fionreadBytes(s, n) s->fionreadBytes += n
#define stat_inc_readBytes(s, n) s->readBytes += n

#endif
