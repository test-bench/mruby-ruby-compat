#!/usr/bin/env bash

set -eu

source set-mruby-env-vars.sh

export TARGET=${TARGET:-release}

echo
echo "Compiling MRuby"
echo "= = ="

cp -v -u build_config.rb $MRUBY_ROOT/$MRUBY_CONFIG

pushd $MRUBY_ROOT </dev/null

rake

popd
