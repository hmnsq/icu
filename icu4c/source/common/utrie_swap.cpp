// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "ucptrie_impl.h"
#ضمن "udataswp.h"
#ضمن "utrie.h"
#ضمن "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

نطاق {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(ثابت UDataSwapper *ds,
           ثابت فراغ *inData, int32_t length, فراغ *outData,
           UErrorCode *pErrorCode) {
    ثابت UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    إذا(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && (uint32_t)length<حجم(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    inTrie=(ثابت UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    إذا( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_كدي-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=حجم(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    إذا(length>=0) {
        UTrieHeader *outTrie;

        إذا(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, حجم(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        إذا(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ثابت uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } وإلا {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ثابت UDataSwapper *ds,
            ثابت فراغ *inData, int32_t length, فراغ *outData,
            UErrorCode *pErrorCode) {
    ثابت UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_درج_Bits درج_Bits;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    إذا(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    inTrie=(ثابت UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    درج_Bits=(UTrie2_درج_Bits)(trie.options&UTRIE2_OPTIONS_درج_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    إذا( trie.signature!=UTRIE2_SIG ||
        درج_Bits<0 || UTRIE2_كدي_درج_BITS<=درج_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    size=حجم(UTrie2Header)+trie.indexLength*2;
    بدل(درج_Bits) {
    حالة UTRIE2_16_درج_BITS:
        size+=dataLength*2;
        اكسر;
    حالة UTRIE2_32_درج_BITS:
        size+=dataLength*4;
        اكسر;
    افتراضي:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UTrie2Header *outTrie;

        إذا(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        بدل(درج_Bits) {
        حالة UTRIE2_16_درج_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            اكسر;
        حالة UTRIE2_32_درج_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ثابت uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            اكسر;
        افتراضي:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            أرجع 0;
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ثابت UDataSwapper *ds,
             ثابت فراغ *inData, int32_t length, فراغ *outData,
             UErrorCode *pErrorCode) {
    ثابت UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_درج_Width درج_Width;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    إذا(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    inTrie=(ثابت UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    درج_Width = (UCPTrie_درج_Width)(trie.options & UCPTRIE_OPTIONS_درج_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    إذا( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        درج_Width > UCPTRIE_درج_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        أرجع 0;
    }

    size=حجم(UCPTrieHeader)+trie.indexLength*2;
    بدل(درج_Width) {
    حالة UCPTRIE_درج_BITS_16:
        size+=dataLength*2;
        اكسر;
    حالة UCPTRIE_درج_BITS_32:
        size+=dataLength*4;
        اكسر;
    حالة UCPTRIE_درج_BITS_8:
        size+=dataLength;
        اكسر;
    افتراضي:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UCPTrieHeader *outTrie;

        إذا(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        ثابت uint16_t *inIndex=reinterpret_cast<ثابت uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        ثابت uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        بدل(درج_Width) {
        حالة UCPTRIE_درج_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            اكسر;
        حالة UCPTRIE_درج_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            اكسر;
        حالة UCPTRIE_درج_BITS_8:
            إذا(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            اكسر;
        افتراضي:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            أرجع 0;
        }
    }

    أرجع size;
}

نطاق {

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
getVersion(ثابت فراغ *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    إذا(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        أرجع 0;
    }
    signature = *static_cast<ثابت uint32_t*>(data);
    إذا(signature==UCPTRIE_SIG) {
        أرجع 3;
    }
    إذا(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        أرجع 3;
    }
    إذا(signature==UTRIE2_SIG) {
        أرجع 2;
    }
    إذا(anyEndianOk && signature==UTRIE2_OE_SIG) {
        أرجع 2;
    }
    إذا(signature==UTRIE_SIG) {
        أرجع 1;
    }
    إذا(anyEndianOk && signature==UTRIE_OE_SIG) {
        أرجع 1;
    }
    أرجع 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ثابت UDataSwapper *ds,
                     ثابت فراغ *inData, int32_t length, فراغ *outData,
                     UErrorCode *pErrorCode) {
    إذا(U_FAILURE(*pErrorCode)) { أرجع 0; }
    بدل(getVersion(inData, length, true)) {
    حالة 1:
        أرجع utrie_swap(ds, inData, length, outData, pErrorCode);
    حالة 2:
        أرجع utrie2_swap(ds, inData, length, outData, pErrorCode);
    حالة 3:
        أرجع ucptrie_swap(ds, inData, length, outData, pErrorCode);
    افتراضي:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }
}
