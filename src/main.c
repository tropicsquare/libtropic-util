/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libtropic.h"


#define HEX_DATA_SIZE 32

/**
 * @brief This is tool for raspberry pi. With TROPIC01 connected over SPI, this tool provides interfaces to TROPIC01 functionalities.
 *
 */
// Default factory pairing keys
int8_t pkey_index_0 =  PAIRING_KEY_SLOT_INDEX_0;
// Engineering samples 02 keys
//uint8_t sh0priv[] = {0x28,0x3F,0x5A,0x0F,0xFC,0x41,0xCF,0x50,0x98,0xA8,0xE1,0x7D,0xB6,0x37,0x2C,0x3C,0xAA,0xD1,0xEE,0xEE,0xDF,0x0F,0x75,0xBC,0x3F,0xBF,0xCD,0x9C,0xAB,0x3D,0xE9,0x72};
//uint8_t sh0pub[]  = {0xF9,0x75,0xEB,0x3C,0x2F,0xD7,0x90,0xC9,0x6F,0x29,0x4F,0x15,0x57,0xA5,0x03,0x17,0x80,0xC9,0xAA,0xFA,0x14,0x0D,0xA2,0x8F,0x55,0xE7,0x51,0x57,0x37,0xB2,0x50,0x2C};
// Engineering samples 01 keys:
uint8_t sh0priv[] = {0xd0,0x99,0x92,0xb1,0xf1,0x7a,0xbc,0x4d,0xb9,0x37,0x17,0x68,0xa2,0x7d,0xa0,0x5b,0x18,0xfa,0xb8,0x56,0x13,0xa7,0x84,0x2c,0xa6,0x4c,0x79,0x10,0xf2,0x2e,0x71,0x6b};
uint8_t sh0pub[]  = {0xe7,0xf7,0x35,0xba,0x19,0xa3,0x3f,0xd6,0x73,0x23,0xab,0x37,0x26,0x2d,0xe5,0x36,0x08,0xca,0x57,0x85,0x76,0x53,0x43,0x52,0xe1,0x8f,0x64,0xe6,0x13,0xd3,0x8d,0x54};
//Model keys:
//uint8_t sh0priv[] = {0xf0,0xc4,0xaa,0x04,0x8f,0x00,0x13,0xa0,0x96,0x84,0xdf,0x05,0xe8,0xa2,0x2e,0xf7,0x21,0x38,0x98,0x28,0x2b,0xa9,0x43,0x12,0xf3,0x13,0xdf,0x2d,0xce,0x8d,0x41,0x64};
//uint8_t sh0pub[]  = {0x84,0x2f,0xe3,0x21,0xa8,0x24,0x74,0x08,0x37,0x37,0xff,0x2b,0x9b,0x88,0xa2,0xaf,0x42,0x44,0x2d,0xb0,0xd8,0xaa,0xcc,0x6d,0xc6,0x9e,0x99,0x53,0x33,0x44,0xb2,0x46};
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

#ifdef USB_DONGLE
void print_usage(void) {
    printf("\r\nUsage (first parameter is serialport with usb dongle, update it if needed):\r\n\n"
"\t./lt-util /dev/ttyACM0 "RNG"    [count] [file]            # Random  - Get 1-255 random bytes and store them into file\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" "  ECC_INSTALL" [slot]  [file]            # ECC key - Install private key from keypair.bin into a given slot\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_GENERATE" [slot]                    # ECC key - Generate private key in a given slot\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_DOWNLOAD" [slot]  [file]            # ECC key - Download public key from given slot into file\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_CLEAR" [slot]                    # ECC key - Clear given ECC slot\r\n"
"\t./lt-util /dev/ttyACM0 "ECC" " ECC_SIGN" [slot]  [file1] [file2]   # ECC key - Sign content of file1 (max size is 4095B) with key from a given slot and store resulting signature into file2\r\n"
"\t./lt-util /dev/ttyACM0 "MEM" " MEM_STORE" [slot]  [file]            # Memory  - Store content of filename (max size is 444B)  into memory slot\r\n"
"\t./lt-util /dev/ttyACM0 "MEM" " MEM_READ" [slot]  [file]            # Memory  - Read content of memory slot (max size is 444B) into filename\r\n"
"\t./lt-util /dev/ttyACM0 "MEM" " MEM_ERASE" [slot]                    # Memory  - Erase content of memory slot\r\n\n"
"\t All commands return 0 if success, otherwise 1\r\n\n");
}
#endif
#ifdef HW_SPI

