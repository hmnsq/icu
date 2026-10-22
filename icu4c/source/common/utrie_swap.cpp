// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucptrie_impl.h"
#அந்தர்பாவ "udataswp.h"
#அந்தர்பாவ "utrie.h"
#அந்தர்பாவ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

நாமாகாஶ {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(மாறிலி UDataSwapper *ds,
           மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
           UErrorCode *pErrorCode) {
    மாறிலி UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* setup and swapping */
    எனில்(length>=0 && (uint32_t)length<அளவு(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    inTrie=(மாறிலி UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    எனில்( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_மொத்தம்-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        திரும்பு 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=அளவு(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    எனில்(length>=0) {
        UTrieHeader *outTrie;

        எனில்(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, அளவு(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        எனில்(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (மாறிலி uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } இல்லையெனில் {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    திரும்பு size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(மாறிலி UDataSwapper *ds,
            மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
            UErrorCode *pErrorCode) {
    மாறிலி UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_மதிப்பு_Bits மதிப்பு_Bits;

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* setup and swapping */
    எனில்(length>=0 && length<(int32_t)அளவு(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    inTrie=(மாறிலி UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    மதிப்பு_Bits=(UTrie2_மதிப்பு_Bits)(trie.options&UTRIE2_OPTIONS_மதிப்பு_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    எனில்( trie.signature!=UTRIE2_SIG ||
        மதிப்பு_Bits<0 || UTRIE2_மொத்தம்_மதிப்பு_BITS<=மதிப்பு_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        திரும்பு 0;
    }

    size=அளவு(UTrie2Header)+trie.indexLength*2;
    தேர்வு(மதிப்பு_Bits) {
    நிலை UTRIE2_16_மதிப்பு_BITS:
        size+=dataLength*2;
        நிறுத்து;
    நிலை UTRIE2_32_மதிப்பு_BITS:
        size+=dataLength*4;
        நிறுத்து;
    இயல்பு:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு 0;
    }

    எனில்(length>=0) {
        UTrie2Header *outTrie;

        எனில்(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        தேர்வு(மதிப்பு_Bits) {
        நிலை UTRIE2_16_மதிப்பு_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            நிறுத்து;
        நிலை UTRIE2_32_மதிப்பு_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (மாறிலி uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            நிறுத்து;
        இயல்பு:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            திரும்பு 0;
        }
    }

    திரும்பு size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(மாறிலி UDataSwapper *ds,
             மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
             UErrorCode *pErrorCode) {
    மாறிலி UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_மதிப்பு_Width மதிப்பு_Width;

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* setup and swapping */
    எனில்(length>=0 && length<(int32_t)அளவு(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    inTrie=(மாறிலி UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    மதிப்பு_Width = (UCPTrie_மதிப்பு_Width)(trie.options & UCPTRIE_OPTIONS_மதிப்பு_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    எனில்( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        மதிப்பு_Width > UCPTRIE_மதிப்பு_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        திரும்பு 0;
    }

    size=அளவு(UCPTrieHeader)+trie.indexLength*2;
    தேர்வு(மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        size+=dataLength*2;
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        size+=dataLength*4;
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        size+=dataLength;
        நிறுத்து;
    இயல்பு:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு 0;
    }

    எனில்(length>=0) {
        UCPTrieHeader *outTrie;

        எனில்(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        மாறிலி uint16_t *inIndex=reinterpret_cast<மாறிலி uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        மாறிலி uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        தேர்வு(மதிப்பு_Width) {
        நிலை UCPTRIE_மதிப்பு_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            நிறுத்து;
        நிலை UCPTRIE_மதிப்பு_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            நிறுத்து;
        நிலை UCPTRIE_மதிப்பு_BITS_8:
            எனில்(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            நிறுத்து;
        இயல்பு:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            திரும்பு 0;
        }
    }

    திரும்பு size;
}

நாமாகாஶ {

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
getVersion(மாறிலி வெறுமை *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    எனில்(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        திரும்பு 0;
    }
    signature = *static_cast<மாறிலி uint32_t*>(data);
    எனில்(signature==UCPTRIE_SIG) {
        திரும்பு 3;
    }
    எனில்(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        திரும்பு 3;
    }
    எனில்(signature==UTRIE2_SIG) {
        திரும்பு 2;
    }
    எனில்(anyEndianOk && signature==UTRIE2_OE_SIG) {
        திரும்பு 2;
    }
    எனில்(signature==UTRIE_SIG) {
        திரும்பு 1;
    }
    எனில்(anyEndianOk && signature==UTRIE_OE_SIG) {
        திரும்பு 1;
    }
    திரும்பு 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(மாறிலி UDataSwapper *ds,
                     மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                     UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) { திரும்பு 0; }
    தேர்வு(getVersion(inData, length, உண்மை)) {
    நிலை 1:
        திரும்பு utrie_swap(ds, inData, length, outData, pErrorCode);
    நிலை 2:
        திரும்பு utrie2_swap(ds, inData, length, outData, pErrorCode);
    நிலை 3:
        திரும்பு ucptrie_swap(ds, inData, length, outData, pErrorCode);
    இயல்பு:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு 0;
    }
}
