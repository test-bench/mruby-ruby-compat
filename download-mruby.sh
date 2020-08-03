#!/usr/bin/env bash

set -eu

source set-mruby-env-vars.sh

echo
echo "Downloading MRuby"
echo "= = ="
echo

if [ ! -d $MRUBY_ROOT ]; then
  git clone --depth 1 git@github.com:mruby/mruby.git $MRUBY_ROOT
fi
