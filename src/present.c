/*! \file present.c
 *  \brief Source file of the PRESENT crypt module.
 *
 *  The file is the C implementation of the PRESENT crypt algorithm. The file
 *  contains global and static function definitions, data structures, type
 *  definitions, etc, of the module.
 *
 *  All functions, variables, algorithms, etc., that used in the module are
 *  designed with respect to the specifications of the article "PRESENT: An
 *  Ultra-Lightweight Block Cipher". For further information, see the article.
 *
 *  All functions, type definitions, data structures, etc., that used in
 *  header or source files were documented in the file which it is declared.
 *  For further information, see its detailed documentation.
 *
 *  \see <a href=
 *       "https://link.springer.com/chapter/10.1007%2F978-3-540-74735-2_31">
 *       PRESENT: An Ultra-Lightweight Block Cipher</a>
 *
 *  \author Furkan Kurt - kurtfu[at]yahoo.com
 *  \date 2018-11-12
 *
 *  \copyright Copyright (c) 2018 Furkan Kurt
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 */
#include <present.h>

/*! ID number of the module. */
#define __ID__ (FILE_ID_PRESENT)

/*****************************************************************************/
/* STANDART C LIBRARIES                                                      */
/*****************************************************************************/

#include <string.h>

/*****************************************************************************/
/* PROJECT LIBRARIES                                                         */
/*****************************************************************************/

#include <assert.h>

/*****************************************************************************/
/* COMPILE TIME ERROR CHECKS                                                 */
/*****************************************************************************/

#if !CONF_PRESENT
    #error "PRESENT must be configured!"
#endif

#if (!PRESENT_USE_KEY80 && !PRESENT_USE_KEY128)
    #error "Key size must be configured!"
#endif

#if (PRESENT_USE_KEY80 && PRESENT_USE_KEY128)
    #error "Only one key size can be chosen!"
#endif

#if (PRESENT_ROUND_COUNT < PRESENT_ROUND_COUNT_MIN)
    #errror "Round count must be greater!"
#endif

#if (PRESENT_ROUND_COUNT > PRESENT_ROUND_COUNT_MAX)
    #errror "Round count must be fewer!"
#endif

/*****************************************************************************/
/* STATIC SYMBOL DEFINITIONS                                                 */
/*****************************************************************************/

/*! Start point offset of the key part that used in the crypt process. */
#define PRESENT_KEY_OFFSET (PRESENT_KEY_SIZE - PRESENT_CRYPT_SIZE)

/*! Shift count for each side in bit. */
#define PRESENT_SHIFT_COUNT (61u)

/*! Key size in 16-bit blocks. */
#define PRESENT_KEY_BLOCK_SIZE (PRESENT_KEY_BIT_SIZE / 16u)

/*! Buffer size that holds the rotated blocks during left shift. */
#if PRESENT_USE_KEY80
    #define PRESENT_ROTATE_BUFF_SIZE_LEFT (2u)
#elif PRESENT_USE_KEY128
    #define PRESENT_ROTATE_BUFF_SIZE_LEFT (5u)
#endif /* PRESENT_USE_KEY128 */

/*! Buffer size that holds the rotated blocks during right shift. */
#define PRESENT_ROTATE_BUFF_SIZE_RIGHT (4u)

/*! The point where LSB and MSB came side to side after rotation to left. */
#define PRESENT_ROTATION_POINT_LEFT (3u)

/*! The point where LSB and MSB came side to side after rotation to right. */
#if PRESENT_USE_KEY80
    #define PRESENT_ROTATION_POINT_RIGHT (1u)
#elif PRESENT_USE_KEY128
    #define PRESENT_ROTATION_POINT_RIGHT (4u)
#endif /* PRESENT_USE_KEY128 */

/*! Block count to be shifted after the rotation point during left shift. */
#if PRESENT_USE_KEY80
    #define PRESENT_UNROTATED_BLOCK_COUNT_LEFT (1u)
