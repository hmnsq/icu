// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
#   include <stdio.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ਦੁਆਧਾਰੀ(UCPTrieType type,
                       UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ,
                       ਅਚਲ ਖਾਲੀ* ਅੰਕੜੇ U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_ਲੰਮਾਈ,
                       UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (length <= 0 || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(ਅੰਕੜੇ, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            ਮੁੱਲ_ਚੌੜਾਈ < UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_ANY || UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 < ਮੁੱਲ_ਚੌੜਾਈ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Enough data for a trie header?
    ਜੇ (length < (int32_t)ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ)) {
        *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Check the signature.
    ਅਚਲ UCPTrie_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ = (ਅਚਲ UCPTrie_ਸਿਰਲੇਖ *)ਅੰਕੜੇ;
    ਜੇ (ਸਿਰਲੇਖ->signature != UCPTRIE_SIG) {
        *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    int32_t options = ਸਿਰਲੇਖ->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t ਮੁੱਲ_ਚੌੜਾਈ_Int = options & UCPTRIE_OPTIONS_ਮੁੱਲ_ਬਿੱਟ_ਨਕਾਬ;
    ਜੇ (typeInt > UCPTRIE_TYPE_SMALL || ਮੁੱਲ_ਚੌੜਾਈ_Int > UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_ਨਕਾਬ) != 0) {
        *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ actual_ਮੁੱਲ_ਚੌੜਾਈ = (UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ)ਮੁੱਲ_ਚੌੜਾਈ_Int;
    ਜੇ (type < 0) {
        type = actualType;
    }
    ਜੇ (ਮੁੱਲ_ਚੌੜਾਈ < 0) {
        ਮੁੱਲ_ਚੌੜਾਈ = actual_ਮੁੱਲ_ਚੌੜਾਈ;
    }
    ਜੇ (type != actualType || ਮੁੱਲ_ਚੌੜਾਈ != actual_ਮੁੱਲ_ਚੌੜਾਈ) {
        *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Get the length values and offsets.
    UCPTrie ਆਰਜ਼ੀ_Trie;
    uprv_memset(&ਆਰਜ਼ੀ_Trie, 0, ਆਕਾਰ(ਆਰਜ਼ੀ_Trie));
    ਆਰਜ਼ੀ_Trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ =
        ((options & UCPTRIE_OPTIONS_ਅੰਕੜੇ_ਲੰਮਾਈ_ਨਕਾਬ) << 4) | ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਆਰਜ਼ੀ_Trie.index3_Null_ਵਿੱਥ = ਸਿਰਲੇਖ->index3_Null_ਵਿੱਥ;
    ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_Null_ਵਿੱਥ =
        ((options & UCPTRIE_OPTIONS_ਅੰਕੜੇ_NULL_ਵਿੱਥ_ਨਕਾਬ) << 8) | ਸਿਰਲੇਖ->ਅੰਕੜੇ_Null_ਵਿੱਥ;

    ਆਰਜ਼ੀ_Trie.ਉੱਚਾ_ਸ਼ੁਰੂ = ਸਿਰਲੇਖ->shifted_ਉੱਚਾ_ਸ਼ੁਰੂ << UCPTRIE_ਖਿਸਕਾਓ_2;
    ਆਰਜ਼ੀ_Trie.shifted12_ਉੱਚਾ_ਸ਼ੁਰੂ = (ਆਰਜ਼ੀ_Trie.ਉੱਚਾ_ਸ਼ੁਰੂ + 0xfff) >> 12;
    ਆਰਜ਼ੀ_Trie.type = type;
    ਆਰਜ਼ੀ_Trie.ਮੁੱਲ_ਚੌੜਾਈ = ਮੁੱਲ_ਚੌੜਾਈ;

    // Calculate the actual length.
    int32_t actual_ਲੰਮਾਈ = (int32_t)ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ) + ਆਰਜ਼ੀ_Trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2;
    ਜੇ (ਮੁੱਲ_ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16) {
        actual_ਲੰਮਾਈ += ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ * 2;
    } ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ_ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32) {
        actual_ਲੰਮਾਈ += ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ * 4;
    } ਨਹੀਂਤਾਂ {
        actual_ਲੰਮਾਈ += ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ;
    }
    ਜੇ (length < actual_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(ਆਕਾਰ(UCPTrie));
    ਜੇ (trie == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(trie, &ਆਰਜ਼ੀ_Trie, ਆਕਾਰ(ਆਰਜ਼ੀ_Trie));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#ਜੇ_ਅੰਤ

    // Set the pointers to its index and data arrays.
    ਅਚਲ uint16_t *p16 = (ਅਚਲ uint16_t *)(ਸਿਰਲੇਖ + 1);
    trie->ਸੂਚਕਾਂਕ = p16;
    p16 += trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;

    // Get the data.
    int32_t null_ਮੁੱਲ_ਵਿੱਥ = trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    ਜੇ (null_ਮੁੱਲ_ਵਿੱਥ >= trie->ਅੰਕੜੇ_ਲੰਮਾਈ) {
        null_ਮੁੱਲ_ਵਿੱਥ = trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ;
    }
    ਚੋਣ (ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        trie->ਅੰਕੜੇ.ptr16 = p16;
        trie->null_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr16[null_ਮੁੱਲ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        trie->ਅੰਕੜੇ.ptr32 = (ਅਚਲ uint32_t *)p16;
        trie->null_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr32[null_ਮੁੱਲ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        trie->ਅੰਕੜੇ.ptr8 = (ਅਚਲ uint8_t *)p16;
        trie->null_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr8[null_ਮੁੱਲ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਮੂਲ:
        // Unreachable because valueWidth was checked above.
        *p_Error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (p_Actual_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
        *p_Actual_ਲੰਮਾਈ = actual_ਲੰਮਾਈ;
    }
    ਵਾਪਸ trie;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ਅਚਲ UCPTrie *trie) {
    ਵਾਪਸ (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ U_EXPORT2
ucptrie_get_ਮੁੱਲ_ਚੌੜਾਈ(ਅਚਲ UCPTrie *trie) {
    ਵਾਪਸ (UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ)trie->ਮੁੱਲ_ਚੌੜਾਈ;
}

U_CAPI int32_t U_EXPORT2
ucptrie_ਅੰਦਰੂਨੀ_Small_ਸੂਚਕਾਂਕ(ਅਚਲ UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_ਖਿਸਕਾਓ_1;
    ਜੇ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ਉੱਚਾ_ਸ਼ੁਰੂ);
        i1 += UCPTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ - UCPTRIE_OMITTED_BMP_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ਉੱਚਾ_ਸ਼ੁਰੂ && trie->ਉੱਚਾ_ਸ਼ੁਰੂ > UCPTRIE_SMALL_ਹੱਦਬੰਦੀ);
        i1 += UCPTRIE_SMALL_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    }
    int32_t i3_ਖੰਡ = trie->ਸੂਚਕਾਂਕ[
        (int32_t)trie->ਸੂਚਕਾਂਕ[i1] + ((c >> UCPTRIE_ਖਿਸਕਾਓ_2) & UCPTRIE_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ)];
    int32_t i3 = (c >> UCPTRIE_ਖਿਸਕਾਓ_3) & UCPTRIE_ਸੂਚਕਾਂਕ_3_ਨਕਾਬ;
    int32_t ਅੰਕੜੇ_ਖੰਡ;
    ਜੇ ((i3_ਖੰਡ & 0x8000) == 0) {
        // 16-bit indexes
        ਅੰਕੜੇ_ਖੰਡ = trie->ਸੂਚਕਾਂਕ[i3_ਖੰਡ + i3];
    } ਨਹੀਂਤਾਂ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_ਖੰਡ = (i3_ਖੰਡ & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        ਅੰਕੜੇ_ਖੰਡ = ((int32_t)trie->ਸੂਚਕਾਂਕ[i3_ਖੰਡ++] << (2 + (2 * i3))) & 0x30000;
        ਅੰਕੜੇ_ਖੰਡ |= trie->ਸੂਚਕਾਂਕ[i3_ਖੰਡ + i3];
    }
    ਵਾਪਸ ਅੰਕੜੇ_ਖੰਡ + (c & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_ਅੰਦਰੂਨੀ_Small_U8_ਸੂਚਕਾਂਕ(ਅਚਲ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ਜੇ (c >= trie->ਉੱਚਾ_ਸ਼ੁਰੂ) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ਵਾਪਸ trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ;
    }
    ਵਾਪਸ ucptrie_ਅੰਦਰੂਨੀ_Small_ਸੂਚਕਾਂਕ(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_ਅੰਦਰੂਨੀ_U8_Prev_ਸੂਚਕਾਂਕ(ਅਚਲ UCPTrie *trie, UChar32 c,
                            ਅਚਲ uint8_t *ਸ਼ੁਰੂ, ਅਚਲ uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ਜੇ ((src - ਸ਼ੁਰੂ) <= 7) {
        i = length = (int32_t)(src - ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
        i = length = 7;
        ਸ਼ੁਰੂ = src - 7;
    }
    c = utf8_prev_Char_ਸੁਰੱਖਿਅਤ_Body(ਸ਼ੁਰੂ, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_ਸੂਚਕਾਂਕ(trie, 0xffff, c);
    ਵਾਪਸ (idx << 3) | i;
}

ਨਾਮਾਕਾਸ਼ {

ਸਤਰਅੰਦਰ uint32_t get_ਮੁੱਲ(UCPTrie_ਅੰਕੜੇ ਅੰਕੜੇ, UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ, int32_t ਅੰਕੜੇ_ਸੂਚਕਾਂਕ) {
    ਚੋਣ (ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        ਵਾਪਸ ਅੰਕੜੇ.ptr16[ਅੰਕੜੇ_ਸੂਚਕਾਂਕ];
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        ਵਾਪਸ ਅੰਕੜੇ.ptr32[ਅੰਕੜੇ_ਸੂਚਕਾਂਕ];
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        ਵਾਪਸ ਅੰਕੜੇ.ptr8[ਅੰਕੜੇ_ਸੂਚਕਾਂਕ];
    ਮੂਲ:
        // Unreachable if the trie is properly initialized.
        ਵਾਪਸ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ਅਚਲ UCPTrie *trie, UChar32 c) {
    int32_t ਅੰਕੜੇ_ਸੂਚਕਾਂਕ;
    ਜੇ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        ਅੰਕੜੇ_ਸੂਚਕਾਂਕ = c;
    } ਨਹੀਂਤਾਂ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        ਅੰਕੜੇ_ਸੂਚਕਾਂਕ = UCPTRIE_CP_ਸੂਚਕਾਂਕ(trie, fastMax, c);
    }
    ਵਾਪਸ get_ਮੁੱਲ(trie->ਅੰਕੜੇ, (UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ)trie->ਮੁੱਲ_ਚੌੜਾਈ, ਅੰਕੜੇ_ਸੂਚਕਾਂਕ);
}

ਨਾਮਾਕਾਸ਼ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

ਸਤਰਅੰਦਰ uint32_t maybe_Filter_ਮੁੱਲ(uint32_t value, uint32_t trie_Null_ਮੁੱਲ, uint32_t null_ਮੁੱਲ,
                                 UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context) {
    ਜੇ (value == trie_Null_ਮੁੱਲ) {
        value = null_ਮੁੱਲ;
    } ਨਹੀਂਤਾਂ ਜੇ (filter != ਖਾਲੀ_ਸੂਚਕ) {
        value = filter(context, value);
    }
    ਵਾਪਸ value;
}

UChar32 getRange(ਅਚਲ ਖਾਲੀ *t, UChar32 ਸ਼ੁਰੂ,
                 UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context, uint32_t *p_ਮੁੱਲ) {
    ਜੇ (static_cast<uint32_t>(ਸ਼ੁਰੂ) > MAX_UNICODE) {
        ਵਾਪਸ U_SENTINEL;
    }
    ਅਚਲ UCPTrie *trie = reinterpret_cast<ਅਚਲ UCPTrie *>(t);
    UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ = static_cast<UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ>(trie->ਮੁੱਲ_ਚੌੜਾਈ);
    ਜੇ (ਸ਼ੁਰੂ >= trie->ਉੱਚਾ_ਸ਼ੁਰੂ) {
        ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) {
            int32_t di = trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ;
            uint32_t value = get_ਮੁੱਲ(trie->ਅੰਕੜੇ, ਮੁੱਲ_ਚੌੜਾਈ, di);
            ਜੇ (filter != ਖਾਲੀ_ਸੂਚਕ) { value = filter(context, value); }
            *p_ਮੁੱਲ = value;
        }
        ਵਾਪਸ MAX_UNICODE;
    }

    uint32_t null_ਮੁੱਲ = trie->null_ਮੁੱਲ;
    ਜੇ (filter != ਖਾਲੀ_ਸੂਚਕ) { null_ਮੁੱਲ = filter(context, null_ਮੁੱਲ); }
    ਅਚਲ uint16_t *ਸੂਚਕਾਂਕ = trie->ਸੂਚਕਾਂਕ;

    int32_t prev_I3_ਖੰਡ = -1;
    int32_t prev_ਖੰਡ = -1;
    UChar32 c = ਸ਼ੁਰੂ;
    uint32_t trie_ਮੁੱਲ, value = null_ਮੁੱਲ;
    ਤਾਰਕਿਕ have_ਮੁੱਲ = ਝੂਠ;
    ਕਰੋ {
        int32_t i3_ਖੰਡ;
        int32_t i3;
        int32_t i3_ਖੰਡ_ਲੰਮਾਈ;
        int32_t ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        ਜੇ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_ਖੰਡ = 0;
            i3 = c >> UCPTRIE_FAST_ਖਿਸਕਾਓ;
            i3_ਖੰਡ_ਲੰਮਾਈ = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ : UCPTRIE_SMALL_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
            ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_ਖਿਸਕਾਓ_1;
            ਜੇ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ਉੱਚਾ_ਸ਼ੁਰੂ);
                i1 += UCPTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ - UCPTRIE_OMITTED_BMP_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                U_ASSERT(c < trie->ਉੱਚਾ_ਸ਼ੁਰੂ && trie->ਉੱਚਾ_ਸ਼ੁਰੂ > UCPTRIE_SMALL_ਹੱਦਬੰਦੀ);
                i1 += UCPTRIE_SMALL_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
            }
            i3_ਖੰਡ = trie->ਸੂਚਕਾਂਕ[
                static_cast<int32_t>(trie->ਸੂਚਕਾਂਕ[i1]) + ((c >> UCPTRIE_ਖਿਸਕਾਓ_2) & UCPTRIE_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ)];
            ਜੇ (i3_ਖੰਡ == prev_I3_ਖੰਡ && (c - ਸ਼ੁਰੂ) >= UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ - 1)) == 0);
                c += UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ;
                ਜਾਰੀ;
            }
            prev_I3_ਖੰਡ = i3_ਖੰਡ;
            ਜੇ (i3_ਖੰਡ == trie->index3_Null_ਵਿੱਥ) {
                // This is the index-3 null block.
                ਜੇ (have_ਮੁੱਲ) {
                    ਜੇ (null_ਮੁੱਲ != value) {
                        ਵਾਪਸ c - 1;
                    }
                } ਨਹੀਂਤਾਂ {
                    trie_ਮੁੱਲ = trie->null_ਮੁੱਲ;
                    value = null_ਮੁੱਲ;
                    ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਮੁੱਲ = null_ਮੁੱਲ; }
                    have_ਮੁੱਲ = ਸੱਚ;
                }
                prev_ਖੰਡ = trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
                c = (c + UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ) & ~(UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ - 1);
                ਜਾਰੀ;
            }
            i3 = (c >> UCPTRIE_ਖਿਸਕਾਓ_3) & UCPTRIE_ਸੂਚਕਾਂਕ_3_ਨਕਾਬ;
            i3_ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ;
            ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        }
        // Enumerate data blocks for one index-3 block.
        ਕਰੋ {
            int32_t ਖੰਡ;
            ਜੇ ((i3_ਖੰਡ & 0x8000) == 0) {
                ਖੰਡ = ਸੂਚਕਾਂਕ[i3_ਖੰਡ + i3];
            } ਨਹੀਂਤਾਂ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t ਸਮੂਹ = (i3_ਖੰਡ & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                ਖੰਡ = (static_cast<int32_t>(ਸੂਚਕਾਂਕ[ਸਮੂਹ++]) << (2 + (2 * gi))) & 0x30000;
                ਖੰਡ |= ਸੂਚਕਾਂਕ[ਸਮੂਹ + gi];
            }
            ਜੇ (ਖੰਡ == prev_ਖੰਡ && (c - ਸ਼ੁਰੂ) >= ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ - 1)) == 0);
                c += ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                int32_t ਅੰਕੜੇ_ਨਕਾਬ = ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ - 1;
                prev_ਖੰਡ = ਖੰਡ;
                ਜੇ (ਖੰਡ == trie->ਅੰਕੜੇ_Null_ਵਿੱਥ) {
                    // This is the data null block.
                    ਜੇ (have_ਮੁੱਲ) {
                        ਜੇ (null_ਮੁੱਲ != value) {
                            ਵਾਪਸ c - 1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        trie_ਮੁੱਲ = trie->null_ਮੁੱਲ;
                        value = null_ਮੁੱਲ;
                        ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਮੁੱਲ = null_ਮੁੱਲ; }
                        have_ਮੁੱਲ = ਸੱਚ;
                    }
                    c = (c + ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) & ~ਅੰਕੜੇ_ਨਕਾਬ;
                } ਨਹੀਂਤਾਂ {
                    int32_t di = ਖੰਡ + (c & ਅੰਕੜੇ_ਨਕਾਬ);
                    uint32_t trieValue2 = get_ਮੁੱਲ(trie->ਅੰਕੜੇ, ਮੁੱਲ_ਚੌੜਾਈ, di);
                    ਜੇ (have_ਮੁੱਲ) {
                        ਜੇ (trieValue2 != trie_ਮੁੱਲ) {
                            ਜੇ (filter == ਖਾਲੀ_ਸੂਚਕ ||
                                    maybe_Filter_ਮੁੱਲ(trieValue2, trie->null_ਮੁੱਲ, null_ਮੁੱਲ,
                                                     filter, context) != value) {
                                ਵਾਪਸ c - 1;
                            }
                            trie_ਮੁੱਲ = trieValue2;  // may or may not help
                        }
                    } ਨਹੀਂਤਾਂ {
                        trie_ਮੁੱਲ = trieValue2;
                        value = maybe_Filter_ਮੁੱਲ(trieValue2, trie->null_ਮੁੱਲ, null_ਮੁੱਲ,
                                                 filter, context);
                        ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਮੁੱਲ = value; }
                        have_ਮੁੱਲ = ਸੱਚ;
                    }
                    ਜਦੋਂ ((++c & ਅੰਕੜੇ_ਨਕਾਬ) != 0) {
                        trieValue2 = get_ਮੁੱਲ(trie->ਅੰਕੜੇ, ਮੁੱਲ_ਚੌੜਾਈ, ++di);
                        ਜੇ (trieValue2 != trie_ਮੁੱਲ) {
                            ਜੇ (filter == ਖਾਲੀ_ਸੂਚਕ ||
                                    maybe_Filter_ਮੁੱਲ(trieValue2, trie->null_ਮੁੱਲ, null_ਮੁੱਲ,
                                                     filter, context) != value) {
                                ਵਾਪਸ c - 1;
                            }
                            trie_ਮੁੱਲ = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ਜਦੋਂ (++i3 < i3_ਖੰਡ_ਲੰਮਾਈ);
    } ਜਦੋਂ (c < trie->ਉੱਚਾ_ਸ਼ੁਰੂ);
    U_ASSERT(have_ਮੁੱਲ);
    int32_t di = trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ;
    uint32_t ਉੱਚਾ_ਮੁੱਲ = get_ਮੁੱਲ(trie->ਅੰਕੜੇ, ਮੁੱਲ_ਚੌੜਾਈ, di);
    ਜੇ (maybe_Filter_ਮੁੱਲ(ਉੱਚਾ_ਮੁੱਲ, trie->null_ਮੁੱਲ, null_ਮੁੱਲ,
                         filter, context) != value) {
        ਵਾਪਸ c - 1;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_ਅੰਦਰੂਨੀ_Get_Range(UCPTrieGetRange *getRange,
                         ਅਚਲ ਖਾਲੀ *trie, UChar32 ਸ਼ੁਰੂ,
                         UCPMapRangeOption option, uint32_t surrogate_ਮੁੱਲ,
                         UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context, uint32_t *p_ਮੁੱਲ) {
    ਜੇ (option == UCPMAP_RANGE_ਸਾਧਾਰਨ) {
        ਵਾਪਸ getRange(trie, ਸ਼ੁਰੂ, filter, context, p_ਮੁੱਲ);
    }
    uint32_t value;
    ਜੇ (p_ਮੁੱਲ == ਖਾਲੀ_ਸੂਚਕ) {
        // We need to examine the range value even if the caller does not want it.
        p_ਮੁੱਲ = &value;
    }
    UChar32 surr_ਸਿਰਾ = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 ਸਿਰਾ = getRange(trie, ਸ਼ੁਰੂ, filter, context, p_ਮੁੱਲ);
    ਜੇ (ਸਿਰਾ < 0xd7ff || ਸ਼ੁਰੂ > surr_ਸਿਰਾ) {
        ਵਾਪਸ ਸਿਰਾ;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ਜੇ (*p_ਮੁੱਲ == surrogate_ਮੁੱਲ) {
        ਜੇ (ਸਿਰਾ >= surr_ਸਿਰਾ) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ਵਾਪਸ ਸਿਰਾ;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (ਸ਼ੁਰੂ <= 0xd7ff) {
            ਵਾਪਸ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_ਮੁੱਲ = surrogate_ਮੁੱਲ;
        ਜੇ (ਸਿਰਾ > surr_ਸਿਰਾ) {
            ਵਾਪਸ surr_ਸਿਰਾ;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_ਸਿਰਾ + 1, filter, context, &value2);
    ਜੇ (value2 == surrogate_ਮੁੱਲ) {
        ਵਾਪਸ end2;
    }
    ਵਾਪਸ surr_ਸਿਰਾ;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ਅਚਲ UCPTrie *trie, UChar32 ਸ਼ੁਰੂ,
                 UCPMapRangeOption option, uint32_t surrogate_ਮੁੱਲ,
                 UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context, uint32_t *p_ਮੁੱਲ) {
    ਵਾਪਸ ucptrie_ਅੰਦਰੂਨੀ_Get_Range(getRange, trie, ਸ਼ੁਰੂ,
                                    option, surrogate_ਮੁੱਲ,
                                    filter, context, p_ਮੁੱਲ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_ਮੰਜ਼ਿਲ_ਦੁਆਧਾਰੀ(ਅਚਲ UCPTrie *trie,
                 ਖਾਲੀ *ਅੰਕੜੇ, int32_t capacity,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ = (UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ)trie->ਮੁੱਲ_ਚੌੜਾਈ;
    ਜੇ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ਮੁੱਲ_ਚੌੜਾਈ < UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16 || UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 < ਮੁੱਲ_ਚੌੜਾਈ ||
            capacity < 0 ||
            (capacity > 0 && (ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(ਅੰਕੜੇ, 3) != 0)))) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t length = (int32_t)ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ) + trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2;
    ਚੋਣ (ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        length += trie->ਅੰਕੜੇ_ਲੰਮਾਈ * 2;
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        length += trie->ਅੰਕੜੇ_ਲੰਮਾਈ * 4;
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        length += trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
        ਰੋਕੋ;
    ਮੂਲ:
        // unreachable
        ਰੋਕੋ;
    }
    ਜੇ (capacity < length) {
        *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ length;
    }

    ਅੱਖਰ *ਬਾਈਟਾਂ = (ਅੱਖਰ *)ਅੰਕੜੇ;
    UCPTrie_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ = (UCPTrie_ਸਿਰਲੇਖ *)ਬਾਈਟਾਂ;
    ਸਿਰਲੇਖ->signature = UCPTRIE_SIG;  // "Tri3"
    ਸਿਰਲੇਖ->options = (uint16_t)(
        ((trie->ਅੰਕੜੇ_ਲੰਮਾਈ & 0xf0000) >> 4) |
        ((trie->ਅੰਕੜੇ_Null_ਵਿੱਥ & 0xf0000) >> 8) |
        (trie->type << 6) |
        ਮੁੱਲ_ਚੌੜਾਈ);
    ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = (uint16_t)trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਲੰਮਾਈ = (uint16_t)trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਸਿਰਲੇਖ->index3_Null_ਵਿੱਥ = trie->index3_Null_ਵਿੱਥ;
    ਸਿਰਲੇਖ->ਅੰਕੜੇ_Null_ਵਿੱਥ = (uint16_t)trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    ਸਿਰਲੇਖ->shifted_ਉੱਚਾ_ਸ਼ੁਰੂ = trie->ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_2;
    ਬਾਈਟਾਂ += ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ);

    uprv_memcpy(ਬਾਈਟਾਂ, trie->ਸੂਚਕਾਂਕ, trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2);
    ਬਾਈਟਾਂ += trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2;

    ਚੋਣ (ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        uprv_memcpy(ਬਾਈਟਾਂ, trie->ਅੰਕੜੇ.ptr16, trie->ਅੰਕੜੇ_ਲੰਮਾਈ * 2);
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        uprv_memcpy(ਬਾਈਟਾਂ, trie->ਅੰਕੜੇ.ptr32, trie->ਅੰਕੜੇ_ਲੰਮਾਈ * 4);
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        uprv_memcpy(ਬਾਈਟਾਂ, trie->ਅੰਕੜੇ.ptr8, trie->ਅੰਕੜੇ_ਲੰਮਾਈ);
        ਰੋਕੋ;
    ਮੂਲ:
        // unreachable
        ਰੋਕੋ;
    }
    ਵਾਪਸ length;
}

ਨਾਮਾਕਾਸ਼ {

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
ਲੰਮਾ ਗਿਣਤੀ_Null(ਅਚਲ UCPTrie *trie) {
    uint32_t null_ਮੁੱਲ=trie->null_ਮੁੱਲ;
    int32_t length=trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਲੰਮਾ count=0;
    ਚੋਣ (trie->ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        ਲਈ(int32_t i=0; i<length; ++i) {
            ਜੇ(trie->ਅੰਕੜੇ.ptr16[i]==null_ਮੁੱਲ) { ++count; }
        }
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        ਲਈ(int32_t i=0; i<length; ++i) {
            ਜੇ(trie->ਅੰਕੜੇ.ptr32[i]==null_ਮੁੱਲ) { ++count; }
        }
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        ਲਈ(int32_t i=0; i<length; ++i) {
            ਜੇ(trie->ਅੰਕੜੇ.ptr8[i]==null_ਮੁੱਲ) { ++count; }
        }
        ਰੋਕੋ;
    ਮੂਲ:
        // unreachable
        ਰੋਕੋ;
    }
    ਵਾਪਸ count;
}

U_CFUNC ਖਾਲੀ
ucptrie_printLengths(ਅਚਲ UCPTrie *trie, ਅਚਲ ਅੱਖਰ *which) {
    ਲੰਮਾ ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਲੰਮਾ ਅੰਕੜੇ_ਲੰਮਾਈ=(ਲੰਮਾ)trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਲੰਮਾ ਕੁੱਲ_ਲੰਮਾਈ=(ਲੰਮਾ)ਆਕਾਰ(UCPTrie_ਸਿਰਲੇਖ)+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2+
            ਅੰਕੜੇ_ਲੰਮਾਈ*(trie->ਮੁੱਲ_ਚੌੜਾਈ==UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16 ? 2 :
                        trie->ਮੁੱਲ_ਚੌੜਾਈ==UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32 ? 4 : 1);
    ਲਿਖੋ("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਅੰਕੜੇ_ਲੰਮਾਈ, ਗਿਣਤੀ_Null(trie), ਕੁੱਲ_ਲੰਮਾਈ);
}
#ਜੇ_ਅੰਤ

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ਅਚਲ UCPMap *map, UChar32 c) {
    ਵਾਪਸ ucptrie_get(reinterpret_cast<ਅਚਲ UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ਅਚਲ UCPMap *map, UChar32 ਸ਼ੁਰੂ,
                UCPMapRangeOption option, uint32_t surrogate_ਮੁੱਲ,
                UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context, uint32_t *p_ਮੁੱਲ) {
    ਵਾਪਸ ucptrie_getRange(reinterpret_cast<ਅਚਲ UCPTrie *>(map), ਸ਼ੁਰੂ,
                            option, surrogate_ਮੁੱਲ,
                            filter, context, p_ਮੁੱਲ);
}
