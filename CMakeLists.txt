cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

#set(CMAKE_BUILD_TYPE Debug)

set(PATH_LIBTROPIC "libtropic/")

option(HW_SPI      "Compile for hw spi on rpi" OFF)
option(USB_DONGLE  "Compile for usb dongle" ON)


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
    add_executable(lt-util src/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_unix_raspberrypi.c)
endif()

if(USB_DONGLE)
    add_executable(lt-util src/main.c
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


###########################################################################
#                                                                         #
#   Link executable                                                       #
#                                                                         #
###########################################################################

target_link_options(lt-util PRIVATE -Wl,--gc-sections)

if(USB_DONGLE)
    target_link_libraries(lt-util PRIVATE tropic)
endif()


if(HW_SPI)
    target_link_libraries(lt-util PRIVATE tropic, wiringpi)
endif()