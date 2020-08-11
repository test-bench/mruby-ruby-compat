#!/usr/bin/env bash

set -eu

echo
echo "Downloading MRuby"
echo "= = ="
echo

if [ ${CLEAN:-off} = 'on' ]; then
  rm -rvf mruby
fi

if [ ! -d mruby ]; then
  git clone --depth 1 git@github.com:mruby/mruby.git
else
  echo "CLEAN is not enabled; preserving existing mruby directory"
  echo
fi
