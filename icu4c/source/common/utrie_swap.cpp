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
utrie_swap(स्थिरांक UDataSwapper *ds,
           स्थिरांक शून्य *inData, int32_t length, शून्य *outData,
           UErrorCode *pErrorCode) {
    स्थिरांक UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    जर(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* setup and swapping */
    जर(length>=0 && (uint32_t)length<आकार(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0;
    }

    inTrie=(स्थिरांक UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    जर( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_मोजणी-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        परत 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=आकार(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    जर(length>=0) {
        UTrieHeader *outTrie;

        जर(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, आकार(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        जर(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (स्थिरांक uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } अन्यथा {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    परत size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(स्थिरांक UDataSwapper *ds,
            स्थिरांक शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    स्थिरांक UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_मूल्य_Bits मूल्य_Bits;

    जर(U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    जर(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* setup and swapping */
    जर(length>=0 && length<(int32_t)आकार(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0;
    }

    inTrie=(स्थिरांक UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    मूल्य_Bits=(UTrie2_मूल्य_Bits)(trie.options&UTRIE2_OPTIONS_मूल्य_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    जर( trie.signature!=UTRIE2_SIG ||
        मूल्य_Bits<0 || UTRIE2_मोजणी_मूल्य_BITS<=मूल्य_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        परत 0;
    }

    size=आकार(UTrie2Header)+trie.indexLength*2;
    निवड(मूल्य_Bits) {
    स्थिती UTRIE2_16_मूल्य_BITS:
        size+=dataLength*2;
        थांबा;
    स्थिती UTRIE2_32_मूल्य_BITS:
        size+=dataLength*4;
        थांबा;
    पूर्वनिर्धारित:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        परत 0;
    }

    जर(length>=0) {
        UTrie2Header *outTrie;

        जर(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        निवड(मूल्य_Bits) {
        स्थिती UTRIE2_16_मूल्य_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            थांबा;
        स्थिती UTRIE2_32_मूल्य_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (स्थिरांक uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            थांबा;
        पूर्वनिर्धारित:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            परत 0;
        }
    }

    परत size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(स्थिरांक UDataSwapper *ds,
             स्थिरांक शून्य *inData, int32_t length, शून्य *outData,
             UErrorCode *pErrorCode) {
    स्थिरांक UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_मूल्य_Width मूल्य_Width;

    जर(U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    जर(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* setup and swapping */
    जर(length>=0 && length<(int32_t)आकार(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0;
    }

    inTrie=(स्थिरांक UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    मूल्य_Width = (UCPTrie_मूल्य_Width)(trie.options & UCPTRIE_OPTIONS_मूल्य_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    जर( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        मूल्य_Width > UCPTRIE_मूल्य_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        परत 0;
    }

    size=आकार(UCPTrieHeader)+trie.indexLength*2;
    निवड(मूल्य_Width) {
    स्थिती UCPTRIE_मूल्य_BITS_16:
        size+=dataLength*2;
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_32:
        size+=dataLength*4;
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_8:
        size+=dataLength;
        थांबा;
    पूर्वनिर्धारित:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        परत 0;
    }

    जर(length>=0) {
        UCPTrieHeader *outTrie;

        जर(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        स्थिरांक uint16_t *inIndex=reinterpret_cast<स्थिरांक uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        स्थिरांक uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        निवड(मूल्य_Width) {
        स्थिती UCPTRIE_मूल्य_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            थांबा;
        स्थिती UCPTRIE_मूल्य_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            थांबा;
        स्थिती UCPTRIE_मूल्य_BITS_8:
            जर(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            थांबा;
        पूर्वनिर्धारित:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            परत 0;
        }
    }

    परत size;
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
getVersion(स्थिरांक शून्य *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    जर(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        परत 0;
    }
    signature = *static_cast<स्थिरांक uint32_t*>(data);
    जर(signature==UCPTRIE_SIG) {
        परत 3;
    }
    जर(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        परत 3;
    }
    जर(signature==UTRIE2_SIG) {
        परत 2;
    }
    जर(anyEndianOk && signature==UTRIE2_OE_SIG) {
        परत 2;
    }
    जर(signature==UTRIE_SIG) {
        परत 1;
    }
    जर(anyEndianOk && signature==UTRIE_OE_SIG) {
        परत 1;
    }
    परत 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(स्थिरांक UDataSwapper *ds,
                     स्थिरांक शून्य *inData, int32_t length, शून्य *outData,
                     UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) { परत 0; }
    निवड(getVersion(inData, length, सत्य)) {
    स्थिती 1:
        परत utrie_swap(ds, inData, length, outData, pErrorCode);
    स्थिती 2:
        परत utrie2_swap(ds, inData, length, outData, pErrorCode);
    स्थिती 3:
        परत ucptrie_swap(ds, inData, length, outData, pErrorCode);
    पूर्वनिर्धारित:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        परत 0;
    }
}
