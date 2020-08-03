#!/usr/bin/env bash

set -eu

source set-mruby-env-vars.sh

echo
echo "Unistalling MRuby"
echo "= = ="
echo

echo "Prefix: $MRUBY_PREFIX"

pushd $MRUBY_ROOT

pushd ./build/host

for bin in bin/*; do
  rm -vf $MRUBY_PREFIX/$bin
done

for lib in lib/*.a; do
  rm -vf $MRUBY_PREFIX/$lib
done

popd

for header in $(find include -name '*.h'); do
  rm -vf $MRUBY_PREFIX/$header
done

for headerDir in $(find include -mindepth 2 -type d); do
  if [ -d $MRUBY_PREFIX/$headerDir ]; then
    rmdir $MRUBY_PREFIX/$headerDir
  fi
done

popd
