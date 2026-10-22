// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ucptrie_impl.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "utrie.h"
#অন্তর্ভাব "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

নামাকাশ {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(ধ্রুব UDataSwapper *ds,
           ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
           UErrorCode *pErrorCode) {
    ধ্রুব UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }
    যদি(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    /* setup and swapping */
    যদি(length>=0 && (uint32_t)length<পরিমাণ(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }

    inTrie=(ধ্রুব UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    যদি( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_গুনতি-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        প্রত্যাগম 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=পরিমাণ(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    যদি(length>=0) {
        UTrieHeader *outTrie;

        যদি(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, পরিমাণ(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        যদি(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ধ্রুব uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } অন্যথা {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    প্রত্যাগম size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ধ্রুব UDataSwapper *ds,
            ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
            UErrorCode *pErrorCode) {
    ধ্রুব UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_মান_Bits মান_Bits;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }
    যদি(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    /* setup and swapping */
    যদি(length>=0 && length<(int32_t)পরিমাণ(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }

    inTrie=(ধ্রুব UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    মান_Bits=(UTrie2_মান_Bits)(trie.options&UTRIE2_OPTIONS_মান_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    যদি( trie.signature!=UTRIE2_SIG ||
        মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        প্রত্যাগম 0;
    }

    size=পরিমাণ(UTrie2Header)+trie.indexLength*2;
    বিকল্প(মান_Bits) {
    পক্ষ UTRIE2_16_মান_BITS:
        size+=dataLength*2;
        ভঙ্গ;
    পক্ষ UTRIE2_32_মান_BITS:
        size+=dataLength*4;
        ভঙ্গ;
    স্বভাব:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম 0;
    }

    যদি(length>=0) {
        UTrie2Header *outTrie;

        যদি(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        বিকল্প(মান_Bits) {
        পক্ষ UTRIE2_16_মান_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            ভঙ্গ;
        পক্ষ UTRIE2_32_মান_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ধ্রুব uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            ভঙ্গ;
        স্বভাব:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            প্রত্যাগম 0;
        }
    }

    প্রত্যাগম size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ধ্রুব UDataSwapper *ds,
             ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
             UErrorCode *pErrorCode) {
    ধ্রুব UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_মান_Width মান_Width;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }
    যদি(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    /* setup and swapping */
    যদি(length>=0 && length<(int32_t)পরিমাণ(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }

    inTrie=(ধ্রুব UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    মান_Width = (UCPTrie_মান_Width)(trie.options & UCPTRIE_OPTIONS_মান_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    যদি( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        মান_Width > UCPTRIE_মান_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        প্রত্যাগম 0;
    }

    size=পরিমাণ(UCPTrieHeader)+trie.indexLength*2;
    বিকল্প(মান_Width) {
    পক্ষ UCPTRIE_মান_BITS_16:
        size+=dataLength*2;
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_32:
        size+=dataLength*4;
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_8:
        size+=dataLength;
        ভঙ্গ;
    স্বভাব:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম 0;
    }

    যদি(length>=0) {
        UCPTrieHeader *outTrie;

        যদি(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        ধ্রুব uint16_t *inIndex=reinterpret_cast<ধ্রুব uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        ধ্রুব uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        বিকল্প(মান_Width) {
        পক্ষ UCPTRIE_মান_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            ভঙ্গ;
        পক্ষ UCPTRIE_মান_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            ভঙ্গ;
        পক্ষ UCPTRIE_মান_BITS_8:
            যদি(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            ভঙ্গ;
        স্বভাব:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            প্রত্যাগম 0;
        }
    }

    প্রত্যাগম size;
}

নামাকাশ {

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
getVersion(ধ্রুব শূন্য *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    যদি(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        প্রত্যাগম 0;
    }
    signature = *static_cast<ধ্রুব uint32_t*>(data);
    যদি(signature==UCPTRIE_SIG) {
        প্রত্যাগম 3;
    }
    যদি(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        প্রত্যাগম 3;
    }
    যদি(signature==UTRIE2_SIG) {
        প্রত্যাগম 2;
    }
    যদি(anyEndianOk && signature==UTRIE2_OE_SIG) {
        প্রত্যাগম 2;
    }
    যদি(signature==UTRIE_SIG) {
        প্রত্যাগম 1;
    }
    যদি(anyEndianOk && signature==UTRIE_OE_SIG) {
        প্রত্যাগম 1;
    }
    প্রত্যাগম 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ধ্রুব UDataSwapper *ds,
                     ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
                     UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) { প্রত্যাগম 0; }
    বিকল্প(getVersion(inData, length, সত্য)) {
    পক্ষ 1:
        প্রত্যাগম utrie_swap(ds, inData, length, outData, pErrorCode);
    পক্ষ 2:
        প্রত্যাগম utrie2_swap(ds, inData, length, outData, pErrorCode);
    পক্ষ 3:
        প্রত্যাগম ucptrie_swap(ds, inData, length, outData, pErrorCode);
    স্বভাব:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম 0;
    }
}
