#include "watch.h"

void sigint_handler(int signum)
{
	time_t t;
	dump_stats(&global_stats);
	time(&t);
	if ((t - global_stats.t) < 1) {
		char line[32], *p;
		printf("Are you sure you want to quit? [y/n] ");
		fflush(stdout);
		p = fgets(line, sizeof(line) - 1, stdin);
		if (p && line[0] == 'y') {
			exit(0);
		}
	}
	global_stats.t = t;
	return;
}