#elif PRESENT_USE_KEY128
    #define PRESENT_UNROTATED_BLOCK_COUNT_LEFT (4u)
#endif /* PRESENT_USE_KEY128 */

/*! Block count to be shifted after the rotation point during right shift. */
#define PRESENT_UNROTATED_BLOCK_COUNT_RIGHT (3u)

/*! Offset value of the LSB bits source block during left shift. */
#if PRESENT_USE_KEY80
    #define PRESENT_ROTATION_LSB_OFFSET (2u)
#elif PRESENT_USE_KEY128
    #define PRESENT_ROTATION_LSB_OFFSET (5u)
#endif /* PRESENT_USE_KEY128 */

/*! Offset value of the MSB bits source block during left shift. */
#if PRESENT_USE_KEY80
    #define PRESENT_ROTATION_MSB_OFFSET (1u)
#elif PRESENT_USE_KEY128
    #define PRESENT_ROTATION_MSB_OFFSET (4u)
#endif /* PRESENT_USE_KEY128 */

/*****************************************************************************/
/* SPECIAL TYPE DEFINITIONS                                                  */
/*****************************************************************************/

/*! \brief PRESENT operation type.
 *
 *  This type is used in functions such as, \ref present_substitution and
 *  \ref present_permutation to describe which process it is used for.
 */
typedef enum {
    /*! Specifies the encryption operation. */
    PRESENT_OP_ENCRYPT,
    /*! Specifies the decryption operation. */
    PRESENT_OP_DECRYPT,
} present_op_t;

/*****************************************************************************/
/* GLOBAL VARIABLES                                                          */
/*****************************************************************************/

/*! Lookup table for PRESENT substitution process. Each index of the table
 *  represents current value of the 4-bit nibble and each element represents
 *  the updated value.
 */
static uint8_t const g_sbox[] = {0x0Cu, 0x05u, 0x06u, 0x0Bu, \
                                 0x09u, 0x00u, 0x0Au, 0x0Du, \
                                 0x03u, 0x0Eu, 0x0Fu, 0x08u, \
                                 0x04u, 0x07u, 0x01u, 0x02u};

/*! Lookup table for PRESENT inverse substitution process. Each index of
 *  the table represents current value of the 4-bit nibble and each element
 *  represents the updated value.
 */
static uint8_t const g_sbox_inv[] = {0x05u, 0x0Eu, 0x0Fu, 0x08u, \
                                     0x0Cu, 0x01u, 0x02u, 0x0Du, \
                                     0x0Bu, 0x04u, 0x06u, 0x03u, \
                                     0x00u, 0x07u, 0x09u, 0x0Au};

/*****************************************************************************/
/* STATIC FUNCTION PROTOTYPES                                                */
/*****************************************************************************/

/*! \brief Add key layer of the algorithm.
 *
 *  The function adjust \p p_key and adds it to \p p_text. Add key operation
 *  is simply an XOR operation of \p p_text and \p p_key. For further
 *  information about the PRESENT add key layer, see article's section 3.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE and parameter \a p_key
 *           points a memory block with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *  \param p_key  Pointer of the crypt key.
 *
 *  \return None.
 */
static void
present_add_key(uint8_t *p_text, uint8_t const *p_key);

/*! \brief Substitution layer of the algorithm.
 *
 *  The function replaces all the bytes of \p p_text with values either from
 *  S-box or inverse S-box. Lookup table is chosen with respect to \p op. For
 *  further information about the PRESENT substitution layer, see article's
 *  section 3.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *  \param op     The operation type.
 *
 *  \return None.
 */
static void
present_substitution(uint8_t *p_text, present_op_t op);

/*! \brief Permutation layer of the algorithm.
 *
 *  The function decides the operation order of the permutation process.
 *  Operation oder is chosen with respect to \p op. For further information
 *  about the permutation layer, see article's section 3.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *  \param op     The operation type.
 *
 *  \return None.
 */
static void
present_permutation(uint8_t *p_text, present_op_t op);

