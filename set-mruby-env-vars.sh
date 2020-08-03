#!/usr/bin/env bash

export MRUBY_CONFIG=${MRUBY_CONFIG:-mruby_local.rb}
export MRUBY_ROOT=${MRUBY_ROOT:-./mruby}
export MRUBY_PREFIX=${MRUBY_PREFIX:-${PREFIX:-~/.local}}
export MRUBY_LOCAL_DIR=$(realpath -P $(dirname $BASH_SOURCE))
