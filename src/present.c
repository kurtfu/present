/**
 * @file present.c
 * @brief Source file of the PRESENT crypt module.
 *
 * The file is the C implementation of the PRESENT crypt algorithm. The file
 * contains global and static function definitions, data structures, type
 * definitions, etc, of the module.
 *
 * All functions, variables, algorithms, etc., that used in the module are
 * designed with respect to the specifications of the article "PRESENT: An
 * Ultra-Lightweight Block Cipher". For further information, see the article.
 *
 * All functions, type definitions, data structures, etc., that used in
 * header or source files were documented in the file which it is declared.
 * For further information, see its detailed documentation.
 *
 * @see <a href=
 *      "https://link.springer.com/chapter/10.1007%2F978-3-540-74735-2_31">
 *      PRESENT: An Ultra-Lightweight Block Cipher</a>
 *
 * @author Furkan Kurt - kurtfu[at]yahoo.com
 * @date 2018-11-12
 *
 * @copyright Copyright (c) 2018 Furkan Kurt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 */
#include <present.h>

/**
 * ID number of the module.
 */
#define ID__ (FILE_ID_PRESENT)

/*****************************************************************************/
/* STANDART C LIBRARIES                                                      */
/*****************************************************************************/

#include <string.h>

/*****************************************************************************/
/* PROJECT LIBRARIES                                                         */
/*****************************************************************************/

#include <assert.h>

/*****************************************************************************/
/* STATIC SYMBOL DEFINITIONS                                                 */
/*****************************************************************************/

/*
 * Start point offset of the key part that used in the crypt process.
 */
#define PRESENT_KEY_OFFSET (PRESENT_KEY_SIZE - PRESENT_CRYPT_SIZE)

/*
 * Shift count for each side in bits.
 */
#define PRESENT_SHIFT_COUNT (61u)

/*
 * Key size in 16-bit blocks.
 */
#define PRESENT_KEY_BLOCK_SIZE (PRESENT_KEY_BIT_SIZE / 16u)

/*
 * Buffer size that holds the new values during the permutation stage.
 */
#define PRESENT_PERMUTATION_BUFF_SIZE (PRESENT_CRYPT_BIT_SIZE / 16u)

/*
 * Buffer size that holds the rotated blocks during left shift.
 */
#if PRESENT_USE_KEY80
#   define PRESENT_ROTATE_BUFF_SIZE_LEFT (2u)
#elif PRESENT_USE_KEY128
#   define PRESENT_ROTATE_BUFF_SIZE_LEFT (5u)
#endif  /* PRESENT_USE_KEY128 */

/*
 * Buffer size that holds the rotated blocks during right shift.
 */
#define PRESENT_ROTATE_BUFF_SIZE_RIGHT (4u)

/*
 * The point where LSB and MSB came side to side after rotation to left.
 */
#define PRESENT_ROTATION_POINT_LEFT (3u)

/*
 * The point where LSB and MSB came side to side after rotation to right.
 */
#if PRESENT_USE_KEY80
#   define PRESENT_ROTATION_POINT_RIGHT (1u)
#elif PRESENT_USE_KEY128
#   define PRESENT_ROTATION_POINT_RIGHT (4u)
#endif  /* PRESENT_USE_KEY128 */

/*
 * Block count to be shifted after the rotation point during left shift.
 */
#if PRESENT_USE_KEY80
#   define PRESENT_UNROTATED_BLOCK_COUNT_LEFT (1u)
#elif PRESENT_USE_KEY128
#   define PRESENT_UNROTATED_BLOCK_COUNT_LEFT (4u)
#endif  /* PRESENT_USE_KEY128 */

/*
 * Block count to be shifted after the rotation point during right shift.
 */
#define PRESENT_UNROTATED_BLOCK_COUNT_RIGHT (3u)

/*
 * Offset value of the LSB bits source block during left shift.
 */
