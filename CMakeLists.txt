cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

#set(CMAKE_BUILD_TYPE Debug)

set(PATH_LIBTROPIC "libtropic/")

option(HW_SPI      "Compile for hw spi on rpi" OFF)
option(USB_DONGLE_TS1301  "Compile for TS1301 USB dongle" OFF)
option(USB_DONGLE_TS1302  "Compile for TS1302 USB dongle" OFF)

# If none of the options are set, enable USB_DONGLE_TS1302 by default
if(NOT HW_SPI AND NOT USB_DONGLE_TS1301 AND NOT USB_DONGLE_TS1302)
    set(USB_DONGLE_TS1302 ON CACHE BOOL "Compile for TS1302 USB dongle" FORCE)
endif()

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(libtropic_unix_spi_example
        VERSION 0.1.0
        DESCRIPTION "Example of libtropic's usage in Unix environment"
        LANGUAGES C)


###########################################################################
#                                                                         #
#   Sources defined by this project                                       #
#                                                                         #
###########################################################################

if(HW_SPI)
    add_executable(lt-util src/main. src/macandd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_raspberrypi_wiringpi.c)
endif()

if(USB_DONGLE_TS1301)
    add_executable(lt-util src/main.c  src/macandd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_unix_usb_dongle.c)
endif()
if(USB_DONGLE_TS1302)
    add_executable(lt-util src/main.c  src/macandd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_unix_usb_dongle.c)
endif()

###########################################################################
#                                                                         #
#   Add libtropic and set it up                                           #
#                                                                         #
###########################################################################

set(LT_UTILS ON)
#set(LT_ADD_EXAMPLES ON)

# Use trezor crypto as a source of backend cryptography code
set(LT_USE_TREZOR_CRYPTO ON)

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -Wall)
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)
target_compile_options(tropic PRIVATE -Wno-implicit-function-declaration)

if(USB_DONGLE_TS1301)
    target_compile_definitions(lt-util PRIVATE USB_DONGLE_TS1301)
endif()
if(USB_DONGLE_TS1302)
    target_compile_definitions(lt-util PRIVATE USB_DONGLE_TS1302)
endif()

if(HW_SPI)
    target_compile_definitions(lt-util PRIVATE HW_SPI)
endif()

# To see debug messages in the console, pass -DCMAKE_BUILD_TYPE=Debug when invoking cmake
target_compile_definitions(lt-util PRIVATE "$<$<CONFIG:DEBUG>:LT_UTIL_DEBUG>")


###########################################################################
#                                                                         #
#   Link executable                                                       #
#                                                                         #
###########################################################################

target_link_options(lt-util PRIVATE -Wl,--gc-sections)

if(USB_DONGLE_TS1301 OR USB_DONGLE_TS1302)
    target_link_libraries(lt-util PRIVATE tropic)
endif()

if(HW_SPI)
    target_link_libraries(lt-util PRIVATE tropic wiringPi)
endif()
