// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucptrie_impl.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "utrie.h"
#अन्तर्भाव "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

नामाकाश {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(ध्रुव UDataSwapper *ds,
           ध्रुव शून्य *inData, int32_t length, शून्य *outData,
           UErrorCode *pErrorCode) {
    ध्रुव UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    सचे(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        पटिनिवत्त 0;
    }
    सचे(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }

    /* setup and swapping */
    सचे(length>=0 && (uint32_t)length<परिमाण(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    inTrie=(ध्रुव UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    सचे( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_गणनम्-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        पटिनिवत्त 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=परिमाण(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    सचे(length>=0) {
        UTrieHeader *outTrie;

        सचे(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, परिमाण(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        सचे(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ध्रुव uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } अञ्ञथा {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    पटिनिवत्त size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    ध्रुव UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_मानम्_Bits मानम्_Bits;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त 0;
    }
    सचे(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }

    /* setup and swapping */
    सचे(length>=0 && length<(int32_t)परिमाण(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    inTrie=(ध्रुव UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    मानम्_Bits=(UTrie2_मानम्_Bits)(trie.options&UTRIE2_OPTIONS_मानम्_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    सचे( trie.signature!=UTRIE2_SIG ||
        मानम्_Bits<0 || UTRIE2_गणनम्_मानम्_BITS<=मानम्_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        पटिनिवत्त 0;
    }

    size=परिमाण(UTrie2Header)+trie.indexLength*2;
    विकल्प(मानम्_Bits) {
    पक्ष UTRIE2_16_मानम्_BITS:
        size+=dataLength*2;
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_BITS:
        size+=dataLength*4;
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        पटिनिवत्त 0;
    }

    सचे(length>=0) {
        UTrie2Header *outTrie;

        सचे(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        विकल्प(मानम्_Bits) {
        पक्ष UTRIE2_16_मानम्_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            भङ्ग;
        पक्ष UTRIE2_32_मानम्_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ध्रुव uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            भङ्ग;
        स्वभाव:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            पटिनिवत्त 0;
        }
    }

    पटिनिवत्त size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ध्रुव UDataSwapper *ds,
             ध्रुव शून्य *inData, int32_t length, शून्य *outData,
             UErrorCode *pErrorCode) {
    ध्रुव UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_मानम्_Width मानम्_Width;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त 0;
    }
    सचे(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }

    /* setup and swapping */
    सचे(length>=0 && length<(int32_t)परिमाण(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    inTrie=(ध्रुव UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    मानम्_Width = (UCPTrie_मानम्_Width)(trie.options & UCPTRIE_OPTIONS_मानम्_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    सचे( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        मानम्_Width > UCPTRIE_मानम्_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        पटिनिवत्त 0;
    }

    size=परिमाण(UCPTrieHeader)+trie.indexLength*2;
    विकल्प(मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        size+=dataLength*2;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        size+=dataLength*4;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        size+=dataLength;
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        पटिनिवत्त 0;
    }

    सचे(length>=0) {
        UCPTrieHeader *outTrie;

        सचे(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        ध्रुव uint16_t *inIndex=reinterpret_cast<ध्रुव uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        ध्रुव uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        विकल्प(मानम्_Width) {
        पक्ष UCPTRIE_मानम्_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            भङ्ग;
        पक्ष UCPTRIE_मानम्_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            भङ्ग;
        पक्ष UCPTRIE_मानम्_BITS_8:
            सचे(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            भङ्ग;
        स्वभाव:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            पटिनिवत्त 0;
        }
    }

    पटिनिवत्त size;
}

नामाकाश {

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
getVersion(ध्रुव शून्य *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    सचे(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        पटिनिवत्त 0;
    }
    signature = *static_cast<ध्रुव uint32_t*>(data);
    सचे(signature==UCPTRIE_SIG) {
        पटिनिवत्त 3;
    }
    सचे(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        पटिनिवत्त 3;
    }
    सचे(signature==UTRIE2_SIG) {
        पटिनिवत्त 2;
    }
    सचे(anyEndianOk && signature==UTRIE2_OE_SIG) {
        पटिनिवत्त 2;
    }
    सचे(signature==UTRIE_SIG) {
        पटिनिवत्त 1;
    }
    सचे(anyEndianOk && signature==UTRIE_OE_SIG) {
        पटिनिवत्त 1;
    }
    पटिनिवत्त 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ध्रुव UDataSwapper *ds,
                     ध्रुव शून्य *inData, int32_t length, शून्य *outData,
                     UErrorCode *pErrorCode) {
    सचे(U_FAILURE(*pErrorCode)) { पटिनिवत्त 0; }
    विकल्प(getVersion(inData, length, सच्च)) {
    पक्ष 1:
        पटिनिवत्त utrie_swap(ds, inData, length, outData, pErrorCode);
    पक्ष 2:
        पटिनिवत्त utrie2_swap(ds, inData, length, outData, pErrorCode);
    पक्ष 3:
        पटिनिवत्त ucptrie_swap(ds, inData, length, outData, pErrorCode);
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        पटिनिवत्त 0;
    }
}
