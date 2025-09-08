/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @details This tool is meant to be used to evaluate TROPIC01 on various platform. It is not meant to be used in production.
 * Currently it supports USB dongle TS1301 and TS1302, and HW SPI interface.
 * Choose the right one by defining -DUSB_DONGLE_TS1301=1, -DUSB_DONGLE_TS1302=1 or -DLINUX_SPI=1 when compiling the project.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libtropic.h"
#if USB_DONGLE_TS1301 || USB_DONGLE_TS1302
#include "lt_port_unix_usb_dongle.h"
#endif
#if LINUX_SPI
#include "lt_port_unix_spi.h"
#endif
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "macandd.h"

#define LT_LOG_CMD(f_, ...) LT_LOG("[CMD] " f_, ##__VA_ARGS__)

#define HEX_DATA_SIZE 32

/**
 * @brief Devices which were distributed by Tropic Square company are populated with different versions of engineering samples of TROPIC01. Each TROPIC01
 * is by default distributed with initial pairing keys (SH0).
 * These keys are used to establish secure channel with the device and to perform various operations.
 * If you are not able to establish handshake with your device, you may need to change the keys.
 *
 * For more info please read datasheet to find out how handshake works.
 *
 * @details As a rule of thumb:
 *     keys used in TS1301 are defined under "Engineering Samples 01"
 *     keys used in TS1302 are defined under "Engineering Samples 02"
 *     Raspberrypi shield (HW SPI) was distributed with both, so you might need to check which keys work.
 *     If you have Mikroe click shield or arduino shield, you might need to check as well which keys work.
 *
 */
int8_t pkey_index_0 =  PAIRING_KEY_SLOT_INDEX_0;
#if USB_DONGLE_TS1301
#pragma message("Compiling for USB_DONGLE_TS1301")
#define ENGINEERING_SAMPLES_01
#endif
#if USB_DONGLE_TS1302
#pragma message("Compiling for USB_DONGLE_TS1302")
#define ENGINEERING_SAMPLES_02
#endif
#if LINUX_SPI
// In rare situation (very old devkit) you might need to define ENGINEERING_SAMPLES_01 here
#define ENGINEERING_SAMPLES_02
#endif
//#if defined(XXX)
//// code for XXX
//#elif defined(YYY)
//// code for YYY
//#else
//// code for ZZZ
//#endif
#ifdef ENGINEERING_SAMPLES_01
// Engineering samples 01 keys:
#pragma message("Compiling lt-util with ENGINEERING_SAMPLES_01 keys, please check if they are correct for your device")
uint8_t sh0priv[] = {0xd0,0x99,0x92,0xb1,0xf1,0x7a,0xbc,0x4d,0xb9,0x37,0x17,0x68,0xa2,0x7d,0xa0,0x5b,0x18,0xfa,0xb8,0x56,0x13,0xa7,0x84,0x2c,0xa6,0x4c,0x79,0x10,0xf2,0x2e,0x71,0x6b};
uint8_t sh0pub[]  = {0xe7,0xf7,0x35,0xba,0x19,0xa3,0x3f,0xd6,0x73,0x23,0xab,0x37,0x26,0x2d,0xe5,0x36,0x08,0xca,0x57,0x85,0x76,0x53,0x43,0x52,0xe1,0x8f,0x64,0xe6,0x13,0xd3,0x8d,0x54};
#endif
#ifdef ENGINEERING_SAMPLES_02
// Engineering samples 02 keys
#pragma message("Compiling lt-util with ENGINEERING_SAMPLES_02 keys, please check if they are correct for your device")
uint8_t sh0priv[] = {0x28,0x3F,0x5A,0x0F,0xFC,0x41,0xCF,0x50,0x98,0xA8,0xE1,0x7D,0xB6,0x37,0x2C,0x3C,0xAA,0xD1,0xEE,0xEE,0xDF,0x0F,0x75,0xBC,0x3F,0xBF,0xCD,0x9C,0xAB,0x3D,0xE9,0x72};
uint8_t sh0pub[]  = {0xF9,0x75,0xEB,0x3C,0x2F,0xD7,0x90,0xC9,0x6F,0x29,0x4F,0x15,0x57,0xA5,0x03,0x17,0x80,0xC9,0xAA,0xFA,0x14,0x0D,0xA2,0x8F,0x55,0xE7,0x51,0x57,0x37,0xB2,0x50,0x2C};
#endif

