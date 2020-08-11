#!/usr/bin/env bash

set -eu

echo
echo "Compiling MRuby"
echo "= = ="
echo

export MRUBY_CONFIG=$(realpath ./build_config.rb)
echo "MRUBY_CONFIG: $MRUBY_CONFIG"

export INSTALL_DIR=$(realpath ./bin)
echo "INSTALL_DIR: $INSTALL_DIR"

jobs=${MRUBY_RAKE_PARALLEL_JOBS:-1}
echo "MRUBY_RAKE_PARALLEL_JOBS: $jobs"
echo

if [ ${CLEAN:-off} = 'on' ]; then
  rm -vf build_config.rb.lock
fi

./download-mruby.sh

echo "Starting compilation"
echo "- - -"
echo

pushd ./mruby

trap popd EXIT

jobs=${MRUBY_RAKE_PARALLEL_JOBS:-1}

rake -m -j $jobs
