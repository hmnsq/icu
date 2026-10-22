// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ucptrie_impl.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "utrie.h"
#അന്തര്ഭാവ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

നാമാകാശ {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(സ്ഥിരം UDataSwapper *ds,
           സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
           UErrorCode *pErrorCode) {
    സ്ഥിരം UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* setup and swapping */
    എങ്കിൽ(length>=0 && (uint32_t)length<വലുപ്പം(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }

    inTrie=(സ്ഥിരം UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    എങ്കിൽ( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_എണ്ണം-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        തിരികെ 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=വലുപ്പം(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    എങ്കിൽ(length>=0) {
        UTrieHeader *outTrie;

        എങ്കിൽ(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, വലുപ്പം(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        എങ്കിൽ(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (സ്ഥിരം uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } അല്ലെങ്കിൽ {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    തിരികെ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(സ്ഥിരം UDataSwapper *ds,
            സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
            UErrorCode *pErrorCode) {
    സ്ഥിരം UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_മൂല്യം_Bits മൂല്യം_Bits;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* setup and swapping */
    എങ്കിൽ(length>=0 && length<(int32_t)വലുപ്പം(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }

    inTrie=(സ്ഥിരം UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    മൂല്യം_Bits=(UTrie2_മൂല്യം_Bits)(trie.options&UTRIE2_OPTIONS_മൂല്യം_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    എങ്കിൽ( trie.signature!=UTRIE2_SIG ||
        മൂല്യം_Bits<0 || UTRIE2_എണ്ണം_മൂല്യം_BITS<=മൂല്യം_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        തിരികെ 0;
    }

    size=വലുപ്പം(UTrie2Header)+trie.indexLength*2;
    തിരഞ്ഞെടുപ്പ്(മൂല്യം_Bits) {
    അവസ്ഥ UTRIE2_16_മൂല്യം_BITS:
        size+=dataLength*2;
        നിർത്തുക;
    അവസ്ഥ UTRIE2_32_മൂല്യം_BITS:
        size+=dataLength*4;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(length>=0) {
        UTrie2Header *outTrie;

        എങ്കിൽ(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        തിരഞ്ഞെടുപ്പ്(മൂല്യം_Bits) {
        അവസ്ഥ UTRIE2_16_മൂല്യം_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            നിർത്തുക;
        അവസ്ഥ UTRIE2_32_മൂല്യം_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (സ്ഥിരം uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            തിരികെ 0;
        }
    }

    തിരികെ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(സ്ഥിരം UDataSwapper *ds,
             സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
             UErrorCode *pErrorCode) {
    സ്ഥിരം UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_മൂല്യം_Width മൂല്യം_Width;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* setup and swapping */
    എങ്കിൽ(length>=0 && length<(int32_t)വലുപ്പം(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }

    inTrie=(സ്ഥിരം UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    മൂല്യം_Width = (UCPTrie_മൂല്യം_Width)(trie.options & UCPTRIE_OPTIONS_മൂല്യം_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    എങ്കിൽ( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        മൂല്യം_Width > UCPTRIE_മൂല്യം_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        തിരികെ 0;
    }

    size=വലുപ്പം(UCPTrieHeader)+trie.indexLength*2;
    തിരഞ്ഞെടുപ്പ്(മൂല്യം_Width) {
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
        size+=dataLength*2;
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
        size+=dataLength*4;
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
        size+=dataLength;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(length>=0) {
        UCPTrieHeader *outTrie;

        എങ്കിൽ(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        സ്ഥിരം uint16_t *inIndex=reinterpret_cast<സ്ഥിരം uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        സ്ഥിരം uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        തിരഞ്ഞെടുപ്പ്(മൂല്യം_Width) {
        അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            നിർത്തുക;
        അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            നിർത്തുക;
        അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
            എങ്കിൽ(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            തിരികെ 0;
        }
    }

    തിരികെ size;
}

നാമാകാശ {

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
getVersion(സ്ഥിരം ശൂന്യം *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    എങ്കിൽ(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        തിരികെ 0;
    }
    signature = *static_cast<സ്ഥിരം uint32_t*>(data);
    എങ്കിൽ(signature==UCPTRIE_SIG) {
        തിരികെ 3;
    }
    എങ്കിൽ(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        തിരികെ 3;
    }
    എങ്കിൽ(signature==UTRIE2_SIG) {
        തിരികെ 2;
    }
    എങ്കിൽ(anyEndianOk && signature==UTRIE2_OE_SIG) {
        തിരികെ 2;
    }
    എങ്കിൽ(signature==UTRIE_SIG) {
        തിരികെ 1;
    }
    എങ്കിൽ(anyEndianOk && signature==UTRIE_OE_SIG) {
        തിരികെ 1;
    }
    തിരികെ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(സ്ഥിരം UDataSwapper *ds,
                     സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                     UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) { തിരികെ 0; }
    തിരഞ്ഞെടുപ്പ്(getVersion(inData, length, സത്യം)) {
    അവസ്ഥ 1:
        തിരികെ utrie_swap(ds, inData, length, outData, pErrorCode);
    അവസ്ഥ 2:
        തിരികെ utrie2_swap(ds, inData, length, outData, pErrorCode);
    അവസ്ഥ 3:
        തിരികെ ucptrie_swap(ds, inData, length, outData, pErrorCode);
    സ്ഥിരസ്ഥിതി:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ 0;
    }
}