#if PRESENT_USE_KEY80
#   define PRESENT_ROTATION_LSB_OFFSET (2u)
#elif PRESENT_USE_KEY128
#   define PRESENT_ROTATION_LSB_OFFSET (5u)
#endif  /* PRESENT_USE_KEY128 */

/*
 * Offset value of the MSB bits source block during left shift.
 */
#if PRESENT_USE_KEY80
#   define PRESENT_ROTATION_MSB_OFFSET (1u)
#elif PRESENT_USE_KEY128
#   define PRESENT_ROTATION_MSB_OFFSET (4u)
#endif  /* PRESENT_USE_KEY128 */

/*****************************************************************************/
/* COMPILE-TIME ERROR CHECKS                                                 */
/*****************************************************************************/

#if !CONF_PRESENT
#   error "PRESENT must be configured!"
#endif

#if (!PRESENT_USE_KEY80 && !PRESENT_USE_KEY128)
#   error "Key size must be configured!"
#endif

#if (PRESENT_USE_KEY80 && PRESENT_USE_KEY128)
#   error "Only one key size can be chosen!"
#endif

#if (PRESENT_ROUND_COUNT < PRESENT_ROUND_COUNT_MIN)
#   errror "Round count must be greater!"
#endif

#if (PRESENT_ROUND_COUNT > PRESENT_ROUND_COUNT_MAX)
#   errror "Round count must be fewer!"
#endif

/*****************************************************************************/
/* DATA TYPE DEFINITIONS                                                     */
/*****************************************************************************/

/**
 * This type is used in functions @ref present_substitution and
 * @ref present_update_key to describe which process is runnig.
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

/**
 * Lookup table for PRESENT substitution process.
 */
static uint8_t const g_sbox[] = {0x0Cu, 0x05u, 0x06u, 0x0Bu, \
                                 0x09u, 0x00u, 0x0Au, 0x0Du, \
                                 0x03u, 0x0Eu, 0x0Fu, 0x08u, \
                                 0x04u, 0x07u, 0x01u, 0x02u};

/**
 * Lookup table for PRESENT inverse substitution process.
 */
static uint8_t const g_sbox_inv[] = {0x05u, 0x0Eu, 0x0Fu, 0x08u, \
                                     0x0Cu, 0x01u, 0x02u, 0x0Du, \
                                     0x0Bu, 0x04u, 0x06u, 0x03u, \
                                     0x00u, 0x07u, 0x09u, 0x0Au};

/*****************************************************************************/
/* STATIC FUNCTION PROTOTYPES                                                */
/*****************************************************************************/

/**
 * @brief Add key layer of the algorithm.
 *
 * The function adjust \p p_key and adds it to \p p_text. Add key operation
 * is simply an XOR operation of \p p_text and \p p_key. For further
 * information about the PRESENT add key layer, see article's section 3.
 *
 * @warning The function assumes parameter \a p_text points a memory block
 *          with length of @ref PRESENT_CRYPT_SIZE and parameter \a p_key
 *          points a memory block with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_text Pointer of the text block.
 * @param[in] p_key  Pointer of the crypt key.
 *
 * @return None.
 */
static void
present_add_key(uint8_t * p_text, uint8_t const * p_key);

/**
 * @brief Substitution layer of the algorithm.
 *
 * The function replaces all the bytes of \p p_text with values either from
 * S-box or inverse S-box. Lookup table is chosen with respect to \p op. For
 * further information about the PRESENT substitution layer, see article's
 * section 3.
 *
 * @warning The function assumes parameter \a p_text points a memory block
 *          with length of @ref PRESENT_CRYPT_SIZE.
 *
 * @param[in] p_text Pointer of the text block.
 * @param[in] op     The operation type.
 *
 *  @return None.
 */
static void
present_substitution(uint8_t * p_text, present_op_t op);

