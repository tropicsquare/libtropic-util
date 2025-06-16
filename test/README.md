# Tests

This folder contains scripting used to check if `lt-util` works correctly.

### Dependencies

* python 3
* cryptography package:

```
pip install cryptography
```

### Run tests

Connect dongle (or spi shield), compile lt-util and then execute appropriate tests:

```bash
cd tests/
# When USB dongle with TROPIC01 is used
./run_tests_usb.sh
# or in case of Raspberrypi shield with TROPIC01
# ./run_tests_hw_spi.sh
```

You should see output similar to this:

```
$ ./run_tests_hw_spi.sh 

[COMMAND] RNG test expected fails with invalid length:
  Status:  1
  Status:  1
  Status:  1
[COMMAND] Get 32 random bytes and save as message:
  Status:  0
[COMMAND] Erase slot 0: 
  Status:  0
[COMMAND] Generate EdDSA keypair there: 
  Status:  0
[COMMAND] Get public key
aaaabbb  Status:  0
[COMMAND] Sign message 5 times
  Status:  0
  Status:  0
  Status:  0
  Status:  0
  Status:  0

[INFO] Verify five signatures with python cryptography library
Message: bb36261854c0debcc6996fee49fc881d1136e93ab721f6fc99873291569e88ba
Public key: c82aee57dfe1793b8d98d866e2cf7b94c0b757f5d3e117c7819614f9919690ef
Signature: 774a4c9b14e282cc2e0773561126ea631b2a06b22720cff14efc215a43a928bd05cb62492d3349610121d891eead5233795c6bd2d90b3e179c015ec8cf88270a
Signature verification SUCCEEDED
Message: bb36261854c0debcc6996fee49fc881d1136e93ab721f6fc99873291569e88ba
Public key: c82aee57dfe1793b8d98d866e2cf7b94c0b757f5d3e117c7819614f9919690ef
Signature: 09f52b90fe18b572fbbe462b74bdbaa2160a1c47d9668df8023497a483a389ee5bdb59750cd51d870a4fef01aa1db2a6046cc77c039a133e7d6e271d9509df0e
Signature verification SUCCEEDED
Message: bb36261854c0debcc6996fee49fc881d1136e93ab721f6fc99873291569e88ba
Public key: c82aee57dfe1793b8d98d866e2cf7b94c0b757f5d3e117c7819614f9919690ef
Signature: d379e6f91559e60d1ac3f6cf534501c5b14d5aae8223fe40480c0f12791bed5b6079c85375e48281f856cedf21a7382089ad3d46d2885e3513ac80d8ec5f3b0d
Signature verification SUCCEEDED
Message: bb36261854c0debcc6996fee49fc881d1136e93ab721f6fc99873291569e88ba
Public key: c82aee57dfe1793b8d98d866e2cf7b94c0b757f5d3e117c7819614f9919690ef
Signature: 4ae61b09f2974e40fbee5ce0631b5c73a21fd803879d92aff524694e91134abc5c8c1e3dfe4c685d6e9b09498ad04df8023e9ec0340036013cb744ae30be0a05
Signature verification SUCCEEDED
Message: bb36261854c0debcc6996fee49fc881d1136e93ab721f6fc99873291569e88ba
Public key: c82aee57dfe1793b8d98d866e2cf7b94c0b757f5d3e117c7819614f9919690ef
Signature: 1910f31b04283698e85d4a4b3196c4af9567cd64c708e3638b4c974928de5f01419e0853d7944bc300e3f0520ad0cb081438bc86d6ccd3ab02305e41e2c8e303
Signature verification SUCCEEDED
/home/pi/libtropic-util/test

```


### Verify signature

Signature verification in testing scripts are done with python. If you just want to verify a signature, this `verify_signature.py` script might be used:
```
./verify_signature.py -h
usage: verify_signature.py [-h] --message MESSAGE --public-key PUBLIC_KEY --signature SIGNATURE

Verify Ed25519 signatures

options:
  -h, --help            show this help message and exit
  --message MESSAGE     Message to verify (as string)
  --public-key PUBLIC_KEY
                        Path to public key file
  --signature SIGNATURE
                        Path to signature file

```

For more info about how to use this script have a look into `run_tests.sh`.