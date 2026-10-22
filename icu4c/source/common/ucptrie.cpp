// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
#   include <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ದ್ವಿಮಾನ(UCPTrieType type,
                       UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ,
                       ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ದತ್ತಾಂಶ U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_ಉದ್ದಳತೆ,
                       UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (length <= 0 || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(ದತ್ತಾಂಶ, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            ಮೌಲ್ಯ_ಅಗಲ < UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ANY || UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 < ಮೌಲ್ಯ_ಅಗಲ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Enough data for a trie header?
    ಆದರೆ (length < (int32_t)ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ)) {
        *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Check the signature.
    ಸ್ಥಿರಾಂಕ UCPTrie_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ = (ಸ್ಥಿರಾಂಕ UCPTrie_ಶೀರ್ಷಿಕೆ *)ದತ್ತಾಂಶ;
    ಆದರೆ (ಶೀರ್ಷಿಕೆ->signature != UCPTRIE_SIG) {
        *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    int32_t options = ಶೀರ್ಷಿಕೆ->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t ಮೌಲ್ಯ_ಅಗಲ_Int = options & UCPTRIE_OPTIONS_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ಮುಖವಾಡ;
    ಆದರೆ (typeInt > UCPTRIE_TYPE_SMALL || ಮೌಲ್ಯ_ಅಗಲ_Int > UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_ಮುಖವಾಡ) != 0) {
        *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_ಮೌಲ್ಯ_ಅಗಲ actual_ಮೌಲ್ಯ_ಅಗಲ = (UCPTrie_ಮೌಲ್ಯ_ಅಗಲ)ಮೌಲ್ಯ_ಅಗಲ_Int;
    ಆದರೆ (type < 0) {
        type = actualType;
    }
    ಆದರೆ (ಮೌಲ್ಯ_ಅಗಲ < 0) {
        ಮೌಲ್ಯ_ಅಗಲ = actual_ಮೌಲ್ಯ_ಅಗಲ;
    }
    ಆದರೆ (type != actualType || ಮೌಲ್ಯ_ಅಗಲ != actual_ಮೌಲ್ಯ_ಅಗಲ) {
        *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Get the length values and offsets.
    UCPTrie ತಾತ್ಕಾಲಿಕ_Trie;
    uprv_memset(&ತಾತ್ಕಾಲಿಕ_Trie, 0, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_Trie));
    ತಾತ್ಕಾಲಿಕ_Trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ =
        ((options & UCPTRIE_OPTIONS_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ_ಮುಖವಾಡ) << 4) | ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ತಾತ್ಕಾಲಿಕ_Trie.index3_Null_ಸ್ಥಾನಾಂತರ = ಶೀರ್ಷಿಕೆ->index3_Null_ಸ್ಥಾನಾಂತರ;
    ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ =
        ((options & UCPTRIE_OPTIONS_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ) << 8) | ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;

    ತಾತ್ಕಾಲಿಕ_Trie.ಮೇಲಿನ_ಆರಂಭ = ಶೀರ್ಷಿಕೆ->shifted_ಮೇಲಿನ_ಆರಂಭ << UCPTRIE_ಸರಿಸು_2;
    ತಾತ್ಕಾಲಿಕ_Trie.shifted12_ಮೇಲಿನ_ಆರಂಭ = (ತಾತ್ಕಾಲಿಕ_Trie.ಮೇಲಿನ_ಆರಂಭ + 0xfff) >> 12;
    ತಾತ್ಕಾಲಿಕ_Trie.type = type;
    ತಾತ್ಕಾಲಿಕ_Trie.ಮೌಲ್ಯ_ಅಗಲ = ಮೌಲ್ಯ_ಅಗಲ;

    // Calculate the actual length.
    int32_t actual_ಉದ್ದಳತೆ = (int32_t)ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ) + ತಾತ್ಕಾಲಿಕ_Trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2;
    ಆದರೆ (ಮೌಲ್ಯ_ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16) {
        actual_ಉದ್ದಳತೆ += ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ_ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32) {
        actual_ಉದ್ದಳತೆ += ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4;
    } ಇಲ್ಲದಿದ್ದರೆ {
        actual_ಉದ್ದಳತೆ += ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    }
    ಆದರೆ (length < actual_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(ಗಾತ್ರ(UCPTrie));
    ಆದರೆ (trie == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(trie, &ತಾತ್ಕಾಲಿಕ_Trie, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_Trie));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#ಆದರೆ_ಅಂತ್ಯ

    // Set the pointers to its index and data arrays.
    ಸ್ಥಿರಾಂಕ uint16_t *p16 = (ಸ್ಥಿರಾಂಕ uint16_t *)(ಶೀರ್ಷಿಕೆ + 1);
    trie->ಸೂಚ್ಯಂಕ = p16;
    p16 += trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;

    // Get the data.
    int32_t null_ಮೌಲ್ಯ_ಸ್ಥಾನಾಂತರ = trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (null_ಮೌಲ್ಯ_ಸ್ಥಾನಾಂತರ >= trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
        null_ಮೌಲ್ಯ_ಸ್ಥಾನಾಂತರ = trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;
    }
    ಆಯ್ಕೆ (ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        trie->ದತ್ತಾಂಶ.ptr16 = p16;
        trie->null_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr16[null_ಮೌಲ್ಯ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        trie->ದತ್ತಾಂಶ.ptr32 = (ಸ್ಥಿರಾಂಕ uint32_t *)p16;
        trie->null_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr32[null_ಮೌಲ್ಯ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        trie->ದತ್ತಾಂಶ.ptr8 = (ಸ್ಥಿರಾಂಕ uint8_t *)p16;
        trie->null_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr8[null_ಮೌಲ್ಯ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // Unreachable because valueWidth was checked above.
        *p_Error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (p_Actual_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
        *p_Actual_ಉದ್ದಳತೆ = actual_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ trie;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ಸ್ಥಿರಾಂಕ UCPTrie *trie) {
    ಹಿಂತಿರುಗಿ (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_ಮೌಲ್ಯ_ಅಗಲ U_EXPORT2
ucptrie_get_ಮೌಲ್ಯ_ಅಗಲ(ಸ್ಥಿರಾಂಕ UCPTrie *trie) {
    ಹಿಂತಿರುಗಿ (UCPTrie_ಮೌಲ್ಯ_ಅಗಲ)trie->ಮೌಲ್ಯ_ಅಗಲ;
}

U_CAPI int32_t U_EXPORT2
ucptrie_ಆಂತರಿಕ_Small_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_ಸರಿಸು_1;
    ಆದರೆ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ಮೇಲಿನ_ಆರಂಭ);
        i1 += UCPTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ - UCPTRIE_OMITTED_BMP_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ಮೇಲಿನ_ಆರಂಭ && trie->ಮೇಲಿನ_ಆರಂಭ > UCPTRIE_SMALL_ಎಲ್ಲೆ);
        i1 += UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    }
    int32_t i3_ಖಂಡ = trie->ಸೂಚ್ಯಂಕ[
        (int32_t)trie->ಸೂಚ್ಯಂಕ[i1] + ((c >> UCPTRIE_ಸರಿಸು_2) & UCPTRIE_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ)];
    int32_t i3 = (c >> UCPTRIE_ಸರಿಸು_3) & UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಮುಖವಾಡ;
    int32_t ದತ್ತಾಂಶ_ಖಂಡ;
    ಆದರೆ ((i3_ಖಂಡ & 0x8000) == 0) {
        // 16-bit indexes
        ದತ್ತಾಂಶ_ಖಂಡ = trie->ಸೂಚ್ಯಂಕ[i3_ಖಂಡ + i3];
    } ಇಲ್ಲದಿದ್ದರೆ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_ಖಂಡ = (i3_ಖಂಡ & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        ದತ್ತಾಂಶ_ಖಂಡ = ((int32_t)trie->ಸೂಚ್ಯಂಕ[i3_ಖಂಡ++] << (2 + (2 * i3))) & 0x30000;
        ದತ್ತಾಂಶ_ಖಂಡ |= trie->ಸೂಚ್ಯಂಕ[i3_ಖಂಡ + i3];
    }
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_ಖಂಡ + (c & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_ಆಂತರಿಕ_Small_U8_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ಆದರೆ (c >= trie->ಮೇಲಿನ_ಆರಂಭ) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ಹಿಂತಿರುಗಿ trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;
    }
    ಹಿಂತಿರುಗಿ ucptrie_ಆಂತರಿಕ_Small_ಸೂಚ್ಯಂಕ(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_ಆಂತರಿಕ_U8_Prev_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c,
                            ಸ್ಥಿರಾಂಕ uint8_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ಆದರೆ ((src - ಆರಂಭ) <= 7) {
        i = length = (int32_t)(src - ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        i = length = 7;
        ಆರಂಭ = src - 7;
    }
    c = utf8_prev_Char_ಸುರಕ್ಷಿತ_Body(ಆರಂಭ, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_ಸೂಚ್ಯಂಕ(trie, 0xffff, c);
    ಹಿಂತಿರುಗಿ (idx << 3) | i;
}

ನಾಮಾಕಾಶ {

ಸಾಲೊಳಗೆ uint32_t get_ಮೌಲ್ಯ(UCPTrie_ದತ್ತಾಂಶ ದತ್ತಾಂಶ, UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ, int32_t ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ) {
    ಆಯ್ಕೆ (ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ptr16[ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ];
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ptr32[ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ];
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ptr8[ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ];
    ಪೂರ್ವನಿಯೋಜಿತ:
        // Unreachable if the trie is properly initialized.
        ಹಿಂತಿರುಗಿ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 c) {
    int32_t ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ;
    ಆದರೆ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ = c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ = UCPTRIE_CP_ಸೂಚ್ಯಂಕ(trie, fastMax, c);
    }
    ಹಿಂತಿರುಗಿ get_ಮೌಲ್ಯ(trie->ದತ್ತಾಂಶ, (UCPTrie_ಮೌಲ್ಯ_ಅಗಲ)trie->ಮೌಲ್ಯ_ಅಗಲ, ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ);
}

ನಾಮಾಕಾಶ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

ಸಾಲೊಳಗೆ uint32_t maybe_Filter_ಮೌಲ್ಯ(uint32_t value, uint32_t trie_Null_ಮೌಲ್ಯ, uint32_t null_ಮೌಲ್ಯ,
                                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    ಆದರೆ (value == trie_Null_ಮೌಲ್ಯ) {
        value = null_ಮೌಲ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filter != ಶೂನ್ಯಸೂಚಕ) {
        value = filter(context, value);
    }
    ಹಿಂತಿರುಗಿ value;
}

UChar32 getRange(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *t, UChar32 ಆರಂಭ,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಆದರೆ (static_cast<uint32_t>(ಆರಂಭ) > MAX_UNICODE) {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
    ಸ್ಥಿರಾಂಕ UCPTrie *trie = reinterpret_cast<ಸ್ಥಿರಾಂಕ UCPTrie *>(t);
    UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ = static_cast<UCPTrie_ಮೌಲ್ಯ_ಅಗಲ>(trie->ಮೌಲ್ಯ_ಅಗಲ);
    ಆದರೆ (ಆರಂಭ >= trie->ಮೇಲಿನ_ಆರಂಭ) {
        ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
            int32_t di = trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;
            uint32_t value = get_ಮೌಲ್ಯ(trie->ದತ್ತಾಂಶ, ಮೌಲ್ಯ_ಅಗಲ, di);
            ಆದರೆ (filter != ಶೂನ್ಯಸೂಚಕ) { value = filter(context, value); }
            *p_ಮೌಲ್ಯ = value;
        }
        ಹಿಂತಿರುಗಿ MAX_UNICODE;
    }

    uint32_t null_ಮೌಲ್ಯ = trie->null_ಮೌಲ್ಯ;
    ಆದರೆ (filter != ಶೂನ್ಯಸೂಚಕ) { null_ಮೌಲ್ಯ = filter(context, null_ಮೌಲ್ಯ); }
    ಸ್ಥಿರಾಂಕ uint16_t *ಸೂಚ್ಯಂಕ = trie->ಸೂಚ್ಯಂಕ;

    int32_t prev_I3_ಖಂಡ = -1;
    int32_t prev_ಖಂಡ = -1;
    UChar32 c = ಆರಂಭ;
    uint32_t trie_ಮೌಲ್ಯ, value = null_ಮೌಲ್ಯ;
    ತಾರ್ಕಿಕ have_ಮೌಲ್ಯ = ಸುಳ್ಳು;
    ಮಾಡು {
        int32_t i3_ಖಂಡ;
        int32_t i3;
        int32_t i3_ಖಂಡ_ಉದ್ದಳತೆ;
        int32_t ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        ಆದರೆ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_ಖಂಡ = 0;
            i3 = c >> UCPTRIE_FAST_ಸರಿಸು;
            i3_ಖಂಡ_ಉದ್ದಳತೆ = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ : UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
            ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_ಸರಿಸು_1;
            ಆದರೆ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ಮೇಲಿನ_ಆರಂಭ);
                i1 += UCPTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ - UCPTRIE_OMITTED_BMP_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                U_ASSERT(c < trie->ಮೇಲಿನ_ಆರಂಭ && trie->ಮೇಲಿನ_ಆರಂಭ > UCPTRIE_SMALL_ಎಲ್ಲೆ);
                i1 += UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
            }
            i3_ಖಂಡ = trie->ಸೂಚ್ಯಂಕ[
                static_cast<int32_t>(trie->ಸೂಚ್ಯಂಕ[i1]) + ((c >> UCPTRIE_ಸರಿಸು_2) & UCPTRIE_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ)];
            ಆದರೆ (i3_ಖಂಡ == prev_I3_ಖಂಡ && (c - ಆರಂಭ) >= UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು - 1)) == 0);
                c += UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು;
                ಮುಂದುವರಿ;
            }
            prev_I3_ಖಂಡ = i3_ಖಂಡ;
            ಆದರೆ (i3_ಖಂಡ == trie->index3_Null_ಸ್ಥಾನಾಂತರ) {
                // This is the index-3 null block.
                ಆದರೆ (have_ಮೌಲ್ಯ) {
                    ಆದರೆ (null_ಮೌಲ್ಯ != value) {
                        ಹಿಂತಿರುಗಿ c - 1;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    trie_ಮೌಲ್ಯ = trie->null_ಮೌಲ್ಯ;
                    value = null_ಮೌಲ್ಯ;
                    ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) { *p_ಮೌಲ್ಯ = null_ಮೌಲ್ಯ; }
                    have_ಮೌಲ್ಯ = ನಿಜ;
                }
                prev_ಖಂಡ = trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
                c = (c + UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು) & ~(UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು - 1);
                ಮುಂದುವರಿ;
            }
            i3 = (c >> UCPTRIE_ಸರಿಸು_3) & UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಮುಖವಾಡ;
            i3_ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ;
            ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        }
        // Enumerate data blocks for one index-3 block.
        ಮಾಡು {
            int32_t ಖಂಡ;
            ಆದರೆ ((i3_ಖಂಡ & 0x8000) == 0) {
                ಖಂಡ = ಸೂಚ್ಯಂಕ[i3_ಖಂಡ + i3];
            } ಇಲ್ಲದಿದ್ದರೆ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t ಗುಂಪು = (i3_ಖಂಡ & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                ಖಂಡ = (static_cast<int32_t>(ಸೂಚ್ಯಂಕ[ಗುಂಪು++]) << (2 + (2 * gi))) & 0x30000;
                ಖಂಡ |= ಸೂಚ್ಯಂಕ[ಗುಂಪು + gi];
            }
            ಆದರೆ (ಖಂಡ == prev_ಖಂಡ && (c - ಆರಂಭ) >= ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ - 1)) == 0);
                c += ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t ದತ್ತಾಂಶ_ಮುಖವಾಡ = ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ - 1;
                prev_ಖಂಡ = ಖಂಡ;
                ಆದರೆ (ಖಂಡ == trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ) {
                    // This is the data null block.
                    ಆದರೆ (have_ಮೌಲ್ಯ) {
                        ಆದರೆ (null_ಮೌಲ್ಯ != value) {
                            ಹಿಂತಿರುಗಿ c - 1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        trie_ಮೌಲ್ಯ = trie->null_ಮೌಲ್ಯ;
                        value = null_ಮೌಲ್ಯ;
                        ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) { *p_ಮೌಲ್ಯ = null_ಮೌಲ್ಯ; }
                        have_ಮೌಲ್ಯ = ನಿಜ;
                    }
                    c = (c + ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) & ~ದತ್ತಾಂಶ_ಮುಖವಾಡ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    int32_t di = ಖಂಡ + (c & ದತ್ತಾಂಶ_ಮುಖವಾಡ);
                    uint32_t trieValue2 = get_ಮೌಲ್ಯ(trie->ದತ್ತಾಂಶ, ಮೌಲ್ಯ_ಅಗಲ, di);
                    ಆದರೆ (have_ಮೌಲ್ಯ) {
                        ಆದರೆ (trieValue2 != trie_ಮೌಲ್ಯ) {
                            ಆದರೆ (filter == ಶೂನ್ಯಸೂಚಕ ||
                                    maybe_Filter_ಮೌಲ್ಯ(trieValue2, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                                     filter, context) != value) {
                                ಹಿಂತಿರುಗಿ c - 1;
                            }
                            trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        trie_ಮೌಲ್ಯ = trieValue2;
                        value = maybe_Filter_ಮೌಲ್ಯ(trieValue2, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                                 filter, context);
                        ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) { *p_ಮೌಲ್ಯ = value; }
                        have_ಮೌಲ್ಯ = ನಿಜ;
                    }
                    ಆಗ ((++c & ದತ್ತಾಂಶ_ಮುಖವಾಡ) != 0) {
                        trieValue2 = get_ಮೌಲ್ಯ(trie->ದತ್ತಾಂಶ, ಮೌಲ್ಯ_ಅಗಲ, ++di);
                        ಆದರೆ (trieValue2 != trie_ಮೌಲ್ಯ) {
                            ಆದರೆ (filter == ಶೂನ್ಯಸೂಚಕ ||
                                    maybe_Filter_ಮೌಲ್ಯ(trieValue2, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                                     filter, context) != value) {
                                ಹಿಂತಿರುಗಿ c - 1;
                            }
                            trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ಆಗ (++i3 < i3_ಖಂಡ_ಉದ್ದಳತೆ);
    } ಆಗ (c < trie->ಮೇಲಿನ_ಆರಂಭ);
    U_ASSERT(have_ಮೌಲ್ಯ);
    int32_t di = trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;
    uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ = get_ಮೌಲ್ಯ(trie->ದತ್ತಾಂಶ, ಮೌಲ್ಯ_ಅಗಲ, di);
    ಆದರೆ (maybe_Filter_ಮೌಲ್ಯ(ಮೇಲಿನ_ಮೌಲ್ಯ, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                         filter, context) != value) {
        ಹಿಂತಿರುಗಿ c - 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_ಆಂತರಿಕ_Get_Range(UCPTrieGetRange *getRange,
                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *trie, UChar32 ಆರಂಭ,
                         UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                         UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಆದರೆ (option == UCPMAP_RANGE_ಸಾಮಾನ್ಯ) {
        ಹಿಂತಿರುಗಿ getRange(trie, ಆರಂಭ, filter, context, p_ಮೌಲ್ಯ);
    }
    uint32_t value;
    ಆದರೆ (p_ಮೌಲ್ಯ == ಶೂನ್ಯಸೂಚಕ) {
        // We need to examine the range value even if the caller does not want it.
        p_ಮೌಲ್ಯ = &value;
    }
    UChar32 surr_ಕೊನೆ = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 ಕೊನೆ = getRange(trie, ಆರಂಭ, filter, context, p_ಮೌಲ್ಯ);
    ಆದರೆ (ಕೊನೆ < 0xd7ff || ಆರಂಭ > surr_ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಕೊನೆ;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ಆದರೆ (*p_ಮೌಲ್ಯ == surrogate_ಮೌಲ್ಯ) {
        ಆದರೆ (ಕೊನೆ >= surr_ಕೊನೆ) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ಹಿಂತಿರುಗಿ ಕೊನೆ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (ಆರಂಭ <= 0xd7ff) {
            ಹಿಂತಿರುಗಿ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_ಮೌಲ್ಯ = surrogate_ಮೌಲ್ಯ;
        ಆದರೆ (ಕೊನೆ > surr_ಕೊನೆ) {
            ಹಿಂತಿರುಗಿ surr_ಕೊನೆ;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_ಕೊನೆ + 1, filter, context, &value2);
    ಆದರೆ (value2 == surrogate_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ end2;
    }
    ಹಿಂತಿರುಗಿ surr_ಕೊನೆ;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UChar32 ಆರಂಭ,
                 UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಹಿಂತಿರುಗಿ ucptrie_ಆಂತರಿಕ_Get_Range(getRange, trie, ಆರಂಭ,
                                    option, surrogate_ಮೌಲ್ಯ,
                                    filter, context, p_ಮೌಲ್ಯ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_ಗಮ್ಯ_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ UCPTrie *trie,
                 ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t capacity,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ = (UCPTrie_ಮೌಲ್ಯ_ಅಗಲ)trie->ಮೌಲ್ಯ_ಅಗಲ;
    ಆದರೆ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ಮೌಲ್ಯ_ಅಗಲ < UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16 || UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 < ಮೌಲ್ಯ_ಅಗಲ ||
            capacity < 0 ||
            (capacity > 0 && (ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(ದತ್ತಾಂಶ, 3) != 0)))) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t length = (int32_t)ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ) + trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2;
    ಆಯ್ಕೆ (ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        length += trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 2;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        length += trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        length += trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // unreachable
        ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (capacity < length) {
        *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ length;
    }

    ಅಕ್ಷರ *ಬೈಟುಗಳು = (ಅಕ್ಷರ *)ದತ್ತಾಂಶ;
    UCPTrie_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ = (UCPTrie_ಶೀರ್ಷಿಕೆ *)ಬೈಟುಗಳು;
    ಶೀರ್ಷಿಕೆ->signature = UCPTRIE_SIG;  // "Tri3"
    ಶೀರ್ಷಿಕೆ->options = (uint16_t)(
        ((trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ & 0xf0000) >> 4) |
        ((trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ & 0xf0000) >> 8) |
        (trie->type << 6) |
        ಮೌಲ್ಯ_ಅಗಲ);
    ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = (uint16_t)trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = (uint16_t)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ಶೀರ್ಷಿಕೆ->index3_Null_ಸ್ಥಾನಾಂತರ = trie->index3_Null_ಸ್ಥಾನಾಂತರ;
    ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ = (uint16_t)trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    ಶೀರ್ಷಿಕೆ->shifted_ಮೇಲಿನ_ಆರಂಭ = trie->ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_2;
    ಬೈಟುಗಳು += ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ);

    uprv_memcpy(ಬೈಟುಗಳು, trie->ಸೂಚ್ಯಂಕ, trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2);
    ಬೈಟುಗಳು += trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2;

    ಆಯ್ಕೆ (ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        uprv_memcpy(ಬೈಟುಗಳು, trie->ದತ್ತಾಂಶ.ptr16, trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 2);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        uprv_memcpy(ಬೈಟುಗಳು, trie->ದತ್ತಾಂಶ.ptr32, trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        uprv_memcpy(ಬೈಟುಗಳು, trie->ದತ್ತಾಂಶ.ptr8, trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // unreachable
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ length;
}

ನಾಮಾಕಾಶ {

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
ದೀರ್ಘ ಎಣಿಕೆ_Null(ಸ್ಥಿರಾಂಕ UCPTrie *trie) {
    uint32_t null_ಮೌಲ್ಯ=trie->null_ಮೌಲ್ಯ;
    int32_t length=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ count=0;
    ಆಯ್ಕೆ (trie->ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        ಗಾಗಿ(int32_t i=0; i<length; ++i) {
            ಆದರೆ(trie->ದತ್ತಾಂಶ.ptr16[i]==null_ಮೌಲ್ಯ) { ++count; }
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        ಗಾಗಿ(int32_t i=0; i<length; ++i) {
            ಆದರೆ(trie->ದತ್ತಾಂಶ.ptr32[i]==null_ಮೌಲ್ಯ) { ++count; }
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        ಗಾಗಿ(int32_t i=0; i<length; ++i) {
            ಆದರೆ(trie->ದತ್ತಾಂಶ.ptr8[i]==null_ಮೌಲ್ಯ) { ++count; }
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // unreachable
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ count;
}

U_CFUNC ಶೂನ್ಯ
ucptrie_printLengths(ಸ್ಥಿರಾಂಕ UCPTrie *trie, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *which) {
    ದೀರ್ಘ ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=(ದೀರ್ಘ)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ ಒಟ್ಟು_ಉದ್ದಳತೆ=(ದೀರ್ಘ)ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ)+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2+
            ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*(trie->ಮೌಲ್ಯ_ಅಗಲ==UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16 ? 2 :
                        trie->ಮೌಲ್ಯ_ಅಗಲ==UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32 ? 4 : 1);
    ಬರೆ("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, ಎಣಿಕೆ_Null(trie), ಒಟ್ಟು_ಉದ್ದಳತೆ);
}
#ಆದರೆ_ಅಂತ್ಯ

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ಸ್ಥಿರಾಂಕ UCPMap *map, UChar32 c) {
    ಹಿಂತಿರುಗಿ ucptrie_get(reinterpret_cast<ಸ್ಥಿರಾಂಕ UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ಸ್ಥಿರಾಂಕ UCPMap *map, UChar32 ಆರಂಭ,
                UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಹಿಂತಿರುಗಿ ucptrie_getRange(reinterpret_cast<ಸ್ಥಿರಾಂಕ UCPTrie *>(map), ಆರಂಭ,
                            option, surrogate_ಮೌಲ್ಯ,
                            filter, context, p_ಮೌಲ್ಯ);
}