// CHIP_ID
#define CHIP_ID "-i"

// RNG
#define RNG         "-r"
// ECC
#define ECC "-e"
#define ECC_INSTALL  "-i"
#define ECC_GENERATE "-g"
#define ECC_DOWNLOAD "-d"
#define ECC_CLEAR    "-c"
#define ECC_SIGN     "-s"
// MEM
#define MEM "-m"
#define MEM_STORE    "-s"
#define MEM_READ     "-r"
#define MEM_ERASE    "-e"
// Mac And Destroy
#define MAC_SET      "-mac-set"
#define MAC_VERIFY   "-mac-ver"

#if (USB_DONGLE_TS1301 || USB_DONGLE_TS1302)
void print_usage(void) {
    printf("\r\nUsage (first parameter is serialport with usb dongle, update it if needed):\r\n\n"
"\t./lt-util /dev/ttyACM0 "CHIP_ID"            		        # Print Chip ID information\r\n"
"\t./lt-util /dev/ttyACM0 "RNG"    <count> <file>            # Random  - Get 1-255 random bytes and store them into file\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" "  ECC_INSTALL" <slot>  <file>            # ECC key - Install private key from keypair.bin into a given slot\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_GENERATE" <slot>                    # ECC key - Generate private key in a given slot\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_DOWNLOAD" <slot>  <file>            # ECC key - Download public key from given slot into file\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_CLEAR" <slot>                    # ECC key - Clear given ECC slot\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_SIGN" <slot>  <file1> <file2>   # ECC key - Sign content of file1 (max size is 4095B) with key from a given slot and store resulting signature into file2\r\n"
"\t./lt-util /dev/ttyACM0 "MEM" " MEM_STORE" <slot>  <file>            # Memory  - Store content of filename (max size is 444B)  into memory slot\r\n"
"\t./lt-util /dev/ttyACM0 "MEM" " MEM_READ" <slot>  <file>            # Memory  - Read content of memory slot (max size is 444B) into filename\r\n"
"\t./lt-util /dev/ttyACM0 "MEM" " MEM_ERASE" <slot>                    # Memory  - Erase content of memory slot\r\n\n"
// Mac and Destroy is not exposed until it works stable
// lt-util -mac-set <pin> <add> <secret_generated>
// lt-util -mac-ver <pin> <add> <secret_returned>
"\t All commands return 0 if success, otherwise 1\r\n\n");
}
#endif

#ifdef LINUX_SPI
void print_usage(void) {
    printf("\r\nUsage:\r\n\n"
"\t./lt-util "CHIP_ID"                              # Print chip identification\r\n"
"\t./lt-util "RNG"    <count> <file>            # Random  - Get 1-255 random bytes and store them into file\r\n"
"\t./lt-util "ECC" "  ECC_INSTALL" <slot>  <file>            # ECC key - Install private key from filename into a given slot (0-31)\r\n"
"\t./lt-util "ECC" " ECC_GENERATE" <slot>                    # ECC key - Generate private key in a given slot (0-31)\r\n"
"\t./lt-util "ECC" " ECC_DOWNLOAD" <slot>  <file>            # ECC key - Download public key from given slot (0-31) into file\r\n"
"\t./lt-util "ECC" " ECC_CLEAR" <slot>                    # ECC key - Clear given ECC slot (0-31)\r\n"
"\t./lt-util "ECC" " ECC_SIGN" <slot>  <file1> <file2>   # ECC key - Sign content of file1 (max size is 4095B) with key from a given slot (0-31) and store resulting signature into file2\r\n"
"\t./lt-util "MEM" " MEM_STORE" <slot>  <file>            # Memory  - Store content of filename (max size is 444B)  into memory slot (0-511)\r\n"
"\t./lt-util "MEM" " MEM_READ" <slot>  <file>            # Memory  - Read content of memory slot (0-511) into filename (max size is 444B)\r\n"
"\t./lt-util "MEM" " MEM_ERASE" <slot>                    # Memory  - Erase content of memory slot (0-511)\r\n\n"
// Mac and Destroy is not exposed until it works stable
// lt-util -mac-set <pin> <add> <secret_generated>
// lt-util -mac-ver <pin> <add> <secret_returned>
"\t All commands return 0 if success, otherwise 1.\r\n\n"
"Notes:\r\n\n"
"\t - Each command creates a new secure session.\r\n"
"\t - Files are read and stored as binaries. Use hexdump or similar tools to inspect contents of the files.\r\n");
}
#endif

