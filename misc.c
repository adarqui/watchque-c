#include "watch.h"
#include "version.h"

void usage(void)
{
	errx(1,
	     "usage: ./watchque [<redishost:port[:redisdb]>|</path/to/bin/dir>] <metadata> <Class1>:<Queue1>:<Events>:<Directory1,...,DirectoryN> ... <ClassN>:<QueueN>:<Events>:<Directory1, ...,DirectoryN>\n" \
         "\n" \
         "* Events:\n" \
         "  c - Create\n" \
         "  C - Close write\n" \
         "  u - Update\n" \
         "  d - Delete\n" \
         "  r - Rename\n" \
         "\n" \
         "* flags:\n" \
         "  --debug=N where N = debug level\n" \
         "  --meta=<metadata string>\n" \
         "  --post=<rpush,lpush,publish>\n" \
         "\n" \
         "VERSION: %s\n" \
         "EOF"
    , VERSION);
}
