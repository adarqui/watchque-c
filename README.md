# watchque-c

## important

sysctl -w fs.inotify.max_queued_events=100000

:f

When inotify overflow's... that sysctl fixes the issue.



## example usage

```
root@local:~/watchque-c# ./watchque
watchque: usage: ./watchque [<redishost:port[:redisdb]>|</path/to/bin/dir>] <metadata> <Class1>:<Queue1>:<Events>:<Directory1,...,DirectoryN> ... <ClassN>:<QueueN>:<Events>:<Directory1, ...,DirectoryN>
* flags:
  --debug=N where N = debug level
  --meta=<metadata string>
  --post=<rpush,lpush,publish>
VERSION 1.1
EOF
```

```
./watchque localhost:6379 --meta=poop class:queue:C:/tmp
./watchque localhost:6379 --post=publish class:queue:C:/tmp
```

^C for stats

```
Dumping stats:
        goodEvent: 3
        badEvent: 0
        goodFilter: 0
        badFilter: 0
        redisReConnect: 0
        dirAdded: 0
        dirRemoved: 0
        cbCalled: 2
        goodEvent / cbCalled: 1.500000
        noWatcher: 0
        zeroMask: 0
        goodRead: 2
        badRead: 0
        badIoctl: 0
        fionreadBytes: 96
        readBytes: 96
        watchers: 11
Are you sure you want to quit? [y/n] y
```



## stuff

not finished yet. Porting watchque-go to C because watchque-go is losing inotify events. C however, is receiving them. I need to dig deeper as to why watchque-go is losing events -> I imagine it has something to do with the fsnotify library.

coding this in C is fairly brutal :F

what's left:
 - automatically add/remove directories if we are recursively watching
 - filters
 - tie up loose edge cases
 - 'no recursiveness' flag, N, ie, cudN
 - remove list after we have it in the watch_t array
 - dynamically grow/shrink watch_t array
 - RPush multi -> multi those big buf's of events
 - ^C limit for 'flooding' , so that the code will exit after N ^C's

no tests unfortunatetly.. extremely short timeline & started off as a prototype, bleh.
