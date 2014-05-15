#include "watch.h"

void sigint_handler(int signum) {
 dump_stats(&global_stats);
 return;
}
