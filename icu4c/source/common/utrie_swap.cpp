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
utrie_swap(अचर UDataSwapper *ds,
           अचर शून्य *inData, int32_t length, शून्य *outData,
           UErrorCode *pErrorCode) {
    अचर UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    यदि(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    /* setup and swapping */
    यदि(length>=0 && (uint32_t)length<परिमाण(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }

    inTrie=(अचर UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    यदि( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_गन्ती-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        फर्काऊ 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=परिमाण(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    यदि(length>=0) {
        UTrieHeader *outTrie;

        यदि(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, परिमाण(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        यदि(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (अचर uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } अन्यथा {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    फर्काऊ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(अचर UDataSwapper *ds,
            अचर शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    अचर UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_मान_Bits मान_Bits;

    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    यदि(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    /* setup and swapping */
    यदि(length>=0 && length<(int32_t)परिमाण(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }

    inTrie=(अचर UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    मान_Bits=(UTrie2_मान_Bits)(trie.options&UTRIE2_OPTIONS_मान_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    यदि( trie.signature!=UTRIE2_SIG ||
        मान_Bits<0 || UTRIE2_गन्ती_मान_BITS<=मान_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        फर्काऊ 0;
    }

    size=परिमाण(UTrie2Header)+trie.indexLength*2;
    विकल्प(मान_Bits) {
    पक्ष UTRIE2_16_मान_BITS:
        size+=dataLength*2;
        रोक;
    पक्ष UTRIE2_32_मान_BITS:
        size+=dataLength*4;
        रोक;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ 0;
    }

    यदि(length>=0) {
        UTrie2Header *outTrie;

        यदि(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        विकल्प(मान_Bits) {
        पक्ष UTRIE2_16_मान_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            रोक;
        पक्ष UTRIE2_32_मान_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (अचर uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            रोक;
        स्वभाव:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            फर्काऊ 0;
        }
    }

    फर्काऊ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(अचर UDataSwapper *ds,
             अचर शून्य *inData, int32_t length, शून्य *outData,
             UErrorCode *pErrorCode) {
    अचर UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_मान_Width मान_Width;

    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    यदि(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    /* setup and swapping */
    यदि(length>=0 && length<(int32_t)परिमाण(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }

    inTrie=(अचर UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    मान_Width = (UCPTrie_मान_Width)(trie.options & UCPTRIE_OPTIONS_मान_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    यदि( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        मान_Width > UCPTRIE_मान_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        फर्काऊ 0;
    }

    size=परिमाण(UCPTrieHeader)+trie.indexLength*2;
    विकल्प(मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        size+=dataLength*2;
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        size+=dataLength*4;
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        size+=dataLength;
        रोक;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ 0;
    }

    यदि(length>=0) {
        UCPTrieHeader *outTrie;

        यदि(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        अचर uint16_t *inIndex=reinterpret_cast<अचर uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        अचर uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        विकल्प(मान_Width) {
        पक्ष UCPTRIE_मान_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            रोक;
        पक्ष UCPTRIE_मान_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            रोक;
        पक्ष UCPTRIE_मान_BITS_8:
            यदि(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            रोक;
        स्वभाव:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            फर्काऊ 0;
        }
    }

    फर्काऊ size;
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
getVersion(अचर शून्य *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    यदि(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        फर्काऊ 0;
    }
    signature = *static_cast<अचर uint32_t*>(data);
    यदि(signature==UCPTRIE_SIG) {
        फर्काऊ 3;
    }
    यदि(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        फर्काऊ 3;
    }
    यदि(signature==UTRIE2_SIG) {
        फर्काऊ 2;
    }
    यदि(anyEndianOk && signature==UTRIE2_OE_SIG) {
        फर्काऊ 2;
    }
    यदि(signature==UTRIE_SIG) {
        फर्काऊ 1;
    }
    यदि(anyEndianOk && signature==UTRIE_OE_SIG) {
        फर्काऊ 1;
    }
    फर्काऊ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(अचर UDataSwapper *ds,
                     अचर शून्य *inData, int32_t length, शून्य *outData,
                     UErrorCode *pErrorCode) {
    यदि(U_FAILURE(*pErrorCode)) { फर्काऊ 0; }
    विकल्प(getVersion(inData, length, सत्य)) {
    पक्ष 1:
        फर्काऊ utrie_swap(ds, inData, length, outData, pErrorCode);
    पक्ष 2:
        फर्काऊ utrie2_swap(ds, inData, length, outData, pErrorCode);
    पक्ष 3:
        फर्काऊ ucptrie_swap(ds, inData, length, outData, pErrorCode);
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ 0;
    }
}