/*! \brief Permutation step of the encryption process.
 *
 *  The function moves all the bits of \p p_text to their new positions. Bit
 *  positioning is described in the article. For further information about
 *  the permutation layer, see article's section 3.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *
 *  \return None.
 */
static void
present_encrypt_permutation(uint8_t *p_text);

/*! \brief Permutation step of the decryption process.
 *
 *  The function moves all the bits of \p p_text to their new positions. Bit
 *  positioning is described in the article. For further information about
 *  the permutation layer, see article's section 3.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *
 *  \return None.
 */
static void
present_decrypt_permutation(uint8_t *p_text);

/*! \brief Update key layer of the algorithm.
 *
 *  The function decides the operation order of the update key process.
 *  Operation oder is chosen with respect to \p op. For further information
 *  about the update key layer, see article's section 3.
 *
 *  \warning The function assumes parameter \a p_key points a memory block
 *           with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key         Pointer of the crypt key.
 *  \param round_counter Index of the algorithm loop.
 *  \param op            The operation type.
 *
 *  \return None.
 */
static void
present_update_key(uint8_t *p_key, uint8_t round_counter, present_op_t op);

/*! \brief Update key step of the encryption operation.
 *
 *  The function rotates \p p_key to the left 61-bit, substitutes the MSB
 *  nibbles of the new value and XOR's related bits with \p round_counter.
 *  For further information about the update key layer, see article's
 *  section 3.
 *
 *  \warning The function assumes parameter \a p_key points a memory block
 *           with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key         Pointer of the crypt key.
 *  \param round_counter Index of the algorithm loop.
 *
 *  \return None.
 */
static void
present_update_encrypt_key(uint8_t *p_key, uint8_t round_counter);

/*! \brief Update key step of the decryption operation.
 *
 *  The function XOR's related bits of \p p_key with \p round_counter,
 *  substitutes the MSB nibbles of the new value and rotates to the right
 *  61-bit. For further information about the update key layer, see article's
 *  section 3.
 *
 *  \warning The function assumes parameter \a p_key points a memory block
 *           with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key         Pointer of the crypt key.
 *  \param round_counter Index of the algorithm loop.
 *
 *  \return None.
 */
static void
present_update_decrypt_key(uint8_t *p_key, uint8_t round_counter);

/*! \brief Generates the decryption key.
 *
 *  The function generates the decryption key from the encryption key.
 *  The decryption key is the value of the encryption key at the last round.
 *
 *  \warning The function assumes parameter \a p_key points a memory block
 *           with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key Pointer of the crypt key.
 *
 *  \return None.
 */
static void
present_generate_decrypt_key(uint8_t *p_key);

/*! \brief Rotates the key to the left 61-bit.
 *
 *  The function rotates the \p p_key to the left 61-bit. The function is
 *  optimized for the PRESENT algorithm.
 *
 *  \warning The function assumes the parameter \a p_key points a memory
 *           block with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key Pointer of the crypt key.
 *
 *  \return None.
 */
static void
present_rotate_key_left(uint8_t *p_key);

/*! \brief Rotates the key to the right 61-bit.
 *
 *  The function rotates the \p p_key to the right 61-bit. The function is
 *  optimized for the PRESENT algorithm.
 *
 *  \warning The function assumes parameter \a p_key points a memory block
 *           with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key Pointer of the crypt key.
 *
 *  \return None.
 */
static void
present_rotate_key_right(uint8_t *p_key);

/*****************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

void
present_encrypt(uint8_t *p_text, uint8_t const *p_key)
{
    uint8_t subkey[PRESENT_KEY_SIZE];
    uint8_t round;

    ASSERT(NULL != p_text);
    ASSERT(NULL != p_key);

    /* Copy the key into a buffer to keep original value unchanged during
     * the encryption process.
     */
    memcpy(subkey, p_key, PRESENT_KEY_SIZE);

    /* Start the loop from round one. */
    round = 1u;

    /* Main loop of the PRESENT encryption algorithm. */
    while (round <= PRESENT_ROUND_COUNT) {
        present_add_key(p_text, subkey);
        present_substitution(p_text, PRESENT_OP_ENCRYPT);
        present_permutation(p_text, PRESENT_OP_ENCRYPT);

        present_update_key(subkey, round, PRESENT_OP_ENCRYPT);
        round++;
    };

    /* Add the last subkey to finish the process. */
    present_add_key(p_text, subkey);
}