void print_usage(void) {
    printf("\r\nUsage:\r\n\n"
"\t./lt-util "RNG"    [count] [file]            # Random  - Get 1-255 random bytes and store them into file\r\n"
"\t./lt-util "ECC" "  ECC_INSTALL" [slot]  [file]            # ECC key - Install private key from keypair.bin into a given slot\r\n"
"\t./lt-util "ECC" " ECC_GENERATE" [slot]                    # ECC key - Generate private key in a given slot\r\n"
"\t./lt-util "ECC" " ECC_DOWNLOAD" [slot]  [file]            # ECC key - Download public key from given slot into file\r\n"
"\t./lt-util "ECC" " ECC_CLEAR" [slot]                    # ECC key - Clear given ECC slot\r\n"
"\t./lt-util "ECC" " ECC_SIGN" [slot]  [file1] [file2]   # ECC key - Sign content of file1 (max size is 4095B) with key from a given slot and store resulting signature into file2\r\n"
"\t./lt-util "MEM" " MEM_STORE" [slot]  [file]            # Memory  - Store content of filename (max size is 444B)  into memory slot\r\n"
"\t./lt-util "MEM" " MEM_READ" [slot]  [file]            # Memory  - Read content of memory slot (max size is 444B) into filename\r\n"
"\t./lt-util "MEM" " MEM_ERASE" [slot]                    # Memory  - Erase content of memory slot\r\n\n"
"\t All commands return 0 if success, otherwise 1\r\n\n");
}
#endif

int process_rng_get(lt_handle_t *h, char *count_in, char *file) {
    if(!count_in || !file) {
        //printf("Error, NULL parameters process_rng_get()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "RNG" %s %s\r\n", count_in, file);
    }

    // Parsing count number
    char *endptr;
    long int count = strtol(count_in, &endptr, 10);
    if((count > RANDOM_VALUE_GET_LEN_MAX) | (count <= 0)) {
        //printf("Invalid length passed, use number between 0-255");
        return 1;
    }
    // TODO check endptr

    // Opening file into which random bytes will be written
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        //perror("Error opening file");
        return 1;
    }

    // Get random bytes from TROPIC01 into bytes[] buffer
    uint8_t bytes[RANDOM_VALUE_GET_LEN_MAX] = {0};
    lt_ret_t ret;
    ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return ret;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    }
    ret = lt_random_get(h, bytes, count);
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    }

    // Store content of bytes[] buffer into file
    size_t written = fwrite(bytes, sizeof(uint8_t), count, fp);
    if(written !=count) {
        //printf("Error writing into file, %zu written\n", written);
        fclose(fp);
        lt_deinit(h);
        return 1;
    }

    fclose(fp);
    lt_deinit(h);

    return 0;
}

int process_ecc_install(lt_handle_t *h, char *slot_in, char *file) {
    if(!slot_in || !file) {
        //printf("Error, NULL parameters process_rng_get()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "ECC" "ECC_INSTALL" %s %s\r\n", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Opening file from which first 32B will be taken as private key
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        //perror("Error process_ecc_install() opening fileE");
        return 1;
    }

    // Read keypair from file into keypair[] buffer
    uint8_t keypair[64] = {0};
    size_t read = fread(keypair, sizeof(uint8_t), 64, fp);
    //printf("Number of elements read: %zu\n", read);
    fclose(fp);

    // Install first 32B from keypair[] into ecc slot priv key
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    ret = lt_ecc_key_store(h, slot, CURVE_ED25519, keypair); // Only first 32B will be taken
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    lt_deinit(h);

    //printf("OK\r\n");
    return 0;
}

