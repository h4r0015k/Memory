# Memory
Simple Memory allocator in C.

## Compiling
```
gcc -o memory.so -shared -fPIC memory.c
```

## Testing
```
export LD_PRELOAD=$PWD/memory.so
ls (Something is wrong if ls results in segmentation fault)
```

## To Do
- [X] Malloc
- [X] Free
- [X] Realloc
- [X] Calloc
