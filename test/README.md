# Tests

This folder contains scripting used to check if `lt-util` works correctly.

### Dependencies

* python 3
* cryptography package:

```
pip install cryptography
```

### Run tests

Connect dingle (or spi shield), compile lt-util and then execute tests:

```bash
cd tests/
./run_tests.sh
```

### Verify signature

If you just want to verify a signature, `verify_signature.py` script might be used:
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