static int process_rng_get(lt_handle_t *h, char *count_in, char *file) {
    if(!count_in || !file) {
        LT_LOG_ERROR("Error, NULL parameters process_rng_get()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "RNG" %s %s", count_in, file);
    }

    // Parsing count number
    char *endptr;
    long int count = strtol(count_in, &endptr, 10);
    if((count > RANDOM_VALUE_GET_LEN_MAX) | (count <= 1)) {
        LT_LOG_ERROR("Invalid length passed, use number between 1-255");
        return 1;
    }
    // TODO check endptr

    // Opening file into which random bytes will be written
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error opening file %s", file);
        return 1;
    }
    LT_LOG_INFO("File \"%s\" opened for writing", file);

    // Get random bytes from TROPIC01 into bytes[] buffer
    uint8_t bytes[RANDOM_VALUE_GET_LEN_MAX] = {0};
    lt_ret_t ret;
    ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return ret;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }

    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }

    ret = lt_random_value_get(h, bytes, count);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    } else {
        LT_LOG_INFO("lt_random_value_get(): %s", lt_ret_verbose(ret));
    }

    // Store content of bytes[] buffer into file
    size_t written = fwrite(bytes, sizeof(uint8_t), count, fp);
    if(written !=count) {
        LT_LOG_ERROR("Error writing into file, %zu written", written);
        fclose(fp);
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Wrote %zu bytes into file \"%s\"", written, file);
    }

    fclose(fp);

    lt_deinit(h);

    return 0;
}

static int process_ecc_install(lt_handle_t *h, char *slot_in, char *file) {
    if(!slot_in || !file) {
        LT_LOG_ERROR("Error, NULL parameters process_rng_get()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "ECC" "ECC_INSTALL" %s %s", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    }
    LT_LOG_INFO("Slot number: %ld is valid", slot);

    // Opening file from which first 32B will be taken as private key
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error process_ecc_install() opening fileE");
        return 1;
    }
    LT_LOG_INFO("File \"%s\" opened for reading", file);

    // Read keypair from file into keypair[] buffer
    uint8_t keypair[64] = {0};
    size_t read = fread(keypair, sizeof(uint8_t), 64, fp);
    LT_LOG_INFO("Number of bytes read: %zu", read);
    fclose(fp);

    // Install first 32B from keypair[] into ecc slot priv key
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    ret = lt_ecc_key_store(h, slot, CURVE_ED25519, keypair); // Only first 32B will be taken
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_ecc_key_store(): %s", lt_ret_verbose(ret));
    }

    lt_deinit(h);

    LT_LOG("OK");
    return 0;
}

static int process_ecc_generate(lt_handle_t *h, char *slot_in) {
    if(!slot_in) {
        LT_LOG_ERROR("Error, NULL parameters process_ecc_clear()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "ECC" "ECC_GENERATE" %s", slot_in);
    }

     // Parsing slot number
     char *endptr;
     long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Generate EdDSA  private key in a given slot
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return ret;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_ecc_key_generate(h, (uint8_t)slot, CURVE_ED25519);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    } else {
        LT_LOG_INFO("lt_ecc_key_generate() : %s", lt_ret_verbose(ret));
    }

    lt_deinit(h);

    return 0;
}

