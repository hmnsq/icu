// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ucptrie_impl.h"
#ଅନ୍ତର୍ଭାଵ "udataswp.h"
#ଅନ୍ତର୍ଭାଵ "utrie.h"
#ଅନ୍ତର୍ଭାଵ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

ନାମାକାଶ {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(ଅଚଳ UDataSwapper *ds,
           ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
           UErrorCode *pErrorCode) {
    ଅଚଳ UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* setup and swapping */
    ଯଦି(length>=0 && (uint32_t)length<ଆକାର(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    inTrie=(ଅଚଳ UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    ଯଦି( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_ଗଣନା-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ଫେରନ୍ତୁ 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=ଆକାର(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    ଯଦି(length>=0) {
        UTrieHeader *outTrie;

        ଯଦି(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, ଆକାର(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        ଯଦି(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ଅଚଳ uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } ଅନ୍ୟଥା {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    ଫେରନ୍ତୁ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ଅଚଳ UDataSwapper *ds,
            ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
            UErrorCode *pErrorCode) {
    ଅଚଳ UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_ମୂଲ୍ୟ_Bits ମୂଲ୍ୟ_Bits;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* setup and swapping */
    ଯଦି(length>=0 && length<(int32_t)ଆକାର(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    inTrie=(ଅଚଳ UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    ମୂଲ୍ୟ_Bits=(UTrie2_ମୂଲ୍ୟ_Bits)(trie.options&UTRIE2_OPTIONS_ମୂଲ୍ୟ_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    ଯଦି( trie.signature!=UTRIE2_SIG ||
        ମୂଲ୍ୟ_Bits<0 || UTRIE2_ଗଣନା_ମୂଲ୍ୟ_BITS<=ମୂଲ୍ୟ_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ଫେରନ୍ତୁ 0;
    }

    size=ଆକାର(UTrie2Header)+trie.indexLength*2;
    ବାଛ(ମୂଲ୍ୟ_Bits) {
    ସ୍ଥିତି UTRIE2_16_ମୂଲ୍ୟ_BITS:
        size+=dataLength*2;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UTRIE2_32_ମୂଲ୍ୟ_BITS:
        size+=dataLength*4;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(length>=0) {
        UTrie2Header *outTrie;

        ଯଦି(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        ବାଛ(ମୂଲ୍ୟ_Bits) {
        ସ୍ଥିତି UTRIE2_16_ମୂଲ୍ୟ_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UTRIE2_32_ମୂଲ୍ୟ_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ଅଚଳ uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    ଫେରନ୍ତୁ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ଅଚଳ UDataSwapper *ds,
             ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
             UErrorCode *pErrorCode) {
    ଅଚଳ UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* setup and swapping */
    ଯଦି(length>=0 && length<(int32_t)ଆକାର(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    inTrie=(ଅଚଳ UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    ମୂଲ୍ୟ_Width = (UCPTrie_ମୂଲ୍ୟ_Width)(trie.options & UCPTRIE_OPTIONS_ମୂଲ୍ୟ_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    ଯଦି( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        ମୂଲ୍ୟ_Width > UCPTRIE_ମୂଲ୍ୟ_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        ଫେରନ୍ତୁ 0;
    }

    size=ଆକାର(UCPTrieHeader)+trie.indexLength*2;
    ବାଛ(ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        size+=dataLength*2;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        size+=dataLength*4;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        size+=dataLength;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(length>=0) {
        UCPTrieHeader *outTrie;

        ଯଦି(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        ଅଚଳ uint16_t *inIndex=reinterpret_cast<ଅଚଳ uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        ଅଚଳ uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        ବାଛ(ମୂଲ୍ୟ_Width) {
        ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
            ଯଦି(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    ଫେରନ୍ତୁ size;
}

ନାମାକାଶ {

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
getVersion(ଅଚଳ ଶୂନ୍ୟ *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    ଯଦି(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        ଫେରନ୍ତୁ 0;
    }
    signature = *static_cast<ଅଚଳ uint32_t*>(data);
    ଯଦି(signature==UCPTRIE_SIG) {
        ଫେରନ୍ତୁ 3;
    }
    ଯଦି(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        ଫେରନ୍ତୁ 3;
    }
    ଯଦି(signature==UTRIE2_SIG) {
        ଫେରନ୍ତୁ 2;
    }
    ଯଦି(anyEndianOk && signature==UTRIE2_OE_SIG) {
        ଫେରନ୍ତୁ 2;
    }
    ଯଦି(signature==UTRIE_SIG) {
        ଫେରନ୍ତୁ 1;
    }
    ଯଦି(anyEndianOk && signature==UTRIE_OE_SIG) {
        ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ଅଚଳ UDataSwapper *ds,
                     ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                     UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) { ଫେରନ୍ତୁ 0; }
    ବାଛ(getVersion(inData, length, ସତ୍ୟ)) {
    ସ୍ଥିତି 1:
        ଫେରନ୍ତୁ utrie_swap(ds, inData, length, outData, pErrorCode);
    ସ୍ଥିତି 2:
        ଫେରନ୍ତୁ utrie2_swap(ds, inData, length, outData, pErrorCode);
    ସ୍ଥିତି 3:
        ଫେରନ୍ତୁ ucptrie_swap(ds, inData, length, outData, pErrorCode);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
}
