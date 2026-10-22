// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucptrie_impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

ਨਾਮਾਕਾਸ਼ {

constexpr int32_t ASCII_ਹੱਦਬੰਦੀ = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
           ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UTrie_ਸਿਰਲੇਖ *ਵਿੱਚ_Trie;
    UTrie_ਸਿਰਲੇਖ trie;
    int32_t size;
    UBool ਅੰਕੜੇ_Is32;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || (length>=0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* setup and swapping */
    ਜੇ(length>=0 && (uint32_t)length<ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_Trie=(ਅਚਲ UTrie_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਅੰਕੜੇ;
    trie.signature=ds->readUInt32(ਵਿੱਚ_Trie->signature);
    trie.options=ds->readUInt32(ਵਿੱਚ_Trie->options);
    trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=udata_readInt32(ds, ਵਿੱਚ_Trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
    trie.ਅੰਕੜੇ_ਲੰਮਾਈ=udata_readInt32(ds, ਵਿੱਚ_Trie->ਅੰਕੜੇ_ਲੰਮਾਈ);

    ਜੇ( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_ਖਿਸਕਾਓ_ਨਕਾਬ)!=UTRIE_ਖਿਸਕਾਓ ||
        ((trie.options>>UTRIE_OPTIONS_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ)&UTRIE_OPTIONS_ਖਿਸਕਾਓ_ਨਕਾਬ)!=UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ ||
        trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ<UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ ||
        (trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ&(UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ-1))!=0 ||
        trie.ਅੰਕੜੇ_ਲੰਮਾਈ<UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ ||
        (trie.ਅੰਕੜੇ_ਲੰਮਾਈ&(UTRIE_ਅੰਕੜੇ_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_ਰੇਖਿਕ)!=0 && trie.ਅੰਕੜੇ_ਲੰਮਾਈ<(UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ+0x100))
    ) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ਵਾਪਸ 0;
    }

    ਅੰਕੜੇ_Is32 = (trie.options & UTRIE_OPTIONS_ਅੰਕੜੇ_IS_32_ਦੋਆਧਾਰੀ_ਅੰਕ) != 0;
    size=ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2+trie.ਅੰਕੜੇ_ਲੰਮਾਈ*(ਅੰਕੜੇ_Is32?4:2);

    ਜੇ(length>=0) {
        UTrie_ਸਿਰਲੇਖ *ਬਾਹਰਲਾ_Trie;

        ਜੇ(length<size) {
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        ਬਾਹਰਲਾ_Trie=(UTrie_ਸਿਰਲੇਖ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ;

        /* swap the header */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_Trie, ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ), ਬਾਹਰਲਾ_Trie, p_Error_ਕੋਡ);

        /* swap the index and the data */
        ਜੇ(ਅੰਕੜੇ_Is32) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_Trie+1, trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2, ਬਾਹਰਲਾ_Trie+1, p_Error_ਕੋਡ);
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, (ਅਚਲ uint16_t *)(ਵਿੱਚ_Trie+1)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, trie.ਅੰਕੜੇ_ਲੰਮਾਈ*4,
                                     (uint16_t *)(ਬਾਹਰਲਾ_Trie+1)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, p_Error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_Trie+1, (trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+trie.ਅੰਕੜੇ_ਲੰਮਾਈ)*2, ਬਾਹਰਲਾ_Trie+1, p_Error_ਕੋਡ);
        }
    }

    ਵਾਪਸ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
            ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UTrie2_ਸਿਰਲੇਖ *ਵਿੱਚ_Trie;
    UTrie2_ਸਿਰਲੇਖ trie;
    int32_t ਅੰਕੜੇ_ਲੰਮਾਈ, size;
    UTrie2_ਮੁੱਲ_ਬਿੱਟ ਮੁੱਲ_ਬਿੱਟ;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || (length>=0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* setup and swapping */
    ਜੇ(length>=0 && length<(int32_t)ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_Trie=(ਅਚਲ UTrie2_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਅੰਕੜੇ;
    trie.signature=ds->readUInt32(ਵਿੱਚ_Trie->signature);
    trie.options=ds->readUInt16(ਵਿੱਚ_Trie->options);
    trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=ds->readUInt16(ਵਿੱਚ_Trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
    trie.shifted_ਅੰਕੜੇ_ਲੰਮਾਈ=ds->readUInt16(ਵਿੱਚ_Trie->shifted_ਅੰਕੜੇ_ਲੰਮਾਈ);

    ਮੁੱਲ_ਬਿੱਟ=(UTrie2_ਮੁੱਲ_ਬਿੱਟ)(trie.options&UTRIE2_OPTIONS_ਮੁੱਲ_ਬਿੱਟ_ਨਕਾਬ);
    ਅੰਕੜੇ_ਲੰਮਾਈ=(int32_t)trie.shifted_ਅੰਕੜੇ_ਲੰਮਾਈ<<UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;

    ਜੇ( trie.signature!=UTRIE2_SIG ||
        ਮੁੱਲ_ਬਿੱਟ<0 || UTRIE2_ਗਿਣਤੀ_ਮੁੱਲ_ਬਿੱਟ<=ਮੁੱਲ_ਬਿੱਟ ||
        trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ<UTRIE2_ਸੂਚਕਾਂਕ_1_ਵਿੱਥ ||
        ਅੰਕੜੇ_ਲੰਮਾਈ<UTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ
    ) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ਵਾਪਸ 0;
    }

    size=ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2;
    ਚੋਣ(ਮੁੱਲ_ਬਿੱਟ) {
    ਹਾਲਤ UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ:
        size+=ਅੰਕੜੇ_ਲੰਮਾਈ*2;
        ਰੋਕੋ;
    ਹਾਲਤ UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ:
        size+=ਅੰਕੜੇ_ਲੰਮਾਈ*4;
        ਰੋਕੋ;
    ਮੂਲ:
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>=0) {
        UTrie2_ਸਿਰਲੇਖ *ਬਾਹਰਲਾ_Trie;

        ਜੇ(length<size) {
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        ਬਾਹਰਲਾ_Trie=(UTrie2_ਸਿਰਲੇਖ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ;

        /* swap the header */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, &ਵਿੱਚ_Trie->signature, 4, &ਬਾਹਰਲਾ_Trie->signature, p_Error_ਕੋਡ);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, &ਵਿੱਚ_Trie->options, 12, &ਬਾਹਰਲਾ_Trie->options, p_Error_ਕੋਡ);

        /* swap the index and the data */
        ਚੋਣ(ਮੁੱਲ_ਬਿੱਟ) {
        ਹਾਲਤ UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ:
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_Trie+1, (trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+ਅੰਕੜੇ_ਲੰਮਾਈ)*2, ਬਾਹਰਲਾ_Trie+1, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਹਾਲਤ UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ:
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_Trie+1, trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2, ਬਾਹਰਲਾ_Trie+1, p_Error_ਕੋਡ);
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, (ਅਚਲ uint16_t *)(ਵਿੱਚ_Trie+1)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਅੰਕੜੇ_ਲੰਮਾਈ*4,
                                     (uint16_t *)(ਬਾਹਰਲਾ_Trie+1)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਮੂਲ:
            *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            ਵਾਪਸ 0;
        }
    }

    ਵਾਪਸ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
             ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UCPTrie_ਸਿਰਲੇਖ *ਵਿੱਚ_Trie;
    UCPTrie_ਸਿਰਲੇਖ trie;
    int32_t ਅੰਕੜੇ_ਲੰਮਾਈ, size;
    UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || (length>=0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* setup and swapping */
    ਜੇ(length>=0 && length<(int32_t)ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ)) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_Trie=(ਅਚਲ UCPTrie_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਅੰਕੜੇ;
    trie.signature=ds->readUInt32(ਵਿੱਚ_Trie->signature);
    trie.options=ds->readUInt16(ਵਿੱਚ_Trie->options);
    trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=ds->readUInt16(ਵਿੱਚ_Trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
    trie.ਅੰਕੜੇ_ਲੰਮਾਈ = ds->readUInt16(ਵਿੱਚ_Trie->ਅੰਕੜੇ_ਲੰਮਾਈ);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    ਮੁੱਲ_ਚੌੜਾਈ = (UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ)(trie.options & UCPTRIE_OPTIONS_ਮੁੱਲ_ਬਿੱਟ_ਨਕਾਬ);
    ਅੰਕੜੇ_ਲੰਮਾਈ = ((int32_t)(trie.options & UCPTRIE_OPTIONS_ਅੰਕੜੇ_ਲੰਮਾਈ_ਨਕਾਬ) << 4) | trie.ਅੰਕੜੇ_ਲੰਮਾਈ;

    int32_t min_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ : UCPTRIE_SMALL_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਜੇ( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_ਨਕਾਬ) != 0 ||
        ਮੁੱਲ_ਚੌੜਾਈ > UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 ||
        trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ < min_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ ||
        ਅੰਕੜੇ_ਲੰਮਾਈ < ASCII_ਹੱਦਬੰਦੀ
    ) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        ਵਾਪਸ 0;
    }

    size=ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ)+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2;
    ਚੋਣ(ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        size+=ਅੰਕੜੇ_ਲੰਮਾਈ*2;
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        size+=ਅੰਕੜੇ_ਲੰਮਾਈ*4;
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        size+=ਅੰਕੜੇ_ਲੰਮਾਈ;
        ਰੋਕੋ;
    ਮੂਲ:
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>=0) {
        UCPTrie_ਸਿਰਲੇਖ *ਬਾਹਰਲਾ_Trie;

        ਜੇ(length<size) {
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        ਬਾਹਰਲਾ_Trie=(UCPTrie_ਸਿਰਲੇਖ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ;

        /* swap the header */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, &ਵਿੱਚ_Trie->signature, 4, &ਬਾਹਰਲਾ_Trie->signature, p_Error_ਕੋਡ);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, &ਵਿੱਚ_Trie->options, 12, &ਬਾਹਰਲਾ_Trie->options, p_Error_ਕੋਡ);

        /* swap the index */
        ਅਚਲ uint16_t *ਵਿੱਚ_ਸੂਚਕਾਂਕ=reinterpret_cast<ਅਚਲ uint16_t *>(ਵਿੱਚ_Trie+1);
        uint16_t *ਬਾਹਰਲਾ_ਸੂਚਕਾਂਕ=reinterpret_cast<uint16_t *>(ਬਾਹਰਲਾ_Trie+1);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਸੂਚਕਾਂਕ, trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2, ਬਾਹਰਲਾ_ਸੂਚਕਾਂਕ, p_Error_ਕੋਡ);

        /* swap the data */
        ਅਚਲ uint16_t *ਵਿੱਚ_ਅੰਕੜੇ=ਵਿੱਚ_ਸੂਚਕਾਂਕ+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
        uint16_t *ਬਾਹਰਲਾ_ਅੰਕੜੇ=ਬਾਹਰਲਾ_ਸੂਚਕਾਂਕ+trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
        ਚੋਣ(ਮੁੱਲ_ਚੌੜਾਈ) {
        ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ*2, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ*4, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
            ਜੇ(ਵਿੱਚ_Trie!=ਬਾਹਰਲਾ_Trie) {
                uprv_memmove(ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਵਿੱਚ_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ);
            }
            ਰੋਕੋ;
        ਮੂਲ:
            *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            ਵਾਪਸ 0;
        }
    }

    ਵਾਪਸ size;
}

