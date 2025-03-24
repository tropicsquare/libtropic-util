#!/bin/bash

PATH_TO_BUILD="../build"

cd ${PATH_TO_BUILD}

echo ""
echo "[COMMAND] Get 32 random bytes and save as message:"
./lt-util -r 32 message; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/message | tr -d '\n' && echo ""

echo "[COMMAND] Erase slot 0: "
./lt-util -e -c 0; echo "  Status: " $?
echo "[COMMAND] Generate EdDSA keypair there: "
./lt-util -e -g 0; echo "  Status: " $?
echo "[COMMAND] Get public key"
./lt-util -e -d 0 public_key; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/public_key | tr -d '\n' && echo ""

echo "[COMMAND] Sign message 5 times"
# Now sign the message five times
./lt-util -e -s 0 message signature1; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/signature1 | tr -d '\n' && echo ""
./lt-util -e -s 0 message signature2; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/signature2 | tr -d '\n' && echo ""
./lt-util -e -s 0 message signature3; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/signature3 | tr -d '\n' && echo ""
./lt-util -e -s 0 message signature4; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/signature4 | tr -d '\n' && echo ""
./lt-util -e -s 0 message signature5; echo "  Status: " $?
#xxd -p ${PATH_TO_BUILD}/signature5 | tr -d '\n' && echo ""

echo ""
echo "[INFO] Verify five signatures with python cryptography library"
../test/verify_signature.py --message message --public-key public_key --signature signature1
../test/verify_signature.py --message message --public-key public_key --signature signature2
../test/verify_signature.py --message message --public-key public_key --signature signature3
../test/verify_signature.py --message message --public-key public_key --signature signature4
../test/verify_signature.py --message message --public-key public_key --signature signature5



cd -