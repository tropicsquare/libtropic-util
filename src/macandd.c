#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

// Needed to access to lt_port_random_bytes()
#include "libtropic_port.h"
// Needed to access HMAC_SHA256
#include "lt_hmac_sha256.h"
#include "macandd.h"

/** @brief Last slot in User memory used for storing of M&D related data (only in this example). */
#define R_MEM_DATA_SLOT_MACANDD (511)

/**
 * @brief Example function how to set PIN with Mac And Destroy
 *
 * @details There are more ways how to implement Mac And Destroy 'PIN set' functionality, differences could be in way of
 * handling nvm data, number of tries, algorithm used for encryption, etc. This function is just one of the possible
 * implementations of "PIN set", therefore we do not expose this through official libtropic API.
 *
 *          Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and
 * MAC_AND_DESTROY_ADD_SIZE_MAX)
 * @param add_size    Length of additional data
 * @param secret      Buffer into which secret will be placed when all went successfully
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
// Might help to stabilize the sequence, until we improve USB communication
#define MACANDD_DELAY 50
lt_ret_t lt_PIN_set(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                           const uint8_t add_size, uint8_t *secret)
{
    if (!h || !PIN || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN) || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX) || !add
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX) || !secret) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Clear variable for released secret so there is known data (zeroes) in case this function ended sooner then secret
    // was prepared
    memset(secret, 0, 32);

    // Variable used during a process of getting a encryption key k_i
    uint8_t v[32] = {0};
    // Variable used during a process of getting a encryption key k_i
    uint8_t w[32] = {0};
    // Encryption key
    uint8_t k_i[32] = {0};
    // Random secret
    uint8_t s[32] = {0};
    // Variable used to initialize slot(s)
    uint8_t u[32] = {0};

    // This organizes data which will be stored into nvm
    struct lt_macandd_nvm_t nvm = {0};

    // User is expected to pass not only PIN, but might also pass another data(e.g. HW ID, ...)
    // Both arrays are concatenated and used together as an input for KDF
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX + MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    memcpy(kdf_input_buff + PIN_size, add, add_size);

    lt_ret_t ret = lt_port_random_bytes(&h->l2, s, 32);
    if (ret != LT_OK) {
        goto exit;
    }

    // Erase a slot in R memory, which will be used as a storage for NVM data
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }

    // Store number of attempts
    nvm.i = MACANDD_ROUNDS;
    // Compute tag t = KDF(s, "0"), save into nvm struct
    // Tag will be later used during lt_PIN_check() to verify validity of secret
    lt_hmac_sha256(s, 32, (uint8_t *)"0", 1, nvm.t);

    // Compute u = KDF(s, "1")
    // This value will be sent through M&D sequence to initialize a slot
    lt_hmac_sha256(s, 32, (uint8_t *)"1", 1, u);

    // Compute v = KDF(0, PIN||A) where 0 is all zeroes key
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size+add_size, v);

    for (int i = 0; i < nvm.i; i++) {
        uint8_t garbage[32] = {0};

        // This call of a M&D sequence results in initialization of one slot
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if (ret != LT_OK) {
            goto exit;
        }
        // This call of a M&D sequence overwrites a previous slot, but key w is returned.
        // This key is later used to derive k_i (used to encrypt precious secret)
        ret = lt_mac_and_destroy(h, i, v, w);
        if (ret != LT_OK) {
            goto exit;
        }
        // Now the slot is initialized again by calling M&S sequence again with 'u'
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if (ret != LT_OK) {
            goto exit;
        }

        // Derive k_i = KDF(w, PIN||A)
        // This key will be used to encrypt secret s
        lt_hmac_sha256(w, 32, kdf_input_buff, PIN_size + add_size, k_i);

        // Encrypt s using k_i as a key
        // TODO implement some better encryption, or discuss if using XOR here is fine
        for (int j = 0; j < 32; j++) {
            *(nvm.ci + (i * 32 + j)) = k_i[j] ^ s[j];
        }
    }
    lt_port_delay(&h->l2, MACANDD_DELAY);
    // Persistently save nvm data into TROPIC01's R memory slot
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }
    lt_port_delay(&h->l2, MACANDD_DELAY);
    // Final secret is released to the caller
    lt_hmac_sha256(s, 32, (uint8_t *)"2", 1, secret);

// Cleanup all sensitive data from memory
exit:

    memset(kdf_input_buff, 0, PIN_size + add_size);
    memset(u, 0, 32);
    memset(v, 0, 32);
    memset(w, 0, 32);
    memset(k_i, 0, 32);

    return ret;
}

/**
 * @brief Check PIN with Mac And Destroy
 *
 * @details There are more ways how to implement Mac And Destroy 'PIN check' functionality, differences could be in way
 * of handling nvm data, number of tries, algorithm used for decryption, etc. This function is just one of the possible
 * implementations of "PIN check", therefore we do not expose this through official libtropic API.
 *
 *          Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and
 * MAC_AND_DESTROY_ADD_SIZE_MAX)
 * @param add_size    Length of additional data
 * @param secret      Buffer ito which secret will be saved
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */

