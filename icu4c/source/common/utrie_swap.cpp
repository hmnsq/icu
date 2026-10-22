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

constexpr int32_t ASCII_सीमा = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap(ध्रुव UDataSwapper *ds,
           ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
           UError_संहिता *p_Error_संहिता) {
    ध्रुव UTrie_शीर्षकम् *अन्तर्_Trie;
    UTrie_शीर्षकम् trie;
    int32_t size;
    UBool दत्त_Is32;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || (length>=0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    यदि(length>=0 && (uint32_t)length<परिमाण(UTrie_शीर्षकम्)) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_Trie=(ध्रुव UTrie_शीर्षकम् *)अन्तर्_दत्त;
    trie.signature=ds->readUInt32(अन्तर्_Trie->signature);
    trie.options=ds->readUInt32(अन्तर्_Trie->options);
    trie.सूचकांक_दीर्घता=udata_readInt32(ds, अन्तर्_Trie->सूचकांक_दीर्घता);
    trie.दत्त_दीर्घता=udata_readInt32(ds, अन्तर्_Trie->दत्त_दीर्घता);

    यदि( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_सरणम्_आवरणम्)!=UTRIE_सरणम् ||
        ((trie.options>>UTRIE_OPTIONS_सूचकांक_सरणम्)&UTRIE_OPTIONS_सरणम्_आवरणम्)!=UTRIE_सूचकांक_सरणम् ||
        trie.सूचकांक_दीर्घता<UTRIE_BMP_सूचकांक_दीर्घता ||
        (trie.सूचकांक_दीर्घता&(UTRIE_SURROGATE_खण्डम्_गणनम्-1))!=0 ||
        trie.दत्त_दीर्घता<UTRIE_दत्त_खण्डम्_दीर्घता ||
        (trie.दत्त_दीर्घता&(UTRIE_दत्त_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_रैखिकम्)!=0 && trie.दत्त_दीर्घता<(UTRIE_दत्त_खण्डम्_दीर्घता+0x100))
    ) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        प्रत्यागम 0;
    }

    दत्त_Is32 = (trie.options & UTRIE_OPTIONS_दत्त_IS_32_BIT) != 0;
    size=परिमाण(UTrie_शीर्षकम्)+trie.सूचकांक_दीर्घता*2+trie.दत्त_दीर्घता*(दत्त_Is32?4:2);

    यदि(length>=0) {
        UTrie_शीर्षकम् *बहिर्गमनम्_Trie;

        यदि(length<size) {
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        बहिर्गमनम्_Trie=(UTrie_शीर्षकम् *)बहिर्गमनम्_दत्त;

        /* swap the header */
        ds->swapArray32(ds, अन्तर्_Trie, परिमाण(UTrie_शीर्षकम्), बहिर्गमनम्_Trie, p_Error_संहिता);

        /* swap the index and the data */
        यदि(दत्त_Is32) {
            ds->swapArray16(ds, अन्तर्_Trie+1, trie.सूचकांक_दीर्घता*2, बहिर्गमनम्_Trie+1, p_Error_संहिता);
            ds->swapArray32(ds, (ध्रुव uint16_t *)(अन्तर्_Trie+1)+trie.सूचकांक_दीर्घता, trie.दत्त_दीर्घता*4,
                                     (uint16_t *)(बहिर्गमनम्_Trie+1)+trie.सूचकांक_दीर्घता, p_Error_संहिता);
        } अन्यथा {
            ds->swapArray16(ds, अन्तर्_Trie+1, (trie.सूचकांक_दीर्घता+trie.दत्त_दीर्घता)*2, बहिर्गमनम्_Trie+1, p_Error_संहिता);
        }
    }

    प्रत्यागम size;
}

