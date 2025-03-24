#!/usr/bin/env python3

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography.exceptions import InvalidSignature

def verify_signature(message: bytes, public_key_bytes: bytes, signature: bytes) -> bool:
    try:
        # Load the public key
        public_key = ed25519.Ed25519PublicKey.from_public_bytes(public_key_bytes)

        # Verify the signature
        try:
            public_key.verify(signature, message)
            return True
        except InvalidSignature:
            return False

    except Exception as e:
        print(f"Error during signature verification: {e}")
        return False

def main():
    import argparse

    parser = argparse.ArgumentParser(description='Verify Ed25519 signatures')
    parser.add_argument('--message', type=str, required=True,
                        help='Message to verify (as string)')
    parser.add_argument('--public-key', type=str, required=True,
                        help='Path to public key file')
    parser.add_argument('--signature', type=str, required=True,
                        help='Path to signature file')

    args = parser.parse_args()

    try:
        # Read message, public key and signature from files
        with open(args.message, 'rb') as f:
            message = f.read()
            print(f"Message: {message.hex()}")

        with open(args.public_key, 'rb') as f:
            public_key_bytes = f.read()
            print(f"Public key: {public_key_bytes.hex()}")

        with open(args.signature, 'rb') as f:
            signature = f.read()
            print(f"Signature: {signature.hex()}")

        # Verify the signature
        is_valid = verify_signature(
            message,
            public_key_bytes,
            signature
        )

        print(f"Signature verification {'SUCCEEDED' if is_valid else 'FAILED'}")

    except FileNotFoundError as e:
        print(f"Error: Could not find file - {e}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == '__main__':
    main()
    