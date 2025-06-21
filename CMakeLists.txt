cmake_minimum_required(VERSION 3.13)

set(START_NETIF "pico_ippktif_init")

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

include(CMakeForceCompiler)

SET(CMAKE_C_COMPILER_FORCED   1)
SET(CMAKE_ASM_COMPILER_FORCED 1)
SET(CMAKE_CXX_COMPILER_FORCED 1)

# SET(SOC "ec718hm")

SET(LIB_FW "oc")
SET(LIB_PS_PLAT "full")

SET(SOC "$ENV{SOC}")
string(TOUPPER ${SOC} SOC_U)

message("SOC: ${SOC}")
message("SOC_U: ${SOC_U}")

SET(TC_VERSION "10-2020")
SET(TC_HOST "arm-none-eabi")

IF(DEFINED ENV{TOOL_CHAIN_PATH})

SET(ENV_TCP "$ENV{TOOL_CHAIN_PATH}")

FILE(GLOB dirs RELATIVE ${ENV_TCP} ${ENV_TCP}/*)
FOREACH(dir ${dirs})
  IF(IS_DIRECTORY ${ENV_TCP}/${dir} AND dir MATCHES "${TC_VERSION}" AND dir MATCHES "${TC_HOST}")
    SET(TC_DIR ${dir})
  ENDIF()
ENDFOREACH()

SET(BUILD_TOOL_CHAIN_PATH "$ENV{TOOL_CHAIN_PATH}/${TC_DIR}")

ELSE()
EXECUTE_PROCESS(COMMAND which ${TC_HOST}-gcc -dumpversion OUTPUT_VARIABLE TOOL_CHAIN_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
GET_FILENAME_COMPONENT(TOOL_CHAIN_PATH ${TOOL_CHAIN_PATH} DIRECTORY)
get_filename_component(BUILD_TOOL_CHAIN_PATH ${TOOL_CHAIN_PATH} DIRECTORY)
ENDIF()

SET(TC_PFX "${BUILD_TOOL_CHAIN_PATH}/bin/${TC_HOST}-")

SET(CMAKE_C_COMPILER   ${TC_PFX}gcc)
SET(CMAKE_CXX_COMPILER ${TC_PFX}g++)
SET(CMAKE_ASM_COMPILER ${TC_PFX}gcc)

set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)
set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)

SET(CMAKE_C_LINK_FLAGS "")
SET(CMAKE_CXX_LINK_FLAGS "")
SET(CMAKE_ASM_LINK_FLAGS "")

SET(CMAKE_C_FLAGS "-std=gnu11")
SET(CMAKE_CXX_FLAGS "-std=gnu++11 -fno-rtti -fno-exceptions")
SET(CMAKE_ASM_FLAGS "")

SET(OBJCOPY_CMD ${TC_PFX}objcopy)
SET(OBJDUMP_CMD ${TC_PFX}objdump)
SET(OBJSIZE_CMD ${TC_PFX}size)

set(out_dir ${CMAKE_CURRENT_BINARY_DIR})

SET(LINKER_FLAGS
  -Wl,-Map=<TARGET>.map
  -L ${out_dir} -T ${out_dir}/${SOC}_script.ld
  # -L ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/libs/${SOC}
  # -L ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/prebuild/PS/lib/gcc/${SOC}/${LIB_PS_PLAT}
  # -L ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/prebuild/PLAT/lib/gcc/${SOC}/${LIB_PS_PLAT}

  -mcpu=cortex-m3
  -mthumb
  --specs=nano.specs
  # --specs=nosys.specs

  # -lc
  # -lc_nano
  # -lm
  # -lgcc
  # -lnosys
  # -nostdlib

  -Wl,--whole-archive
  -Wl,--start-group
  <LINK_LIBRARIES>
  <OBJECTS>
  -Wl,--end-group
  -Wl,--no-whole-archive

  -Wl,--cref
  -Wl,--check-sections
  -Wl,--gc-sections
  -Wl,--no-undefined
  -Wl,--no-print-map-discarded
  -Wl,--print-memory-usage

  -Wl,--wrap=_malloc_r
  -Wl,--wrap=_free_r
  -Wl,--wrap=_realloc_r
  -Wl,--wrap=clock
  -Wl,--wrap=localtime
  -Wl,--wrap=gmtime
  -Wl,--wrap=time
)
string(REPLACE ";" " " LINKER_FLAGS "${LINKER_FLAGS}")

SET(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> ${LINKER_FLAGS} -o <TARGET>")

SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")

project(
  eigen_sdk
  VERSION 0.0.1
  LANGUAGES C CXX ASM
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_options(
  # -g3
  # -Os
  # -mcpu=cortex-m3
  # -mthumb
  # -nostartfiles
  # -mapcs-frame
  # -ffunction-sections
  # -fdata-sections
  # -fno-isolate-erroneous-paths-dereference
  # -freorder-blocks-algorithm=stc

  # -mslow-flash-data
  # -flto -fuse-linker-plugin -ffat-lto-objects -flto-partition=none -Wno-lto-type-mismatch

  -mcpu=cortex-m3
  -mthumb
  -mapcs-frame
  -mslow-flash-data
  -mfloat-abi=soft
  -march=armv7-m
  -g -g3 -Os
  -ffunction-sections
  -fdata-sections
  -fno-isolate-erroneous-paths-dereference
  -freorder-blocks-algorithm=stc
)

file(GLOB_RECURSE sdk_src CONFIGURE_DEPENDS
  PLAT/middleware/*.c
  PLAT/core/code/*.c

  PLAT/driver/board/ec7xx_0h00/src/bsp.c
  PLAT/driver/board/ec7xx_0h00/src/plat_config.c

  PLAT/driver/chip/ec7xx/ap/src/apmu2Peripheral.c
  PLAT/driver/chip/ec7xx/ap/src/clock.c
  PLAT/driver/chip/ec7xx/ap/src/gpio.c
  PLAT/driver/chip/ec7xx/ap/src/kpc.c
  PLAT/driver/chip/ec7xx/ap/src/lspi.c
  PLAT/driver/chip/ec7xx/ap/src/oneWire.c
  PLAT/driver/chip/ec7xx/ap/src/pad.c
  PLAT/driver/chip/ec7xx/ap/src/timer.c
  PLAT/driver/chip/ec7xx/ap/src/wdt.c

  PLAT/driver/chip/ec7xx/ap/src/ec718/adc.c
  PLAT/driver/chip/ec7xx/ap/src/usb/open/*.c

  PLAT/driver/chip/ec7xx/common/gcc/memcpy-armv7m.S
  PLAT/driver/chip/ec7xx/common/src/VPU/vem_cfg_default.c

  PLAT/driver/hal/common/src/*.c

  PLAT/driver/hal/ec7xx/ap/src/hal_adc.c
  PLAT/driver/hal/ec7xx/ap/src/hal_alarm.c
  PLAT/driver/hal/ec7xx/ap/src/hal_dumpMedia.c
  PLAT/driver/hal/ec7xx/ap/src/hal_misc.c
  PLAT/driver/hal/ec7xx/ap/src/hal_uartDump.c

  PLAT/os/freertos/src/*.c
  PLAT/os/freertos/portable/gcc/port.c
  PLAT/os/freertos/portable/mem/tlsf/tlsf.c
  PLAT/os/freertos/CMSIS/*.c

  # PLAT/os/freertos/src/event_groups.c
  # # PLAT/os/freertos/src/heap_4.c
  # PLAT/os/freertos/src/heap_6.c
  # # PLAT/os/freertos/src/heap_6_cust.c
  # # PLAT/os/freertos/src/heap_6_psram.c
  # PLAT/os/freertos/src/list.c
  # PLAT/os/freertos/src/queue.c
  # PLAT/os/freertos/src/tasks.c
  # PLAT/os/freertos/src/timers.c
  # PLAT/os/freertos/portable/gcc/port.c
  # PLAT/os/freertos/portable/mem/tlsf/tlsf.c
  # PLAT/os/freertos/CMSIS/ap/src/cmsis_os2.c
  # PLAT/os/freertos/CMSIS/ap/src/os_callback_hook.c
  # PLAT/os/freertos/CMSIS/ap/src/os_systick.c
  # PLAT/os/freertos/CMSIS/common/src/mm_debug.c

  thirdparty/littlefs/*.c
  thirdparty/printf/*.c
)

add_library(sdk_lib ${sdk_src})

target_include_directories(sdk_lib PUBLIC
  PLAT/tools/${SOC}

  thirdparty/littlefs
  thirdparty/littlefs/port
  thirdparty/printf

  project/example/inc

  PLAT/middleware/thirdparty/lzma2201/C
  PLAT/middleware/thirdparty/lzma2201/C/wrapper

  PLAT/device/target/board/common/ARMCM3/inc
  PLAT/device/target/board/ec7xx_0h00/common/inc
  PLAT/device/target/board/ec7xx_0h00/common/pkginc
  PLAT/device/target/board/ec7xx_0h00/ap/gcc
  PLAT/device/target/board/ec7xx_0h00/ap/inc
  PLAT/driver/board/ec7xx_0h00/inc
  PLAT/driver/board/ec7xx_0h00/inc/eeprom
  PLAT/driver/board/ec7xx_0h00/inc/camera
  PLAT/driver/board/ec7xx_0h00/inc/camera/cameraDev
  PLAT/driver/board/ec7xx_0h00/inc/codec
  PLAT/driver/board/ec7xx_0h00/inc/codec/codecDev
  PLAT/driver/board/ec7xx_0h00/inc/audio
  PLAT/driver/board/ec7xx_0h00/inc/audio/codec
  PLAT/driver/board/ec7xx_0h00/inc/audio/codec/es8311
  PLAT/driver/board/ec7xx_0h00/inc/lcd/lcdDev
  PLAT/driver/board/ec7xx_0h00/inc/lcd
  PLAT/driver/board/ec7xx_0h00/inc/ntc
  PLAT/driver/board/ec7xx_0h00/inc/exstorage
  PLAT/driver/hal/common/inc
  PLAT/driver/hal/ec7xx/ap/inc
  PLAT/driver/hal/ec7xx/ap/inc/ec718
  PLAT/driver/chip/ec7xx/ap/inc
  PLAT/driver/chip/ec7xx/ap/inc/ec718
  PLAT/driver/chip/ec7xx/ap/inc_cmsis
  PLAT/os/freertos/inc
  PLAT/os/freertos/CMSIS/common/inc
  PLAT/os/freertos/CMSIS/ap/inc
  PLAT/os/freertos/portable/mem/tlsf
  PLAT/os/freertos/portable/gcc
  PLAT/middleware/developed/nvram/inc
  PLAT/middleware/developed/nvram/ec7xx/inc
  PLAT/middleware/developed/cms/psdial/inc
  PLAT/middleware/developed/cms/cms/inc
  PLAT/middleware/developed/cms/psil/inc
  PLAT/middleware/developed/cms/psstk/inc
  PLAT/middleware/developed/cms/sockmgr/inc
  PLAT/middleware/developed/cms/cmsnetlight/inc
  PLAT/middleware/developed/ecapi/aal/inc
  PLAT/middleware/developed/ecapi/appmwapi/inc
  PLAT/middleware/developed/ecapi/psapi/inc
  PLAT/middleware/developed/common/inc
  PLAT/middleware/developed/psnv/inc
  PLAT/middleware/developed/tcpipmgr/app/inc
  PLAT/middleware/developed/tcpipmgr/common/inc
  PLAT/middleware/developed/yrcompress
  PLAT/middleware/thirdparty/lwip/src/include
  PLAT/middleware/thirdparty/lwip/src/include/lwip
  PLAT/middleware/thirdparty/lwip/src/include/posix
  PLAT/middleware/developed/ccio/pub
  PLAT/middleware/developed/ccio/device/inc
  PLAT/middleware/developed/ccio/service/inc
  PLAT/middleware/developed/ccio/custom/inc
  PLAT/middleware/developed/fota/pub
  PLAT/middleware/developed/fota/custom/inc
  PLAT/middleware/developed/at/atdecoder/inc
  PLAT/middleware/developed/at/atps/inc
  PLAT/middleware/developed/at/atps/inc/cnfind
  PLAT/middleware/developed/at/atcust/inc
  PLAT/middleware/developed/at/atcust/inc/cnfind
  PLAT/middleware/developed/at/atentity/inc
  PLAT/middleware/developed/at/atreply/inc
  PLAT/middleware/developed/at/atref/inc
  PLAT/middleware/developed/at/atref/inc/cnfind
  PLAT/core/driver/include
  PLAT/core/common/include
  PLAT/core/multimedia/include
  PLAT/core/tts/include
  PLAT/prebuild/PS/inc
  PLAT/prebuild/PLAT/inc

  PLAT/prebuild/PLAT/inc/usb
  PLAT/driver/board/ec7xx_0h00/inc/audio/storge/flashex
  PLAT/driver/board/ec7xx_0h00/inc/audio/storge/flashex/lfsex
)

target_compile_definitions(sdk_lib PUBLIC
  CHIP_EC718
  TYPE_EC718M
  TYPE_EC718HM

  __USER_CODE__
  CORE_IS_AP
  SDK_REL_BUILD
  RAMCODE_COMPRESS_EN
  REL_COMPRESS_EN
  ARM_MATH_CM3
  FEATURE_LZMA_ENABLE
  WDT_FEATURE_ENABLE=1
  TRACE_LEVEL=5
  SOFTPACK_VERSION=\"\"
  HAVE_STRUCT_TIMESPEC
  FEATURE_FOTAPAR_ENABLE
  __ASSEMBLY__
  __CURRENT_FILE_NAME__=__FILE__

  PSRAM_FEATURE_ENABLE
  FEATURE_EXCEPTION_FLASH_DUMP_ENABLE

  LWIP_NUM_SOCKETS=16

  EC_ASSERT_FLAG
  PM_FEATURE_ENABLE
  UINILOG_FEATURE_ENABLE
  FEATURE_OS_ENABLE
  FEATURE_FREERTOS_ENABLE
  configUSE_NEWLIB_REENTRANT=1
  FEATURE_YRCOMPRESS_ENABLE
  FEATURE_CCIO_ENABLE
  LWIP_CONFIG_FILE=\"lwip_config_cat.h\"
  LFS_NAME_MAX=63
  LFS_DEBUG_TRACE
  FEATURE_UART_HELP_DUMP_ENABLE
  HTTPS_WITH_CA
  FEATURE_HTTPC_ENABLE
  RTE_USB_EN=1
  RTE_ONE_UART_AT=0
  RTE_TWO_UART_AT=0
  __USER_CODE__
  LUAT_USE_NETWORK
  LUAT_USE_LWIP
  __USE_SDK_LWIP__
  LUAT_USE_DNS
  __PRINT_ALIGNED_32BIT__
  _REENT_SMALL
  _REENT_GLOBAL_ATEXIT
  LWIP_INCLUDED_POLARSSL_MD5=1
  LUAT_EC7XX_CSDK
  LUAT_USE_STD_STRING
  LUAT_LOG_NO_NEWLINE
  FEATURE_PS_SMS_AT_ENABLE

  DEBUG_LOG_HEADER_FILE=\"debug_log_ap.h\"

  # printf=printf_
  # sprintf=sprintf_
  # snprintf=snprintf_
  # vsnprintf=vsnprintf_
  # vprintf=vprintf_

  # sprintf=sprintf_
  # snprintf=snprintf_
  # vsnprintf=vsnprintf_
)

SET(FCELF "fcelf.exe")
SET(CP_PKG_IMG "PLAT/prebuild/FW/lib/gcc/${SOC}/${LIB_FW}/cp-demo-flash.bin")
SET(APP_SECT_INFO "PLAT/device/target/board/ec7xx_0h00/ap/gcc/sectionInfo_${SOC}.json")
SET(BTL_SECT_INFO "PLAT/project/ec7xx_0h00/ap/apps/bootloader/GCC/sectionInfo_${SOC}.json")

ADD_CUSTOM_COMMAND(
  TARGET sdk_lib
  PRE_BUILD
  COMMAND ${CMAKE_C_COMPILER}
    -DTYPE_${SOC_U}
    # -DPSRAM_FEATURE_ENABLE
    # -DFEATURE_IMS_ENABLE
    -DFEATURE_FREERTOS_ENABLE
    -E -P -x c PLAT/core/ld/ec7xxxm_0h00_flash.c
    -I PLAT/device/target/board/ec7xx_0h00/common/inc
    -I PLAT/device/target/board/ec7xx_0h00/common/pkginc
    -o ${SOC}_script.ld

  COMMAND ${CMAKE_C_COMPILER}
    -DTYPE_${SOC_U}
    # -DFEATURE_IMS_ENABLE
    -DFEATURE_FREERTOS_ENABLE
    -E -dM PLAT/device/target/board/ec7xx_0h00/common/inc/mem_map.h
    -I PLAT/device/target/board/ec7xx_0h00/common/pkginc
    -o app_mem_map.txt


  DEPENDS PLAT/core/ld/ec7xxxm_0h00_flash.c
)

add_executable(app
  project/example/src/example_main.c
)

ADD_CUSTOM_COMMAND(
  TARGET app
  POST_BUILD
  COMMAND ${OBJCOPY_CMD} -O binary $<TARGET_FILE:app> tmp_app.bin

  COMMAND echo "echo Generating binpkg..." > mk_binpkg.bat
  # COMMAND echo ${FCELF} -C tmp_btl.bin -cfg ${BTL_SECT_INFO} -map btl.elf.map -out ./btl.bin >> mk_binpkg.bat
  COMMAND echo copy app_mem_map.txt mem_map.txt >> mk_binpkg.bat
  COMMAND echo ${FCELF} -C -bin ./tmp_app.bin -cfg ${APP_SECT_INFO} -map ./app.elf.map -out ./app.bin >> mk_binpkg.bat
  COMMAND echo ${FCELF} -M
    # -input btl.bin -addrname BL_PKGIMG_LNA -flashsize BOOTLOADER_PKGIMG_LIMIT_SIZE
    -input app.bin -addrname AP_PKGIMG_LNA -flashsize AP_PKGIMG_LIMIT_SIZE
    -input ${CP_PKG_IMG} -addrname CP_PKGIMG_LNA -flashsize CP_PKGIMG_LIMIT_SIZE
    -pkgmode 1 -banoldtool 1 -productname ${SOC_U}_PRD -def app_mem_map.txt -outfile core.binpkg >> mk_binpkg.bat
)

target_compile_definitions(app PRIVATE
  # STATIC_RECORDS
)

set(PLAT_sdk_libs
  libcore_airm2m.a
  liblwip.a
  liblzma.a
  libmiddleware_ec.a
  libpsnv.a
  libstartup.a
  libtcpipmgr.a
)

set(PLAT_sdk_prebuild_PS_libs
  libps.a
  libpsl1.a
  libpsif.a
)

set(PLAT_sdk_prebuild_PLAT_libs
  libosa.a
  libccio.a
  libdeltapatch.a
  libmiddleware_ec_private.a
  libdriver_private.a
  libusb_private.a
  libfota.a
)

list(TRANSFORM PLAT_sdk_libs PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/PLAT/libs/${SOC}/")
list(TRANSFORM PLAT_sdk_prebuild_PS_libs PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/PLAT/prebuild/PS/lib/gcc/${SOC}/${LIB_PS_PLAT}/")
list(TRANSFORM PLAT_sdk_prebuild_PLAT_libs PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/PLAT/prebuild/PLAT/lib/gcc/${SOC}/${LIB_PS_PLAT}/")

file(GLOB prebuilt_libs
  PLAT/libs/${SOC}/*.a
  PLAT/prebuild/PS/lib/gcc/${SOC}/${LIB_PS_PLAT}/*.a
  PLAT/prebuild/PLAT/lib/gcc/${SOC}/${LIB_PS_PLAT}/*.a
)

target_link_libraries(app
  sdk_lib
  # ${prebuilt_libs}

  ${PLAT_sdk_libs}
  ${PLAT_sdk_prebuild_PS_libs}
  ${PLAT_sdk_prebuild_PLAT_libs}

  # -lps
  # -lpsl1
  # -lpsif

  # -lcore_airm2m
  # -llwip
  # -llzma
  # -lmiddleware_ec
  # -lpsnv
  # -lstartup
  # -ltcpipmgr

  # -losa
  # -lccio
  # -ldeltapatch
  # -lmiddleware_ec_private
  # -ldriver_private
  # -lusb_private
  # -lfota
)

set(all_app_libs
  ${out_dir}/libsdk_lib.a
  ${PLAT_sdk_libs}
  ${PLAT_sdk_prebuild_PS_libs}
  ${PLAT_sdk_prebuild_PLAT_libs}
)

message("all_app_libs ${all_app_libs}")

ADD_CUSTOM_COMMAND(TARGET app POST_BUILD
  COMMAND ${OBJDUMP_CMD} -h app.elf > app.size
  COMMAND ${OBJSIZE_CMD} -G app.elf >> app.size
)
# foreach(lib IN LISTS all_app_libs)
#   ADD_CUSTOM_COMMAND(TARGET app POST_BUILD COMMAND ${OBJSIZE_CMD} -G ${lib} >> app.size)
# endforeach()