ਨਾਮਾਕਾਸ਼ {

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
get_ਸੰਸਕਰਣ(ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    ਜੇ(length<16 || ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(ਅੰਕੜੇ, 3)!=0)) {
        ਵਾਪਸ 0;
    }
    signature = *static_cast<ਅਚਲ uint32_t*>(ਅੰਕੜੇ);
    ਜੇ(signature==UCPTRIE_SIG) {
        ਵਾਪਸ 3;
    }
    ਜੇ(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        ਵਾਪਸ 3;
    }
    ਜੇ(signature==UTRIE2_SIG) {
        ਵਾਪਸ 2;
    }
    ਜੇ(anyEndianOk && signature==UTRIE2_OE_SIG) {
        ਵਾਪਸ 2;
    }
    ਜੇ(signature==UTRIE_SIG) {
        ਵਾਪਸ 1;
    }
    ਜੇ(anyEndianOk && signature==UTRIE_OE_SIG) {
        ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_ਅਦਲਾ_ਬਦਲੀ_Any_ਸੰਸਕਰਣ(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ 0; }
    ਚੋਣ(get_ਸੰਸਕਰਣ(ਵਿੱਚ_ਅੰਕੜੇ, length, ਸੱਚ)) {
    ਹਾਲਤ 1:
        ਵਾਪਸ utrie_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਹਾਲਤ 2:
        ਵਾਪਸ utrie2_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਹਾਲਤ 3:
        ਵਾਪਸ ucptrie_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਮੂਲ:
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ 0;
    }
}
