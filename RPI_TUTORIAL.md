# Tutorial: Evaluation of libtropic on Raspberry Pi with Unix SPI drivers
Libtropic is an SDK written in C. It is application interface for communication with the TROPIC01 secure element chip. Library is maintained by Tropic Square and simplifies integration into user application. Tropic Square strongly recommend using this library as encapsulate low level communication on SPI interface to exposed API calls (check out [API documentation](https://github.com/tropicsquare/tropic01/blob/main/doc/api/tropic01_user_api_v1.1.2.pdf)).   
For documentation of the libtropic SDK, please visit our [GitHub Pages](https://tropicsquare.github.io/libtropic/).

In this document, we provide information on application example lt-util, which integrates the [libtropic SDK](https://github.com/tropicsquare/libtropic) repository as a submodule and builds a command line application which demonstrates TROPIC01 features and recommended integration. The lt-util application implements support for the SPI present on the RPi using both wiringPi drivers and a native SPI kernel driver (spidev). In this tutorial, we will focus on using native SPI driver.

## Quick Start
The lt-util app has to be compiled from source code. To build lt-util application from the source code you will need CMake build system on your target platform. 

### Steps to build lt-utils from source
1. Clone the repository:
```sh
git clone --recursive https://github.com/tropicsquare/libtropic-util.git
```

2. Build with CMake:
```sh
cd libtropic-util
mkdir build
cd build
cmake -DUNIX_SPI=1 -DLT_BUILD_EXAMPLES=1 ..
make
```

> [!TIP]
> If you need more verbose output or use debugging tools, add `-DCMAKE_BUILD_TYPE=Debug` to the other CMake flags on the line 4 above.

3. Run:
```sh
./lt-util
```

> [!INFO]
> This will output the usage of this example, showing all available flags and arguments that can be passed to the program.

### Verifying chip information
The first step is to check out information about the chip. Run `lt-util` with `-i`:
```sh
./lt-util -i 
```

This will check that your platform can communicate with TROPIC01 properly via SPI and also print
various information about the chip, like firmware version, silicon revision and so on.

### Running demo script
We have prepared a script, which will run various commands and output results as a demonstration
of lt-util. Check out [`run_tests_hw_spi.sh`](test/run_tests_hw_spi.sh).

## Troubleshooting
1. The SPI is not working.
> You may need to activate the kernel module using `raspi-config`. Check out [this tutorial](https://www.raspberrypi-spy.co.uk/2014/08/enabling-the-spi-interface-on-the-raspberry-pi/).

2. The Secure Session cannot be estabilished or the chip does not respond.
> Beware on which revision of the chip you use. The chip revisions are called ABAB for Engineering Samples and ACAB for Production Silicon. 
> Run following command to identify which silicon revision you have available:
> ```sh
> ./lt-util -i  
> ```
> 
> By default, keys for Production Silicon are used in `lt-util`. If you need to use Engineering Sample for some reason, 
> compile with appropriate keys like this:
> ```sh
> cd libtropic-util
> mkdir build
> cd build
> cmake -DUNIX_SPI=1 -DLT_BUILD_EXAMPLES=1 -DLT_SH0_PRIV_PATH=../libtropic/provisioning_data/sh0_priv_engineering_sample01.pem ..
> make
> ```

3. Other problems
> - If you are a customer, contact Tropic Square via [Support Portal](http://support.tropicsquare.com), or contact your business partners.
> - Otherwise, [open an issue](https://github.com/tropicsquare/libtropic-util/issues/new/choose).

## Details About the libtropic-util Implementation
The libtropic-util example is implemented in the [main.c](src/main.c) source file.

To make the libtropic API and other related functionalities available, a few includes are done in the beginning of the file:
```c
#include "libtropic.h"         // The core API functions.
#include "libtropic_port.h"    // Common HAL functions, required by libtropic for every port.
#include "libtropic_logging.h" // Macros for logging at INFO, WARNING and ERROR levels.
#include "libtropic_common.h"  // Definitions of commonly used macros, structures or enums.
```

> [!INFO]
> All libtropic API functions are prefixed with 'lt_'.

In the `main.c` file, there are macros for various communication interfaces, but the `UNIX_SPI` is the important one for this document / RPi example.

In the `main()` function for the `UNIX_SPI` interface, the device is firstly initialized:
```c
// This will setup mappings compatible with RPi and our RPi shield.
lt_dev_unix_spi_t device = {0}; // This structure is defined in libtropic_port.h.
strcpy(device.gpio_dev, "/dev/gpiochip0");
strcpy(device.spi_dev, "/dev/spidev0.0");
device.spi_speed = 1000000; // 1 MHz
device.gpio_cs_num = 25;    // GPIO 25 as on RPi shield.

// This initializes the device field in the libtropic's handle, later used in all API functions.
lt_handle_t h; // This structure is defined in libtropic_common.h.
h.l2.device = &device;
```

After the initialization, based on the input from the command line, the arguments are parsed and specific commands are executed. For example, if the `-i` argument is passed, the `process_chip_id()` function is called:

```c
if (argc == 2) {
  if (strcmp(argv[1], CHIP_ID) == 0) {
      return process_chip_id(&h);
  }
}
```

The `process_chip_id()` function is not an API function and is defined in the discussed `main.c` file. This function is just a wrapper for a sequence of libtropic API calls to print the chip ID to stdout. Let’s take a look at it:

```c
static int process_chip_id(lt_handle_t *h) {
    
    struct lt_chip_id_t chip_id; // Defined in libtropic_common.h, encapsulates the CHIP_ID structure.

    // Libtropic API function to initialize the libtropic's handle to be ready for the communication.
    // It should always be called in the beginning of the program or after lt_deinit().
    lt_ret_t ret = lt_init(h);
    
    // Check if lt_init() was successful. Return value should be checked for every API call.
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }

    // Libtropic API function to execute a Layer 2 request for the CHIP_ID structure from the chip.
    ret = lt_get_info_chip_id(h, &chip_id);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Error lt_get_info_chip_id: %s", lt_ret_verbose(ret));
        return 1;
    }
    
    // Helper libtropic function to parse and print the CHIP_ID structure using the passed printf-like function -- in this case, directly printf().
    ret = lt_print_chip_id(&chip_id, printf);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Error lt_print_chip_id: %s", lt_ret_verbose(ret));
        return 1;
    }

    // Libtropic API function to Deinitialize the handle. The result of this call also deinitializes the device interface used for communication with the chip.
    // Return value should be checked, but in this case, there is nothing to do anyway if the error occurs.
    lt_deinit(h);

    return 0;
}
```

> ![INFO]
> Naturally, there are also other wrapper functions like `process_chip_id` in the discussed `main.c` file (all have the prefix `process_`). Some of them execute Layer 3 API calls, which also require establishing a secure session with one of the pairing key slots, usually with the provisioned slot 0. For establishing the secure session, we recommend using the helper function `verify_chip_and_start_secure_session()` from `libtropic.c`, which executes multiple API command calls, including the two most important ones: `lt_get_st_pub()` and `lt_session_start()`.

## Creating a New Application / Embedded Platform with libtropic SDK
All necessary HAL functions are declared in the libtropic header [`libtropic_port.h`](https://github.com/tropicsquare/libtropic/blob/master/include/libtropic_port.h). The implementation of these HAL functions should be put in the directory `libtropic/hal/port/`.

More specifically, the discussed Unix SPI port is implemented in the file [`libtropic/hal/port/unix/lt_port_unix.c`](https://github.com/tropicsquare/libtropic/blob/develop/hal/port/unix/lt_port_unix.c).