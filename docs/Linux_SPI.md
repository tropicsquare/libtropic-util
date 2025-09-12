
# Linux SPI

Instruction described here are relevant when `libtropic-util` is used with `[Raspberrypi shield](https://github.com/tropicsquare/tropic01-raspberrypi-shield-hw), or with any Linux system where [TROPIC01](https://www.tropicsquare.com/tropic01) is connected over SPI.

# Wiring on Raspberrypi

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

## Prerequisites
Enable hardware SPI kernel module. On Raspberry Pi you can use `raspi-config`:

```sh
sudo raspi-config  # then go into Interfaces and enable SPI
```

# Clone

Use following command to clone repository:
```bash
git clone --recurse-submodules https://github.com/tropicsquare/libtropic-util
```

# Build
Go to the root of repository and then use one-liner for compiling:
```bash
mkdir build &&  cd build && cmake -DLINUX_SPI=1 .. && make && cd ../
```

Binary will be produced in `build/`. When you execute it without arguments:
```bash
./lt-util
```

It will print out `help` message:
```bash
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




## Troubleshooting
If you have an older chip (engineering sample), you need to use appropriate keys. To use engineering
sample keys, compile lt-util like this:

```sh
mkdir build &&  cd build && cmake -DLINUX_SPI=1 -DLT_BUILD_EXAMPLES=1 -DLT_SH0_PRIV_PATH=../libtropic/provisioning_data/sh0_priv_engineering_sample01.pem .. && make
```