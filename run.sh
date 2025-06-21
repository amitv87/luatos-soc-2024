set -e
# set -x

PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

TOOLS_DIR=$PROJECT_DIR/tools
SRC_ROOT_DIR=$PROJECT_DIR

cd $SRC_ROOT_DIR

PROJECT_PATH="$SRC_ROOT_DIR"

BUILD_PATH="$SRC_ROOT_DIR/.build"
[[ ! -z "${getDevBuildPath}" ]] && eval "$getDevBuildPath" && BUILD_PATH=$(getDevBuildPath);

echo "BUILD_PATH: $BUILD_PATH"
[[ ! -d "$BUILD_PATH" ]] && mkdir -p $BUILD_PATH

[[ ! -d "$BUILD_PATH/PLAT" ]] && ln -sf $SRC_ROOT_DIR/PLAT $BUILD_PATH/
[[ ! -f "$BUILD_PATH/fcelf.exe" ]] && ln -sf $SRC_ROOT_DIR/PLAT/tools/fcelf.exe $BUILD_PATH/

[[ -z "$NUM_CPU" ]] && NUM_CPU=$(getconf _NPROCESSORS_ONLN)

SILENT_MAKE="-s"
# VERBOSE_ARGS="-DCMAKE_VERBOSE_MAKEFILE=TRUE"

if [[ $1 == "-v" ]]; then
  shift;
  SILENT_MAKE=""
fi

# MC60ECAR01A07
export SOC="ec718hm"
SIZE=arm-none-eabi-size
SERIAL_DEVICE=/dev/cu.usbmodem0000000000013
SERIAL_BL_DEVICE=/dev/cu.usbmodem0000000000011

run(){
  tio -e -b 921600 $SERIAL_DEVICE
}

flash(){
  _app=$BUILD_PATH/$1.bin
  node $TOOLS_DIR/flashTool.js $SERIAL_DEVICE $_app
  # python3 upload.py -p $UART_DEVICE -b $UART_BAUD $BUILD_PATH/$SOC.bin
}

print_size(){
  echo BUILD_PATH: $BUILD_PATH
  cd $BUILD_PATH

  exec_files_array=(${@})
  if [ -z "$exec_files_array" ]; then
    md5 *.elf
    $SIZE *.elf
    ls -la tmp_*.bin*
    return;
  fi

  for exec in "${exec_files_array[@]}"
  do
    md5 $exec.elf
    $SIZE $exec.elf
    # $TOOLS_DIR/gen_tree_map.sh $exec.elf.map
    ls -la tmp_$exec.bin
  done
  cd ~-
}

build(){
  mkdir -p $BUILD_PATH
  cd $BUILD_PATH
  [[ ! -f "Makefile" ]] && cmake $VERBOSE_ARGS $PROJECT_PATH
  make $SILENT_MAKE -j $NUM_CPU $@
  cd ~-
}

compile(){
  build $@;
  print_size $@;
}

ACTION="$1"

if [ ! -z "$ACTION" ]; then
  shift;
fi

if [[ $ACTION == "c" ]]; then
  build clean;
elif [[ $ACTION == "b" ]]; then
  compile $@;
elif [[ $ACTION == "br" ]]; then
  app=$1
  shift;
  compile $app;
  flash $app;
  run
elif [[ $ACTION == "ss" ]]; then
  find $BUILD_PATH -type f -name '*.su' | xargs cat | sort -k2 -n
else
  run
fi