void
present_decrypt(uint8_t *p_text, uint8_t const *p_key)
{
    uint8_t subkey[PRESENT_KEY_SIZE];
    uint8_t round;

    ASSERT(NULL != p_text);
    ASSERT(NULL != p_key);

    /* Copy the key into a buffer to keep original value unchanged during
     * the decryption process.
     */
    memcpy(subkey, p_key, PRESENT_KEY_SIZE);

    /* Generate decryption key from the encryption key. */
    present_generate_decrypt_key(subkey);

    /* Last step of the encryption process is the first step of
     * the decryption. Add generated decryption key first.
     */
    present_add_key(p_text, subkey);

    /* Start the loop from the last round. */
    round = PRESENT_ROUND_COUNT;

    /* Main loop of the PRESENT decryption algorithm. */
    while (round > 0u) {
        present_permutation(p_text, PRESENT_OP_DECRYPT);
        present_substitution(p_text, PRESENT_OP_DECRYPT);

        present_update_key(subkey, round, PRESENT_OP_DECRYPT);
        present_add_key(p_text, subkey);
        round--;
    }
}

/*****************************************************************************/
/* STATIC FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

static void
present_add_key(uint8_t *p_text, uint8_t const *p_key)
{
    uint8_t byte_idx;

    ASSERT(NULL != p_text);
    ASSERT(NULL != p_key);

    /* Move key pointer to the start byte of the key part that specified in
     * the article. For further information, see article's section 3.
     */
    p_key += PRESENT_KEY_OFFSET;

    /* Adding key is simply logic XOR operation. */
    for (byte_idx = 0u; byte_idx < PRESENT_CRYPT_SIZE; byte_idx++) {
        p_text[byte_idx] = p_text[byte_idx] ^ p_key[byte_idx];
    }
}

static void
present_substitution(uint8_t *p_text, present_op_t op)
{
    uint8_t const *p_sbox;
    uint8_t        high_nibble;
    uint8_t        low_nibble;
    uint8_t        byte_idx;

    ASSERT(NULL != p_text);

    /* If the operation is the encryption, use substitution box in further
     * steps. Otherwise, use the inverse substitution box.
     */
    switch (op) {
    case PRESENT_OP_ENCRYPT:
        p_sbox = g_sbox;
        break;
    case PRESENT_OP_DECRYPT:
        p_sbox = g_sbox_inv;
        break;
    default:
        /* An undefined operation occurred. Use forced assertion. */
        ASSERT(0);
    }

    /* Replace all the bytes in the text block. */
    for (byte_idx = 0u; byte_idx < PRESENT_CRYPT_SIZE; byte_idx++) {
        high_nibble = (p_text[byte_idx] & 0xF0u) >> 4;
        high_nibble = p_sbox[high_nibble];

        low_nibble = p_text[byte_idx] & 0x0Fu;
        low_nibble = p_sbox[low_nibble];

        p_text[byte_idx] = (high_nibble << 4) | low_nibble;
    }
}

static void
present_permutation(uint8_t *p_text, present_op_t op)
{
    ASSERT(NULL != p_text);

    switch (op) {
    case PRESENT_OP_ENCRYPT:
        present_encrypt_permutation(p_text);
        break;
    case PRESENT_OP_DECRYPT:
        present_decrypt_permutation(p_text);
        break;
    default:
        /* An undefined operation occurred. Use forced assertion. */
        ASSERT(0);
    }
}