/**
 * @brief Permutation layer of the algorithm.
 *
 * The function decides the operation order of the permutation process.
 * Operation oder is chosen with respect to \p op. For further information
 * about the permutation layer, see article's section 3.
 *
 * @warning The function assumes parameter \a p_text points a memory block
 *          with length of \ref PRESENT_CRYPT_SIZE.
 *
 * @param[in] p_text Pointer of the text block.
 * @param[in] op     The operation type.
 *
 * @return None.
 */
static void
present_permutation(uint8_t * p_text, present_op_t op);

/**
 * @brief Permutation step of the encryption process.
 *
 * The function moves all the bits of \p p_text to their new positions. Bit
 * positioning is described in the article. For further information about
 * the permutation layer, see article's section 3.
 *
 * @warning The function assumes parameter \a p_text points a memory block
 *          with length of \ref PRESENT_CRYPT_SIZE.
 *
 * @param[in] p_text Pointer of the text block.
 *
 * @return None.
 */
static void
present_encrypt_permutation(uint8_t * p_text);

/**
 * @brief Permutation step of the decryption process.
 *
 * The function moves all the bits of \p p_text to their new positions. Bit
 * positioning is described in the article. For further information about
 * the permutation layer, see article's section 3.
 *
 * @warning The function assumes parameter \a p_text points a memory block
 *          with length of \ref PRESENT_CRYPT_SIZE.
 *
 * @param[in] p_text Pointer of the text block.
 *
 * @return None.
 */
static void
present_decrypt_permutation(uint8_t * p_text);

/**
 * @brief Update key layer of the algorithm.
 *
 * The function decides the operation order of the update key process.
 * Operation oder is chosen with respect to \p op. For further information
 * about the update key layer, see article's section 3.
 *
 * @warning The function assumes parameter \a p_key points a memory block
 *          with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_key         Pointer of the crypt key.
 * @param[in] round_counter Index of the algorithm loop.
 * @param[in] op            The operation type.
 *
 * @return None.
 */
static void
present_update_key(uint8_t * p_key, uint8_t round_counter, present_op_t op);

/**
 * @brief Update key step of the encryption operation.
 *
 * The function rotates \p p_key to the left 61-bit, substitutes the MSB
 * nibbles of the new value and XOR's related bits with \p round_counter.
 * For further information about the update key layer, see article's
 * section 3.
 *
 * @warning The function assumes parameter \a p_key points a memory block
 *          with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_key         Pointer of the crypt key.
 * @param[in] round_counter Index of the algorithm loop.
 *
 * @return None.
 */
static void
present_update_encrypt_key(uint8_t * p_key, uint8_t round_counter);

/**
 * @brief Update key step of the decryption operation.
 *
 * The function XOR's related bits of \p p_key with \p round_counter,
 * substitutes the MSB nibbles of the new value and rotates to the right
 * 61-bit. For further information about the update key layer, see article's
 * section 3.
 *
 * @warning The function assumes parameter \a p_key points a memory block
 *          with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_key         Pointer of the crypt key.
 * @param[in] round_counter Index of the algorithm loop.
 *
 * @return None.
 */
static void
present_update_decrypt_key(uint8_t * p_key, uint8_t round_counter);

/**
 * @brief Generates the decryption key.
 *
 * The function generates the decryption key from the encryption key.
 * The decryption key is the value of the encryption key at the last round.
 *
 * @warning The function assumes parameter \a p_key points a memory block
 *          with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_key Pointer of the crypt key.
 *
 * @return None.
 */
static void
present_generate_decrypt_key(uint8_t * p_key);

/**
 * @brief Rotates the key to the left 61-bit.
 *
 * The function rotates the \p p_key to the left 61-bit. The function is
 * optimized for the PRESENT algorithm.
 *
 * @warning The function assumes the parameter \a p_key points a memory
 *          block with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_key Pointer of the crypt key.
 *
 * @return None.
 */
static void
present_rotate_key_left(uint8_t * p_key);