int process_ecc_generate(lt_handle_t *h, char *slot_in) {
    if(!slot_in) {
        //printf("Error, NULL parameters process_ecc_clear()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "ECC" "ECC_GENERATE" %s\r\n", slot_in);
    }

     // Parsing slot number
     char *endptr;
     long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Generate EdDSA  private key in a given slot
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return ret;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_ecc_key_generate(h, (uint8_t)slot, CURVE_ED25519);
    if(ret != LT_OK) {
        printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return ret;
    }

    lt_deinit(h);

    return 0;
}

int process_ecc_download(lt_handle_t *h, char *slot_in, char *file) {

    if(!slot_in || !file) {
        //printf("Error, NULL parameters process_rng_get()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "ECC" "ECC_DOWNLOAD" %s %s\r\n", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Opening file
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        //perror("Error process_ecc_download() opening file");
        return 1;
    }

    // Get random bytes from TROPIC01
    uint8_t pubkey[64] = {0};
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_ecc_key_read(h, (uint8_t)slot, pubkey, 64, &curve, &origin);
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    size_t written = fwrite(pubkey, sizeof(uint8_t), 32, fp);
    //printf("Number of elements written: %zu\n", written);

    fclose(fp);

    lt_deinit(h);

    return 0;
}

int process_ecc_clear(lt_handle_t *h, char *slot_in) {
    if(!slot_in) {
        //printf("Error, NULL parameters process_ecc_clear()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "ECC" "ECC_CLEAR" %s\r\n", slot_in);
    }

     // Parsing slot number
     char *endptr;
     long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Clear given slot in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_ecc_key_erase(h, (uint8_t)slot);
    if(ret != LT_OK) {
        printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    lt_deinit(h);

    return 0;
}

int process_ecc_sign(lt_handle_t *h, char *slot_in, char *msg_file_in, char* signature_file_out) {
    if(!slot_in || !msg_file_in || !signature_file_out) {
        //printf("Error, NULL parameters process_rng_get()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "ECC" "ECC_SIGN" %s %s %s\r\n", slot_in, msg_file_in, signature_file_out);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 31)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Opening file from which a hash will be read
    FILE *msg_fp = fopen(msg_file_in, "rb");
    if (msg_fp == NULL) {
        //perror("Error opening file");
        return 1;
    }

    fseek(msg_fp, 0L, SEEK_END);
    long msg_size = ftell(msg_fp);
    rewind(msg_fp);

    // Opening file into which a signature will be written
    FILE *fp_sig = fopen(signature_file_out, "wb");
    if (fp_sig == NULL) {
        //perror("Error opening file");
        return 1;
    }

    // Read hash from file
    uint8_t msg[4095] = {0};
    size_t read = fread(msg, sizeof(uint8_t), msg_size, msg_fp);
    //printf("Number of elements read: %zu\n", read);
    fclose(msg_fp);

    // Sign hash in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    uint8_t signature_rs[64] = {0};
    ret = lt_ecc_eddsa_sign(h, (uint8_t)slot, msg, msg_size, signature_rs, 64);
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    // Write signature into file
    size_t written = fwrite(signature_rs, sizeof(uint8_t), 64, fp_sig);
    if(written != 64) {
        //printf("Error writing into file, written: %zu\n", written);
        fclose(fp_sig);
        lt_deinit(h);
        return 1;
    }

    fclose(fp_sig);

    lt_deinit(h);

    return 0;
}

int process_mem_store(lt_handle_t *h, char *slot_in, char *file) {
    if(!slot_in || !file) {
        //printf("Error, NULL parameters process_mem_store()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "MEM" "MEM_STORE" %s %s\r\n", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 511)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Opening file
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        //perror("Error process_ecc_download() opening file");
        return 1;
    }

    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);

    if((sz < 1) || (sz > 444)) {
        //printf("Error, size of file to store must be between 1 - 444 B\r\n");
        return 1;
    }

    // Read keypair from file into keypair[] buffer
    uint8_t mem_content[444] = {0};
    size_t read = fread(mem_content, sizeof(uint8_t), sz, fp);
    if(read != sz) {
        //printf("Error when reading a file\r\n");
        return 1;
    }
    //printf("Number of elements read: %zu\n", read);
    fclose(fp);

    // Store the content into r memory slot
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    ret = lt_r_mem_data_write(h, (uint16_t)slot, mem_content,(uint16_t)sz);
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    lt_deinit(h);

    return 0;

}

int process_mem_read(lt_handle_t *h, char *slot_in, char *file) {
    if(!slot_in || !file) {
        //printf("Error, NULL parameters process_mem_read()\r\n");
        return 1;
    } else {
        //printf("Processing lt-util "MEM" "MEM_READ" %s %s\r\n", slot_in, file);
    }

    // Parsing slot number
    char *endptr;
    long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 511)) {
        //printf("Error, wrong slot number ");
        return 1;
    }

    // Opening file
    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        //perror("Error process_ecc_download() opening file");
        return 1;
    }

    // Read keypair from file into keypair[] buffer
    uint8_t mem_content[444] = {0};

    // Store the content into r memory slot
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_r_mem_data_read(h, (uint16_t)slot, mem_content, 32);
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }


    // Store content of bytes[] buffer into file
    size_t written = fwrite(mem_content, sizeof(uint8_t), 32, fp);
    if(written != 32) {
        printf("Error writing into file, %zu written\n", written);
        fclose(fp);
        lt_deinit(h);
        return 1;
    }

    fclose(fp);

    lt_deinit(h);

    return 0;

}