static void
present_encrypt_permutation(uint8_t *p_text)
{
    uint8_t buff[PRESENT_CRYPT_SIZE];
    uint8_t base_bit;
    uint8_t base_byte;
    uint8_t bit_idx;

    ASSERT(NULL != p_text);

    /* Clear buffer first to do logical OR'ing directly. */
    memset(buff, 0u, sizeof(buff));

    base_bit  = 0u;
    base_byte = 0u;
    bit_idx   = 0u;

    /* Every new byte has two bits from four bytes of the old text block.
     * Even bits are from low and odd bits are from high nibbles. In every
     * step of the loop, bit values are picked from related bytes.
     */
    while (bit_idx < PRESENT_CRYPT_SIZE) {
        buff[0] |= BITVAL(p_text[base_byte + 0], base_bit + 0u) << bit_idx;
        buff[1] |= BITVAL(p_text[base_byte + 4], base_bit + 0u) << bit_idx;

        buff[2] |= BITVAL(p_text[base_byte + 0], base_bit + 1u) << bit_idx;
        buff[3] |= BITVAL(p_text[base_byte + 4], base_bit + 1u) << bit_idx;

        buff[4] |= BITVAL(p_text[base_byte + 0], base_bit + 2u) << bit_idx;
        buff[5] |= BITVAL(p_text[base_byte + 4], base_bit + 2u) << bit_idx;

        buff[6] |= BITVAL(p_text[base_byte + 0], base_bit + 3u) << bit_idx;
        buff[7] |= BITVAL(p_text[base_byte + 4], base_bit + 3u) << bit_idx;

        bit_idx++;

        /* For odd indexes, set base bit to 4 to pick high nibble bit values
         * and for even indexes, set it to 0 to pick low nibble bit values.
         */
        if (0u != (bit_idx % 2u)) {
            base_bit = 4u;
        }
        else {
            base_bit = 0u;
            base_byte++;
        }
    }

    /* Copy the new value to the cipher block. */
    memcpy(p_text, buff, PRESENT_CRYPT_SIZE);
}

static void
present_decrypt_permutation(uint8_t *p_text)
{
    uint8_t buff[PRESENT_CRYPT_SIZE];
    uint8_t base_bit;
    uint8_t base_byte;
    uint8_t bit_idx;

    ASSERT(NULL != p_text);

    /* Clear buffer first to do logical OR'ing directly. */
    memset(buff, 0u, sizeof(buff));

    base_bit  = 0u;
    base_byte = 0u;
    bit_idx   = 0u;

    /* Every new byte has two bits from four bytes of the old text block.
     * Low nibble values from even bits and high nibble values from odd bits.
     * In every step of the loop, bit values are picked from related bytes.
     */
    while (bit_idx < PRESENT_CRYPT_SIZE) {
        buff[0] |= BITVAL(p_text[base_byte + 0], base_bit + 0u) << bit_idx;
        buff[1] |= BITVAL(p_text[base_byte + 0], base_bit + 2u) << bit_idx;

        buff[2] |= BITVAL(p_text[base_byte + 0], base_bit + 4u) << bit_idx;
        buff[3] |= BITVAL(p_text[base_byte + 0], base_bit + 6u) << bit_idx;

        buff[4] |= BITVAL(p_text[base_byte + 1], base_bit + 0u) << bit_idx;
        buff[5] |= BITVAL(p_text[base_byte + 1], base_bit + 2u) << bit_idx;

        buff[6] |= BITVAL(p_text[base_byte + 1], base_bit + 4u) << bit_idx;
        buff[7] |= BITVAL(p_text[base_byte + 1], base_bit + 6u) << bit_idx;

        bit_idx++;

        /* At the first index of the high nibble, reset the base byte and
         * set base bit to 1 to get odd bit values.
         */
        if (0u == (bit_idx % 4u)) {
            base_byte = 0u;
            base_bit  = 1u;
        }
        else {
            base_byte += 2u;
        }
    }

    /* Copy the new value to the decipher block. */
    memcpy(p_text, buff, PRESENT_CRYPT_SIZE);
}