/**
 * @brief Rotates the key to the right 61-bit.
 *
 * The function rotates the \p p_key to the right 61-bit. The function is
 * optimized for the PRESENT algorithm.
 *
 * @warning The function assumes parameter \a p_key points a memory block
 *          with length of @ref PRESENT_KEY_SIZE.
 *
 * @param[in] p_key Pointer of the crypt key.
 *
 * @return None.
 */
static void
present_rotate_key_right(uint8_t * p_key);

/*****************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

void
present_encrypt (uint8_t * p_text, uint8_t const * p_key)
{
    uint8_t subkey[PRESENT_KEY_SIZE];
    uint8_t round = 1u;

    ASSERT(NULL != p_text);
    ASSERT(NULL != p_key);

    /* 
     * Copy the key into a buffer to keep original value unchanged during
     * the encryption process.
     */
    memcpy(subkey, p_key, PRESENT_KEY_SIZE);

    /*
     * Main loop of the PRESENT encryption algorithm.
     */
    while (round <= PRESENT_ROUND_COUNT)
    {
        present_add_key(p_text, subkey);
        present_substitution(p_text, PRESENT_OP_ENCRYPT);
        present_permutation(p_text, PRESENT_OP_ENCRYPT);

        present_update_key(subkey, round, PRESENT_OP_ENCRYPT);

        round++;
    };

    /*
     * Add the last subkey to finish the process.
     */
    present_add_key(p_text, subkey);
}  /* present_encrypt() */

void
present_decrypt (uint8_t * p_text, uint8_t const * p_key)
{
    uint8_t subkey[PRESENT_KEY_SIZE];
    uint8_t round = PRESENT_ROUND_COUNT;

    ASSERT(NULL != p_text);
    ASSERT(NULL != p_key);

    /* 
     * Copy the key into a buffer to keep original value unchanged during
     * the decryption process.
     */
    memcpy(subkey, p_key, PRESENT_KEY_SIZE);

    /*
     * Generate decryption key from the encryption key.
     */
    present_generate_decrypt_key(subkey);

    /*
     * Last step of the encryption process is the first step of
     * the decryption. Add generated decryption key first.
     */
    present_add_key(p_text, subkey);

    /*
     * Main loop of the PRESENT decryption algorithm.
     */
    while (round > 0u)
    {
        present_permutation(p_text, PRESENT_OP_DECRYPT);
        present_substitution(p_text, PRESENT_OP_DECRYPT);

        present_update_key(subkey, round, PRESENT_OP_DECRYPT);
        present_add_key(p_text, subkey);

        round--;
    }
}  /* present_decrypt() */

/*****************************************************************************/
/* STATIC FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

static void
present_add_key (uint8_t * p_text, uint8_t const * p_key)
{
    uint8_t byte;

    ASSERT(NULL != p_text);
    ASSERT(NULL != p_key);

    /*
     * Move key pointer to the start byte of the key part that specified in
     * the article. For further information, see article's section 3.
     */
    p_key += PRESENT_KEY_OFFSET;

    /*
     * Adding key is simply logic XOR operation.
     */
    for (byte = 0u; byte < PRESENT_CRYPT_SIZE; byte++)
    {
        p_text[byte] = p_text[byte] ^ p_key[byte];
    }
}  /* present_add_key() */

static void
present_substitution (uint8_t * p_text, present_op_t op)
{
    uint8_t const * p_sbox;
    uint8_t         high_nibble;
    uint8_t         low_nibble;
    uint8_t         byte;

    ASSERT(NULL != p_text);

    /*
     * If the operation is the encryption, use substitution box in further
     * steps. Otherwise, use the inverse substitution box.
     */
    switch (op)
    {
        case PRESENT_OP_ENCRYPT:
            p_sbox = g_sbox;
        break;

        case PRESENT_OP_DECRYPT:
            p_sbox = g_sbox_inv;
        break;

        default:
            /*
             * An undefined operation occurred. Use forced assertion.
             */
            ASSERT(0);
        break;
    }

    /*
     * Replace all the bytes in the text block.
     */
    for (byte = 0u; byte < PRESENT_CRYPT_SIZE; byte++)
    {
        high_nibble = (p_text[byte] & 0xF0u) >> 4;
        high_nibble = p_sbox[high_nibble];

        low_nibble = p_text[byte] & 0x0Fu;
        low_nibble = p_sbox[low_nibble];

        p_text[byte] = (high_nibble << 4) | low_nibble;
    }
}  /* present_substitution() */

