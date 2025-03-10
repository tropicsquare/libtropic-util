# lt-util

This utility is a C wrapper for libtropic library. Once compiled, binary can be executed from bash.

Two hardware options are supported, HW SPI or USB dongle.

They are switched by uncommenting expected port layer file in `CMakeLists.txt:34`.


```
# Example usage:

$ ./lt-util

Usage:

	./lt-util -r    [count] [file]            # Random  - Get number of random bytes and store them into file
	./lt-util -e -i [slot]  [file]            # ECC key - Install private key from keypair.bin into a given slot
	./lt-util -e -g [slot]                    # ECC key - Generate private key in a given slot
	./lt-util -e -d [slot]  [file]            # ECC key - Download public key from given slot into file
	./lt-util -e -c [slot]                    # ECC key - Clear given ECC slot
	./lt-util -e -s [slot]  [file1] [file2]   # ECC key - Sign content of file1 with key from a given slot and store resulting signature into file2
	./lt-util -m -s [slot]  [file]            # Memory  - Store content of filename into memory slot
	./lt-util -m -r [slot]  [file]            # Memory  - Read content of memory slot into filename
	./lt-util -m -e [slot]                    # Memory  - Erase content of memory slot

```


# USB Dongle with TROPIC01 chip

One-liner for compiling lt-utils:

```
mkdir build &&  cd build && cmake -DUSB_DONGLE=1 .. && make && cd ../
```

Binary will be produced in `build/` folder.

# HW SPI - raspberrypi shield

If our rpi shield is used, put jumper on a CS2 position. Example is tested on rpi3 and rpi4.

### Wiring

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

### Dependencies

Enable spi:

```
sudo raspi-config  # then go into Interfaces and enable SPI
```

On raspberry pi install wiringPi from released package

```bash
$ wget https://github.com/WiringPi/WiringPi/releases/download/3.14/wiringpi_3.14_arm64.deb

$ sudo apt install ./wiringpi_3.14_arm64.deb
```

Check out installed version:

```bash
$ gpio -v
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

### Compiling

Use following one-liner to produce binary (tested on rpi3 and rpi4):

```
mkdir build &&  cd build && cmake -DHW_SPI=1 .. && make && cd ../
```
Binary will be produced in `build/` folder.

