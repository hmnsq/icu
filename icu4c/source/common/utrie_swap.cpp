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

constexpr int32_t ASCII_حد = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_جابجا(ثابت UDataSwapper *ds,
           ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
           UError_کد *p_Error_کد) {
    ثابت UTrie_سرآیند *درون_Trie;
    UTrie_سرآیند trie;
    int32_t size;
    UBool داده_Is32;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || (length>=0 && بیرون_داده==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && (uint32_t)length<حجم(UTrie_سرآیند)) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    درون_Trie=(ثابت UTrie_سرآیند *)درون_داده;
    trie.signature=ds->readUInt32(درون_Trie->signature);
    trie.options=ds->readUInt32(درون_Trie->options);
    trie.شاخص_درازی=udata_readInt32(ds, درون_Trie->شاخص_درازی);
    trie.داده_درازی=udata_readInt32(ds, درون_Trie->داده_درازی);

    إذا( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_لغزش_ماسک)!=UTRIE_لغزش ||
        ((trie.options>>UTRIE_OPTIONS_شاخص_لغزش)&UTRIE_OPTIONS_لغزش_ماسک)!=UTRIE_شاخص_لغزش ||
        trie.شاخص_درازی<UTRIE_BMP_شاخص_درازی ||
        (trie.شاخص_درازی&(UTRIE_SURROGATE_بلوک_شمار-1))!=0 ||
        trie.داده_درازی<UTRIE_داده_بلوک_درازی ||
        (trie.داده_درازی&(UTRIE_داده_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_خطی)!=0 && trie.داده_درازی<(UTRIE_داده_بلوک_درازی+0x100))
    ) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    داده_Is32 = (trie.options & UTRIE_OPTIONS_داده_IS_32_بیت) != 0;
    size=حجم(UTrie_سرآیند)+trie.شاخص_درازی*2+trie.داده_درازی*(داده_Is32?4:2);

    إذا(length>=0) {
        UTrie_سرآیند *بیرون_Trie;

        إذا(length<size) {
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بیرون_Trie=(UTrie_سرآیند *)بیرون_داده;

        /* swap the header */
        ds->جابجا_Array32(ds, درون_Trie, حجم(UTrie_سرآیند), بیرون_Trie, p_Error_کد);

        /* swap the index and the data */
        إذا(داده_Is32) {
            ds->جابجا_Array16(ds, درون_Trie+1, trie.شاخص_درازی*2, بیرون_Trie+1, p_Error_کد);
            ds->جابجا_Array32(ds, (ثابت uint16_t *)(درون_Trie+1)+trie.شاخص_درازی, trie.داده_درازی*4,
                                     (uint16_t *)(بیرون_Trie+1)+trie.شاخص_درازی, p_Error_کد);
        } وإلا {
            ds->جابجا_Array16(ds, درون_Trie+1, (trie.شاخص_درازی+trie.داده_درازی)*2, بیرون_Trie+1, p_Error_کد);
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
utrie2_جابجا(ثابت UDataSwapper *ds,
            ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
            UError_کد *p_Error_کد) {
    ثابت UTrie2_سرآیند *درون_Trie;
    UTrie2_سرآیند trie;
    int32_t داده_درازی, size;
    UTrie2_مقدار_بیتها مقدار_بیتها;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || (length>=0 && بیرون_داده==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UTrie2_سرآیند)) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    درون_Trie=(ثابت UTrie2_سرآیند *)درون_داده;
    trie.signature=ds->readUInt32(درون_Trie->signature);
    trie.options=ds->readUInt16(درون_Trie->options);
    trie.شاخص_درازی=ds->readUInt16(درون_Trie->شاخص_درازی);
    trie.shifted_داده_درازی=ds->readUInt16(درون_Trie->shifted_داده_درازی);

    مقدار_بیتها=(UTrie2_مقدار_بیتها)(trie.options&UTRIE2_OPTIONS_مقدار_بیتها_ماسک);
    داده_درازی=(int32_t)trie.shifted_داده_درازی<<UTRIE2_شاخص_لغزش;

    إذا( trie.signature!=UTRIE2_SIG ||
        مقدار_بیتها<0 || UTRIE2_شمار_مقدار_بیتها<=مقدار_بیتها ||
        trie.شاخص_درازی<UTRIE2_شاخص_1_جابه_جایی ||
        داده_درازی<UTRIE2_داده_شروع_جابه_جایی
    ) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    size=حجم(UTrie2_سرآیند)+trie.شاخص_درازی*2;
    بدل(مقدار_بیتها) {
    حالة UTRIE2_16_مقدار_بیتها:
        size+=داده_درازی*2;
        اكسر;
    حالة UTRIE2_32_مقدار_بیتها:
        size+=داده_درازی*4;
        اكسر;
    افتراضي:
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UTrie2_سرآیند *بیرون_Trie;

        إذا(length<size) {
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بیرون_Trie=(UTrie2_سرآیند *)بیرون_داده;

        /* swap the header */
        ds->جابجا_Array32(ds, &درون_Trie->signature, 4, &بیرون_Trie->signature, p_Error_کد);
        ds->جابجا_Array16(ds, &درون_Trie->options, 12, &بیرون_Trie->options, p_Error_کد);

        /* swap the index and the data */
        بدل(مقدار_بیتها) {
        حالة UTRIE2_16_مقدار_بیتها:
            ds->جابجا_Array16(ds, درون_Trie+1, (trie.شاخص_درازی+داده_درازی)*2, بیرون_Trie+1, p_Error_کد);
            اكسر;
        حالة UTRIE2_32_مقدار_بیتها:
            ds->جابجا_Array16(ds, درون_Trie+1, trie.شاخص_درازی*2, بیرون_Trie+1, p_Error_کد);
            ds->جابجا_Array32(ds, (ثابت uint16_t *)(درون_Trie+1)+trie.شاخص_درازی, داده_درازی*4,
                                     (uint16_t *)(بیرون_Trie+1)+trie.شاخص_درازی, p_Error_کد);
            اكسر;
        افتراضي:
            *p_Error_کد=U_INVALID_FORMAT_ERROR;
            أرجع 0;
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_جابجا(ثابت UDataSwapper *ds,
             ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
             UError_کد *p_Error_کد) {
    ثابت UCPTrie_سرآیند *درون_Trie;
    UCPTrie_سرآیند trie;
    int32_t داده_درازی, size;
    UCPTrie_مقدار_عرض مقدار_عرض;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || (length>=0 && بیرون_داده==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UCPTrie_سرآیند)) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    درون_Trie=(ثابت UCPTrie_سرآیند *)درون_داده;
    trie.signature=ds->readUInt32(درون_Trie->signature);
    trie.options=ds->readUInt16(درون_Trie->options);
    trie.شاخص_درازی=ds->readUInt16(درون_Trie->شاخص_درازی);
    trie.داده_درازی = ds->readUInt16(درون_Trie->داده_درازی);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    مقدار_عرض = (UCPTrie_مقدار_عرض)(trie.options & UCPTRIE_OPTIONS_مقدار_بیتها_ماسک);
    داده_درازی = ((int32_t)(trie.options & UCPTRIE_OPTIONS_داده_درازی_ماسک) << 4) | trie.داده_درازی;

    int32_t min_شاخص_درازی = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_شاخص_درازی : UCPTRIE_SMALL_شاخص_درازی;
    إذا( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_ماسک) != 0 ||
        مقدار_عرض > UCPTRIE_مقدار_بیتها_8 ||
        trie.شاخص_درازی < min_شاخص_درازی ||
        داده_درازی < ASCII_حد
    ) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        أرجع 0;
    }

    size=حجم(UCPTrie_سرآیند)+trie.شاخص_درازی*2;
    بدل(مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        size+=داده_درازی*2;
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        size+=داده_درازی*4;
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        size+=داده_درازی;
        اكسر;
    افتراضي:
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UCPTrie_سرآیند *بیرون_Trie;

        إذا(length<size) {
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بیرون_Trie=(UCPTrie_سرآیند *)بیرون_داده;

        /* swap the header */
        ds->جابجا_Array32(ds, &درون_Trie->signature, 4, &بیرون_Trie->signature, p_Error_کد);
        ds->جابجا_Array16(ds, &درون_Trie->options, 12, &بیرون_Trie->options, p_Error_کد);

        /* swap the index */
        ثابت uint16_t *درون_شاخص=reinterpret_cast<ثابت uint16_t *>(درون_Trie+1);
        uint16_t *بیرون_شاخص=reinterpret_cast<uint16_t *>(بیرون_Trie+1);
        ds->جابجا_Array16(ds, درون_شاخص, trie.شاخص_درازی*2, بیرون_شاخص, p_Error_کد);

        /* swap the data */
        ثابت uint16_t *درون_داده=درون_شاخص+trie.شاخص_درازی;
        uint16_t *بیرون_داده=بیرون_شاخص+trie.شاخص_درازی;
        بدل(مقدار_عرض) {
        حالة UCPTRIE_مقدار_بیتها_16:
            ds->جابجا_Array16(ds, درون_داده, داده_درازی*2, بیرون_داده, p_Error_کد);
            اكسر;
        حالة UCPTRIE_مقدار_بیتها_32:
            ds->جابجا_Array32(ds, درون_داده, داده_درازی*4, بیرون_داده, p_Error_کد);
            اكسر;
        حالة UCPTRIE_مقدار_بیتها_8:
            إذا(درون_Trie!=بیرون_Trie) {
                uprv_memmove(بیرون_داده, درون_داده, داده_درازی);
            }
            اكسر;
        افتراضي:
            *p_Error_کد=U_INVALID_FORMAT_ERROR;
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
get_نسخه(ثابت فراغ *داده, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    إذا(length<16 || داده==اشاره_تهی || (U_اشاره_گر_ماسک_LSB(داده, 3)!=0)) {
        أرجع 0;
    }
    signature = *static_cast<ثابت uint32_t*>(داده);
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
utrie_جابجا_Any_نسخه(ثابت UDataSwapper *ds,
                     ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                     UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) { أرجع 0; }
    بدل(get_نسخه(درون_داده, length, درست)) {
    حالة 1:
        أرجع utrie_جابجا(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    حالة 2:
        أرجع utrie2_جابجا(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    حالة 3:
        أرجع ucptrie_جابجا(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    افتراضي:
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }
}
