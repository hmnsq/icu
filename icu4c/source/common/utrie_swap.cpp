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
utrie_swap(ধ্রুবক UDataSwapper *ds,
           ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
           UErrorCode *pErrorCode) {
    ধ্রুবক UTrieHeader *inTrie;
    UTrieHeader trie;
    int32_t size;
    UBool তথ্য_Is32;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || (length>=0 && out_তথ্য==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* setup and swapping */
    যদি(length>=0 && (uint32_t)length<আকার(UTrieHeader)) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    inTrie=(ধ্রুবক UTrieHeader *)in_তথ্য;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt32(inTrie->options);
    trie.সূচক_দৈর্ঘ্য=udata_readInt32(ds, inTrie->সূচক_দৈর্ঘ্য);
    trie.তথ্য_দৈর্ঘ্য=udata_readInt32(ds, inTrie->তথ্য_দৈর্ঘ্য);

    যদি( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((trie.options>>UTRIE_OPTIONS_সূচক_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_সূচক_SHIFT ||
        trie.সূচক_দৈর্ঘ্য<UTRIE_BMP_সূচক_দৈর্ঘ্য ||
        (trie.সূচক_দৈর্ঘ্য&(UTRIE_SURROGATE_BLOCK_গুনতি-1))!=0 ||
        trie.তথ্য_দৈর্ঘ্য<UTRIE_তথ্য_BLOCK_দৈর্ঘ্য ||
        (trie.তথ্য_দৈর্ঘ্য&(UTRIE_তথ্য_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_LINEAR)!=0 && trie.তথ্য_দৈর্ঘ্য<(UTRIE_তথ্য_BLOCK_দৈর্ঘ্য+0x100))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ফেরত 0;
    }

    তথ্য_Is32 = (trie.options & UTRIE_OPTIONS_তথ্য_IS_32_BIT) != 0;
    size=আকার(UTrieHeader)+trie.সূচক_দৈর্ঘ্য*2+trie.তথ্য_দৈর্ঘ্য*(তথ্য_Is32?4:2);

    যদি(length>=0) {
        UTrieHeader *outTrie;

        যদি(length<size) {
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }

        outTrie=(UTrieHeader *)out_তথ্য;

        /* swap the header */
        ds->swapArray32(ds, inTrie, আকার(UTrieHeader), outTrie, pErrorCode);

        /* swap the index and the data */
        যদি(তথ্য_Is32) {
            ds->swapArray16(ds, inTrie+1, trie.সূচক_দৈর্ঘ্য*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ধ্রুবক uint16_t *)(inTrie+1)+trie.সূচক_দৈর্ঘ্য, trie.তথ্য_দৈর্ঘ্য*4,
                                     (uint16_t *)(outTrie+1)+trie.সূচক_দৈর্ঘ্য, pErrorCode);
        } অন্যথা {
            ds->swapArray16(ds, inTrie+1, (trie.সূচক_দৈর্ঘ্য+trie.তথ্য_দৈর্ঘ্য)*2, outTrie+1, pErrorCode);
        }
    }

    ফেরত size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ধ্রুবক UDataSwapper *ds,
            ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
            UErrorCode *pErrorCode) {
    ধ্রুবক UTrie2Header *inTrie;
    UTrie2Header trie;
    int32_t তথ্য_দৈর্ঘ্য, size;
    UTrie2_মান_Bits মান_Bits;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || (length>=0 && out_তথ্য==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* setup and swapping */
    যদি(length>=0 && length<(int32_t)আকার(UTrie2Header)) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    inTrie=(ধ্রুবক UTrie2Header *)in_তথ্য;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.সূচক_দৈর্ঘ্য=ds->readUInt16(inTrie->সূচক_দৈর্ঘ্য);
    trie.shifted_তথ্য_দৈর্ঘ্য=ds->readUInt16(inTrie->shifted_তথ্য_দৈর্ঘ্য);

    মান_Bits=(UTrie2_মান_Bits)(trie.options&UTRIE2_OPTIONS_মান_BITS_MASK);
    তথ্য_দৈর্ঘ্য=(int32_t)trie.shifted_তথ্য_দৈর্ঘ্য<<UTRIE2_সূচক_SHIFT;

    যদি( trie.signature!=UTRIE2_SIG ||
        মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits ||
        trie.সূচক_দৈর্ঘ্য<UTRIE2_সূচক_1_OFFSET ||
        তথ্য_দৈর্ঘ্য<UTRIE2_তথ্য_START_OFFSET
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ফেরত 0;
    }

    size=আকার(UTrie2Header)+trie.সূচক_দৈর্ঘ্য*2;
    নির্বাচন(মান_Bits) {
    ক্ষেত্র UTRIE2_16_মান_BITS:
        size+=তথ্য_দৈর্ঘ্য*2;
        বিরতি;
    ক্ষেত্র UTRIE2_32_মান_BITS:
        size+=তথ্য_দৈর্ঘ্য*4;
        বিরতি;
    পূর্বনির্ধারিত:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত 0;
    }

    যদি(length>=0) {
        UTrie2Header *outTrie;

        যদি(length<size) {
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }

        outTrie=(UTrie2Header *)out_তথ্য;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index and the data */
        নির্বাচন(মান_Bits) {
        ক্ষেত্র UTRIE2_16_মান_BITS:
            ds->swapArray16(ds, inTrie+1, (trie.সূচক_দৈর্ঘ্য+তথ্য_দৈর্ঘ্য)*2, outTrie+1, pErrorCode);
            বিরতি;
        ক্ষেত্র UTRIE2_32_মান_BITS:
            ds->swapArray16(ds, inTrie+1, trie.সূচক_দৈর্ঘ্য*2, outTrie+1, pErrorCode);
            ds->swapArray32(ds, (ধ্রুবক uint16_t *)(inTrie+1)+trie.সূচক_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য*4,
                                     (uint16_t *)(outTrie+1)+trie.সূচক_দৈর্ঘ্য, pErrorCode);
            বিরতি;
        পূর্বনির্ধারিত:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ফেরত 0;
        }
    }

    ফেরত size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ধ্রুবক UDataSwapper *ds,
             ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
             UErrorCode *pErrorCode) {
    ধ্রুবক UCPTrieHeader *inTrie;
    UCPTrieHeader trie;
    int32_t তথ্য_দৈর্ঘ্য, size;
    UCPTrie_মান_Width মান_Width;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || (length>=0 && out_তথ্য==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* setup and swapping */
    যদি(length>=0 && length<(int32_t)আকার(UCPTrieHeader)) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    inTrie=(ধ্রুবক UCPTrieHeader *)in_তথ্য;
    trie.signature=ds->readUInt32(inTrie->signature);
    trie.options=ds->readUInt16(inTrie->options);
    trie.সূচক_দৈর্ঘ্য=ds->readUInt16(inTrie->সূচক_দৈর্ঘ্য);
    trie.তথ্য_দৈর্ঘ্য = ds->readUInt16(inTrie->তথ্য_দৈর্ঘ্য);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    মান_Width = (UCPTrie_মান_Width)(trie.options & UCPTRIE_OPTIONS_মান_BITS_MASK);
    তথ্য_দৈর্ঘ্য = ((int32_t)(trie.options & UCPTRIE_OPTIONS_তথ্য_দৈর্ঘ্য_MASK) << 4) | trie.তথ্য_দৈর্ঘ্য;

    int32_t min_সূচক_দৈর্ঘ্য = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_সূচক_দৈর্ঘ্য : UCPTRIE_SMALL_সূচক_দৈর্ঘ্য;
    যদি( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0 ||
        মান_Width > UCPTRIE_মান_BITS_8 ||
        trie.সূচক_দৈর্ঘ্য < min_সূচক_দৈর্ঘ্য ||
        তথ্য_দৈর্ঘ্য < ASCII_LIMIT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        ফেরত 0;
    }

    size=আকার(UCPTrieHeader)+trie.সূচক_দৈর্ঘ্য*2;
    নির্বাচন(মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        size+=তথ্য_দৈর্ঘ্য*2;
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        size+=তথ্য_দৈর্ঘ্য*4;
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        size+=তথ্য_দৈর্ঘ্য;
        বিরতি;
    পূর্বনির্ধারিত:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত 0;
    }

    যদি(length>=0) {
        UCPTrieHeader *outTrie;

        যদি(length<size) {
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }

        outTrie=(UCPTrieHeader *)out_তথ্য;

        /* swap the header */
        ds->swapArray32(ds, &inTrie->signature, 4, &outTrie->signature, pErrorCode);
        ds->swapArray16(ds, &inTrie->options, 12, &outTrie->options, pErrorCode);

        /* swap the index */
        ধ্রুবক uint16_t *in_সূচক=reinterpret_cast<ধ্রুবক uint16_t *>(inTrie+1);
        uint16_t *out_সূচক=reinterpret_cast<uint16_t *>(outTrie+1);
        ds->swapArray16(ds, in_সূচক, trie.সূচক_দৈর্ঘ্য*2, out_সূচক, pErrorCode);

        /* swap the data */
        ধ্রুবক uint16_t *in_তথ্য=in_সূচক+trie.সূচক_দৈর্ঘ্য;
        uint16_t *out_তথ্য=out_সূচক+trie.সূচক_দৈর্ঘ্য;
        নির্বাচন(মান_Width) {
        ক্ষেত্র UCPTRIE_মান_BITS_16:
            ds->swapArray16(ds, in_তথ্য, তথ্য_দৈর্ঘ্য*2, out_তথ্য, pErrorCode);
            বিরতি;
        ক্ষেত্র UCPTRIE_মান_BITS_32:
            ds->swapArray32(ds, in_তথ্য, তথ্য_দৈর্ঘ্য*4, out_তথ্য, pErrorCode);
            বিরতি;
        ক্ষেত্র UCPTRIE_মান_BITS_8:
            যদি(inTrie!=outTrie) {
                uprv_memmove(out_তথ্য, in_তথ্য, তথ্য_দৈর্ঘ্য);
            }
            বিরতি;
        পূর্বনির্ধারিত:
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ফেরত 0;
        }
    }

    ফেরত size;
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
getVersion(ধ্রুবক শূন্য *তথ্য, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    যদি(length<16 || তথ্য==nullptr || (U_POINTER_MASK_LSB(তথ্য, 3)!=0)) {
        ফেরত 0;
    }
    signature = *static_cast<ধ্রুবক uint32_t*>(তথ্য);
    যদি(signature==UCPTRIE_SIG) {
        ফেরত 3;
    }
    যদি(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        ফেরত 3;
    }
    যদি(signature==UTRIE2_SIG) {
        ফেরত 2;
    }
    যদি(anyEndianOk && signature==UTRIE2_OE_SIG) {
        ফেরত 2;
    }
    যদি(signature==UTRIE_SIG) {
        ফেরত 1;
    }
    যদি(anyEndianOk && signature==UTRIE_OE_SIG) {
        ফেরত 1;
    }
    ফেরত 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swapAnyVersion(ধ্রুবক UDataSwapper *ds,
                     ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                     UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) { ফেরত 0; }
    নির্বাচন(getVersion(in_তথ্য, length, সত্য)) {
    ক্ষেত্র 1:
        ফেরত utrie_swap(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    ক্ষেত্র 2:
        ফেরত utrie2_swap(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    ক্ষেত্র 3:
        ফেরত ucptrie_swap(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    পূর্বনির্ধারিত:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত 0;
    }
}
