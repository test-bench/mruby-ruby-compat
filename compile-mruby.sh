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
echo

./download-mruby.sh

echo "Starting compilation"
echo "- - -"
echo

pushd ./mruby

trap popd EXIT

rake -m
