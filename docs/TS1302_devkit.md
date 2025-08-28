
# TS1302 devkit with TROPIC01 chip

Instruction described here are relevant when `libtropic-util` is used with `[TS1302 USB devkit](https://github.com/tropicsquare/tropic01-stm32u5-usb-devkit-hw)



# Build
Go to the root of repository and then use one-liner for compiling:
```bash
mkdir build &&  cd build && cmake -DUSB_DONGLE_TS1302=1 .. && make && cd ../
```

Binary will be produced in `build/`. When you execute it without arguments:

```bash
./lt-util
```

It will print out `help` message:
```bash
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

# Example usage

For example getting 100 random bytes into file `filename`:

```
./lt-util /dev/ttyACM0  -r 100 filename
```

For more examples have a look into `test/` folder. You can execute tests there to be sure that all works.

# Access rights

In case of problems with accessing serialport, make sure that your user is a member of `dialout` group. On usual linux distributions you can do it like this:

```
sudo adduser YOUR_USER dialout
```

Don't forget log out and log in afterwards.