static void
present_permutation (uint8_t * p_text, present_op_t op)
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
            /*
             * An undefined operation occurred. Use forced assertion.
             */
            ASSERT(0);
    }
}  /* present_permutation() */

static void
present_encrypt_permutation (uint8_t * p_text)
{
    uint16_t buff[PRESENT_PERMUTATION_BUFF_SIZE] = {0u};
    uint8_t  bit                                 = 0u;
    uint8_t  byte                                = 0u;

    ASSERT(NULL != p_text);

    /*
     * Every new 16-bit block has two bits from every bytes of the old text
     * block. Even bits are from low and odd bits are from high nibbles.
     * In every step of the loop, bit values are picked from related bytes.
     * For detailed explanation of the bit positioning, see the article.
     */
    while (byte < PRESENT_CRYPT_SIZE)
    {
        buff[0] |= BITVAL(p_text[byte], 0u) << (2 * bit);
        buff[0] |= BITVAL(p_text[byte], 4u) << (2 * bit + 1);

        buff[1] |= BITVAL(p_text[byte], 1u) << (2 * bit);
        buff[1] |= BITVAL(p_text[byte], 5u) << (2 * bit + 1);

        buff[2] |= BITVAL(p_text[byte], 2u) << (2 * bit);
        buff[2] |= BITVAL(p_text[byte], 6u) << (2 * bit + 1);

        buff[3] |= BITVAL(p_text[byte], 3u) << (2 * bit);
        buff[3] |= BITVAL(p_text[byte], 7u) << (2 * bit + 1);

        bit++;
        byte++;
    }

    /*
     * Copy the new value to the cipher block.
     */
    memcpy(p_text, buff, PRESENT_CRYPT_SIZE);
}  /* present_encrypt_permutation() */

static void
present_decrypt_permutation (uint8_t * p_text)
{
    uint8_t   buff[PRESENT_CRYPT_SIZE] = {0u};
    uint16_t *p_block                  = (uint16_t *)p_text;
    uint8_t   bit                      = 0u;
    uint8_t   byte                     = 0u;

    ASSERT(NULL != p_text);

    /*
     * Every new byte has two bits from every 16-bit blocks of the old
     * permutated text. In every step of the loop, bit values are picked
     * from related bytes. For detailed explanation of the bit positioning,
     * see the article.
     */
    while (byte < PRESENT_CRYPT_SIZE)
    {
        buff[byte] |= BITVAL(p_block[0], (2 * bit))     << 0u;
        buff[byte] |= BITVAL(p_block[0], (2 * bit) + 1) << 4u;

        buff[byte] |= BITVAL(p_block[1], (2 * bit))     << 1u;
        buff[byte] |= BITVAL(p_block[1], (2 * bit) + 1) << 5u;

        buff[byte] |= BITVAL(p_block[2], (2 * bit))     << 2u;
        buff[byte] |= BITVAL(p_block[2], (2 * bit) + 1) << 6u;

        buff[byte] |= BITVAL(p_block[3], (2 * bit))     << 3u;
        buff[byte] |= BITVAL(p_block[3], (2 * bit) + 1) << 7u;

        bit++;
        byte++;
    }

    /*
     * Copy the new value to the decipher block.
     */
    memcpy(p_text, buff, PRESENT_CRYPT_SIZE);
}  /* present_decrypt_permutation() */

