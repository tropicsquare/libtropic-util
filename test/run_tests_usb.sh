#!/bin/bash

PATH_TO_BUILD="../build"
UART_PORT="/dev/ttyACM0"
cd ${PATH_TO_BUILD}


LINE="---------------------------------------------------------------------------"
#echo ""
#echo "[COMMAND] RNG test expected fails with invalid length:"
./lt-util ${UART_PORT} -r -1 message; echo "[<<] lt-util returned status: " $?
echo ${LINE}
./lt-util ${UART_PORT}  -r 0 message; echo "[<<] lt-util returned status: " $?
echo ${LINE}
./lt-util ${UART_PORT}  -r 256 message; echo "[<<] lt-util returned status: " $?
echo ${LINE}

#echo "[COMMAND] Get 32 random bytes and save as message:"
./lt-util ${UART_PORT}  -r 32 message; echo "[<<] lt-util returned status: " $?
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/message | tr -d '\n' && echo ""

#echo "[COMMAND] Erase slot 0: "
./lt-util ${UART_PORT}  -e -c 0; echo "[<<] lt-util returned status: " $?
echo ${LINE}
#echo "[COMMAND] Generate EdDSA keypair there: "
./lt-util ${UART_PORT}  -e -g 0; echo "[<<] lt-util returned status: " $?
echo ${LINE}
#echo "[COMMAND] Get public key"
./lt-util ${UART_PORT}  -e -d 0 public_key; echo "[<<] lt-util returned status: " $?
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/public_key | tr -d '\n' && echo ""

#echo "[COMMAND] Sign message 5 times"
# Now sign the message five times
./lt-util ${UART_PORT}  -e -s 0 message signature1; echo "[<<] lt-util returned status: " $?
../test/verify_signature.py --message message --public-key public_key --signature signature1
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/signature1 | tr -d '\n' && echo ""
./lt-util ${UART_PORT}  -e -s 0 message signature2; echo "[<<] lt-util returned status: " $?
../test/verify_signature.py --message message --public-key public_key --signature signature2
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/signature2 | tr -d '\n' && echo ""
./lt-util ${UART_PORT}  -e -s 0 message signature3; echo "[<<] lt-util returned status: " $?
../test/verify_signature.py --message message --public-key public_key --signature signature3
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/signature3 | tr -d '\n' && echo ""
./lt-util ${UART_PORT}  -e -s 0 message signature4; echo "[<<] lt-util returned status: " $?
../test/verify_signature.py --message message --public-key public_key --signature signature4
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/signature4 | tr -d '\n' && echo ""
./lt-util ${UART_PORT}  -e -s 0 message signature5; echo "[<<] lt-util returned status: " $?
../test/verify_signature.py --message message --public-key public_key --signature signature5
echo ${LINE}
#xxd -p ${PATH_TO_BUILD}/signature5 | tr -d '\n' && echo ""

cd -