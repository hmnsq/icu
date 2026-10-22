// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "ucptrie_impl.h"
#شامل "udataswp.h"
#شامل "utrie.h"
#شامل "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

نامخلا {

constexpr int32_t ASCII_حد_بندی = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_تبادلہ(ثابت UDataSwapper *ds,
           ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
           UError_رمز *p_Error_رمز) {
    ثابت UTrie_سرخی *اندر_Trie;
    UTrie_سرخی trie;
    int32_t size;
    UBool ڈیٹا_Is32;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || (length>=0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* setup and swapping */
    اگر(length>=0 && (uint32_t)length<حجم(UTrie_سرخی)) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اندر_Trie=(ثابت UTrie_سرخی *)اندر_ڈیٹا;
    trie.signature=ds->readUInt32(اندر_Trie->signature);
    trie.options=ds->readUInt32(اندر_Trie->options);
    trie.اشاریہ_لمبائی=udata_readInt32(ds, اندر_Trie->اشاریہ_لمبائی);
    trie.ڈیٹا_لمبائی=udata_readInt32(ds, اندر_Trie->ڈیٹا_لمبائی);

    اگر( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_سرکاؤ_نقاب)!=UTRIE_سرکاؤ ||
        ((trie.options>>UTRIE_OPTIONS_اشاریہ_سرکاؤ)&UTRIE_OPTIONS_سرکاؤ_نقاب)!=UTRIE_اشاریہ_سرکاؤ ||
        trie.اشاریہ_لمبائی<UTRIE_BMP_اشاریہ_لمبائی ||
        (trie.اشاریہ_لمبائی&(UTRIE_SURROGATE_بلاک_گنتی-1))!=0 ||
        trie.ڈیٹا_لمبائی<UTRIE_ڈیٹا_بلاک_لمبائی ||
        (trie.ڈیٹا_لمبائی&(UTRIE_ڈیٹا_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_خطی)!=0 && trie.ڈیٹا_لمبائی<(UTRIE_ڈیٹا_بلاک_لمبائی+0x100))
    ) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        واپس 0;
    }

    ڈیٹا_Is32 = (trie.options & UTRIE_OPTIONS_ڈیٹا_IS_32_ثنائی_ہندسہ) != 0;
    size=حجم(UTrie_سرخی)+trie.اشاریہ_لمبائی*2+trie.ڈیٹا_لمبائی*(ڈیٹا_Is32?4:2);

    اگر(length>=0) {
        UTrie_سرخی *برآمد_Trie;

        اگر(length<size) {
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        برآمد_Trie=(UTrie_سرخی *)برآمد_ڈیٹا;

        /* swap the header */
        ds->تبادلہ_Array32(ds, اندر_Trie, حجم(UTrie_سرخی), برآمد_Trie, p_Error_رمز);

        /* swap the index and the data */
        اگر(ڈیٹا_Is32) {
            ds->تبادلہ_Array16(ds, اندر_Trie+1, trie.اشاریہ_لمبائی*2, برآمد_Trie+1, p_Error_رمز);
            ds->تبادلہ_Array32(ds, (ثابت uint16_t *)(اندر_Trie+1)+trie.اشاریہ_لمبائی, trie.ڈیٹا_لمبائی*4,
                                     (uint16_t *)(برآمد_Trie+1)+trie.اشاریہ_لمبائی, p_Error_رمز);
        } ورنہ {
            ds->تبادلہ_Array16(ds, اندر_Trie+1, (trie.اشاریہ_لمبائی+trie.ڈیٹا_لمبائی)*2, برآمد_Trie+1, p_Error_رمز);
        }
    }

    واپس size;
}

U_CAPI int32_t U_EXPORT2
utrie2_تبادلہ(ثابت UDataSwapper *ds,
            ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
            UError_رمز *p_Error_رمز) {
    ثابت UTrie2_سرخی *اندر_Trie;
    UTrie2_سرخی trie;
    int32_t ڈیٹا_لمبائی, size;
    UTrie2_قدر_بٹ قدر_بٹ;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || (length>=0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* setup and swapping */
    اگر(length>=0 && length<(int32_t)حجم(UTrie2_سرخی)) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اندر_Trie=(ثابت UTrie2_سرخی *)اندر_ڈیٹا;
    trie.signature=ds->readUInt32(اندر_Trie->signature);
    trie.options=ds->readUInt16(اندر_Trie->options);
    trie.اشاریہ_لمبائی=ds->readUInt16(اندر_Trie->اشاریہ_لمبائی);
    trie.shifted_ڈیٹا_لمبائی=ds->readUInt16(اندر_Trie->shifted_ڈیٹا_لمبائی);

    قدر_بٹ=(UTrie2_قدر_بٹ)(trie.options&UTRIE2_OPTIONS_قدر_بٹ_نقاب);
    ڈیٹا_لمبائی=(int32_t)trie.shifted_ڈیٹا_لمبائی<<UTRIE2_اشاریہ_سرکاؤ;

    اگر( trie.signature!=UTRIE2_SIG ||
        قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ ||
        trie.اشاریہ_لمبائی<UTRIE2_اشاریہ_1_انحراف ||
        ڈیٹا_لمبائی<UTRIE2_ڈیٹا_آغاز_انحراف
    ) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        واپس 0;
    }

    size=حجم(UTrie2_سرخی)+trie.اشاریہ_لمبائی*2;
    انتخاب(قدر_بٹ) {
    حالت UTRIE2_16_قدر_بٹ:
        size+=ڈیٹا_لمبائی*2;
        توڑو;
    حالت UTRIE2_32_قدر_بٹ:
        size+=ڈیٹا_لمبائی*4;
        توڑو;
    طےشدہ:
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس 0;
    }

    اگر(length>=0) {
        UTrie2_سرخی *برآمد_Trie;

        اگر(length<size) {
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        برآمد_Trie=(UTrie2_سرخی *)برآمد_ڈیٹا;

        /* swap the header */
        ds->تبادلہ_Array32(ds, &اندر_Trie->signature, 4, &برآمد_Trie->signature, p_Error_رمز);
        ds->تبادلہ_Array16(ds, &اندر_Trie->options, 12, &برآمد_Trie->options, p_Error_رمز);

        /* swap the index and the data */
        انتخاب(قدر_بٹ) {
        حالت UTRIE2_16_قدر_بٹ:
            ds->تبادلہ_Array16(ds, اندر_Trie+1, (trie.اشاریہ_لمبائی+ڈیٹا_لمبائی)*2, برآمد_Trie+1, p_Error_رمز);
            توڑو;
        حالت UTRIE2_32_قدر_بٹ:
            ds->تبادلہ_Array16(ds, اندر_Trie+1, trie.اشاریہ_لمبائی*2, برآمد_Trie+1, p_Error_رمز);
            ds->تبادلہ_Array32(ds, (ثابت uint16_t *)(اندر_Trie+1)+trie.اشاریہ_لمبائی, ڈیٹا_لمبائی*4,
                                     (uint16_t *)(برآمد_Trie+1)+trie.اشاریہ_لمبائی, p_Error_رمز);
            توڑو;
        طےشدہ:
            *p_Error_رمز=U_INVALID_FORMAT_ERROR;
            واپس 0;
        }
    }

    واپس size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_تبادلہ(ثابت UDataSwapper *ds,
             ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
             UError_رمز *p_Error_رمز) {
    ثابت UCPTrie_سرخی *اندر_Trie;
    UCPTrie_سرخی trie;
    int32_t ڈیٹا_لمبائی, size;
    UCPTrie_قدر_چوڑائی قدر_چوڑائی;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || (length>=0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* setup and swapping */
    اگر(length>=0 && length<(int32_t)حجم(UCPTrie_سرخی)) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اندر_Trie=(ثابت UCPTrie_سرخی *)اندر_ڈیٹا;
    trie.signature=ds->readUInt32(اندر_Trie->signature);
    trie.options=ds->readUInt16(اندر_Trie->options);
    trie.اشاریہ_لمبائی=ds->readUInt16(اندر_Trie->اشاریہ_لمبائی);
    trie.ڈیٹا_لمبائی = ds->readUInt16(اندر_Trie->ڈیٹا_لمبائی);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    قدر_چوڑائی = (UCPTrie_قدر_چوڑائی)(trie.options & UCPTRIE_OPTIONS_قدر_بٹ_نقاب);
    ڈیٹا_لمبائی = ((int32_t)(trie.options & UCPTRIE_OPTIONS_ڈیٹا_لمبائی_نقاب) << 4) | trie.ڈیٹا_لمبائی;

    int32_t min_اشاریہ_لمبائی = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_اشاریہ_لمبائی : UCPTRIE_SMALL_اشاریہ_لمبائی;
    اگر( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_نقاب) != 0 ||
        قدر_چوڑائی > UCPTRIE_قدر_بٹ_8 ||
        trie.اشاریہ_لمبائی < min_اشاریہ_لمبائی ||
        ڈیٹا_لمبائی < ASCII_حد_بندی
    ) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        واپس 0;
    }

    size=حجم(UCPTrie_سرخی)+trie.اشاریہ_لمبائی*2;
    انتخاب(قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        size+=ڈیٹا_لمبائی*2;
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        size+=ڈیٹا_لمبائی*4;
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        size+=ڈیٹا_لمبائی;
        توڑو;
    طےشدہ:
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس 0;
    }

    اگر(length>=0) {
        UCPTrie_سرخی *برآمد_Trie;

        اگر(length<size) {
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        برآمد_Trie=(UCPTrie_سرخی *)برآمد_ڈیٹا;

        /* swap the header */
        ds->تبادلہ_Array32(ds, &اندر_Trie->signature, 4, &برآمد_Trie->signature, p_Error_رمز);
        ds->تبادلہ_Array16(ds, &اندر_Trie->options, 12, &برآمد_Trie->options, p_Error_رمز);

        /* swap the index */
        ثابت uint16_t *اندر_اشاریہ=reinterpret_cast<ثابت uint16_t *>(اندر_Trie+1);
        uint16_t *برآمد_اشاریہ=reinterpret_cast<uint16_t *>(برآمد_Trie+1);
        ds->تبادلہ_Array16(ds, اندر_اشاریہ, trie.اشاریہ_لمبائی*2, برآمد_اشاریہ, p_Error_رمز);

        /* swap the data */
        ثابت uint16_t *اندر_ڈیٹا=اندر_اشاریہ+trie.اشاریہ_لمبائی;
        uint16_t *برآمد_ڈیٹا=برآمد_اشاریہ+trie.اشاریہ_لمبائی;
        انتخاب(قدر_چوڑائی) {
        حالت UCPTRIE_قدر_بٹ_16:
            ds->تبادلہ_Array16(ds, اندر_ڈیٹا, ڈیٹا_لمبائی*2, برآمد_ڈیٹا, p_Error_رمز);
            توڑو;
        حالت UCPTRIE_قدر_بٹ_32:
            ds->تبادلہ_Array32(ds, اندر_ڈیٹا, ڈیٹا_لمبائی*4, برآمد_ڈیٹا, p_Error_رمز);
            توڑو;
        حالت UCPTRIE_قدر_بٹ_8:
            اگر(اندر_Trie!=برآمد_Trie) {
                uprv_memmove(برآمد_ڈیٹا, اندر_ڈیٹا, ڈیٹا_لمبائی);
            }
            توڑو;
        طےشدہ:
            *p_Error_رمز=U_INVALID_FORMAT_ERROR;
            واپس 0;
        }
    }

    واپس size;
}

نامخلا {

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
get_نسخہ(ثابت خالی *ڈیٹا, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    اگر(length<16 || ڈیٹا==خالی_اشاریہ || (U_اشارہ_گر_نقاب_LSB(ڈیٹا, 3)!=0)) {
        واپس 0;
    }
    signature = *static_cast<ثابت uint32_t*>(ڈیٹا);
    اگر(signature==UCPTRIE_SIG) {
        واپس 3;
    }
    اگر(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        واپس 3;
    }
    اگر(signature==UTRIE2_SIG) {
        واپس 2;
    }
    اگر(anyEndianOk && signature==UTRIE2_OE_SIG) {
        واپس 2;
    }
    اگر(signature==UTRIE_SIG) {
        واپس 1;
    }
    اگر(anyEndianOk && signature==UTRIE_OE_SIG) {
        واپس 1;
    }
    واپس 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_تبادلہ_Any_نسخہ(ثابت UDataSwapper *ds,
                     ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                     UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) { واپس 0; }
    انتخاب(get_نسخہ(اندر_ڈیٹا, length, سچ)) {
    حالت 1:
        واپس utrie_تبادلہ(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    حالت 2:
        واپس utrie2_تبادلہ(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    حالت 3:
        واپس ucptrie_تبادلہ(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    طےشدہ:
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس 0;
    }
}
