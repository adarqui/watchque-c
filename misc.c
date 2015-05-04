#include "watch.h"

void usage(void)
{
	errx(1,
	     "usage: ./watchque [<redishost:port[:redisdb]>|</path/to/bin/dir>] <Class1>:<Queue1>:<Events>:<Directory1,...,DirectoryN> ... <ClassN>:<QueueN>:<Events>:<Directory1, ...,DirectoryN>");
}
