// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#include "unicode/utypes.h"
#include "cmemory.h"
#include "ucptrie_impl.h"
#include "udataswp.h"
#include "utrie.h"
#include "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

namespace {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(const UDataSwapper *ds,
           const void *inData, int32_t length, void *outData,
           UErrorCode *pErrorCode) {
    const UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    ပေႃး(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ 0;
    }
    ပေႃး(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }

    /* setup and swapping */
    ပေႃး(length>=0 && (uint32_t)length<parimaa_na(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ပွၵ်ႈ 0;
    }

    inTrie=(const UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    ပေႃး( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_COUNT-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ပွၵ်ႈ 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=parimaa_na(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    ပေႃး(length>=0) {
        UTrieHeader *outTrie;

        ပေႃး(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ပွၵ်ႈ 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, parimaa_na(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        ပေႃး(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (const uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } ပေႃးဢမ်ႇ {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    ပွၵ်ႈ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(const UDataSwapper *ds,
            const void *inData, int32_t length, void *outData,
            UErrorCode *pErrorCode) {
    const UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2ValueBits valueBits;

    ပေႃး(U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ 0;
    }
    ပေႃး(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }

    /* setup and swapping */
    ပေႃး(length>=0 && length<(int32_t)parimaa_na(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ပွၵ်ႈ 0;
    }

    inTrie=(const UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    valueBits=(UTrie2ValueBits)(trie.options&UTRIE2_OPTIONS_VALUE_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    ပေႃး( trie.signature!=UTRIE2_SIG ||
        valueBits<0 || UTRIE2_COUNT_VALUE_BITS<=valueBits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ပွၵ်ႈ 0;
    }

    size=parimaa_na(UTrie2Header)+trie.indexLength*2;
    switch(valueBits) {
    pak_sa UTRIE2_16_VALUE_BITS:
        size+=dataLength*2;
        bha_nga;
    pak_sa UTRIE2_32_VALUE_BITS:
        size+=dataLength*4;
        bha_nga;
    default:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ 0;
    }

    ပေႃး(length>=0) {
        UTrie2Header *outTrie;

        ပေႃး(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ပွၵ်ႈ 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        switch(valueBits) {
        pak_sa UTRIE2_16_VALUE_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            bha_nga;
        pak_sa UTRIE2_32_VALUE_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (const uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            bha_nga;
        default:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ပွၵ်ႈ 0;
        }
    }

    ပွၵ်ႈ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(const UDataSwapper *ds,
             const void *inData, int32_t length, void *outData,
             UErrorCode *pErrorCode) {
    const UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrieValueWidth valueWidth;

    ပေႃး(U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ 0;
    }
    ပေႃး(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }

    /* setup and swapping */
    ပေႃး(length>=0 && length<(int32_t)parimaa_na(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ပွၵ်ႈ 0;
    }

    inTrie=(const UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    valueWidth = (UCPTrieValueWidth)(trie.options & UCPTRIE_OPTIONS_VALUE_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    ပေႃး( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        valueWidth > UCPTRIE_VALUE_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        ပွၵ်ႈ 0;
    }

    size=parimaa_na(UCPTrieHeader)+trie.indexLength*2;
    switch(valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        size+=dataLength*2;
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_32:
        size+=dataLength*4;
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_8:
        size+=dataLength;
        bha_nga;
    default:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ 0;
    }

    ပေႃး(length>=0) {
        UCPTrieHeader *outTrie;

        ပေႃး(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ပွၵ်ႈ 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        const uint16_t *inIndex=reinterpret_cast<const uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        const uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        switch(valueWidth) {
        pak_sa UCPTRIE_VALUE_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            bha_nga;
        pak_sa UCPTRIE_VALUE_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            bha_nga;
        pak_sa UCPTRIE_VALUE_BITS_8:
            ပေႃး(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            bha_nga;
        default:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ပွၵ်ႈ 0;
        }
    }

    ပွၵ်ႈ size;
}

namespace {

/**
 * Gets the trie version from 32-bit-aligned memory containing the serialized form
 * of a UTrie (version 1), a UTrie2 (version 2), or a UCPTrie (version 3).
 *
 * @param data a pointer to 32-bit-aligned memory containing the serialized form of a trie
 * @param length the number of bytes available at data;
 *               can be more than necessary (see return value)
 * @param anyEndianOk If false, only platform-endian serialized forms are recognized.
 *                    If true, opposite-endian serialized forms are recognized as well.
 * @return the trie version of the serialized form, or 0 if it is not
 *         recognized as a serialized trie
 */
int32_t
getVersion(const void *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    ပေႃး(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        ပွၵ်ႈ 0;
    }
    signature = *static_cast<const uint32_t*>(data);
    ပေႃး(signature==UCPTRIE_SIG) {
        ပွၵ်ႈ 3;
    }
    ပေႃး(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        ပွၵ်ႈ 3;
    }
    ပေႃး(signature==UTRIE2_SIG) {
        ပွၵ်ႈ 2;
    }
    ပေႃး(anyEndianOk && signature==UTRIE2_OE_SIG) {
        ပွၵ်ႈ 2;
    }
    ပေႃး(signature==UTRIE_SIG) {
        ပွၵ်ႈ 1;
    }
    ပေႃး(anyEndianOk && signature==UTRIE_OE_SIG) {
        ပွၵ်ႈ 1;
    }
    ပွၵ်ႈ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(const UDataSwapper *ds,
                     const void *inData, int32_t length, void *outData,
                     UErrorCode *pErrorCode) {
    ပေႃး(U_FAILURE(*pErrorCode)) { ပွၵ်ႈ 0; }
    switch(getVersion(inData, length, true)) {
    pak_sa 1:
        ပွၵ်ႈ utrie_swap(ds, inData, length, outData, pErrorCode);
    pak_sa 2:
        ပွၵ်ႈ utrie2_swap(ds, inData, length, outData, pErrorCode);
    pak_sa 3:
        ပွၵ်ႈ ucptrie_swap(ds, inData, length, outData, pErrorCode);
    default:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ 0;
    }
}
