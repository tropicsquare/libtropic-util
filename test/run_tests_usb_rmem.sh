#!/bin/bash

PATH_TO_BUILD="../build"
UART_PORT="/dev/ttyACM0"
SLOT_NUM=0
cd ${PATH_TO_BUILD}
LINE="---------------------------------------------------------------------------"

# Get 32 random bytes and save as message:"
./lt-util ${UART_PORT}  -r 32 data_to_store; echo "[<<] lt-util returned status: " $?
echo ${LINE}
xxd -p ${PATH_TO_BUILD}/data_to_store | tr -d '\n' && echo ""
# Erase slot 0: "
./lt-util ${UART_PORT}  -m -e ${SLOT_NUM}; echo "[<<] lt-util returned status: " $?
echo ${LINE}
# Store the data
./lt-util ${UART_PORT}  -m -s ${SLOT_NUM} data_to_store; echo "[<<] lt-util returned status: " $?
echo ${LINE}
# Read back the data
./lt-util ${UART_PORT}  -m -r ${SLOT_NUM} data_to_store_readed; echo "[<<] lt-util returned status: " $?
echo ${LINE}
xxd -p ${PATH_TO_BUILD}/data_to_store_readed | tr -d '\n' && echo ""

cd -