static int process_ecc_download(lt_handle_t *h, char *slot_in, char *file) {

    if(!slot_in || !file) {
        LT_LOG_ERROR("Error, NULL parameters process_rng_get()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "ECC" "ECC_DOWNLOAD" %s %s", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Opening file
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error process_ecc_download() opening file");
        return 1;
    } else {
        LT_LOG_INFO("File \"%s\" opened for writing", file);
    }

    // Get random bytes from TROPIC01
    uint8_t pubkey[64] = {0};
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_ecc_key_read(h, (uint8_t)slot, pubkey, &curve, &origin);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_ecc_key_read(): %s", lt_ret_verbose(ret));
    }

    size_t written = fwrite(pubkey, sizeof(uint8_t), 32, fp);
    LT_LOG_INFO("Number of elements written: %zu", written);

    fclose(fp);

    lt_deinit(h);

    return 0;
}

static int process_ecc_clear(lt_handle_t *h, char *slot_in) {
    if(!slot_in) {
        LT_LOG_ERROR("Error, NULL parameters process_ecc_clear()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "ECC" "ECC_CLEAR" %s", slot_in);
    }

     // Parsing slot number
     char *endptr;
     long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Clear given slot in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_ecc_key_erase(h, (uint8_t)slot);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_ecc_key_erase(): %s", lt_ret_verbose(ret));
    }

    lt_deinit(h);

    return 0;
}

static int process_ecc_sign(lt_handle_t *h, char *slot_in, char *msg_file_in, char* signature_file_out) {
    if(!slot_in || !msg_file_in || !signature_file_out) {
        LT_LOG_ERROR("Error, NULL parameters process_rng_get()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "ECC" "ECC_SIGN" %s %s %s", slot_in, msg_file_in, signature_file_out);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Opening file from which a hash will be read
    FILE *msg_fp = fopen(msg_file_in, "rb");
    if (msg_fp == NULL) {
        LT_LOG_ERROR("Error opening file %s", msg_file_in);
        return 1;
    } else {
        LT_LOG_INFO("File \"%s\" opened for reading", msg_file_in);
    }

    fseek(msg_fp, 0L, SEEK_END);
    long msg_size = ftell(msg_fp);
    rewind(msg_fp);

    // Opening file into which a signature will be written
    FILE *fp_sig = fopen(signature_file_out, "wb");
    if (fp_sig == NULL) {
        LT_LOG_ERROR("Error opening file %s", signature_file_out);
        return 1;
    } else {
        LT_LOG_INFO("File \"%s\" opened for writing", signature_file_out);
    }

    // Read hash from file
    uint8_t msg[4095] = {0};
    size_t read = fread(msg, sizeof(uint8_t), msg_size, msg_fp);
    LT_LOG_INFO("Number of bytes read: %zu", read);
    fclose(msg_fp);

    // Sign hash in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    uint8_t signature_rs[64] = {0};
    ret = lt_ecc_eddsa_sign(h, (uint8_t)slot, msg, msg_size, signature_rs);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_ecc_eddsa_sign(): %s", lt_ret_verbose(ret));
    }

    // Write signature into file
    size_t written = fwrite(signature_rs, sizeof(uint8_t), 64, fp_sig);
    if(written != 64) {
        LT_LOG_ERROR("Error writing into file, written: %zu", written);
        fclose(fp_sig);
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Wrote %zu bytes into file \"%s\"", written, signature_file_out);
    }

    fclose(fp_sig);

    lt_deinit(h);

    return 0;
}

