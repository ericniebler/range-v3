#!/bin/bash
#
# Install libc++ under travis
CMAKE_URL="http://www.cmake.org/files/v3.5/cmake-3.5.0-Linux-x86_64.tar.gz"
wget ${CMAKE_URL} --no-check-certificate
mkdir cmake
tar -xzf cmake-3.5.0-Linux-x86_64.tar.gz -C cmake --strip-components=1
git clone --depth=1 https://github.com/llvm-mirror/libcxx.git
mkdir libcxx/build
(cd libcxx/build && ../../cmake/bin/cmake .. -DLIBCXX_CXX_ABI=libstdc++ -DLIBCXX_CXX_ABI_INCLUDE_PATHS="/usr/include/c++/4.6;/usr/include/c++/4.6/x86_64-linux-gnu")
make -C libcxx/build cxx -j2
sudo cp libcxx/build/lib/libc++.so.1.0 /usr/lib/
sudo cp -r libcxx/build/include/c++/v1 /usr/include/c++/v1/
sudo ln -sf /usr/lib/libc++.so.1.0 /usr/lib/libc++.so
sudo ln -sf /usr/lib/libc++.so.1.0 /usr/lib/libc++.so.1
