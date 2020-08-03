#!/usr/bin/env bash

set -eu

source set-mruby-env-vars.sh

echo
echo "Installing MRuby"
echo "= = ="
echo

echo "Prefix: $MRUBY_PREFIX"

mkdir -vp $MRUBY_PREFIX/{bin,include,lib}

pushd $MRUBY_ROOT

pushd ./build/host

for bin in bin/*; do
  cp -v $bin $MRUBY_PREFIX/bin
done

for lib in lib/*.a; do
  cp -v $lib $MRUBY_PREFIX/lib
done

popd

for header in $(find include -name '*.h'); do
  dir=$(dirname $header)

  if [ ! -d $dir ]; then
    mkdir -p $MRUBY_PREFIX/$dir
  fi

  cp -v $header $MRUBY_PREFIX/$dir
done

popd
