cmake_minimum_required(VERSION 3.13)

set(START_NETIF "pico_ippktif_init")

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

include(CMakeForceCompiler)

SET(CMAKE_C_COMPILER_FORCED   1)
SET(CMAKE_ASM_COMPILER_FORCED 1)
SET(CMAKE_CXX_COMPILER_FORCED 1)

#[[
ec718hm
CHIP_EC718
TYPE_EC718M
TYPE_EC718HM
]]

SET(TARGET_CHIP "ec718hm")

SET(LIB_FW "oc")
SET(LIB_PS_PLAT "full")

SET(SOC "$ENV{SOC}")
string(TOUPPER IS_${SOC} IS_SOC)

message("SOC: ${SOC}")
message("IS_SOC: ${IS_SOC}")

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

set(out_dir ${CMAKE_CURRENT_BINARY_DIR})

SET(LINKER_FLAGS
  -Wl,-Map=<TARGET>.map
  -L ${out_dir} -T ${out_dir}/${TARGET_CHIP}_script.ld
  -L ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/libs/${TARGET_CHIP}
  -L ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/prebuild/PS/lib/gcc/${TARGET_CHIP}/${LIB_PS_PLAT}
  -L ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/prebuild/PLAT/lib/gcc/${TARGET_CHIP}/${LIB_PS_PLAT}

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
  -g3
  -mcpu=cortex-m3
  -mthumb
  -nostartfiles
  -mapcs-frame
  -ffunction-sections
  -fdata-sections
  -fno-isolate-erroneous-paths-dereference
  -freorder-blocks-algorithm=stc

  -fno-strict-aliasing
  -mslow-flash-data
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

  thirdparty/littlefs/*.c
  thirdparty/printf/*.c
)

add_library(sdk_lib ${sdk_src})

target_include_directories(sdk_lib PUBLIC
  PLAT/tools/${TARGET_CHIP}

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

ADD_CUSTOM_COMMAND(
  TARGET sdk_lib
  PRE_BUILD
  COMMAND ${CMAKE_C_COMPILER}
    -DCHIP_EC718
    -DTYPE_EC718M
    -DTYPE_EC718HM
    -E -P -x c ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/core/ld/ec7xxxm_0h00_flash.c
    -I ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/device/target/board/ec7xx_0h00/common/inc
    -I ${CMAKE_CURRENT_SOURCE_DIR}/PLAT/device/target/board/ec7xx_0h00/common/pkginc
    -o ${out_dir}/${TARGET_CHIP}_script.ld
)

add_executable(app
  project/example/src/example_main.c
)

ADD_CUSTOM_COMMAND(
    TARGET app
    POST_BUILD
    COMMAND ${OBJCOPY_CMD} -O binary $<TARGET_FILE:app> app.bin
  )

target_compile_definitions(app PRIVATE
  # STATIC_RECORDS
)

file(GLOB prebuilt_libs
  PLAT/libs/${TARGET_CHIP}/*.a
  PLAT/prebuild/PS/lib/gcc/${TARGET_CHIP}/${LIB_PS_PLAT}/*.a
  PLAT/prebuild/PLAT/lib/gcc/${TARGET_CHIP}/${LIB_PS_PLAT}/*.a
)

target_link_libraries(app
  sdk_lib
  # ${prebuilt_libs}

  -lps
  -lpsl1
  -lpsif

  -lcore_airm2m
  -llwip
  -llzma
  -lmiddleware_ec
  -lpsnv
  -lstartup
  -ltcpipmgr

  -losa
  -lccio
  -ldeltapatch
  -lmiddleware_ec_private
  -ldriver_private
  -lusb_private
  -lfota
)