static int process_mem_store(lt_handle_t *h, char *slot_in, char *file) {
    if(!slot_in || !file) {
        LT_LOG_ERROR("Error, NULL parameters process_mem_store()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "MEM" "MEM_STORE" %s %s", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 511)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Opening file
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error process_ecc_download() opening file");
        return 1;
    } else {
        LT_LOG_INFO("File \"%s\" opened for reading", file);
    }

    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);

    if((sz < 1) || (sz > 444)) {
        LT_LOG_ERROR("Error, size of file to store must be between 1 - 444 B");
        return 1;
    } else {
        LT_LOG_INFO("File size: %ld is valid", sz);
    }

    // Read keypair from file into keypair[] buffer
    uint8_t mem_content[444] = {0};
    size_t read = fread(mem_content, sizeof(uint8_t), sz, fp);
    if(read != sz) {
        LT_LOG_ERROR("Error when reading a file");
        return 1;
    } else {
        LT_LOG_INFO("Read %zu bytes from file", read);
    }

    fclose(fp);

    // Store the content into r memory slot
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }

    ret = lt_r_mem_data_write(h, (uint16_t)slot, mem_content,(uint16_t)sz);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_r_mem_data_write(): %s", lt_ret_verbose(ret));
    }

    lt_deinit(h);

    return 0;

}

static int process_mem_read(lt_handle_t *h, char *slot_in, char *file) {
    if(!slot_in || !file) {
        LT_LOG_ERROR("Error, NULL parameters process_mem_read()");
        return 1;
    } else {
        LT_LOG_CMD("lt-util "MEM" "MEM_READ" %s %s", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 511)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Opening file
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error process_ecc_download() opening file");
        return 1;
    } else {
        LT_LOG_INFO("File \"%s\" opened for writing", file);
    }

    // Read keypair from file into keypair[] buffer
    uint8_t mem_content[444] = {0};

    // Store the content into r memory slot
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    uint16_t data_size;
    ret = lt_r_mem_data_read(h, (uint16_t)slot, mem_content, &data_size);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_r_mem_data_read(): %s", lt_ret_verbose(ret));
    }

    // Store content of bytes[] buffer into file
    size_t written = fwrite(mem_content, sizeof(uint8_t), data_size, fp);
    if(written != data_size) {
        LT_LOG_ERROR("Error writing into file, %zu written", written);
        fclose(fp);
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Wrote %zu bytes into file \"%s\"", written, file);
    }

    fclose(fp);

    lt_deinit(h);

    return 0;

}

static int process_mem_erase(lt_handle_t *h, char *slot_in)
{
    if(!slot_in) {
        LT_LOG_ERROR("Error, NULL parameters process_mem_erase()");
    } else {
        LT_LOG_CMD("lt-util "MEM" "MEM_ERASE" %s", slot_in);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 511)) {
        LT_LOG_ERROR("Error, wrong slot number ");
        return 1;
    } else {
        LT_LOG_INFO("Slot number: %ld is valid", slot);
    }

    // Clear given slot in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_r_mem_data_erase(h, (uint16_t)slot);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("lt_r_mem_data_erase(): %s", lt_ret_verbose(ret));
    }

    lt_deinit(h);

    return 0;
}

