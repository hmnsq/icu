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

constexpr int32_t ASCII_حد_بندي = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_مٽا_سٽا(ثابت UDataSwapper *ds,
           ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
           UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UTrie_سرو *اندريون_Trie;
    UTrie_سرو trie;
    int32_t size;
    UBool معلومات_Is32;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || (length>=0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && (uint32_t)length<حجم(UTrie_سرو)) {
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    اندريون_Trie=(ثابت UTrie_سرو *)اندريون_معلومات;
    trie.signature=ds->readUInt32(اندريون_Trie->signature);
    trie.options=ds->readUInt32(اندريون_Trie->options);
    trie.اشاريو_ڊيگهائي=udata_readInt32(ds, اندريون_Trie->اشاريو_ڊيگهائي);
    trie.معلومات_ڊيگهائي=udata_readInt32(ds, اندريون_Trie->معلومات_ڊيگهائي);

    إذا( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_سرڪايو_نقاب)!=UTRIE_سرڪايو ||
        ((trie.options>>UTRIE_OPTIONS_اشاريو_سرڪايو)&UTRIE_OPTIONS_سرڪايو_نقاب)!=UTRIE_اشاريو_سرڪايو ||
        trie.اشاريو_ڊيگهائي<UTRIE_BMP_اشاريو_ڊيگهائي ||
        (trie.اشاريو_ڊيگهائي&(UTRIE_SURROGATE_بلاڪ_ڳڻپ-1))!=0 ||
        trie.معلومات_ڊيگهائي<UTRIE_معلومات_بلاڪ_ڊيگهائي ||
        (trie.معلومات_ڊيگهائي&(UTRIE_معلومات_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_لڪيري)!=0 && trie.معلومات_ڊيگهائي<(UTRIE_معلومات_بلاڪ_ڊيگهائي+0x100))
    ) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    معلومات_Is32 = (trie.options & UTRIE_OPTIONS_معلومات_IS_32_ٻٽو_انگ) != 0;
    size=حجم(UTrie_سرو)+trie.اشاريو_ڊيگهائي*2+trie.معلومات_ڊيگهائي*(معلومات_Is32?4:2);

    إذا(length>=0) {
        UTrie_سرو *ٻاهريون_Trie;

        إذا(length<size) {
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        ٻاهريون_Trie=(UTrie_سرو *)ٻاهريون_معلومات;

        /* swap the header */
        ds->مٽا_سٽا_Array32(ds, اندريون_Trie, حجم(UTrie_سرو), ٻاهريون_Trie, p_Error_ڪوڊ);

        /* swap the index and the data */
        إذا(معلومات_Is32) {
            ds->مٽا_سٽا_Array16(ds, اندريون_Trie+1, trie.اشاريو_ڊيگهائي*2, ٻاهريون_Trie+1, p_Error_ڪوڊ);
            ds->مٽا_سٽا_Array32(ds, (ثابت uint16_t *)(اندريون_Trie+1)+trie.اشاريو_ڊيگهائي, trie.معلومات_ڊيگهائي*4,
                                     (uint16_t *)(ٻاهريون_Trie+1)+trie.اشاريو_ڊيگهائي, p_Error_ڪوڊ);
        } وإلا {
            ds->مٽا_سٽا_Array16(ds, اندريون_Trie+1, (trie.اشاريو_ڊيگهائي+trie.معلومات_ڊيگهائي)*2, ٻاهريون_Trie+1, p_Error_ڪوڊ);
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
utrie2_مٽا_سٽا(ثابت UDataSwapper *ds,
            ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
            UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UTrie2_سرو *اندريون_Trie;
    UTrie2_سرو trie;
    int32_t معلومات_ڊيگهائي, size;
    UTrie2_قدر_بٽ قدر_بٽ;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || (length>=0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UTrie2_سرو)) {
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    اندريون_Trie=(ثابت UTrie2_سرو *)اندريون_معلومات;
    trie.signature=ds->readUInt32(اندريون_Trie->signature);
    trie.options=ds->readUInt16(اندريون_Trie->options);
    trie.اشاريو_ڊيگهائي=ds->readUInt16(اندريون_Trie->اشاريو_ڊيگهائي);
    trie.shifted_معلومات_ڊيگهائي=ds->readUInt16(اندريون_Trie->shifted_معلومات_ڊيگهائي);

    قدر_بٽ=(UTrie2_قدر_بٽ)(trie.options&UTRIE2_OPTIONS_قدر_بٽ_نقاب);
    معلومات_ڊيگهائي=(int32_t)trie.shifted_معلومات_ڊيگهائي<<UTRIE2_اشاريو_سرڪايو;

    إذا( trie.signature!=UTRIE2_SIG ||
        قدر_بٽ<0 || UTRIE2_ڳڻپ_قدر_بٽ<=قدر_بٽ ||
        trie.اشاريو_ڊيگهائي<UTRIE2_اشاريو_1_وٿي ||
        معلومات_ڊيگهائي<UTRIE2_معلومات_شروعات_وٿي
    ) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    size=حجم(UTrie2_سرو)+trie.اشاريو_ڊيگهائي*2;
    بدل(قدر_بٽ) {
    حالة UTRIE2_16_قدر_بٽ:
        size+=معلومات_ڊيگهائي*2;
        اكسر;
    حالة UTRIE2_32_قدر_بٽ:
        size+=معلومات_ڊيگهائي*4;
        اكسر;
    افتراضي:
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UTrie2_سرو *ٻاهريون_Trie;

        إذا(length<size) {
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        ٻاهريون_Trie=(UTrie2_سرو *)ٻاهريون_معلومات;

        /* swap the header */
        ds->مٽا_سٽا_Array32(ds, &اندريون_Trie->signature, 4, &ٻاهريون_Trie->signature, p_Error_ڪوڊ);
        ds->مٽا_سٽا_Array16(ds, &اندريون_Trie->options, 12, &ٻاهريون_Trie->options, p_Error_ڪوڊ);

        /* swap the index and the data */
        بدل(قدر_بٽ) {
        حالة UTRIE2_16_قدر_بٽ:
            ds->مٽا_سٽا_Array16(ds, اندريون_Trie+1, (trie.اشاريو_ڊيگهائي+معلومات_ڊيگهائي)*2, ٻاهريون_Trie+1, p_Error_ڪوڊ);
            اكسر;
        حالة UTRIE2_32_قدر_بٽ:
            ds->مٽا_سٽا_Array16(ds, اندريون_Trie+1, trie.اشاريو_ڊيگهائي*2, ٻاهريون_Trie+1, p_Error_ڪوڊ);
            ds->مٽا_سٽا_Array32(ds, (ثابت uint16_t *)(اندريون_Trie+1)+trie.اشاريو_ڊيگهائي, معلومات_ڊيگهائي*4,
                                     (uint16_t *)(ٻاهريون_Trie+1)+trie.اشاريو_ڊيگهائي, p_Error_ڪوڊ);
            اكسر;
        افتراضي:
            *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
            أرجع 0;
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_مٽا_سٽا(ثابت UDataSwapper *ds,
             ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
             UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UCPTrie_سرو *اندريون_Trie;
    UCPTrie_سرو trie;
    int32_t معلومات_ڊيگهائي, size;
    UCPTrie_قدر_ويڪر قدر_ويڪر;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || (length>=0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UCPTrie_سرو)) {
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    اندريون_Trie=(ثابت UCPTrie_سرو *)اندريون_معلومات;
    trie.signature=ds->readUInt32(اندريون_Trie->signature);
    trie.options=ds->readUInt16(اندريون_Trie->options);
    trie.اشاريو_ڊيگهائي=ds->readUInt16(اندريون_Trie->اشاريو_ڊيگهائي);
    trie.معلومات_ڊيگهائي = ds->readUInt16(اندريون_Trie->معلومات_ڊيگهائي);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    قدر_ويڪر = (UCPTrie_قدر_ويڪر)(trie.options & UCPTRIE_OPTIONS_قدر_بٽ_نقاب);
    معلومات_ڊيگهائي = ((int32_t)(trie.options & UCPTRIE_OPTIONS_معلومات_ڊيگهائي_نقاب) << 4) | trie.معلومات_ڊيگهائي;

    int32_t min_اشاريو_ڊيگهائي = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_اشاريو_ڊيگهائي : UCPTRIE_SMALL_اشاريو_ڊيگهائي;
    إذا( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_نقاب) != 0 ||
        قدر_ويڪر > UCPTRIE_قدر_بٽ_8 ||
        trie.اشاريو_ڊيگهائي < min_اشاريو_ڊيگهائي ||
        معلومات_ڊيگهائي < ASCII_حد_بندي
    ) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        أرجع 0;
    }

    size=حجم(UCPTrie_سرو)+trie.اشاريو_ڊيگهائي*2;
    بدل(قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        size+=معلومات_ڊيگهائي*2;
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        size+=معلومات_ڊيگهائي*4;
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        size+=معلومات_ڊيگهائي;
        اكسر;
    افتراضي:
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UCPTrie_سرو *ٻاهريون_Trie;

        إذا(length<size) {
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        ٻاهريون_Trie=(UCPTrie_سرو *)ٻاهريون_معلومات;

        /* swap the header */
        ds->مٽا_سٽا_Array32(ds, &اندريون_Trie->signature, 4, &ٻاهريون_Trie->signature, p_Error_ڪوڊ);
        ds->مٽا_سٽا_Array16(ds, &اندريون_Trie->options, 12, &ٻاهريون_Trie->options, p_Error_ڪوڊ);

        /* swap the index */
        ثابت uint16_t *اندريون_اشاريو=reinterpret_cast<ثابت uint16_t *>(اندريون_Trie+1);
        uint16_t *ٻاهريون_اشاريو=reinterpret_cast<uint16_t *>(ٻاهريون_Trie+1);
        ds->مٽا_سٽا_Array16(ds, اندريون_اشاريو, trie.اشاريو_ڊيگهائي*2, ٻاهريون_اشاريو, p_Error_ڪوڊ);

        /* swap the data */
        ثابت uint16_t *اندريون_معلومات=اندريون_اشاريو+trie.اشاريو_ڊيگهائي;
        uint16_t *ٻاهريون_معلومات=ٻاهريون_اشاريو+trie.اشاريو_ڊيگهائي;
        بدل(قدر_ويڪر) {
        حالة UCPTRIE_قدر_بٽ_16:
            ds->مٽا_سٽا_Array16(ds, اندريون_معلومات, معلومات_ڊيگهائي*2, ٻاهريون_معلومات, p_Error_ڪوڊ);
            اكسر;
        حالة UCPTRIE_قدر_بٽ_32:
            ds->مٽا_سٽا_Array32(ds, اندريون_معلومات, معلومات_ڊيگهائي*4, ٻاهريون_معلومات, p_Error_ڪوڊ);
            اكسر;
        حالة UCPTRIE_قدر_بٽ_8:
            إذا(اندريون_Trie!=ٻاهريون_Trie) {
                uprv_memmove(ٻاهريون_معلومات, اندريون_معلومات, معلومات_ڊيگهائي);
            }
            اكسر;
        افتراضي:
            *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
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
get_نسخو(ثابت فراغ *معلومات, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    إذا(length<16 || معلومات==خالي_اشارو || (U_اشارو_نقاب_LSB(معلومات, 3)!=0)) {
        أرجع 0;
    }
    signature = *static_cast<ثابت uint32_t*>(معلومات);
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
utrie_مٽا_سٽا_Any_نسخو(ثابت UDataSwapper *ds,
                     ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) { أرجع 0; }
    بدل(get_نسخو(اندريون_معلومات, length, سچ)) {
    حالة 1:
        أرجع utrie_مٽا_سٽا(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    حالة 2:
        أرجع utrie2_مٽا_سٽا(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    حالة 3:
        أرجع ucptrie_مٽا_سٽا(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    افتراضي:
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }
}
