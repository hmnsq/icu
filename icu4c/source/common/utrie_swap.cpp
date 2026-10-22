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

constexpr int32_t ASCII_حد_بندی = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_وٹاندرا(ثابت UDataSwapper *ds,
           ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
           UError_کوڈ *p_Error_کوڈ) {
    ثابت UTrie_سرخی *وچ_Trie;
    UTrie_سرخی trie;
    int32_t size;
    UBool انکڑے_Is32;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || (length>=0 && باہرلا_انکڑے==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && (uint32_t)length<حجم(UTrie_سرخی)) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    وچ_Trie=(ثابت UTrie_سرخی *)وچ_انکڑے;
    trie.signature=ds->readUInt32(وچ_Trie->signature);
    trie.options=ds->readUInt32(وچ_Trie->options);
    trie.اشاریہ_لمائی=udata_readInt32(ds, وچ_Trie->اشاریہ_لمائی);
    trie.انکڑے_لمائی=udata_readInt32(ds, وچ_Trie->انکڑے_لمائی);

    إذا( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_کھسکاؤ_نقاب)!=UTRIE_کھسکاؤ ||
        ((trie.options>>UTRIE_OPTIONS_اشاریہ_کھسکاؤ)&UTRIE_OPTIONS_کھسکاؤ_نقاب)!=UTRIE_اشاریہ_کھسکاؤ ||
        trie.اشاریہ_لمائی<UTRIE_BMP_اشاریہ_لمائی ||
        (trie.اشاریہ_لمائی&(UTRIE_SURROGATE_بلاک_گنتی-1))!=0 ||
        trie.انکڑے_لمائی<UTRIE_انکڑے_بلاک_لمائی ||
        (trie.انکڑے_لمائی&(UTRIE_انکڑے_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_خطی)!=0 && trie.انکڑے_لمائی<(UTRIE_انکڑے_بلاک_لمائی+0x100))
    ) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    انکڑے_Is32 = (trie.options & UTRIE_OPTIONS_انکڑے_IS_32_ثنائی_ہندسہ) != 0;
    size=حجم(UTrie_سرخی)+trie.اشاریہ_لمائی*2+trie.انکڑے_لمائی*(انکڑے_Is32?4:2);

    إذا(length>=0) {
        UTrie_سرخی *باہرلا_Trie;

        إذا(length<size) {
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        باہرلا_Trie=(UTrie_سرخی *)باہرلا_انکڑے;

        /* swap the header */
        ds->وٹاندرا_Array32(ds, وچ_Trie, حجم(UTrie_سرخی), باہرلا_Trie, p_Error_کوڈ);

        /* swap the index and the data */
        إذا(انکڑے_Is32) {
            ds->وٹاندرا_Array16(ds, وچ_Trie+1, trie.اشاریہ_لمائی*2, باہرلا_Trie+1, p_Error_کوڈ);
            ds->وٹاندرا_Array32(ds, (ثابت uint16_t *)(وچ_Trie+1)+trie.اشاریہ_لمائی, trie.انکڑے_لمائی*4,
                                     (uint16_t *)(باہرلا_Trie+1)+trie.اشاریہ_لمائی, p_Error_کوڈ);
        } وإلا {
            ds->وٹاندرا_Array16(ds, وچ_Trie+1, (trie.اشاریہ_لمائی+trie.انکڑے_لمائی)*2, باہرلا_Trie+1, p_Error_کوڈ);
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
utrie2_وٹاندرا(ثابت UDataSwapper *ds,
            ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
            UError_کوڈ *p_Error_کوڈ) {
    ثابت UTrie2_سرخی *وچ_Trie;
    UTrie2_سرخی trie;
    int32_t انکڑے_لمائی, size;
    UTrie2_قدر_بٹ قدر_بٹ;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || (length>=0 && باہرلا_انکڑے==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UTrie2_سرخی)) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    وچ_Trie=(ثابت UTrie2_سرخی *)وچ_انکڑے;
    trie.signature=ds->readUInt32(وچ_Trie->signature);
    trie.options=ds->readUInt16(وچ_Trie->options);
    trie.اشاریہ_لمائی=ds->readUInt16(وچ_Trie->اشاریہ_لمائی);
    trie.shifted_انکڑے_لمائی=ds->readUInt16(وچ_Trie->shifted_انکڑے_لمائی);

    قدر_بٹ=(UTrie2_قدر_بٹ)(trie.options&UTRIE2_OPTIONS_قدر_بٹ_نقاب);
    انکڑے_لمائی=(int32_t)trie.shifted_انکڑے_لمائی<<UTRIE2_اشاریہ_کھسکاؤ;

    إذا( trie.signature!=UTRIE2_SIG ||
        قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ ||
        trie.اشاریہ_لمائی<UTRIE2_اشاریہ_1_وتھ ||
        انکڑے_لمائی<UTRIE2_انکڑے_شروع_وتھ
    ) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    size=حجم(UTrie2_سرخی)+trie.اشاریہ_لمائی*2;
    بدل(قدر_بٹ) {
    حالة UTRIE2_16_قدر_بٹ:
        size+=انکڑے_لمائی*2;
        اكسر;
    حالة UTRIE2_32_قدر_بٹ:
        size+=انکڑے_لمائی*4;
        اكسر;
    افتراضي:
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UTrie2_سرخی *باہرلا_Trie;

        إذا(length<size) {
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        باہرلا_Trie=(UTrie2_سرخی *)باہرلا_انکڑے;

        /* swap the header */
        ds->وٹاندرا_Array32(ds, &وچ_Trie->signature, 4, &باہرلا_Trie->signature, p_Error_کوڈ);
        ds->وٹاندرا_Array16(ds, &وچ_Trie->options, 12, &باہرلا_Trie->options, p_Error_کوڈ);

        /* swap the index and the data */
        بدل(قدر_بٹ) {
        حالة UTRIE2_16_قدر_بٹ:
            ds->وٹاندرا_Array16(ds, وچ_Trie+1, (trie.اشاریہ_لمائی+انکڑے_لمائی)*2, باہرلا_Trie+1, p_Error_کوڈ);
            اكسر;
        حالة UTRIE2_32_قدر_بٹ:
            ds->وٹاندرا_Array16(ds, وچ_Trie+1, trie.اشاریہ_لمائی*2, باہرلا_Trie+1, p_Error_کوڈ);
            ds->وٹاندرا_Array32(ds, (ثابت uint16_t *)(وچ_Trie+1)+trie.اشاریہ_لمائی, انکڑے_لمائی*4,
                                     (uint16_t *)(باہرلا_Trie+1)+trie.اشاریہ_لمائی, p_Error_کوڈ);
            اكسر;
        افتراضي:
            *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
            أرجع 0;
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_وٹاندرا(ثابت UDataSwapper *ds,
             ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
             UError_کوڈ *p_Error_کوڈ) {
    ثابت UCPTrie_سرخی *وچ_Trie;
    UCPTrie_سرخی trie;
    int32_t انکڑے_لمائی, size;
    UCPTrie_قدر_چوڑائی قدر_چوڑائی;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || (length>=0 && باہرلا_انکڑے==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UCPTrie_سرخی)) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    وچ_Trie=(ثابت UCPTrie_سرخی *)وچ_انکڑے;
    trie.signature=ds->readUInt32(وچ_Trie->signature);
    trie.options=ds->readUInt16(وچ_Trie->options);
    trie.اشاریہ_لمائی=ds->readUInt16(وچ_Trie->اشاریہ_لمائی);
    trie.انکڑے_لمائی = ds->readUInt16(وچ_Trie->انکڑے_لمائی);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    قدر_چوڑائی = (UCPTrie_قدر_چوڑائی)(trie.options & UCPTRIE_OPTIONS_قدر_بٹ_نقاب);
    انکڑے_لمائی = ((int32_t)(trie.options & UCPTRIE_OPTIONS_انکڑے_لمائی_نقاب) << 4) | trie.انکڑے_لمائی;

    int32_t min_اشاریہ_لمائی = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_اشاریہ_لمائی : UCPTRIE_SMALL_اشاریہ_لمائی;
    إذا( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_نقاب) != 0 ||
        قدر_چوڑائی > UCPTRIE_قدر_بٹ_8 ||
        trie.اشاریہ_لمائی < min_اشاریہ_لمائی ||
        انکڑے_لمائی < ASCII_حد_بندی
    ) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        أرجع 0;
    }

    size=حجم(UCPTrie_سرخی)+trie.اشاریہ_لمائی*2;
    بدل(قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        size+=انکڑے_لمائی*2;
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        size+=انکڑے_لمائی*4;
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        size+=انکڑے_لمائی;
        اكسر;
    افتراضي:
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UCPTrie_سرخی *باہرلا_Trie;

        إذا(length<size) {
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        باہرلا_Trie=(UCPTrie_سرخی *)باہرلا_انکڑے;

        /* swap the header */
        ds->وٹاندرا_Array32(ds, &وچ_Trie->signature, 4, &باہرلا_Trie->signature, p_Error_کوڈ);
        ds->وٹاندرا_Array16(ds, &وچ_Trie->options, 12, &باہرلا_Trie->options, p_Error_کوڈ);

        /* swap the index */
        ثابت uint16_t *وچ_اشاریہ=reinterpret_cast<ثابت uint16_t *>(وچ_Trie+1);
        uint16_t *باہرلا_اشاریہ=reinterpret_cast<uint16_t *>(باہرلا_Trie+1);
        ds->وٹاندرا_Array16(ds, وچ_اشاریہ, trie.اشاریہ_لمائی*2, باہرلا_اشاریہ, p_Error_کوڈ);

        /* swap the data */
        ثابت uint16_t *وچ_انکڑے=وچ_اشاریہ+trie.اشاریہ_لمائی;
        uint16_t *باہرلا_انکڑے=باہرلا_اشاریہ+trie.اشاریہ_لمائی;
        بدل(قدر_چوڑائی) {
        حالة UCPTRIE_قدر_بٹ_16:
            ds->وٹاندرا_Array16(ds, وچ_انکڑے, انکڑے_لمائی*2, باہرلا_انکڑے, p_Error_کوڈ);
            اكسر;
        حالة UCPTRIE_قدر_بٹ_32:
            ds->وٹاندرا_Array32(ds, وچ_انکڑے, انکڑے_لمائی*4, باہرلا_انکڑے, p_Error_کوڈ);
            اكسر;
        حالة UCPTRIE_قدر_بٹ_8:
            إذا(وچ_Trie!=باہرلا_Trie) {
                uprv_memmove(باہرلا_انکڑے, وچ_انکڑے, انکڑے_لمائی);
            }
            اكسر;
        افتراضي:
            *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
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
get_نسخہ(ثابت فراغ *انکڑے, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    إذا(length<16 || انکڑے==خالی_سوچک || (U_اشارہ_گر_نقاب_LSB(انکڑے, 3)!=0)) {
        أرجع 0;
    }
    signature = *static_cast<ثابت uint32_t*>(انکڑے);
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
utrie_وٹاندرا_Any_نسخہ(ثابت UDataSwapper *ds,
                     ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                     UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) { أرجع 0; }
    بدل(get_نسخہ(وچ_انکڑے, length, سچ)) {
    حالة 1:
        أرجع utrie_وٹاندرا(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    حالة 2:
        أرجع utrie2_وٹاندرا(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    حالة 3:
        أرجع ucptrie_وٹاندرا(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    افتراضي:
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }
}
