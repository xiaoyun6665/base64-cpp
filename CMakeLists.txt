cmake_minimum_required(VERSION 4.3)
project(base64_cpp)

set(CMAKE_CXX_STANDARD 26)

## 性能优化编译选项
#if(MSVC)
#    # MSVC 优化选项
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL /DNDEBUG")
#    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF")
#
#    # 启用所有优化
#    add_compile_options(
#            /O2           # 最大速度优化
#            /Oi           # 启用内在函数
#            /Ot           #  favor fast code
#            /Oy           # 省略帧指针
#            /GL           # 全程序优化
#            /arch:AVX512    # 使用 AVX2 指令集（根据你的CPU调整）
#            /Qpar         # 自动并行化
#    )
#
#    # 链接时优化
#    add_link_options(
#            /LTCG         # 链接时代码生成
#            /OPT:REF      # 消除未引用函数和数据
#            /OPT:ICF      # 相同COMDAT折叠
#    )
#else()
#    # GCC/Clang 优化选项
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto -DNDEBUG")
#    add_compile_options(
#            -O3           # 最高级别优化
#            -march=native # 使用本机CPU架构
#            -mtune=native # 针对本机CPU调优
#            -flto         # 链接时优化
#            -ffast-math   # 快速数学运算（可能影响精度）
#            -funroll-loops # 循环展开
#    )
#endif()

add_executable(base64_cpp main.cpp)
target_sources(base64_cpp PRIVATE FILE_SET CXX_MODULES FILES base64.cppm)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS_RELEASE}")
