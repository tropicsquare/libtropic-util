# lt-util

[!WARNING]
This software shall not be used in production. Use with care. Basically this is a C wrapper for libtropic library. Once compiled, it can be executed from bash and used for direct access to TROPIC01 features. 

Contributors, please follow [guidelines](https://github.com/tropicsquare/libtropic-util/blob/main/CONTRIBUTING.md).

## Clone

Use following command to clone repository:
```
git clone --recurse-submodules https://github.com/tropicsquare/libtropic-util
```

and follow building instruction based on what hardware you have.


## Compile

Compilation differs based on what hardware are you compiling for. At the moment we support two hardware devkits - `raspberrypi shield` or `USB dongle`.

# USB Dongle with TROPIC01 chip

First get into the folder of cloned repository:
```
cd <path_to_cloned_libtropic-util_repository>
```

One-liner for compiling:
```
mkdir build &&  cd build && cmake -DUSB_DONGLE_TS1302=1 .. && make && cd ../
```

Binary will be produced in `build/` folder.

When compiled for USB dongle, interface allows to specify serialport.

Executing compiled lt-util like this:
```bash
./lt-util
```

Expected output:
```bash

Usage:
Usage (first parameter is serialport with usb dongle, update it if needed):

	./lt-util <serialport> -r    <count> <file>            # Random  - Get 1-255 random bytes and store them into file
	./lt-util <serialport> -e -i <slot>  <file>            # ECC key - Install private key from keypair.bin into a given slot
	./lt-util <serialport> -e -g <slot>                    # ECC key - Generate private key in a given slot
	./lt-util <serialport> -e -d <slot>  <file>            # ECC key - Download public key from given slot into file
	./lt-util <serialport> -e -c <slot>                    # ECC key - Clear given ECC slot
	./lt-util <serialport> -e -s <slot>  <file1> <file2>   # ECC key - Sign content of file1 (max size is 4095B) with key from a given slot and store resulting signature into file2
	./lt-util <serialport> -m -s <slot>  <file>            # Memory  - Store content of filename (max size is 444B)  into memory slot
	./lt-util <serialport> -m -r <slot>  <file>            # Memory  - Read content of memory slot (max size is 444B) into filename
	./lt-util <serialport> -m -e <slot>                    # Memory  - Erase content of memory slot

	 All commands return 0 if success, otherwise 1


```

In case of problems with accessing serialport, make sure that your user is a member of `dialout` group. On usual linux distributions you can do it like this:

```
sudo adduser YOUR_USER dialout
```

Don't forget log out and log in afterwards.


# Raspberry Pi shield (uses hw SPI)

There is more instructions when compiling for our raspberrypi shield.

## Wire chip

If you have our official shield, put jumper on CS2 position, otherwise wire TROPIC01 according to following scheme:

```
                    Connector J8 on Raspberry Pi:
         3v3  -->        3V3  (1) (2)  5V    
                       GPIO2  (3) (4)  5V    
                       GPIO3  (5) (6)  GND   
                       GPIO4  (7) (8)  GPIO14
         GND  -->        GND  (9) (10) GPIO15
                      GPIO17 (11) (12) GPIO18
                      GPIO27 (13) (14) GND   
                      GPIO22 (15) (16) GPIO23
                         3V3 (17) (18) GPIO24
         MOSI -->     GPIO10 (19) (20) GND   
         MISO -->      GPIO9 (21) (22) GPIO25    <--  CS
          CLK -->     GPIO11 (23) (24) GPIO8 
                         GND (25) (26) GPIO7 
                       GPIO0 (27) (28) GPIO1 
                       GPIO5 (29) (30) GND   
                       GPIO6 (31) (32) GPIO12
                      GPIO13 (33) (34) GND   
                      GPIO19 (35) (36) GPIO16
                      GPIO26 (37) (38) GPIO20
                         GND (39) (40) GPIO21

```

## Install dependencies

Enable hardware spi:

```
sudo raspi-config  # then go into Interfaces and enable SPI
```

Install wiringPi from released package

```bash
wget https://github.com/WiringPi/WiringPi/releases/download/3.14/wiringpi_3.14_arm64.deb

sudo apt install ./wiringpi_3.14_arm64.deb
```

Check out installed version:

```bash
gpio -v
```

Expected output:
```
gpio version: 3.14
Copyright (c) 2012-2025 Gordon Henderson and contributors
This is free software with ABSOLUTELY NO WARRANTY.
For details type: gpio -warranty

Hardware details:
  Type: Pi 3, Revision: 02, Memory: 1024MB, Maker: Sony UK

System details:
  * Device tree present.
      Model: Raspberry Pi 3 Model B Rev 1.2
  * Supports full  user-level GPIO access via memory.
  * Supports basic user-level GPIO access via /dev/gpiomem.
  * Supports basic user-level GPIO access via /dev/gpiochip (slow).

```

## Compile

First get into the folder of cloned repository:
```
cd <path_to_cloned_libtropic-util_repository>
```

Then use this one-line comand for compiling (tested on rpi3 and rpi4):
```
mkdir build &&  cd build && cmake -DHW_SPI=1 .. && make && cd ../
```
Binary will be produced in `build/` folder. When compiled with `HW-SPI=1`, note that serialport parameter is not available.

```
 ./build/lt-util
```

Expected output:
```

Usage:

	./lt-util -r    [count] [file]            # Random  - Get 1-255 random bytes and store them into file
	./lt-util -e -i [slot]  [file]            # ECC key - Install private key from keypair.bin into a given slot
	./lt-util -e -g [slot]                    # ECC key - Generate private key in a given slot
	./lt-util -e -d [slot]  [file]            # ECC key - Download public key from given slot into file
	./lt-util -e -c [slot]                    # ECC key - Clear given ECC slot
	./lt-util -e -s [slot]  [file1] [file2]   # ECC key - Sign content of file1 (max size is 4095B) with key from a given slot and store resulting signature into file2
	./lt-util -m -s [slot]  [file]            # Memory  - Store content of filename (max size is 444B)  into memory slot
	./lt-util -m -r [slot]  [file]            # Memory  - Read content of memory slot (max size is 444B) into filename
	./lt-util -m -e [slot]                    # Memory  - Erase content of memory slot

```

# Test

Check out [test](https://github.com/tropicsquare/libtropic-util/test/README.md) readme in `test/` folder, there are steps how to check if everything works properly.