static void
present_update_key(uint8_t *p_key, uint8_t round_counter, present_op_t op)
{
    ASSERT(NULL != p_key);
    ASSERT(round_counter >= PRESENT_ROUND_COUNT_MIN);
    ASSERT(round_counter <= PRESENT_ROUND_COUNT_MAX);

    switch (op) {
    case PRESENT_OP_ENCRYPT:
        present_update_encrypt_key(p_key, round_counter);
        break;
    case PRESENT_OP_DECRYPT:
        present_update_decrypt_key(p_key, round_counter);
        break;
    default:
        /* An undefined operation occurred. Use forced assertion. */
        ASSERT(0);
    }
}

static void
present_update_encrypt_key(uint8_t *p_key, uint8_t round_counter)
{
    uint8_t high_nibble;
    uint8_t low_nibble;

    ASSERT(NULL != p_key);
    ASSERT(round_counter >= PRESENT_ROUND_COUNT_MIN);
    ASSERT(round_counter <= PRESENT_ROUND_COUNT_MAX);

    /* Rotate the key to the left as first step of the key scheduling. */
    present_rotate_key_left(p_key);

    /* Substitute the MSB high nibble of the key. */
    high_nibble = (p_key[PRESENT_KEY_SIZE - 1] & 0xF0u) >> 4;
    high_nibble = g_sbox[high_nibble];

    low_nibble = p_key[PRESENT_KEY_SIZE - 1] & 0x0Fu;

#if PRESENT_USE_KEY128
    /* Substitute the MSB low nibble if 128-bit key is used. */
    low_nibble = g_sbox[low_nibble];
#endif /* PRESENT_USE_KEY128 */

    p_key[PRESENT_KEY_SIZE - 1] = (high_nibble << 4) | low_nibble;

#if PRESENT_USE_KEY80
    /* XOR the from 15th to 19th bits with the round counter. */
    p_key[2] ^= round_counter >> 1;
    p_key[1] ^= round_counter << 7;
#else /* PRESENT_USE_KEY128 */
    /* XOR the from 62th to 66th bits with the round counter. */
    p_key[8] ^= round_counter >> 2;
    p_key[7] ^= round_counter << 6;
#endif /* PRESENT_USE_KEY128 */
}

static void
present_update_decrypt_key(uint8_t *p_key, uint8_t round_counter)
{
    uint8_t high_nibble;
    uint8_t low_nibble;

    ASSERT(NULL != p_key);
    ASSERT(round_counter >= PRESENT_ROUND_COUNT_MIN);
    ASSERT(round_counter <= PRESENT_ROUND_COUNT_MAX);

#if PRESENT_USE_KEY80
    /* XOR the from 15th to 19th bits with the round counter. */
    p_key[2] ^= round_counter >> 1;
    p_key[1] ^= round_counter << 7;
#else /* PRESENT_USE_KEY128 */
    /* XOR the from 62th to 66th bits with the round counter. */
    p_key[8] ^= round_counter >> 2;
    p_key[7] ^= round_counter << 6;
#endif /* PRESENT_USE_KEY128 */

    /* Substitute the MSB high nibble of the key. */
    high_nibble = (p_key[PRESENT_KEY_SIZE - 1] & 0xF0u) >> 4;
    high_nibble = g_sbox_inv[high_nibble];

    low_nibble = p_key[PRESENT_KEY_SIZE - 1] & 0x0Fu;

#if PRESENT_USE_KEY128
    /* Substitute the MSB low nibble if 128-bit key is used. */
    low_nibble = g_sbox_inv[low_nibble];
#endif /* PRESENT_USE_KEY128 */

    p_key[PRESENT_KEY_SIZE - 1] = (high_nibble << 4) | low_nibble;

    /* Rotate the key to the right to end the reverse scheduling. */
    present_rotate_key_right(p_key);
}

