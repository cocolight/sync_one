cmake_minimum_required(VERSION 3.14)
project(SyncOne LANGUAGES CXX)

# C++17 是硬需求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 可执行文件名字：syncone
set(TARGET_NAME "syncone")

add_executable(${TARGET_NAME}
    sync.cpp        # 主实现
    gitignore.hpp   # 头文件-only 库，也写进列表方便 IDE 索引
)

# 让 MSVC 也使用 UTF-8 编码，避免中文路径乱码（MinGW/GCC 可忽略）
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

# 安装规则（可选）
install(TARGETS ${TARGET_NAME} DESTINATION bin)