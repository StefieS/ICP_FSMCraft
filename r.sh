#!/bin/bash
export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/share/Qt-5.5.1/5.5/gcc_64/plugins/platforms

rm -rf build
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/usr/local/share/Qt-5.5.1/5.5/gcc_64/lib/cmake
make -j$(nproc) && ./src/fsmtool