int process_mem_erase(lt_handle_t *h, char *slot_in)
{
    if(!slot_in) {
        printf("Error, NULL parameters process_mem_erase()\r\n");
    } else {
        //printf("Processing lt-util "MEM" "MEM_ERASE" %s\r\n", slot_in);
    }

     // Parsing slot number
     char *endptr;
     long int slot = strtol(slot_in, &endptr, 10);
    // TODO check endptr

    if((slot < 0) || (slot > 511)) {
        printf("Error, wrong slot number ");
        return 1;
    }

    // Clear given slot in TROPIC01
    lt_ret_t ret = lt_init(h);
    if(ret != LT_OK) {
        //printf("Error lt_init(): %s", lt_ret_verbose(ret));
        return 1;
    }
    ret = verify_chip_and_start_secure_session(h, sh0priv, sh0pub, pkey_index_0);
    if(ret != LT_OK) {
        //printf("Error sec channel: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }
    lt_ecc_curve_type_t curve = 2;
    ecc_key_origin_t origin = 2;
    ret = lt_r_mem_data_erase(h, (uint16_t)slot);
    if(ret != LT_OK) {
        //printf("Error l3 cmd: %s", lt_ret_verbose(ret));
        lt_deinit(h);
        return 1;
    }

    lt_deinit(h);

    return 0;
}

#ifdef USB_DONGLE
int main(int argc, char *argv[]) {
    //printf ("argc %d   %s  %s  %s  %s \r\n", argc, argv[0], argv[1], argv[2], argv[3]);
    if ((argc == 1)) {
        print_usage();
        return 0;
    }

    lt_handle_t h;
    lt_uart_def_unix_t uart = {0};
    h.l2.device = &uart;
    uart.baud_rate = 115200;
    strncpy(uart.device, argv[1], UART_DEV_MAX_LEN);

    if (argc == 5) {
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
        }
    } else if (argc == 7) {
        if(strcmp(argv[2], ECC) == 0) {
            if (strcmp(argv[3], ECC_SIGN) == 0) {
                return process_ecc_sign(&h, argv[4], argv[5], argv[6]);
            }
        }
    }

    printf("ERROR wrong parameters entered\r\n");
    return 1;
}
#endif
#ifdef HW_SPI
int main(int argc, char *argv[]) {
    //printf ("argc %d   %s  %s  %s  %s \r\n", argc, argv[0], argv[1], argv[2], argv[3]);
    if ((argc == 1)) {
        print_usage();
        return 0;
    }

    lt_handle_t h;
    lt_uart_def_unix_t uart = {0};
    h.l2.device = &uart;
    uart.baud_rate = 115200;
    strncpy(uart.device, argv[1], UART_DEV_MAX_LEN);

    if (argc == 4) {
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
                return process_mem_erase(&h, argv[4]);
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

    printf("ERROR wrong parameters entered\r\n");
    return 1;
}

#endif
