#!/bin/bash
# Based on DXVK build script - https://github.com/doitsujin/dxvk/blob/master/package-release.sh

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: package-release.sh version destdir [--no-package]"
  exit 1
fi

export WINEPREFIX=~/.wine/VK9-build

VK9_VERSION="$1"
VK9_SRC_DIR=`dirname $(readlink -f $0)`
VK9_BUILD_DIR=$(realpath "$2")"/vk9-$VK9_VERSION"
VK9_ARCHIVE_PATH=$(realpath "$2")"/vk9-$VK9_VERSION.tar.gz"

function build_arch {
  cd "$VK9_SRC_DIR"

  export PKG_CONFIG_PATH=./dep$1
  meson --cross-file "$VK9_SRC_DIR/build-win$1.txt"   \
        --buildtype "release"                         \
        --prefix "$VK9_BUILD_DIR/install.$1"          \
        --unity off                                   \
        --strip                                       \
        -Denable_tests=false                          \
        "$VK9_BUILD_DIR/build.$1"

  cd "$VK9_BUILD_DIR/build.$1"
  ninja install

  mkdir "$VK9_BUILD_DIR/x$1"

  cp "$VK9_BUILD_DIR/install.$1/bin/d3d9.dll" "$VK9_BUILD_DIR/x$1/d3d9.dll"
  cp "$VK9_SRC_DIR/VK9-Library/VK9.conf" "$VK9_BUILD_DIR/x$1/VK9.conf"
  cp "$VK9_BUILD_DIR/install.$1/bin/setup_vk9.sh" "$VK9_BUILD_DIR/x$1/setup_vk9.sh"

  if [ $2 == true ]; then
    mkdir "$VK9_BUILD_DIR/Shaders"
    # *.spv suffix must be outside of quotes
    cp "$VK9_BUILD_DIR/build.$1/VK9-Library/Shaders/Shaders@cus/"*.spv "$VK9_BUILD_DIR/Shaders/"
  fi

  rm -R "$VK9_BUILD_DIR/build.$1"
  rm -R "$VK9_BUILD_DIR/install.$1"
}

function package {
  cd "$VK9_BUILD_DIR/.."
  tar -czf "$VK9_ARCHIVE_PATH" "vk9-$VK9_VERSION"
  rm -R "vk9-$VK9_VERSION"
}

build_arch 32 true
build_arch 64 false

if [ "$3" != "--no-package" ]; then
  package
fi