U_CAPI int32_t U_EXPORT2
utrie2_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
            UError_संहिता *p_Error_संहिता) {
    ध्रुव UTrie2_शीर्षकम् *अन्तर्_Trie;
    UTrie2_शीर्षकम् trie;
    int32_t दत्त_दीर्घता, size;
    UTrie2_मानम्_द्वयङ्काः मानम्_द्वयङ्काः;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || (length>=0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    यदि(length>=0 && length<(int32_t)परिमाण(UTrie2_शीर्षकम्)) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_Trie=(ध्रुव UTrie2_शीर्षकम् *)अन्तर्_दत्त;
    trie.signature=ds->readUInt32(अन्तर्_Trie->signature);
    trie.options=ds->readUInt16(अन्तर्_Trie->options);
    trie.सूचकांक_दीर्घता=ds->readUInt16(अन्तर्_Trie->सूचकांक_दीर्घता);
    trie.shifted_दत्त_दीर्घता=ds->readUInt16(अन्तर्_Trie->shifted_दत्त_दीर्घता);

    मानम्_द्वयङ्काः=(UTrie2_मानम्_द्वयङ्काः)(trie.options&UTRIE2_OPTIONS_मानम्_द्वयङ्काः_आवरणम्);
    दत्त_दीर्घता=(int32_t)trie.shifted_दत्त_दीर्घता<<UTRIE2_सूचकांक_सरणम्;

    यदि( trie.signature!=UTRIE2_SIG ||
        मानम्_द्वयङ्काः<0 || UTRIE2_गणनम्_मानम्_द्वयङ्काः<=मानम्_द्वयङ्काः ||
        trie.सूचकांक_दीर्घता<UTRIE2_सूचकांक_1_विस्थापनम् ||
        दत्त_दीर्घता<UTRIE2_दत्त_START_विस्थापनम्
    ) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        प्रत्यागम 0;
    }

    size=परिमाण(UTrie2_शीर्षकम्)+trie.सूचकांक_दीर्घता*2;
    विकल्प(मानम्_द्वयङ्काः) {
    पक्ष UTRIE2_16_मानम्_द्वयङ्काः:
        size+=दत्त_दीर्घता*2;
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_द्वयङ्काः:
        size+=दत्त_दीर्घता*4;
        भङ्ग;
    स्वभाव:
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>=0) {
        UTrie2_शीर्षकम् *बहिर्गमनम्_Trie;

        यदि(length<size) {
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        बहिर्गमनम्_Trie=(UTrie2_शीर्षकम् *)बहिर्गमनम्_दत्त;

        /* swap the header */
        ds->swapArray32(ds, &अन्तर्_Trie->signature, 4, &बहिर्गमनम्_Trie->signature, p_Error_संहिता);
        ds->swapArray16(ds, &अन्तर्_Trie->options, 12, &बहिर्गमनम्_Trie->options, p_Error_संहिता);

        /* swap the index and the data */
        विकल्प(मानम्_द्वयङ्काः) {
        पक्ष UTRIE2_16_मानम्_द्वयङ्काः:
            ds->swapArray16(ds, अन्तर्_Trie+1, (trie.सूचकांक_दीर्घता+दत्त_दीर्घता)*2, बहिर्गमनम्_Trie+1, p_Error_संहिता);
            भङ्ग;
        पक्ष UTRIE2_32_मानम्_द्वयङ्काः:
            ds->swapArray16(ds, अन्तर्_Trie+1, trie.सूचकांक_दीर्घता*2, बहिर्गमनम्_Trie+1, p_Error_संहिता);
            ds->swapArray32(ds, (ध्रुव uint16_t *)(अन्तर्_Trie+1)+trie.सूचकांक_दीर्घता, दत्त_दीर्घता*4,
                                     (uint16_t *)(बहिर्गमनम्_Trie+1)+trie.सूचकांक_दीर्घता, p_Error_संहिता);
            भङ्ग;
        स्वभाव:
            *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
            प्रत्यागम 0;
        }
    }

    प्रत्यागम size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_swap(ध्रुव UDataSwapper *ds,
             ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
             UError_संहिता *p_Error_संहिता) {
    ध्रुव UCPTrie_शीर्षकम् *अन्तर्_Trie;
    UCPTrie_शीर्षकम् trie;
    int32_t दत्त_दीर्घता, size;
    UCPTrie_मानम्_विस्तारः मानम्_विस्तारः;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || (length>=0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    यदि(length>=0 && length<(int32_t)परिमाण(UCPTrie_शीर्षकम्)) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_Trie=(ध्रुव UCPTrie_शीर्षकम् *)अन्तर्_दत्त;
    trie.signature=ds->readUInt32(अन्तर्_Trie->signature);
    trie.options=ds->readUInt16(अन्तर्_Trie->options);
    trie.सूचकांक_दीर्घता=ds->readUInt16(अन्तर्_Trie->सूचकांक_दीर्घता);
    trie.दत्त_दीर्घता = ds->readUInt16(अन्तर्_Trie->दत्त_दीर्घता);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    मानम्_विस्तारः = (UCPTrie_मानम्_विस्तारः)(trie.options & UCPTRIE_OPTIONS_मानम्_द्वयङ्काः_आवरणम्);
    दत्त_दीर्घता = ((int32_t)(trie.options & UCPTRIE_OPTIONS_दत्त_दीर्घता_आवरणम्) << 4) | trie.दत्त_दीर्घता;

    int32_t min_सूचकांक_दीर्घता = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_सूचकांक_दीर्घता : UCPTRIE_SMALL_सूचकांक_दीर्घता;
    यदि( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_आवरणम्) != 0 ||
        मानम्_विस्तारः > UCPTRIE_मानम्_द्वयङ्काः_8 ||
        trie.सूचकांक_दीर्घता < min_सूचकांक_दीर्घता ||
        दत्त_दीर्घता < ASCII_सीमा
    ) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        प्रत्यागम 0;
    }

    size=परिमाण(UCPTrie_शीर्षकम्)+trie.सूचकांक_दीर्घता*2;
    विकल्प(मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        size+=दत्त_दीर्घता*2;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        size+=दत्त_दीर्घता*4;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        size+=दत्त_दीर्घता;
        भङ्ग;
    स्वभाव:
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>=0) {
        UCPTrie_शीर्षकम् *बहिर्गमनम्_Trie;

        यदि(length<size) {
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        बहिर्गमनम्_Trie=(UCPTrie_शीर्षकम् *)बहिर्गमनम्_दत्त;

        /* swap the header */
        ds->swapArray32(ds, &अन्तर्_Trie->signature, 4, &बहिर्गमनम्_Trie->signature, p_Error_संहिता);
        ds->swapArray16(ds, &अन्तर्_Trie->options, 12, &बहिर्गमनम्_Trie->options, p_Error_संहिता);

        /* swap the index */
        ध्रुव uint16_t *अन्तर्_सूचकांक=reinterpret_cast<ध्रुव uint16_t *>(अन्तर्_Trie+1);
        uint16_t *बहिर्गमनम्_सूचकांक=reinterpret_cast<uint16_t *>(बहिर्गमनम्_Trie+1);
        ds->swapArray16(ds, अन्तर्_सूचकांक, trie.सूचकांक_दीर्घता*2, बहिर्गमनम्_सूचकांक, p_Error_संहिता);

        /* swap the data */
        ध्रुव uint16_t *अन्तर्_दत्त=अन्तर्_सूचकांक+trie.सूचकांक_दीर्घता;
        uint16_t *बहिर्गमनम्_दत्त=बहिर्गमनम्_सूचकांक+trie.सूचकांक_दीर्घता;
        विकल्प(मानम्_विस्तारः) {
        पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
            ds->swapArray16(ds, अन्तर्_दत्त, दत्त_दीर्घता*2, बहिर्गमनम्_दत्त, p_Error_संहिता);
            भङ्ग;
        पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
            ds->swapArray32(ds, अन्तर्_दत्त, दत्त_दीर्घता*4, बहिर्गमनम्_दत्त, p_Error_संहिता);
            भङ्ग;
        पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
            यदि(अन्तर्_Trie!=बहिर्गमनम्_Trie) {
                uprv_memmove(बहिर्गमनम्_दत्त, अन्तर्_दत्त, दत्त_दीर्घता);
            }
            भङ्ग;
        स्वभाव:
            *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
            प्रत्यागम 0;
        }
    }

    प्रत्यागम size;
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
get_संस्करणम्(ध्रुव शून्य *दत्त, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    यदि(length<16 || दत्त==शून्यसूचकः || (U_POINTER_आवरणम्_LSB(दत्त, 3)!=0)) {
        प्रत्यागम 0;
    }
    signature = *static_cast<ध्रुव uint32_t*>(दत्त);
    यदि(signature==UCPTRIE_SIG) {
        प्रत्यागम 3;
    }
    यदि(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        प्रत्यागम 3;
    }
    यदि(signature==UTRIE2_SIG) {
        प्रत्यागम 2;
    }
    यदि(anyEndianOk && signature==UTRIE2_OE_SIG) {
        प्रत्यागम 2;
    }
    यदि(signature==UTRIE_SIG) {
        प्रत्यागम 1;
    }
    यदि(anyEndianOk && signature==UTRIE_OE_SIG) {
        प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_swap_Any_संस्करणम्(ध्रुव UDataSwapper *ds,
                     ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                     UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) { प्रत्यागम 0; }
    विकल्प(get_संस्करणम्(अन्तर्_दत्त, length, सत्य)) {
    पक्ष 1:
        प्रत्यागम utrie_swap(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    पक्ष 2:
        प्रत्यागम utrie2_swap(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    पक्ष 3:
        प्रत्यागम ucptrie_swap(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    स्वभाव:
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }
}
