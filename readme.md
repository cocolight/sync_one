## Features

Mirror any two folders in one click: incremental scan, real-time diff, auto-filter.  
Bypass Unicode path bugs, honor .gitignore, and play nicely with FreeFileSync, Huawei Home Storage, NAS, or whatever combo you choose.  
Local ↔ USB ↔ sync-drive — your data, your pipeline, your rules.

## usage

```shell
# Run
syncone  <source>  <destination>  [gitignore-path]
```



## build

```shell
# 1. Generate build directory
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 2. Compile
cmake --build build --parallel
```