// Debug output function to print data in hex format
void print_hex(const uint8_t *data, size_t len) {
    if (!data) {
        printf("(null)\n");
        return;
    }
    for (size_t i = 0; i < len; ++i) {
        printf("%02X", data[i]);
        if (i < len - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

static int process_macandd_set(lt_handle_t *h, char *pin, char *add, char *filename)
{
    if(!h || !pin || !add || !filename) {
        LT_LOG_ERROR("Error, NULL parameters process_macandd_set()");
    } else {
        LT_LOG_CMD("lt-util "MAC_SET" %s %s %s", pin, add, filename);
    }

    // Parse 4-digit PIN string into uint8_t array
    uint8_t pin_bytes[4] = {0};
    size_t pin_len = strlen(pin);
    if (pin_len != 4) {
        LT_LOG_ERROR("PIN must be exactly 4 digits");
        return 1;
    }
    for (size_t i = 0; i < 4; ++i) {
        if (!isdigit((unsigned char)pin[i])) {
            LT_LOG_ERROR("PIN must contain only digits");
            return 1;
        }
        pin_bytes[i] = (uint8_t)(pin[i] - '0');
    }

    // Parse hexadecimal string from 'add' into uint8_t array
    size_t add_len = strlen(add);
    if (add_len % 2 != 0) {
        LT_LOG_ERROR("Address hex string must have even length");
        return 1;
    }
    uint8_t add_bytes_len = add_len / 2;
    uint8_t add_bytes[32] = {0}; // adjust size as needed
    if (add_bytes_len > sizeof(add_bytes)) {
        LT_LOG_ERROR("Address too long, max %zu bytes", sizeof(add_bytes));
        return 1;
    }
    for (size_t i = 0; i < add_bytes_len; ++i) {
        char byte_str[3] = { add[2*i], add[2*i+1], '\0' };
        char *endptr = NULL;
        long val = strtol(byte_str, &endptr, 16);
        if (*endptr != '\0' || val < 0 || val > 0xFF) {
            LT_LOG_ERROR("Invalid hex character in address");
            return 1;
        }
        add_bytes[i] = (uint8_t)val;
    }

    // Clear given slot in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }

    uint8_t secret[32];

    print_hex(pin_bytes, 4);
    print_hex(add_bytes, add_bytes_len);
    printf("%d\r\n", add_bytes_len);

    ret = lt_PIN_set(h, pin_bytes, 4, add_bytes, add_bytes_len, secret);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Error setting PIN and address: %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("PIN and add bytes set successfully");
    }
    print_hex(secret, sizeof(secret));
    printf("\r\n");

    // store secret into file
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error opening file %s for writing", filename);
        return 1;
    }
    size_t written = fwrite(secret, sizeof(uint8_t), sizeof(secret), fp);
    if (written != sizeof(secret)) {
        LT_LOG_ERROR("Error writing secret to file, %zu bytes written", written);
        fclose(fp);
        return 1;
    } else {
        LT_LOG_INFO("Wrote %zu bytes into file \"%s\"", written, filename);
    }

    lt_deinit(h);

    return 0;
}

static int process_macandd_verify(lt_handle_t *h, char *pin, char *add, char *filename)
{
    if(!h || !pin || !add || !filename) {
        LT_LOG_ERROR("Error, NULL parameters process_macandd_verify()");
    } else {
        LT_LOG_CMD("lt-util "MAC_VERIFY" %s %s %s", pin, add, filename);
    }

    // Parse 4-digit PIN string into uint8_t array
    uint8_t pin_bytes[4] = {0};
    size_t pin_len = strlen(pin);
    if (pin_len != 4) {
        LT_LOG_ERROR("PIN must be exactly 4 digits");
        return 1;
    }
    for (size_t i = 0; i < 4; ++i) {
        if (!isdigit((unsigned char)pin[i])) {
            LT_LOG_ERROR("PIN must contain only digits");
            return 1;
        }
        pin_bytes[i] = (uint8_t)(pin[i] - '0');
    }

    // Parse hexadecimal string in 'add' into uint8_t array
    size_t add_len = strlen(add);
    if (add_len % 2 != 0) {
        LT_LOG_ERROR("Address hex string must have even length");
        return 1;
    }
    uint8_t add_bytes_len = add_len / 2;
    uint8_t add_bytes[32] = {0}; // adjust size as needed
    if (add_bytes_len > sizeof(add_bytes)) {
        LT_LOG_ERROR("Address too long, max %zu bytes", sizeof(add_bytes));
        return 1;
    }
    for (size_t i = 0; i < add_bytes_len; ++i) {
        char byte_str[3] = { add[2*i], add[2*i+1], '\0' };
        char *endptr = NULL;
        long val = strtol(byte_str, &endptr, 16);
        if (*endptr != '\0' || val < 0 || val > 0xFF) {
            LT_LOG_ERROR("Invalid hex character in address");
            return 1;
        }
        add_bytes[i] = (uint8_t)val;
    }

    // Clear given slot in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("lt_init(): %s", lt_ret_verbose(ret));
    }
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        LT_LOG_ERROR("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    } else {
        LT_LOG_INFO("Secure channel established: %s", lt_ret_verbose(ret));
    }

    uint8_t secret[32] = {0};
    print_hex(pin_bytes, 4);
    print_hex(add_bytes, add_bytes_len);
    print_hex(secret, sizeof(secret));
    printf("%d\r\n", add_bytes_len);

    ret = lt_PIN_check(h, pin_bytes, 4, add_bytes, add_bytes_len, secret);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_PIN_check(): %s", lt_ret_verbose(ret));
        return 1;
    } else {
        LT_LOG_INFO("PIN checked successfully");
    }

    printf("Secret after\r\n");
    print_hex(secret, sizeof(secret));
    // store secret into file
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        LT_LOG_ERROR("Error opening file %s for writing", filename);
        return 1;
    }
    size_t written = fwrite(secret, sizeof(uint8_t), sizeof(secret), fp);
    if (written != sizeof(secret)) {
        LT_LOG_ERROR("Error writing secret to file, %zu bytes written", written);
        fclose(fp);
        return 1;
    } else {
        LT_LOG_INFO("Wrote %zu bytes into file \"%s\"", written, filename);
    }

    lt_deinit(h);

    return 0;
}

