// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "utrie.h"
#ಅನ್ತರ್ಭಾವ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

ನಾಮಾಕಾಶ {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(ಧ್ರುವ UDataSwapper *ds,
           ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
           UErrorCode *pErrorCode) {
    ಧ್ರುವ UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* setup and swapping */
    ಯದಿ(length>=0 && (uint32_t)length<ಪರಿಮಾಣ(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inTrie=(ಧ್ರುವ UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    ಯದಿ( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_ಎಣಿಕೆ-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=ಪರಿಮಾಣ(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    ಯದಿ(length>=0) {
        UTrieHeader *outTrie;

        ಯದಿ(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, ಪರಿಮಾಣ(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        ಯದಿ(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ಧ್ರುವ uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } ಅನ್ಯಥಾ {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    ಪ್ರತ್ಯಾಗಮ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ಧ್ರುವ UDataSwapper *ds,
            ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
            UErrorCode *pErrorCode) {
    ಧ್ರುವ UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_ಮೌಲ್ಯ_Bits ಮೌಲ್ಯ_Bits;

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* setup and swapping */
    ಯದಿ(length>=0 && length<(int32_t)ಪರಿಮಾಣ(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inTrie=(ಧ್ರುವ UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    ಮೌಲ್ಯ_Bits=(UTrie2_ಮೌಲ್ಯ_Bits)(trie.options&UTRIE2_OPTIONS_ಮೌಲ್ಯ_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    ಯದಿ( trie.signature!=UTRIE2_SIG ||
        ಮೌಲ್ಯ_Bits<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_BITS<=ಮೌಲ್ಯ_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    size=ಪರಿಮಾಣ(UTrie2Header)+trie.indexLength*2;
    ವಿಕಲ್ಪ(ಮೌಲ್ಯ_Bits) {
    ಪಕ್ಷ UTRIE2_16_ಮೌಲ್ಯ_BITS:
        size+=dataLength*2;
        ಭಙ್ಗ;
    ಪಕ್ಷ UTRIE2_32_ಮೌಲ್ಯ_BITS:
        size+=dataLength*4;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(length>=0) {
        UTrie2Header *outTrie;

        ಯದಿ(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        ವಿಕಲ್ಪ(ಮೌಲ್ಯ_Bits) {
        ಪಕ್ಷ UTRIE2_16_ಮೌಲ್ಯ_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            ಭಙ್ಗ;
        ಪಕ್ಷ UTRIE2_32_ಮೌಲ್ಯ_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ಧ್ರುವ uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    ಪ್ರತ್ಯಾಗಮ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ಧ್ರುವ UDataSwapper *ds,
             ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
             UErrorCode *pErrorCode) {
    ಧ್ರುವ UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width;

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* setup and swapping */
    ಯದಿ(length>=0 && length<(int32_t)ಪರಿಮಾಣ(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inTrie=(ಧ್ರುವ UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    ಮೌಲ್ಯ_Width = (UCPTrie_ಮೌಲ್ಯ_Width)(trie.options & UCPTRIE_OPTIONS_ಮೌಲ್ಯ_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    ಯದಿ( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        ಮೌಲ್ಯ_Width > UCPTRIE_ಮೌಲ್ಯ_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    size=ಪರಿಮಾಣ(UCPTrieHeader)+trie.indexLength*2;
    ವಿಕಲ್ಪ(ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        size+=dataLength*2;
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        size+=dataLength*4;
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        size+=dataLength;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(length>=0) {
        UCPTrieHeader *outTrie;

        ಯದಿ(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        ಧ್ರುವ uint16_t *inIndex=reinterpret_cast<ಧ್ರುವ uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        ಧ್ರುವ uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        ವಿಕಲ್ಪ(ಮೌಲ್ಯ_Width) {
        ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            ಭಙ್ಗ;
        ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            ಭಙ್ಗ;
        ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
            ಯದಿ(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    ಪ್ರತ್ಯಾಗಮ size;
}

ನಾಮಾಕಾಶ {

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
getVersion(ಧ್ರುವ ಶೂನ್ಯ *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    ಯದಿ(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    signature = *static_cast<ಧ್ರುವ uint32_t*>(data);
    ಯದಿ(signature==UCPTRIE_SIG) {
        ಪ್ರತ್ಯಾಗಮ 3;
    }
    ಯದಿ(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        ಪ್ರತ್ಯಾಗಮ 3;
    }
    ಯದಿ(signature==UTRIE2_SIG) {
        ಪ್ರತ್ಯಾಗಮ 2;
    }
    ಯದಿ(anyEndianOk && signature==UTRIE2_OE_SIG) {
        ಪ್ರತ್ಯಾಗಮ 2;
    }
    ಯದಿ(signature==UTRIE_SIG) {
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಯದಿ(anyEndianOk && signature==UTRIE_OE_SIG) {
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ಧ್ರುವ UDataSwapper *ds,
                     ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                     UErrorCode *pErrorCode) {
    ಯದಿ(U_FAILURE(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ವಿಕಲ್ಪ(getVersion(inData, length, ಸತ್ಯ)) {
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ utrie_swap(ds, inData, length, outData, pErrorCode);
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ utrie2_swap(ds, inData, length, outData, pErrorCode);
    ಪಕ್ಷ 3:
        ಪ್ರತ್ಯಾಗಮ ucptrie_swap(ds, inData, length, outData, pErrorCode);
    ಸ್ವಭಾವ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}
