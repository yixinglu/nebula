#!/usr/bin/env bash

# parameters:
# - build_type
#
# example: build.sh build_type=Release

set -e

for param in $@; do
    eval "$param"
done

[[ ! -z "$build_type" ]] || build_type=Release

project_dir="$(cd "$(dirname "$0")" && pwd)"/../..

export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/lib/x86_64-linux-gnu:$LIBRARY_PATH
export NEBULA_DEP_BIN=/opt/nebula/third-party/bin

[[ -d "$project_dir/_build" ]] || mkdir -p $project_dir/_build
cd $project_dir/_build
$NEBULA_DEP_BIN/cmake -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_C_COMPILER=$NEBULA_DEP_BIN/gcc -DCMAKE_CXX_COMPILER=$NEBULA_DEP_BIN/g++ $project_dir
make -j$(nproc)
$NEBULA_DEP_BIN/ctest -j $(nproc) --output-on-failure
