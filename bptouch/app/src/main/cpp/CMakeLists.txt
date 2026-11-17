cmake_minimum_required(VERSION 3.22.1)
project(bptouch CXX)

add_library(bptouch SHARED
    jni_bridge.cpp
    engine.cpp
    selection.cpp
    maxflow.cpp)

target_compile_options(bptouch PRIVATE -O3 -ffast-math -fno-finite-math-only -Wall -Wno-unused-parameter)
# links "max-page-size" de 16 KB: exigido pelos aparelhos Android 15+ com páginas de 16 KB
target_link_options(bptouch PRIVATE "-Wl,-z,max-page-size=16384")
target_link_libraries(bptouch GLESv3 EGL android jnigraphics log)
