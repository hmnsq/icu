// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश "ucptrie_impl.h"
#समावेश "udataswp.h"
#समावेश "utrie.h"
#समावेश "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

namespace {

constexpr int32_t ASCII_LIMIT = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(स्थिर UDataSwapper *ds,
           स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
           UErrorCode *pErrorCode) {
    स्थिर UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool तथ्य_Is32;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || (length>=0 && out_तथ्य==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* setup and swapping */
    अगर(length>=0 && (uint32_t)length<माप(UTrieHeader)) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    inTrie=(स्थिर UTrieHeader *)in_तथ्य;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.सूचकांक_लंबाई=udata_readInt32(ds, inTrie->सूचकांक_लंबाई);
    trie.तथ्य_लंबाई=udata_readInt32(ds, inTrie->तथ्य_लंबाई);

    अगर( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_सूचकांक_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_सूचकांक_SHIFT ||
        trie.सूचकांक_लंबाई<UTRIE_BMP_सूचकांक_लंबाई ||
        (trie.सूचकांक_लंबाई&(UTRIE_SURROGATE_BLOCK_गिनती-1))!=0 ||
        trie.तथ्य_लंबाई<UTRIE_तथ्य_BLOCK_लंबाई ||
        (trie.तथ्य_लंबाई&(UTRIE_तथ्य_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.तथ्य_लंबाई<(UTRIE_तथ्य_BLOCK_लंबाई+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        वापस 0;
    }

    तथ्य_Is32 = (trie.options & UTRIE_OPTIONS_तथ्य_IS_32_BIT) != 0;
    size=माप(UTrieHeader)+trie.सूचकांक_लंबाई*2+trie.तथ्य_लंबाई*(तथ्य_Is32?4:2);

    अगर(length>=0) {
        UTrieHeader *outTrie;

        अगर(length<size) {
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        outTrie=(UTrieHeader *)out_तथ्य;

        /* swap the header */
        ds->swapArray32(ds, inTrie, माप(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        अगर(तथ्य_Is32) {
            ds->swapArray16(ds, inTrie+1, trie.सूचकांक_लंबाई*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (स्थिर uint16_t *)(inTrie+1)+trie.सूचकांक_लंबाई, trie.तथ्य_लंबाई*4,
                                     (uint16_t *)(outTrie+1)+trie.सूचकांक_लंबाई, pErrorCode);
        } अन्यथा {
            ds->swapArray16(ds, inTrie+1, (trie.सूचकांक_लंबाई+trie.तथ्य_लंबाई)*2, outTrie+1, pErrorCode);
        }
    }

    वापस size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(स्थिर UDataSwapper *ds,
            स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
            UErrorCode *pErrorCode) {
    स्थिर UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t तथ्य_लंबाई, size;
    UTrie2_मूल्य_Bits मूल्य_Bits;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || (length>=0 && out_तथ्य==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* setup and swapping */
    अगर(length>=0 && length<(int32_t)माप(UTrie2Header)) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    inTrie=(स्थिर UTrie2Header *)in_तथ्य;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.सूचकांक_लंबाई=ds->readUInt16(inTrie->सूचकांक_लंबाई);
    trie.shifted_तथ्य_लंबाई=ds->readUInt16(inTrie->shifted_तथ्य_लंबाई);

    मूल्य_Bits=(UTrie2_मूल्य_Bits)(trie.options&UTRIE2_OPTIONS_मूल्य_BITS_MASK);
    तथ्य_लंबाई=(int32_t)trie.shifted_तथ्य_लंबाई<<UTRIE2_सूचकांक_SHIFT;

    अगर( trie.signature!=UTRIE2_SIG ||
        मूल्य_Bits<0 || UTRIE2_गिनती_मूल्य_BITS<=मूल्य_Bits ||
        trie.सूचकांक_लंबाई<UTRIE2_सूचकांक_1_OFFSET ||
        तथ्य_लंबाई<UTRIE2_तथ्य_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        वापस 0;
    }

    size=माप(UTrie2Header)+trie.सूचकांक_लंबाई*2;
    चयन(मूल्य_Bits) {
    हाल UTRIE2_16_मूल्य_BITS:
        size+=तथ्य_लंबाई*2;
        अवरोध;
    हाल UTRIE2_32_मूल्य_BITS:
        size+=तथ्य_लंबाई*4;
        अवरोध;
    शेष:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस 0;
    }

    अगर(length>=0) {
        UTrie2Header *outTrie;

        अगर(length<size) {
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        outTrie=(UTrie2Header *)out_तथ्य;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        चयन(मूल्य_Bits) {
        हाल UTRIE2_16_मूल्य_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.सूचकांक_लंबाई+तथ्य_लंबाई)*2, outTrie+1, pErrorCode);
            अवरोध;
        हाल UTRIE2_32_मूल्य_BITS:
            ds->swapArray16(ds, inTrie+1, trie.सूचकांक_लंबाई*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (स्थिर uint16_t *)(inTrie+1)+trie.सूचकांक_लंबाई, तथ्य_लंबाई*4,
                                     (uint16_t *)(outTrie+1)+trie.सूचकांक_लंबाई, pErrorCode);
            अवरोध;
        शेष:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            वापस 0;
        }
    }

    वापस size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(स्थिर UDataSwapper *ds,
             स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
             UErrorCode *pErrorCode) {
    स्थिर UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t तथ्य_लंबाई, size;
    UCPTrie_मूल्य_Width मूल्य_Width;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || (length>=0 && out_तथ्य==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* setup and swapping */
    अगर(length>=0 && length<(int32_t)माप(UCPTrieHeader)) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    inTrie=(स्थिर UCPTrieHeader *)in_तथ्य;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.सूचकांक_लंबाई=ds->readUInt16(inTrie->सूचकांक_लंबाई);
    trie.तथ्य_लंबाई = ds->readUInt16(inTrie->तथ्य_लंबाई);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    मूल्य_Width = (UCPTrie_मूल्य_Width)(trie.options & UCPTRIE_OPTIONS_मूल्य_BITS_MASK);
    तथ्य_लंबाई = ((int32_t)(trie.options & UCPTRIE_OPTIONS_तथ्य_लंबाई_MASK) << 4) | trie.तथ्य_लंबाई;

    int32_t min_सूचकांक_लंबाई = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_सूचकांक_लंबाई : UCPTRIE_SMALL_सूचकांक_लंबाई;
    अगर( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        मूल्य_Width > UCPTRIE_मूल्य_BITS_8 ||
        trie.सूचकांक_लंबाई < min_सूचकांक_लंबाई ||
        तथ्य_लंबाई < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        वापस 0;
    }

    size=माप(UCPTrieHeader)+trie.सूचकांक_लंबाई*2;
    चयन(मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        size+=तथ्य_लंबाई*2;
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        size+=तथ्य_लंबाई*4;
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        size+=तथ्य_लंबाई;
        अवरोध;
    शेष:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस 0;
    }

    अगर(length>=0) {
        UCPTrieHeader *outTrie;

        अगर(length<size) {
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        outTrie=(UCPTrieHeader *)out_तथ्य;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        स्थिर uint16_t *in_सूचकांक=reinterpret_cast<स्थिर uint16_t *>(inTrie+1);
        uint16_t *out_सूचकांक=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, in_सूचकांक, trie.सूचकांक_लंबाई*2, out_सूचकांक, pErrorCode);

        /* swap the data */
        स्थिर uint16_t *in_तथ्य=in_सूचकांक+trie.सूचकांक_लंबाई;
        uint16_t *out_तथ्य=out_सूचकांक+trie.सूचकांक_लंबाई;
        चयन(मूल्य_Width) {
        हाल UCPTRIE_मूल्य_BITS_16:
            ds->swapArray16(ds, in_तथ्य, तथ्य_लंबाई*2, out_तथ्य, pErrorCode);
            अवरोध;
        हाल UCPTRIE_मूल्य_BITS_32:
            ds->swapArray32(ds, in_तथ्य, तथ्य_लंबाई*4, out_तथ्य, pErrorCode);
            अवरोध;
        हाल UCPTRIE_मूल्य_BITS_8:
            अगर(inTrie!=outTrie) {
                uprv_memmove(out_तथ्य, in_तथ्य, तथ्य_लंबाई);
            }
            अवरोध;
        शेष:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            वापस 0;
        }
    }

    वापस size;
}

namespace {

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
getVersion(स्थिर व्योम *तथ्य, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    अगर(length<16 || तथ्य==nullptr || (U_POINTER_MASK_LSB(तथ्य, 3)!=0)) {
        वापस 0;
    }
    signature = *static_cast<स्थिर uint32_t*>(तथ्य);
    अगर(signature==UCPTRIE_SIG) {
        वापस 3;
    }
    अगर(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        वापस 3;
    }
    अगर(signature==UTRIE2_SIG) {
        वापस 2;
    }
    अगर(anyEndianOk && signature==UTRIE2_OE_SIG) {
        वापस 2;
    }
    अगर(signature==UTRIE_SIG) {
        वापस 1;
    }
    अगर(anyEndianOk && signature==UTRIE_OE_SIG) {
        वापस 1;
    }
    वापस 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(स्थिर UDataSwapper *ds,
                     स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                     UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) { वापस 0; }
    चयन(getVersion(in_तथ्य, length, true)) {
    हाल 1:
        वापस utrie_swap(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    हाल 2:
        वापस utrie2_swap(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    हाल 3:
        वापस ucptrie_swap(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    शेष:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस 0;
    }
}
