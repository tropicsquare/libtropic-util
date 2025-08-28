#!/bin/bash

# WARNING: THIS IS A WORK IN PROGRESS AND IS NOT YET FULLY FUNCTIONAL

# This script uses lt-util tool together with USB devkit and does following:
# 1. Initializes Mac And Destroy pin verification engine with a pin, additional data and gets back a secret (engine is set to allow 3 tries)
# 2. Does a failed try to retrieve secret - because PIN entered is wrong
# 3. Again does a failed try to retrieve secret - because PIN entered is wrong
# 4. Does a successful try to retrieve secret - because PIN entered is correct

PATH_TO_BUILD="../../build"
UART_PORT="/dev/ttyACM0"
cd ${PATH_TO_BUILD}
LINE="---------------------------------------------------------------------------"

# Get 32 random bytes and save as message:"
./lt-util ${UART_PORT} -mac-set 1234 abcdabcd secret_generated; echo "[<<] lt-util returned status: " $?
echo ${LINE}
xxd -p ${PATH_TO_BUILD}/secret_generated | tr -d '\n' && echo ""
rm secret_generated

# Guess n.1 with a wrong PIN
./lt-util ${UART_PORT} -mac-ver 1233 abcdabcd secret_returned; echo "[<<] lt-util returned status: " $?
xxd -p ${PATH_TO_BUILD}/secret_returned | tr -d '\n' && echo ""
echo ${LINE}

# Guess n.2 with a wrong PIN
./lt-util ${UART_PORT} -mac-ver 1233 abcdabcd secret_returned; echo "[<<] lt-util returned status: " $?
xxd -p ${PATH_TO_BUILD}/secret_returned | tr -d '\n' && echo ""
echo ${LINE}

# Finally guess n.3 with a correct PIN
./lt-util ${UART_PORT} -mac-ver 1234 abcdabcd secret_returned; echo "[<<] lt-util returned status: " $?
xxd -p ${PATH_TO_BUILD}/secret_returned | tr -d '\n' && echo ""
echo ${LINE}

cd -
