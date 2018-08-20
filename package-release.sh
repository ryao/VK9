#!/bin/bash
# Based on DXVK build script - https://github.com/doitsujin/dxvk/blob/master/package-release.sh

function print_usage {
  echo "Usage: package-release.sh version destdir [options]
    Options are:
        --no-package:    Do not build tar package
        --keep-builddir: Do not delete builddir, use this to keep logfiles for debugging
        --no-32bit:      Do not build 32 bit binaries
        --no-64bit:      Do not build 64 bit binaries
        --debug:         Build debug version of the library
        --no-pch:        Do not use precompiled headers"
}

if [ -z "$1" ] || [ -z "$2" ]; then
  print_usage
  exit -1
fi

if [ -z $(export -p | grep WINEPREFIX\=) ]; then
  export WINEPREFIX=~/.wine/VK9-build
fi

# Get positional arguments and remove them from argument list
VK9_VERSION="$1"
VK9_SRC_DIR=`dirname $(readlink -f $0)`
VK9_BUILD_DIR=$(realpath "$2")"/vk9-$VK9_VERSION"
VK9_ARCHIVE_PATH=$(realpath "$2")"/vk9-$VK9_VERSION.tar.gz"
shift
shift

BUILD_PACKAGE=true
BUILD_32BIT=true
BUILD_64BIT=true
BUILD_TYPE="release"
KEEP_BUILDDIR=false
USE_PCH=true

# Parse options
while [[ $# -gt 0 ]]
do
  key="$1"

  case $key in
    --no-package)
    BUILD_PACKAGE=false
    shift
    ;;
  --keep-builddir)
    KEEP_BUILDDIR=true
    shift
    ;;
  --no-32bit)
    BUILD_32BIT=false
    shift
    ;;
  --no-64bit)
    BUILD_64BIT=false
    shift
    ;;
  --debug)
    $BUILD_TYPE="debug"
    shift
    ;;
  --no-pch)
    USE_PCH=false
    shift
    ;;
  *)    # unknown option
    echo "Found unknown argument \"$key\". Abort!"
    print_usage
    exit -1
    ;;
  esac
done

echo "Building VK9 with the following options:"
echo "    tar package:        $BUILD_PACKAGE"
echo "    32 bit:             $BUILD_32BIT"
echo "    64 bit:             $BUILD_64BIT"
echo "    build type:         $BUILD_TYPE"
echo "    keep builddir:      $KEEP_BUILDDIR"
echo "    precompiled header: $USE_PCH"

function build_arch {
  cd "$VK9_SRC_DIR"

  if [ -z $(export -p | grep PKG_CONFIG_PATH\=) ]; then
    export PKG_CONFIG_PATH=./dep$1
  fi
  if [ -z $(export -p | grep PKG_CONFIG_PATH_CUSTOM\=) ]; then
    # Some distributions use PKG_CONFIG_PATH_CUSTOM instead.
    export PKG_CONFIG_PATH_CUSTOM=./dep$1
  fi

  source ./dep$1/boost.sh

  meson --cross-file "$VK9_SRC_DIR/build-win$1.txt"   \
        --buildtype $BUILD_TYPE                       \
        --prefix "$VK9_BUILD_DIR/install.$1"          \
        --unity off                                   \
        --strip                                       \
        -Db_pch=$USE_PCH                              \
        -Denable_tests=false                          \
        "$VK9_BUILD_DIR/build.$1"

  cd "$VK9_BUILD_DIR/build.$1"
  ninja install

  mkdir -p "$VK9_BUILD_DIR/x$1"

  cp "$VK9_BUILD_DIR/install.$1/bin/d3d9.dll" "$VK9_BUILD_DIR/x$1/d3d9.dll"
  cp "$VK9_SRC_DIR/VK9-Library/VK9.conf" "$VK9_BUILD_DIR/x$1/VK9.conf"
  cp "$VK9_BUILD_DIR/install.$1/bin/setup_vk9.sh" "$VK9_BUILD_DIR/x$1/setup_vk9.sh"

  if [ $2 == true ]; then
    mkdir -p "$VK9_BUILD_DIR/Shaders"
    # *.spv suffix must be outside of quotes
    cp "$VK9_BUILD_DIR/build.$1/VK9-Library/Shaders/Shaders@cus/"*.spv "$VK9_BUILD_DIR/Shaders/"
  fi

  if [ $KEEP_BUILDDIR == false ]; then
    rm -R "$VK9_BUILD_DIR/build.$1"
    rm -R "$VK9_BUILD_DIR/install.$1"
  fi
}

function package {
  echo "Building package..."
  cd "$VK9_BUILD_DIR/.."
  TAR_DIRS=""
  if [ -d "vk9-$VK9_VERSION/x64" ]; then
    TAR_DIRS="vk9-$VK9_VERSION/x64"
  fi
  if [ -d "vk9-$VK9_VERSION/x32" ]; then
    TAR_DIRS="$TAR_DIRS vk9-$VK9_VERSION/x32"
  fi
  if [ -d "vk9-$VK9_VERSION/Shaders" ]; then
    TAR_DIRS="$TAR_DIRS vk9-$VK9_VERSION/Shaders"
  fi

  tar -czvf "$VK9_ARCHIVE_PATH" $TAR_DIRS
  if [ $KEEP_BUILDDIR == false ]; then
    rm -R "vk9-$VK9_VERSION"
  fi
}

BUILD_SHADERS=true

if [ $BUILD_32BIT == true ]; then
  build_arch 32 $BUILD_SHADERS
  BUILD_SHADERS=false
fi

if [ $BUILD_64BIT == true ]; then
  build_arch 64 $BUILD_SHADERS
  BUILD_SHADERS=false
fi

if [ $BUILD_PACKAGE == true ]; then
  package
fi

exit 0