static void
present_update_key (uint8_t * p_key, uint8_t round_counter, present_op_t op)
{
    ASSERT(NULL != p_key);
    ASSERT(round_counter >= PRESENT_ROUND_COUNT_MIN);
    ASSERT(round_counter <= PRESENT_ROUND_COUNT_MAX);

    switch (op)
    {
        case PRESENT_OP_ENCRYPT:
            present_update_encrypt_key(p_key, round_counter);
        break;

        case PRESENT_OP_DECRYPT:
            present_update_decrypt_key(p_key, round_counter);
        break;

        default:
            /*
             * An undefined operation occurred. Use forced assertion.
             */
            ASSERT(0);
        break;
    }
}  /* present_update_key() */

static void
present_update_encrypt_key (uint8_t * p_key, uint8_t round_counter)
{
    uint8_t high_nibble;
    uint8_t low_nibble;

    ASSERT(NULL != p_key);
    ASSERT(round_counter >= PRESENT_ROUND_COUNT_MIN);
    ASSERT(round_counter <= PRESENT_ROUND_COUNT_MAX);

    /*
     * Rotate the key to the left as first step of the key scheduling.
     */
    present_rotate_key_left(p_key);

    /*
     * Substitute the MSB high nibble of the key.
     */
    high_nibble = (p_key[PRESENT_KEY_SIZE - 1] & 0xF0u) >> 4;
    high_nibble = g_sbox[high_nibble];

    low_nibble = p_key[PRESENT_KEY_SIZE - 1] & 0x0Fu;

#if PRESENT_USE_KEY128
    /*
     * Substitute the MSB low nibble if 128-bit key is used.
     */
    low_nibble = g_sbox[low_nibble];
#endif  /* PRESENT_USE_KEY128 */

    p_key[PRESENT_KEY_SIZE - 1] = (high_nibble << 4) | low_nibble;

#if PRESENT_USE_KEY80
    /*
     * XOR the from 15th to 19th bits with the round counter.
     */
    p_key[2] ^= round_counter >> 1;
    p_key[1] ^= round_counter << 7;
#else  /* PRESENT_USE_KEY128 */
    /*
     * XOR the from 62th to 66th bits with the round counter.
     */
    p_key[8] ^= round_counter >> 2;
    p_key[7] ^= round_counter << 6;
#endif  /* PRESENT_USE_KEY128 */
}  /* present_update_encrypt_key() */

static void
present_update_decrypt_key (uint8_t * p_key, uint8_t round_counter)
{
    uint8_t high_nibble;
    uint8_t low_nibble;

    ASSERT(NULL != p_key);
    ASSERT(round_counter >= PRESENT_ROUND_COUNT_MIN);
    ASSERT(round_counter <= PRESENT_ROUND_COUNT_MAX);

#if PRESENT_USE_KEY80
    /*
     * XOR the from 15th to 19th bits with the round counter.
     */
    p_key[2] ^= round_counter >> 1;
    p_key[1] ^= round_counter << 7;
#else  /* PRESENT_USE_KEY128 */
    /*
     * XOR the from 62th to 66th bits with the round counter.
     */
    p_key[8] ^= round_counter >> 2;
    p_key[7] ^= round_counter << 6;
#endif  /* PRESENT_USE_KEY128 */

    /*
     * Substitute the MSB high nibble of the key.
     */
    high_nibble = (p_key[PRESENT_KEY_SIZE - 1] & 0xF0u) >> 4;
    high_nibble = g_sbox_inv[high_nibble];

    low_nibble = p_key[PRESENT_KEY_SIZE - 1] & 0x0Fu;

#if PRESENT_USE_KEY128
    /*
     * Substitute the MSB low nibble if 128-bit key is used.
     */
    low_nibble = g_sbox_inv[low_nibble];
#endif  /* PRESENT_USE_KEY128 */

    p_key[PRESENT_KEY_SIZE - 1] = (high_nibble << 4) | low_nibble;

    /*
     * Rotate the key to the right to end the reverse scheduling.
     */
    present_rotate_key_right(p_key);
}  /* present_update_decrypt_key() */

