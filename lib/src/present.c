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

/*****************************************************************************/
/* STANDART C LIBRARIES                                                      */
/*****************************************************************************/

#include <string.h>

/*****************************************************************************/
/* STATIC SYMBOL DEFINITIONS                                                 */
/*****************************************************************************/

/*! PRESENT algorithm round count. */
#define PRESENT_ROUND_COUNT (31u)

/*! Start point offset of the key part that used in the crypt process. */
#define PRESENT_KEY_OFFSET (PRESENT_KEY_SIZE - PRESENT_CRYPT_SIZE)

/*! Shift count for each side in bit. */
#define PRESENT_SHIFT_COUNT (61u)

/*! Key size in 16-bit blocks. */
#define PRESENT_KEY_BLOCK_SIZE (PRESENT_KEY_BIT_SIZE / 16u)

/*! Buffer size that holds the rotated blocks during left shift. */
#define PRESENT_ROTATE_BUFF_SIZE_LEFT (2u)

/*! The point where LSB and MSB came side to side after rotation to left. */
#define PRESENT_ROTATION_POINT_LEFT (3u)

/*! Block count to be shifted after the rotation point during left shift. */
#define PRESENT_UNROTATED_BLOCK_COUNT_LEFT (1u)

/*! Offset value of the LSB bits source block during left shift. */
#define PRESENT_ROTATION_LSB_OFFSET (2u)

/*! Offset value of the MSB bits source block during left shift. */
#define PRESENT_ROTATION_MSB_OFFSET (1u)

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

/*****************************************************************************/
/* STATIC FUNCTION PROTOTYPES                                                */
/*****************************************************************************/

/*! \brief Add key layer of the algorithm.
 *
 *  The function adjust the encryption round key and adds it to the text block.
 *  Add key operation is simply an XOR operation of \p p_text and \p p_key.
 *  For further information about the PRESENT add key layer, see the article's
 *  section 3.
 *
 *  \warning The function assumes the parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE and the parameter \a p_key
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
 *  The function replaces all the bytes of the \p p_text with values from
 *  S-box. For further information about the PRESENT substitution layer,
 *  see the article's section 3.
 *
 *  \warning The function assumes the parameter \a p_text points a memory
 *           block with length of \ref PRESENT_CRYPT_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *
 *  \return None.
 */
static void
present_substitution(uint8_t *p_text);

/*! \brief Permutation layer of the encryption algorithm.
 *
 *  The function moves all the bits of the \p p_text to their new positions.
 *  Bit positioning is described in the article. For further information
 *  about the permutation step, see the article's section 3.
 *
 *  \warning The function assumes the parameter \a p_text points a memory
 *           block with length of \ref PRESENT_CRYPT_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *
 *  \return None.
 */
static void
present_permutation(uint8_t *p_text);

/*! \brief Update key layer of the encryption operation.
 *
 *  The function rotates the \p p_key to the left 61-bit, substitutes
 *  the MSB nibbles of the new value and XOR's the related bits with the
 *  \p round_counter. For further information about the update key layer,
 *  see the article's section 3.
 *
 *  \warning The function assumes the parameter \a p_key points a memory
 *           block with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_key         Pointer of the crypt key.
 *  \param round_counter Index of the algorithm loop.
 *
 *  \return None.
 */
static void
present_update_key(uint8_t *p_key, uint8_t round_counter);

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

/*****************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

void
present_encrypt(uint8_t *p_text, uint8_t const *p_key)
{
    uint8_t subkey[PRESENT_KEY_SIZE];
    uint8_t round;

    /* Copy key into a buffer to keep original key value unchanged during
     * the encryption process.
     */
    memcpy(subkey, p_key, PRESENT_KEY_SIZE);

    /* Start the loop from round one. */
    round = 1u;

    /* Main loop of the PRESENT encryption algorithm. */
    while (round <= PRESENT_ROUND_COUNT) {
        present_add_key(p_text, subkey);
        present_substitution(p_text);
        present_permutation(p_text);

        present_update_key(subkey, round);
        round++;
    };

    /* Add the last subkey to finish the process. */
    present_add_key(p_text, subkey);
}

/*****************************************************************************/
/* STATIC FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

static void
present_add_key(uint8_t *p_text, uint8_t const *p_key)
{
    uint8_t byte_idx;

    /* Move the key pointer to the start byte of the key part that specified
     * in the article. For further information, see the article's section 3.
     */
    p_key += PRESENT_KEY_OFFSET;

    /* Adding key is simply logic XOR operation. */
    for (byte_idx = 0u; byte_idx < PRESENT_CRYPT_SIZE; byte_idx++) {
        p_text[byte_idx] = p_text[byte_idx] ^ p_key[byte_idx];
    }
}

static void
present_substitution(uint8_t *p_text)
{
    uint8_t high_nibble;
    uint8_t low_nibble;
    uint8_t byte_idx;

    /* Replace all the bytes in the text block. */
    for (byte_idx = 0u; byte_idx < PRESENT_CRYPT_SIZE; byte_idx++) {
        high_nibble = (p_text[byte_idx] & 0xF0u) >> 4;
        high_nibble = g_sbox[high_nibble];

        low_nibble = p_text[byte_idx] & 0x0Fu;
        low_nibble = g_sbox[low_nibble];

        p_text[byte_idx] = (high_nibble << 4) | low_nibble;
    }
}

static void
present_permutation(uint8_t *p_text)
{
    uint8_t buff[PRESENT_CRYPT_SIZE];
    uint8_t base_bit;
    uint8_t base_byte;
    uint8_t bit_idx;

    /* Clear buffer first to do logical OR'ing directly. */
    memset(buff, 0u, sizeof(buff));

    base_bit  = 0u;
    base_byte = 0u;
    bit_idx   = 0u;

    /* Every new byte has two bit values from four bytes of the old text
     * block. Even bits are from low and odd bits are from high nibbles.
     * In every step of the loop, bit values are picked from related bytes.
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
present_update_key(uint8_t *p_key, uint8_t round_counter)
{
    uint8_t high_nibble;
    uint8_t low_nibble;

    /* Rotate the key to the left as first step of the key scheduling. */
    present_rotate_key_left(p_key);

    /* Substitute the MSB high nibble of the key. */
    high_nibble = (p_key[PRESENT_KEY_SIZE - 1] & 0xF0u) >> 4;
    high_nibble = g_sbox[high_nibble];

    low_nibble = p_key[PRESENT_KEY_SIZE - 1] & 0x0Fu;

    p_key[PRESENT_KEY_SIZE - 1] = (high_nibble << 4) | low_nibble;

    /* XOR the from 15th to 19th bits with the round counter. */
    p_key[2] ^= round_counter >> 1;
    p_key[1] ^= round_counter << 7;
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

    p_block   = (uint16_t *)p_key;
    block_idx = 0u;

    /* Fill the buffer with the values that changes during the first loop. */
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
