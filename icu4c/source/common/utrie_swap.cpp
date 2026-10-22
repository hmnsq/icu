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
utrie_بدلون(ثابت UDataSwapper *ds,
           ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
           UError_کوډ *p_Error_کوډ) {
    ثابت UTrie_سرلیک *دننه_Trie;
    UTrie_سرلیک trie;
    int32_t size;
    UBool معلومات_Is32;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || (length>=0 && بهر_معلومات==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && (uint32_t)length<حجم(UTrie_سرلیک)) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    دننه_Trie=(ثابت UTrie_سرلیک *)دننه_معلومات;
    trie.signature=ds->readUInt32(دننه_Trie->signature);
    trie.options=ds->readUInt32(دننه_Trie->options);
    trie.شاخص_اوږدتیا=udata_readInt32(ds, دننه_Trie->شاخص_اوږدتیا);
    trie.معلومات_اوږدتیا=udata_readInt32(ds, دننه_Trie->معلومات_اوږدتیا);

    إذا( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_ښويدنه_ماسک)!=UTRIE_ښويدنه ||
        ((trie.options>>UTRIE_OPTIONS_شاخص_ښويدنه)&UTRIE_OPTIONS_ښويدنه_ماسک)!=UTRIE_شاخص_ښويدنه ||
        trie.شاخص_اوږدتیا<UTRIE_BMP_شاخص_اوږدتیا ||
        (trie.شاخص_اوږدتیا&(UTRIE_SURROGATE_بلاک_شمېر-1))!=0 ||
        trie.معلومات_اوږدتیا<UTRIE_معلومات_بلاک_اوږدتیا ||
        (trie.معلومات_اوږدتیا&(UTRIE_معلومات_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_خطي)!=0 && trie.معلومات_اوږدتیا<(UTRIE_معلومات_بلاک_اوږدتیا+0x100))
    ) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    معلومات_Is32 = (trie.options & UTRIE_OPTIONS_معلومات_IS_32_بټ) != 0;
    size=حجم(UTrie_سرلیک)+trie.شاخص_اوږدتیا*2+trie.معلومات_اوږدتیا*(معلومات_Is32?4:2);

    إذا(length>=0) {
        UTrie_سرلیک *بهر_Trie;

        إذا(length<size) {
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بهر_Trie=(UTrie_سرلیک *)بهر_معلومات;

        /* swap the header */
        ds->بدلون_Array32(ds, دننه_Trie, حجم(UTrie_سرلیک), بهر_Trie, p_Error_کوډ);

        /* swap the index and the data */
        إذا(معلومات_Is32) {
            ds->بدلون_Array16(ds, دننه_Trie+1, trie.شاخص_اوږدتیا*2, بهر_Trie+1, p_Error_کوډ);
            ds->بدلون_Array32(ds, (ثابت uint16_t *)(دننه_Trie+1)+trie.شاخص_اوږدتیا, trie.معلومات_اوږدتیا*4,
                                     (uint16_t *)(بهر_Trie+1)+trie.شاخص_اوږدتیا, p_Error_کوډ);
        } وإلا {
            ds->بدلون_Array16(ds, دننه_Trie+1, (trie.شاخص_اوږدتیا+trie.معلومات_اوږدتیا)*2, بهر_Trie+1, p_Error_کوډ);
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
utrie2_بدلون(ثابت UDataSwapper *ds,
            ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
            UError_کوډ *p_Error_کوډ) {
    ثابت UTrie2_سرلیک *دننه_Trie;
    UTrie2_سرلیک trie;
    int32_t معلومات_اوږدتیا, size;
    UTrie2_ارزښت_بټونه ارزښت_بټونه;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || (length>=0 && بهر_معلومات==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UTrie2_سرلیک)) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    دننه_Trie=(ثابت UTrie2_سرلیک *)دننه_معلومات;
    trie.signature=ds->readUInt32(دننه_Trie->signature);
    trie.options=ds->readUInt16(دننه_Trie->options);
    trie.شاخص_اوږدتیا=ds->readUInt16(دننه_Trie->شاخص_اوږدتیا);
    trie.shifted_معلومات_اوږدتیا=ds->readUInt16(دننه_Trie->shifted_معلومات_اوږدتیا);

    ارزښت_بټونه=(UTrie2_ارزښت_بټونه)(trie.options&UTRIE2_OPTIONS_ارزښت_بټونه_ماسک);
    معلومات_اوږدتیا=(int32_t)trie.shifted_معلومات_اوږدتیا<<UTRIE2_شاخص_ښويدنه;

    إذا( trie.signature!=UTRIE2_SIG ||
        ارزښت_بټونه<0 || UTRIE2_شمېر_ارزښت_بټونه<=ارزښت_بټونه ||
        trie.شاخص_اوږدتیا<UTRIE2_شاخص_1_دننه_واټن ||
        معلومات_اوږدتیا<UTRIE2_معلومات_پیل_دننه_واټن
    ) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        أرجع 0;
    }

    size=حجم(UTrie2_سرلیک)+trie.شاخص_اوږدتیا*2;
    بدل(ارزښت_بټونه) {
    حالة UTRIE2_16_ارزښت_بټونه:
        size+=معلومات_اوږدتیا*2;
        اكسر;
    حالة UTRIE2_32_ارزښت_بټونه:
        size+=معلومات_اوږدتیا*4;
        اكسر;
    افتراضي:
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UTrie2_سرلیک *بهر_Trie;

        إذا(length<size) {
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بهر_Trie=(UTrie2_سرلیک *)بهر_معلومات;

        /* swap the header */
        ds->بدلون_Array32(ds, &دننه_Trie->signature, 4, &بهر_Trie->signature, p_Error_کوډ);
        ds->بدلون_Array16(ds, &دننه_Trie->options, 12, &بهر_Trie->options, p_Error_کوډ);

        /* swap the index and the data */
        بدل(ارزښت_بټونه) {
        حالة UTRIE2_16_ارزښت_بټونه:
            ds->بدلون_Array16(ds, دننه_Trie+1, (trie.شاخص_اوږدتیا+معلومات_اوږدتیا)*2, بهر_Trie+1, p_Error_کوډ);
            اكسر;
        حالة UTRIE2_32_ارزښت_بټونه:
            ds->بدلون_Array16(ds, دننه_Trie+1, trie.شاخص_اوږدتیا*2, بهر_Trie+1, p_Error_کوډ);
            ds->بدلون_Array32(ds, (ثابت uint16_t *)(دننه_Trie+1)+trie.شاخص_اوږدتیا, معلومات_اوږدتیا*4,
                                     (uint16_t *)(بهر_Trie+1)+trie.شاخص_اوږدتیا, p_Error_کوډ);
            اكسر;
        افتراضي:
            *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
            أرجع 0;
        }
    }

    أرجع size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_بدلون(ثابت UDataSwapper *ds,
             ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
             UError_کوډ *p_Error_کوډ) {
    ثابت UCPTrie_سرلیک *دننه_Trie;
    UCPTrie_سرلیک trie;
    int32_t معلومات_اوږدتیا, size;
    UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || (length>=0 && بهر_معلومات==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    إذا(length>=0 && length<(int32_t)حجم(UCPTrie_سرلیک)) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    دننه_Trie=(ثابت UCPTrie_سرلیک *)دننه_معلومات;
    trie.signature=ds->readUInt32(دننه_Trie->signature);
    trie.options=ds->readUInt16(دننه_Trie->options);
    trie.شاخص_اوږدتیا=ds->readUInt16(دننه_Trie->شاخص_اوږدتیا);
    trie.معلومات_اوږدتیا = ds->readUInt16(دننه_Trie->معلومات_اوږدتیا);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    ارزښت_پلنوالی = (UCPTrie_ارزښت_پلنوالی)(trie.options & UCPTRIE_OPTIONS_ارزښت_بټونه_ماسک);
    معلومات_اوږدتیا = ((int32_t)(trie.options & UCPTRIE_OPTIONS_معلومات_اوږدتیا_ماسک) << 4) | trie.معلومات_اوږدتیا;

    int32_t min_شاخص_اوږدتیا = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_شاخص_اوږدتیا : UCPTRIE_SMALL_شاخص_اوږدتیا;
    إذا( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_ماسک) != 0 ||
        ارزښت_پلنوالی > UCPTRIE_ارزښت_بټونه_8 ||
        trie.شاخص_اوږدتیا < min_شاخص_اوږدتیا ||
        معلومات_اوږدتیا < ASCII_حد
    ) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        أرجع 0;
    }

    size=حجم(UCPTrie_سرلیک)+trie.شاخص_اوږدتیا*2;
    بدل(ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        size+=معلومات_اوږدتیا*2;
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        size+=معلومات_اوږدتیا*4;
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        size+=معلومات_اوږدتیا;
        اكسر;
    افتراضي:
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        UCPTrie_سرلیک *بهر_Trie;

        إذا(length<size) {
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بهر_Trie=(UCPTrie_سرلیک *)بهر_معلومات;

        /* swap the header */
        ds->بدلون_Array32(ds, &دننه_Trie->signature, 4, &بهر_Trie->signature, p_Error_کوډ);
        ds->بدلون_Array16(ds, &دننه_Trie->options, 12, &بهر_Trie->options, p_Error_کوډ);

        /* swap the index */
        ثابت uint16_t *دننه_شاخص=reinterpret_cast<ثابت uint16_t *>(دننه_Trie+1);
        uint16_t *بهر_شاخص=reinterpret_cast<uint16_t *>(بهر_Trie+1);
        ds->بدلون_Array16(ds, دننه_شاخص, trie.شاخص_اوږدتیا*2, بهر_شاخص, p_Error_کوډ);

        /* swap the data */
        ثابت uint16_t *دننه_معلومات=دننه_شاخص+trie.شاخص_اوږدتیا;
        uint16_t *بهر_معلومات=بهر_شاخص+trie.شاخص_اوږدتیا;
        بدل(ارزښت_پلنوالی) {
        حالة UCPTRIE_ارزښت_بټونه_16:
            ds->بدلون_Array16(ds, دننه_معلومات, معلومات_اوږدتیا*2, بهر_معلومات, p_Error_کوډ);
            اكسر;
        حالة UCPTRIE_ارزښت_بټونه_32:
            ds->بدلون_Array32(ds, دننه_معلومات, معلومات_اوږدتیا*4, بهر_معلومات, p_Error_کوډ);
            اكسر;
        حالة UCPTRIE_ارزښت_بټونه_8:
            إذا(دننه_Trie!=بهر_Trie) {
                uprv_memmove(بهر_معلومات, دننه_معلومات, معلومات_اوږدتیا);
            }
            اكسر;
        افتراضي:
            *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
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
get_نسخه(ثابت فراغ *معلومات, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    إذا(length<16 || معلومات==تش_اشاره || (U_اشاره_ګر_ماسک_LSB(معلومات, 3)!=0)) {
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
utrie_بدلون_Any_نسخه(ثابت UDataSwapper *ds,
                     ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                     UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) { أرجع 0; }
    بدل(get_نسخه(دننه_معلومات, length, ریښتیا)) {
    حالة 1:
        أرجع utrie_بدلون(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    حالة 2:
        أرجع utrie2_بدلون(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    حالة 3:
        أرجع ucptrie_بدلون(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    افتراضي:
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }
}