lt_ret_t lt_PIN_check(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                             const uint8_t add_size, uint8_t *secret)
{
    if (!h || !PIN || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN) || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX) || !add
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX) || !secret) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Clear variable for released secret so there is known data (zeroes) in case this function ended sooner then secret
    // was prepared
    memset(secret, 0, 32);

    // Variable used during a process of getting a decryption key k_i
    uint8_t v_[32] = {0};
    // Variable used during a process of getting a decryption key k_i
    uint8_t w_[32] = {0};
    // Decryption key
    uint8_t k_i[32] = {0};
    // Secret
    uint8_t s_[32] = {0};
    // Tag
    uint8_t t_[32] = {0};
    // Value used to initialize Mac And Destroy's slot after a correct PIN try
    uint8_t u[32] = {0};

    // This organizes data which will be read from nvm
    struct lt_macandd_nvm_t nvm = {0};

    // User is expected to pass not only PIN, but might also pass another data(e.g. HW ID, ...)
    // Both arrays are concatenated and used together as an input for KDF
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX + MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    memcpy(kdf_input_buff + PIN_size, add, add_size);

    // Load M&D data from TROPIC01's R memory
    uint16_t res_size;
    lt_ret_t ret = lt_r_mem_data_read(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, &res_size);
    if (ret != LT_OK) {
        goto exit;
    }

    // if i == 0: FAIL (no attempts remaining)
    if (nvm.i == 0) {
        goto exit;
    }

    // Decrement variable which holds number of tries
    // Let i = i - 1
    nvm.i--;

    // and store M&D data back to TROPIC01's R memory
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }
    lt_port_delay(&h->l2, MACANDD_DELAY);
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }
    lt_port_delay(&h->l2, MACANDD_DELAY);
    // Compute v’ = KDF(0, PIN’||A).
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size + add_size, v_);

    // Execute w’ = MACANDD(i, v’)
    ret = lt_mac_and_destroy(h, nvm.i, v_, w_);
    if (ret != LT_OK) {
        goto exit;
    }

    // Compute k’_i = KDF(w’, PIN’||A)
    lt_hmac_sha256(w_, 32, kdf_input_buff, PIN_size + add_size, k_i);

    // Read the ciphertext c_i and tag t from NVM, decrypt c_i with k’_i as the key and obtain s_
    // TODO figure out if XOR can be used here?
    for (int j = 0; j < 32; j++) {
        s_[j] = *(nvm.ci + (nvm.i * 32 + j)) ^ k_i[j];
    }

    // Compute tag t = KDF(s, "0x00")
    lt_hmac_sha256(s_, 32, (uint8_t *)"0", 1, t_);

    // If t’ != t: FAIL
    if (memcmp(nvm.t, t_, 32) != 0) {
        ret = LT_FAIL;
        goto exit;
    }

    // Pin is correct, now initialize macandd slots again:
    // Compute u = KDF(s’, "0x01")
    lt_hmac_sha256(s_, 32, (uint8_t *)"1", 1, u);

    for (int x = nvm.i; x < MACANDD_ROUNDS - 1; x++) {
        uint8_t garbage[32] = {0};

        ret = lt_mac_and_destroy(h, x, u, garbage);
        if (ret != LT_OK) {
            goto exit;
        }
    }

    // Set variable which holds number of tries back to initial state MACANDD_ROUNDS
    nvm.i = MACANDD_ROUNDS;

    // Store NVM data for future use
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }
    lt_port_delay(&h->l2, MACANDD_DELAY);
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }
    lt_port_delay(&h->l2, MACANDD_DELAY);
    // Calculate secret and store it into passed array
    lt_hmac_sha256(s_, 32, (uint8_t *)"2", 1, secret);

// Cleanup all sensitive data from memory
exit:

    memset(kdf_input_buff, 0, PIN_size + add_size);
    memset(w_, 0, 32);
    memset(k_i, 0, 32);
    memset(v_, 0, 32);

    return ret;
}
