#include "watch.h"

void usage(void)
{
	errx(1,
	     "usage: ./watchque [<redishost:port[:redisdb]>|</path/to/bin/dir>] <metadata> <Class1>:<Queue1>:<Events>:<Directory1,...,DirectoryN> ... <ClassN>:<QueueN>:<Events>:<Directory1, ...,DirectoryN>\n" \
         "* flags:\n" \
         "  --debug=N where N = debug level\n" \
         "  --meta=<metadata string>\n" \
         "  --post=<rpush,lpush,publish>\n" \
         "EOF"
    );
}
