#ifndef MACANDD_H
#define MACANDD_H

#include "libtropic.h"

#ifndef MACANDD_ROUNDS
#define MACANDD_ROUNDS 12
#endif

#if (MACANDD_ROUNDS > 12)
#error \
    "MACANDD_ROUNDS must be less than 12 here, or generally than MACANDD_ROUNDS_MAX. Read explanation at the beginning of this file"
#endif

/** @brief Minimal size of MAC-and-Destroy additional data */
#define MAC_AND_DESTROY_ADD_SIZE_MIN 0
/** @brief Maximal size of MAC-and-Destroy additional data */
#define MAC_AND_DESTROY_ADD_SIZE_MAX 128u
/** @brief Minimal size of MAC-and-Destroy PIN input */
#define MAC_AND_DESTROY_PIN_SIZE_MIN 4u
/** @brief Maximal size of MAC-and-Destroy PIN input */
#define MAC_AND_DESTROY_PIN_SIZE_MAX 8u

/**
 * @brief This structure holds data used by host during MAC and Destroy sequence
 * Content of this struct must be stored in non-volatile memory, because it is used
 * between power cycles
 */
struct lt_macandd_nvm_t {
    uint8_t i;
    uint8_t ci[MACANDD_ROUNDS * 32];
    uint8_t t[32];
} __attribute__((__packed__));

lt_ret_t lt_PIN_set(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                           const uint8_t add_size, uint8_t *secret);


lt_ret_t lt_PIN_check(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                             const uint8_t add_size, uint8_t *secret);


#endif