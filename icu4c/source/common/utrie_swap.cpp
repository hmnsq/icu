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

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    यदि(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    यदि(length>=0 && (uint32_t)length<परिमाण(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    inTrie=(ध्रुव UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    यदि( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_गिनती-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        प्रत्यागम 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=परिमाण(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    यदि(length>=0) {
        UTrieHeader *outTrie;

        यदि(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, परिमाण(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        यदि(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ध्रुव uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } अन्यथा {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    प्रत्यागम size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    ध्रुव UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_मूल्य_Bits मूल्य_Bits;

    यदि(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    यदि(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    यदि(length>=0 && length<(int32_t)परिमाण(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    inTrie=(ध्रुव UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    मूल्य_Bits=(UTrie2_मूल्य_Bits)(trie.options&UTRIE2_OPTIONS_मूल्य_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    यदि( trie.signature!=UTRIE2_SIG ||
        मूल्य_Bits<0 || UTRIE2_गिनती_मूल्य_BITS<=मूल्य_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        प्रत्यागम 0;
    }

    size=परिमाण(UTrie2Header)+trie.indexLength*2;
    विकल्प(मूल्य_Bits) {
    पक्ष UTRIE2_16_मूल्य_BITS:
        size+=dataLength*2;
        भङ्ग;
    पक्ष UTRIE2_32_मूल्य_BITS:
        size+=dataLength*4;
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>=0) {
        UTrie2Header *outTrie;

        यदि(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        विकल्प(मूल्य_Bits) {
        पक्ष UTRIE2_16_मूल्य_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            भङ्ग;
        पक्ष UTRIE2_32_मूल्य_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ध्रुव uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            भङ्ग;
        स्वभाव:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            प्रत्यागम 0;
        }
    }

    प्रत्यागम size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ध्रुव UDataSwapper *ds,
             ध्रुव शून्य *inData, int32_t length, शून्य *outData,
             UErrorCode *pErrorCode) {
    ध्रुव UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_मूल्य_Width मूल्य_Width;

    यदि(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    यदि(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    यदि(length>=0 && length<(int32_t)परिमाण(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    inTrie=(ध्रुव UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    मूल्य_Width = (UCPTrie_मूल्य_Width)(trie.options & UCPTRIE_OPTIONS_मूल्य_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    यदि( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        मूल्य_Width > UCPTRIE_मूल्य_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        प्रत्यागम 0;
    }

    size=परिमाण(UCPTrieHeader)+trie.indexLength*2;
    विकल्प(मूल्य_Width) {
    पक्ष UCPTRIE_मूल्य_BITS_16:
        size+=dataLength*2;
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_32:
        size+=dataLength*4;
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_8:
        size+=dataLength;
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>=0) {
        UCPTrieHeader *outTrie;

        यदि(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
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
        विकल्प(मूल्य_Width) {
        पक्ष UCPTRIE_मूल्य_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            भङ्ग;
        पक्ष UCPTRIE_मूल्य_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            भङ्ग;
        पक्ष UCPTRIE_मूल्य_BITS_8:
            यदि(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            भङ्ग;
        स्वभाव:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            प्रत्यागम 0;
        }
    }

    प्रत्यागम size;
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
    यदि(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        प्रत्यागम 0;
    }
    signature = *static_cast<ध्रुव uint32_t*>(data);
    यदि(signature==UCPTRIE_SIG) {
        प्रत्यागम 3;
    }
    यदि(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        प्रत्यागम 3;
    }
    यदि(signature==UTRIE2_SIG) {
        प्रत्यागम 2;
    }
    यदि(anyEndianOk && signature==UTRIE2_OE_SIG) {
        प्रत्यागम 2;
    }
    यदि(signature==UTRIE_SIG) {
        प्रत्यागम 1;
    }
    यदि(anyEndianOk && signature==UTRIE_OE_SIG) {
        प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ध्रुव UDataSwapper *ds,
                     ध्रुव शून्य *inData, int32_t length, शून्य *outData,
                     UErrorCode *pErrorCode) {
    यदि(U_FAILURE(*pErrorCode)) { प्रत्यागम 0; }
    विकल्प(getVersion(inData, length, सत्य)) {
    पक्ष 1:
        प्रत्यागम utrie_swap(ds, inData, length, outData, pErrorCode);
    पक्ष 2:
        प्रत्यागम utrie2_swap(ds, inData, length, outData, pErrorCode);
    पक्ष 3:
        प्रत्यागम ucptrie_swap(ds, inData, length, outData, pErrorCode);
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }
}
