// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ucptrie_impl.h"
#અન્તર્ભાવ "udataswp.h"
#અન્તર્ભાવ "utrie.h"
#અન્તર્ભાવ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

નામાકાશ {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(અચલ UDataSwapper *ds,
           અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
           UErrorCode *pErrorCode) {
    અચલ UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool dataIs32;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* setup and swapping */
    જો(length>=0 && (uint32_t)length<કદ(UTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    inTrie=(અચલ UTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.indexLength=udata_readInt32(ds, inTrie->indexLength);
    trie.dataLength=udata_readInt32(ds, inTrie->dataLength);

    જો( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT ||
        trie.indexLength<UTRIE_BMP_INDEX_LENGTH ||
        (trie.indexLength&(UTRIE_SURROGATE_BLOCK_ગણતરી-1))!=0 ||
        trie.dataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (trie.dataLength&(UTRIE_DATA_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.dataLength<(UTRIE_DATA_BLOCK_LENGTH+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        પાછા 0;
    }

    dataIs32 = (trie.options & UTRIE_OPTIONS_DATA_IS_32_BIT) != 0;
    size=કદ(UTrieHeader)+trie.indexLength*2+trie.dataLength*(dataIs32?4:2);

    જો(length>=0) {
        UTrieHeader *outTrie;

        જો(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }

        outTrie=(UTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, inTrie, કદ(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        જો(dataIs32) {
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (અચલ uint16_t *)(inTrie+1)+trie.indexLength, trie.dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
        } નહીંતર {
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+trie.dataLength)*2, outTrie+1, pErrorCode);
        }
    }

    પાછા size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(અચલ UDataSwapper *ds,
            અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
            UErrorCode *pErrorCode) {
    અચલ UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t dataLength, size;
    UTrie2_કિંમત_Bits કિંમત_Bits;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* setup and swapping */
    જો(length>=0 && length<(int32_t)કદ(UTrie2Header)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    inTrie=(અચલ UTrie2Header *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.shiftedDataLength=ds->readUInt16(inTrie->shiftedDataLength);

    કિંમત_Bits=(UTrie2_કિંમત_Bits)(trie.options&UTRIE2_OPTIONS_કિંમત_BITS_MASK);
    dataLength=(int32_t)trie.shiftedDataLength<<UTRIE2_INDEX_SHIFT;

    જો( trie.signature!=UTRIE2_SIG ||
        કિંમત_Bits<0 || UTRIE2_ગણતરી_કિંમત_BITS<=કિંમત_Bits ||
        trie.indexLength<UTRIE2_INDEX_1_OFFSET ||
        dataLength<UTRIE2_DATA_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        પાછા 0;
    }

    size=કદ(UTrie2Header)+trie.indexLength*2;
    પસંદગી(કિંમત_Bits) {
    સ્થિતિ UTRIE2_16_કિંમત_BITS:
        size+=dataLength*2;
        તોડો;
    સ્થિતિ UTRIE2_32_કિંમત_BITS:
        size+=dataLength*4;
        તોડો;
    મૂળભૂત:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા 0;
    }

    જો(length>=0) {
        UTrie2Header *outTrie;

        જો(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }

        outTrie=(UTrie2Header *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        પસંદગી(કિંમત_Bits) {
        સ્થિતિ UTRIE2_16_કિંમત_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.indexLength+dataLength)*2, outTrie+1, pErrorCode);
            તોડો;
        સ્થિતિ UTRIE2_32_કિંમત_BITS:
            ds->swapArray16(ds, inTrie+1, trie.indexLength*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (અચલ uint16_t *)(inTrie+1)+trie.indexLength, dataLength*4,
                                     (uint16_t *)(outTrie+1)+trie.indexLength, pErrorCode);
            તોડો;
        મૂળભૂત:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            પાછા 0;
        }
    }

    પાછા size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(અચલ UDataSwapper *ds,
             અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
             UErrorCode *pErrorCode) {
    અચલ UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t dataLength, size;
    UCPTrie_કિંમત_Width કિંમત_Width;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || (length>=0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* setup and swapping */
    જો(length>=0 && length<(int32_t)કદ(UCPTrieHeader)) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    inTrie=(અચલ UCPTrieHeader *)inData;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.indexLength=ds->readUInt16(inTrie->indexLength);
    trie.dataLength = ds->readUInt16(inTrie->dataLength);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    કિંમત_Width = (UCPTrie_કિંમત_Width)(trie.options & UCPTRIE_OPTIONS_કિંમત_BITS_MASK);
    dataLength = ((int32_t)(trie.options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | trie.dataLength;

    int32_t minIndexLength = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
    જો( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        કિંમત_Width > UCPTRIE_કિંમત_BITS_8 ||
        trie.indexLength < minIndexLength ||
        dataLength < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        પાછા 0;
    }

    size=કદ(UCPTrieHeader)+trie.indexLength*2;
    પસંદગી(કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        size+=dataLength*2;
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        size+=dataLength*4;
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        size+=dataLength;
        તોડો;
    મૂળભૂત:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા 0;
    }

    જો(length>=0) {
        UCPTrieHeader *outTrie;

        જો(length<size) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }

        outTrie=(UCPTrieHeader *)outData;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        અચલ uint16_t *inIndex=reinterpret_cast<અચલ uint16_t *>(inTrie+1);
        uint16_t *outIndex=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, inIndex, trie.indexLength*2, outIndex, pErrorCode);

        /* swap the data */
        અચલ uint16_t *inData=inIndex+trie.indexLength;
        uint16_t *outData=outIndex+trie.indexLength;
        પસંદગી(કિંમત_Width) {
        સ્થિતિ UCPTRIE_કિંમત_BITS_16:
            ds->swapArray16(ds, inData, dataLength*2, outData, pErrorCode);
            તોડો;
        સ્થિતિ UCPTRIE_કિંમત_BITS_32:
            ds->swapArray32(ds, inData, dataLength*4, outData, pErrorCode);
            તોડો;
        સ્થિતિ UCPTRIE_કિંમત_BITS_8:
            જો(inTrie!=outTrie) {
                uprv_memmove(outData, inData, dataLength);
            }
            તોડો;
        મૂળભૂત:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            પાછા 0;
        }
    }

    પાછા size;
}

નામાકાશ {

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
getVersion(અચલ શૂન્ય *data, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    જો(length<16 || data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)) {
        પાછા 0;
    }
    signature = *static_cast<અચલ uint32_t*>(data);
    જો(signature==UCPTRIE_SIG) {
        પાછા 3;
    }
    જો(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        પાછા 3;
    }
    જો(signature==UTRIE2_SIG) {
        પાછા 2;
    }
    જો(anyEndianOk && signature==UTRIE2_OE_SIG) {
        પાછા 2;
    }
    જો(signature==UTRIE_SIG) {
        પાછા 1;
    }
    જો(anyEndianOk && signature==UTRIE_OE_SIG) {
        પાછા 1;
    }
    પાછા 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(અચલ UDataSwapper *ds,
                     અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                     UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) { પાછા 0; }
    પસંદગી(getVersion(inData, length, સાચું)) {
    સ્થિતિ 1:
        પાછા utrie_swap(ds, inData, length, outData, pErrorCode);
    સ્થિતિ 2:
        પાછા utrie2_swap(ds, inData, length, outData, pErrorCode);
    સ્થિતિ 3:
        પાછા ucptrie_swap(ds, inData, length, outData, pErrorCode);
    મૂળભૂત:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા 0;
    }
}