static void
present_generate_decrypt_key (uint8_t * p_key)
{
    uint8_t round;

    ASSERT(NULL != p_key);

    /*
     * Start the loop from the first round.
     */
    round = 1u;

    /*
     * Update the key until the last round.
     */
    while (round <= PRESENT_ROUND_COUNT)
    {
        present_update_key(p_key, round, PRESENT_OP_ENCRYPT);
        round++;
    }
}  /* present_generate_decrypt_key() */

static void
present_rotate_key_left (uint8_t * p_key)
{
    uint16_t   buff[PRESENT_ROTATE_BUFF_SIZE_LEFT];
    uint16_t * p_block;
    uint8_t    block;

    uint8_t const rotation_point   = PRESENT_ROTATION_POINT_LEFT;
    uint8_t const unrotated_blocks = PRESENT_UNROTATED_BLOCK_COUNT_LEFT;
    uint8_t const lsb_offset       = PRESENT_ROTATION_LSB_OFFSET;
    uint8_t const msb_offset       = PRESENT_ROTATION_MSB_OFFSET;

    ASSERT(NULL != p_key);

    p_block = (uint16_t *)p_key;

    /*
     * Fill the buffer with values that changes during the first loop.
     */
    for (block = 0u; block < PRESENT_ROTATE_BUFF_SIZE_LEFT; block++)
    {
        buff[block] = p_block[block];
    }

    /*
     * Place the LSB 3-bit and the MSB 13-bit of the related blocks to the
     * new place index until the rotation point.
     */
    for (block = 0u; block < rotation_point; block++)
    {
        p_block[block] = (p_block[block + lsb_offset] << 13) \
                         | (p_block[block + msb_offset] >> 3);
    }

    /*
     * Place the rotation point value by hand. Since the first block of the
     * key has changed during the first loop, use the buffer value.
     */
    p_block[rotation_point] = (buff[0] << 13) \
                              | (p_block[PRESENT_KEY_BLOCK_SIZE - 1] >> 3);

    /*
     * Fill the remain blocks with buffer values.
     */
    for (block = 0u; block < unrotated_blocks; block++)
    {
        p_block[block + 4] = (buff[block + 1] << 13) | (buff[block] >> 3);
    }
}  /* present_rotate_key_left() */

static void
present_rotate_key_right (uint8_t * p_key)
{
    uint16_t   buff[PRESENT_ROTATE_BUFF_SIZE_RIGHT];
    uint16_t * p_block;
    uint8_t    block;

    uint8_t const rotation_point   = PRESENT_ROTATION_POINT_RIGHT;
    uint8_t const unrotated_blocks = PRESENT_UNROTATED_BLOCK_COUNT_RIGHT;
    uint8_t const place_offset     = PRESENT_ROTATION_POINT_RIGHT + 1u;

    ASSERT(NULL != p_key);

    p_block = (uint16_t *)p_key;

    /*
     * Fill the buffer with values that changes during the first loop.
     */
    for (block = 0u; block < PRESENT_ROTATE_BUFF_SIZE_RIGHT; block++)
    {
        buff[block] = p_block[block];
    }

    /*
     * Place the LSB 13-bit and the MSB 3-bit of the related blocks to the
     * new place index until the rotation point.
     */
    for (block = 0u; block < rotation_point; block++)
    {
        p_block[block] = (p_block[block + 4] << 3) | (p_block[block + 3] >> 13);
    }

    /*
     * Place the rotation point value by hand. Since the first block of key
     * has changed during the first loop, use the buffer value.
     */
    p_block[rotation_point] = (buff[0] << 3) \
                              | (p_block[PRESENT_KEY_BLOCK_SIZE - 1] >> 13);

    /*
     * Fill the remain blocks with buffer values.
     */
    for (block = 0u; block < unrotated_blocks; block++)
    {
        p_block[block + place_offset] = (buff[block + 1] << 3) \
                                        | (buff[block] >> 13);
    }
}  /* present_rotate_key_right() */

/*** END OF FILE ***/