static void
present_generate_decrypt_key(uint8_t *p_key)
{
    uint8_t round;

    ASSERT(NULL != p_key);

    /* Start the loop from the first round. */
    round = 1u;

    /* Update the key until the last round. */
    while (round <= PRESENT_ROUND_COUNT) {
        present_update_key(p_key, round, PRESENT_OP_ENCRYPT);
        round++;
    }
}

static void
present_rotate_key_left(uint8_t *p_key)
{
    uint16_t  buff[PRESENT_ROTATE_BUFF_SIZE_LEFT];
    uint16_t *p_block;
    uint8_t   block_idx;

    uint8_t const rotation_point   = PRESENT_ROTATION_POINT_LEFT;
    uint8_t const unrotated_blocks = PRESENT_UNROTATED_BLOCK_COUNT_LEFT;
    uint8_t const lsb_offset       = PRESENT_ROTATION_LSB_OFFSET;
    uint8_t const msb_offset       = PRESENT_ROTATION_MSB_OFFSET;

    ASSERT(NULL != p_key);

    p_block   = (uint16_t *)p_key;
    block_idx = 0u;

    /* Fill the buffer with values that changes during the first loop. */
    for (; block_idx < PRESENT_ROTATE_BUFF_SIZE_LEFT; block_idx++) {
        buff[block_idx] = p_block[block_idx];
    }

    /* Place the LSB 3-bit and the MSB 13-bit of the related blocks to the
     * new place index until the rotation point.
     */
    for (block_idx = 0u; block_idx < rotation_point; block_idx++) {
        p_block[block_idx] = (p_block[block_idx + lsb_offset] << 13) \
                             | (p_block[block_idx + msb_offset] >> 3);
    }

    /* Place the rotation point value by hand. Since the first block of the
     * key has changed during the first loop, use the buffer value.
     */
    p_block[rotation_point] = (buff[0] << 13) \
                              | (p_block[PRESENT_KEY_BLOCK_SIZE - 1] >> 3);

    /* Fill the remain blocks with buffer values. */
    for (block_idx = 0u; block_idx < unrotated_blocks; block_idx++) {
        p_block[block_idx + 4] = (buff[block_idx + 1] << 13) \
                                 | (buff[block_idx] >> 3);
    }
}

static void
present_rotate_key_right(uint8_t *p_key)
{
    uint16_t  buff[PRESENT_ROTATE_BUFF_SIZE_RIGHT];
    uint16_t *p_block;
    uint8_t   block_idx;

    uint8_t const rotation_point   = PRESENT_ROTATION_POINT_RIGHT;
    uint8_t const unrotated_blocks = PRESENT_UNROTATED_BLOCK_COUNT_RIGHT;
    uint8_t const place_offset     = PRESENT_ROTATION_POINT_RIGHT + 1u;

    ASSERT(NULL != p_key);

    p_block   = (uint16_t *)p_key;
    block_idx = 0u;

    /* Fill the buffer with values that changes during the first loop. */
    for (; block_idx < PRESENT_ROTATE_BUFF_SIZE_RIGHT; block_idx++) {
        buff[block_idx] = p_block[block_idx];
    }

    block_idx = 0u;

    /* Place the LSB 13-bit and the MSB 3-bit of the related blocks to the
     * new place index until the rotation point.
     */
    for (; block_idx < rotation_point; block_idx++) {
        p_block[block_idx] = (p_block[block_idx + 4] << 3) \
                             | (p_block[block_idx + 3] >> 13);
    }

    /* Place the rotation point value by hand. Since the first block of key
     * has changed during the first loop, use the buffer value.
     */
    p_block[rotation_point] = (buff[0] << 3) \
                              | (p_block[PRESENT_KEY_BLOCK_SIZE - 1] >> 13);

    /* Fill the remain blocks with buffer values. */
    for (block_idx = 0u; block_idx < unrotated_blocks; block_idx++) {
        p_block[block_idx + place_offset] = (buff[block_idx + 1] << 3) \
                                            | (buff[block_idx] >> 13);
    }
}
