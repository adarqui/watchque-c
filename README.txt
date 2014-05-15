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

no tests unfortunatetly.. extremely short timeline & started off as a prototype, bleh.

