## 简介

任意两文件夹，一指镜像：增量扫描、实时比对、自动过滤。  
完美绕过中文路径乱码，兼容 Git 忽略规则，可与 FreeFileSync、华为家庭存储、NAS 等工具任意组合。  
本地 ↔ 移动硬盘 ↔ 同步盘，怎么搭都行，你的数据你做主。

## 用法



## 构建

```shell
# 1. 生成构建目录
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 2. 编译
cmake --build build --parallel

# 3. 运行
./build/sync  <源路径>  <目标路径>  [gitignore路径]
```