static int process_chip_id(lt_handle_t *h) {

    struct lt_chip_id_t chip_id;

    lt_ret_t ret = lt_init(h);

    if(ret != LT_OK) {
        LT_LOG_ERROR("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }

    ret = lt_get_info_chip_id(h, &chip_id);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Error lt_get_info_chip_id: %s", lt_ret_verbose(ret));
        return 1;
    }

    ret = lt_print_chip_id(&chip_id, printf);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Error lt_print_chip_id: %s", lt_ret_verbose(ret));
        return 1;
    }

    lt_deinit(h);

    return 0;
}

// When compiled for usb dongle, besides inputs used by TROPIC01, API also receives serialport string
#if (USB_DONGLE_TS1301 || USB_DONGLE_TS1302)
int main(int argc, char *argv[]) {
    //LT_LOG("argc %d   %s  %s  %s  %s ", argc, argv[0], argv[1], argv[2], argv[3]);
    if ((argc == 1)) {
        print_usage();
        return 0;
    }

    lt_handle_t h;
    lt_dev_unix_usb_dongle_t uart = {0};
    h.l2.device = &uart;
    uart.baud_rate = 115200;
    strncpy(uart.dev_path, argv[1], DEVICE_PATH_MAX_LEN);
    if (argc == 3) {
        if (strcmp(argv[2], CHIP_ID) == 0) {
            return process_chip_id(&h);
        }
    }
    else if (argc == 5) {
        // RNG
        if(strcmp(argv[2], RNG) == 0) {
            return process_rng_get(&h, argv[3], argv[4]);
        }
        // ECC 5 arguments
        else if(strcmp(argv[2], ECC) == 0) {
            if (strcmp(argv[3], ECC_GENERATE) == 0) {
                process_ecc_generate(&h, argv[4]);
                return 0;
            } else if (strcmp(argv[3], ECC_CLEAR) == 0) {
                return process_ecc_clear(&h, argv[4]);
            }
        }
        // MEM 4 arguments
        else if(strcmp(argv[2], MEM) == 0) {
            if (strcmp(argv[3], MEM_ERASE) == 0) {
                return process_mem_erase(&h, argv[4]);
            }
        }
    } else if (argc == 6) {
        if(strcmp(argv[2], ECC) == 0) {
            if (strcmp(argv[3], ECC_INSTALL) == 0) {
                return process_ecc_install(&h, argv[4], argv[5]);
            } else if (strcmp(argv[3], ECC_DOWNLOAD) == 0) {
                return process_ecc_download(&h, argv[4], argv[5]);
            }
        } else if(strcmp(argv[2], MEM) == 0) {
            if (strcmp(argv[3], MEM_STORE) == 0) {
                return process_mem_store(&h, argv[4], argv[5]);
            } else if (strcmp(argv[3], MEM_READ) == 0) {
                return process_mem_read(&h, argv[4], argv[5]);
            }
        } // Macandd set 6 arguments
        else if(strcmp(argv[2], MAC_SET) == 0) {
            return process_macandd_set(&h, argv[3], argv[4], argv[5]);
        } // Macandd verify 6 arguments
        else if(strcmp(argv[2], MAC_VERIFY) == 0) {
            return process_macandd_verify(&h, argv[3], argv[4], argv[5]);
        }
    } else if (argc == 7) {
        if(strcmp(argv[2], ECC) == 0) {
            if (strcmp(argv[3], ECC_SIGN) == 0) {
                return process_ecc_sign(&h, argv[4], argv[5], argv[6]);
            }
        }
    }
    LT_LOG_ERROR("ERROR wrong parameters entered");
    return 2;
}
#endif
// When compiled for usb dongle, besides inputs used by TROPIC01, API also receives SPI strings
#ifdef LINUX_SPI
int main(int argc, char *argv[]) {
    //LT_LOG ("argc %d   %s  %s  %s  %s \r\n", argc, argv[0], argv[1], argv[2], argv[3]);
    if ((argc == 1)) {
        print_usage();
        return 0;
    }

    // This will setup mappings compatible with RPi and our RPi shield.
    lt_dev_unix_spi_t device = {0};
    strcpy(device.gpio_dev, "/dev/gpiochip0");
    strcpy(device.spi_dev, "/dev/spidev0.0");
    device.spi_speed = 1000000; // 1 MHz
    device.gpio_cs_num = 25;    // GPIO 25 as on RPi shield.

    lt_handle_t h;
    h.l2.device = &device;

    if (argc == 2) {
        if (strcmp(argv[1], CHIP_ID) == 0) {
            return process_chip_id(&h);
        }
    }
    else if (argc == 4) {
        // RNG
        if(strcmp(argv[1], RNG) == 0) {
            return process_rng_get(&h, argv[2], argv[3]);
        }
        // ECC 5 arguments
        else if(strcmp(argv[1], ECC) == 0) {
            if (strcmp(argv[2], ECC_GENERATE) == 0) {
                process_ecc_generate(&h, argv[3]);
                return 0;
            } else if (strcmp(argv[2], ECC_CLEAR) == 0) {
                return process_ecc_clear(&h, argv[3]);
            }
        }
        // MEM 4 arguments
        else if(strcmp(argv[1], MEM) == 0) {
            if (strcmp(argv[2], MEM_ERASE) == 0) {
                return process_mem_erase(&h, argv[3]);
            }
        }
    } else if (argc == 5) {
        if(strcmp(argv[1], ECC) == 0) {
            if (strcmp(argv[2], ECC_INSTALL) == 0) {
                return process_ecc_install(&h, argv[3], argv[4]);
            } else if (strcmp(argv[2], ECC_DOWNLOAD) == 0) {
                return process_ecc_download(&h, argv[3], argv[4]);
            }
        } else if(strcmp(argv[1], MEM) == 0) {
            if (strcmp(argv[2], MEM_STORE) == 0) {
                return process_mem_store(&h, argv[3], argv[4]);
            } else if (strcmp(argv[2], MEM_READ) == 0) {
                return process_mem_read(&h, argv[3], argv[4]);
            }
        }
    } else if (argc == 6) {
        if(strcmp(argv[1], ECC) == 0) {
            if (strcmp(argv[2], ECC_SIGN) == 0) {
                return process_ecc_sign(&h, argv[3], argv[4], argv[5]);
            }
        }
    }

    LT_LOG_ERROR("ERROR wrong parameters entered\r\n");
    return 1;
}
#endif
