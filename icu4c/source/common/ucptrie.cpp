// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
#   include <stdio.h>
#جيڪڏهن_پورو

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ٻٽو(UCPTrieType type,
                       UCPTrie_قدر_ويڪر قدر_ويڪر,
                       ثابت فراغ* معلومات U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_ڊيگهائي,
                       UError_ڪوڊ* p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    إذا (length <= 0 || (U_اشارو_نقاب_LSB(معلومات, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            قدر_ويڪر < UCPTRIE_قدر_بٽ_ANY || UCPTRIE_قدر_بٽ_8 < قدر_ويڪر) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    // Enough data for a trie header?
    إذا (length < (int32_t)حجم(UCPTrie_سرو)) {
        *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    // Check the signature.
    ثابت UCPTrie_سرو *سرو = (ثابت UCPTrie_سرو *)معلومات;
    إذا (سرو->signature != UCPTRIE_SIG) {
        *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    int32_t options = سرو->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t قدر_ويڪر_Int = options & UCPTRIE_OPTIONS_قدر_بٽ_نقاب;
    إذا (typeInt > UCPTRIE_TYPE_SMALL || قدر_ويڪر_Int > UCPTRIE_قدر_بٽ_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_نقاب) != 0) {
        *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_قدر_ويڪر actual_قدر_ويڪر = (UCPTrie_قدر_ويڪر)قدر_ويڪر_Int;
    إذا (type < 0) {
        type = actualType;
    }
    إذا (قدر_ويڪر < 0) {
        قدر_ويڪر = actual_قدر_ويڪر;
    }
    إذا (type != actualType || قدر_ويڪر != actual_قدر_ويڪر) {
        *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    // Get the length values and offsets.
    UCPTrie عارضي_Trie;
    uprv_memset(&عارضي_Trie, 0, حجم(عارضي_Trie));
    عارضي_Trie.اشاريو_ڊيگهائي = سرو->اشاريو_ڊيگهائي;
    عارضي_Trie.معلومات_ڊيگهائي =
        ((options & UCPTRIE_OPTIONS_معلومات_ڊيگهائي_نقاب) << 4) | سرو->معلومات_ڊيگهائي;
    عارضي_Trie.index3_Null_وٿي = سرو->index3_Null_وٿي;
    عارضي_Trie.معلومات_Null_وٿي =
        ((options & UCPTRIE_OPTIONS_معلومات_NULL_وٿي_نقاب) << 8) | سرو->معلومات_Null_وٿي;

    عارضي_Trie.مٿاهون_شروعات = سرو->shifted_مٿاهون_شروعات << UCPTRIE_سرڪايو_2;
    عارضي_Trie.shifted12_مٿاهون_شروعات = (عارضي_Trie.مٿاهون_شروعات + 0xfff) >> 12;
    عارضي_Trie.type = type;
    عارضي_Trie.قدر_ويڪر = قدر_ويڪر;

    // Calculate the actual length.
    int32_t actual_ڊيگهائي = (int32_t)حجم(UCPTrie_سرو) + عارضي_Trie.اشاريو_ڊيگهائي * 2;
    إذا (قدر_ويڪر == UCPTRIE_قدر_بٽ_16) {
        actual_ڊيگهائي += عارضي_Trie.معلومات_ڊيگهائي * 2;
    } وإلا إذا (قدر_ويڪر == UCPTRIE_قدر_بٽ_32) {
        actual_ڊيگهائي += عارضي_Trie.معلومات_ڊيگهائي * 4;
    } وإلا {
        actual_ڊيگهائي += عارضي_Trie.معلومات_ڊيگهائي;
    }
    إذا (length < actual_ڊيگهائي) {
        *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        أرجع خالي_اشارو;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    إذا (trie == خالي_اشارو) {
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(trie, &عارضي_Trie, حجم(عارضي_Trie));
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#جيڪڏهن_پورو

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(سرو + 1);
    trie->اشاريو = p16;
    p16 += trie->اشاريو_ڊيگهائي;

    // Get the data.
    int32_t null_قدر_وٿي = trie->معلومات_Null_وٿي;
    إذا (null_قدر_وٿي >= trie->معلومات_ڊيگهائي) {
        null_قدر_وٿي = trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي;
    }
    بدل (قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        trie->معلومات.ptr16 = p16;
        trie->null_قدر = trie->معلومات.ptr16[null_قدر_وٿي];
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        trie->معلومات.ptr32 = (ثابت uint32_t *)p16;
        trie->null_قدر = trie->معلومات.ptr32[null_قدر_وٿي];
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        trie->معلومات.ptr8 = (ثابت uint8_t *)p16;
        trie->null_قدر = trie->معلومات.ptr8[null_قدر_وٿي];
        اكسر;
    افتراضي:
        // Unreachable because valueWidth was checked above.
        *p_Error_ڪوڊ = U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    إذا (p_Actual_ڊيگهائي != خالي_اشارو) {
        *p_Actual_ڊيگهائي = actual_ڊيگهائي;
    }
    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ثابت UCPTrie *trie) {
    أرجع (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_قدر_ويڪر U_EXPORT2
ucptrie_get_قدر_ويڪر(ثابت UCPTrie *trie) {
    أرجع (UCPTrie_قدر_ويڪر)trie->قدر_ويڪر;
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندروني_Small_اشاريو(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_سرڪايو_1;
    إذا (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->مٿاهون_شروعات);
        i1 += UCPTRIE_BMP_اشاريو_ڊيگهائي - UCPTRIE_OMITTED_BMP_اشاريو_1_ڊيگهائي;
    } وإلا {
        U_ASSERT((uint32_t)c < (uint32_t)trie->مٿاهون_شروعات && trie->مٿاهون_شروعات > UCPTRIE_SMALL_حد_بندي);
        i1 += UCPTRIE_SMALL_اشاريو_ڊيگهائي;
    }
    int32_t i3_بلاڪ = trie->اشاريو[
        (int32_t)trie->اشاريو[i1] + ((c >> UCPTRIE_سرڪايو_2) & UCPTRIE_اشاريو_2_نقاب)];
    int32_t i3 = (c >> UCPTRIE_سرڪايو_3) & UCPTRIE_اشاريو_3_نقاب;
    int32_t معلومات_بلاڪ;
    إذا ((i3_بلاڪ & 0x8000) == 0) {
        // 16-bit indexes
        معلومات_بلاڪ = trie->اشاريو[i3_بلاڪ + i3];
    } وإلا {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_بلاڪ = (i3_بلاڪ & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        معلومات_بلاڪ = ((int32_t)trie->اشاريو[i3_بلاڪ++] << (2 + (2 * i3))) & 0x30000;
        معلومات_بلاڪ |= trie->اشاريو[i3_بلاڪ + i3];
    }
    أرجع معلومات_بلاڪ + (c & UCPTRIE_SMALL_معلومات_نقاب);
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندروني_Small_U8_اشاريو(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    إذا (c >= trie->مٿاهون_شروعات) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        أرجع trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي;
    }
    أرجع ucptrie_اندروني_Small_اشاريو(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندروني_U8_Prev_اشاريو(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *شروعات, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    إذا ((src - شروعات) <= 7) {
        i = length = (int32_t)(src - شروعات);
    } وإلا {
        i = length = 7;
        شروعات = src - 7;
    }
    c = utf8_prev_Char_محفوظ_Body(شروعات, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_اشاريو(trie, 0xffff, c);
    أرجع (idx << 3) | i;
}

نطاق {

مضمن uint32_t get_قدر(UCPTrie_معلومات معلومات, UCPTrie_قدر_ويڪر قدر_ويڪر, int32_t معلومات_اشاريو) {
    بدل (قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        أرجع معلومات.ptr16[معلومات_اشاريو];
    حالة UCPTRIE_قدر_بٽ_32:
        أرجع معلومات.ptr32[معلومات_اشاريو];
    حالة UCPTRIE_قدر_بٽ_8:
        أرجع معلومات.ptr8[معلومات_اشاريو];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        أرجع 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t معلومات_اشاريو;
    إذا ((uint32_t)c <= 0x7f) {
        // linear ASCII
        معلومات_اشاريو = c;
    } وإلا {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        معلومات_اشاريو = UCPTRIE_CP_اشاريو(trie, fastMax, c);
    }
    أرجع get_قدر(trie->معلومات, (UCPTrie_قدر_ويڪر)trie->قدر_ويڪر, معلومات_اشاريو);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybe_Filter_قدر(uint32_t value, uint32_t trie_Null_قدر, uint32_t null_قدر,
                                 UCPMap_قدر_Filter *filter, ثابت فراغ *context) {
    إذا (value == trie_Null_قدر) {
        value = null_قدر;
    } وإلا إذا (filter != خالي_اشارو) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 شروعات,
                 UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    إذا (static_cast<uint32_t>(شروعات) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_قدر_ويڪر قدر_ويڪر = static_cast<UCPTrie_قدر_ويڪر>(trie->قدر_ويڪر);
    إذا (شروعات >= trie->مٿاهون_شروعات) {
        إذا (p_قدر != خالي_اشارو) {
            int32_t di = trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي;
            uint32_t value = get_قدر(trie->معلومات, قدر_ويڪر, di);
            إذا (filter != خالي_اشارو) { value = filter(context, value); }
            *p_قدر = value;
        }
        أرجع MAX_UNICODE;
    }

    uint32_t null_قدر = trie->null_قدر;
    إذا (filter != خالي_اشارو) { null_قدر = filter(context, null_قدر); }
    ثابت uint16_t *اشاريو = trie->اشاريو;

    int32_t prev_I3_بلاڪ = -1;
    int32_t prev_بلاڪ = -1;
    UChar32 c = شروعات;
    uint32_t trie_قدر, value = null_قدر;
    منطقي have_قدر = خطأ;
    افعل {
        int32_t i3_بلاڪ;
        int32_t i3;
        int32_t i3_بلاڪ_ڊيگهائي;
        int32_t معلومات_بلاڪ_ڊيگهائي;
        إذا (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_بلاڪ = 0;
            i3 = c >> UCPTRIE_FAST_سرڪايو;
            i3_بلاڪ_ڊيگهائي = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_اشاريو_ڊيگهائي : UCPTRIE_SMALL_اشاريو_ڊيگهائي;
            معلومات_بلاڪ_ڊيگهائي = UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي;
        } وإلا {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_سرڪايو_1;
            إذا (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->مٿاهون_شروعات);
                i1 += UCPTRIE_BMP_اشاريو_ڊيگهائي - UCPTRIE_OMITTED_BMP_اشاريو_1_ڊيگهائي;
            } وإلا {
                U_ASSERT(c < trie->مٿاهون_شروعات && trie->مٿاهون_شروعات > UCPTRIE_SMALL_حد_بندي);
                i1 += UCPTRIE_SMALL_اشاريو_ڊيگهائي;
            }
            i3_بلاڪ = trie->اشاريو[
                static_cast<int32_t>(trie->اشاريو[i1]) + ((c >> UCPTRIE_سرڪايو_2) & UCPTRIE_اشاريو_2_نقاب)];
            إذا (i3_بلاڪ == prev_I3_بلاڪ && (c - شروعات) >= UCPTRIE_CP_في_اشاريو_2_داخلا) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_في_اشاريو_2_داخلا - 1)) == 0);
                c += UCPTRIE_CP_في_اشاريو_2_داخلا;
                استمر;
            }
            prev_I3_بلاڪ = i3_بلاڪ;
            إذا (i3_بلاڪ == trie->index3_Null_وٿي) {
                // This is the index-3 null block.
                إذا (have_قدر) {
                    إذا (null_قدر != value) {
                        أرجع c - 1;
                    }
                } وإلا {
                    trie_قدر = trie->null_قدر;
                    value = null_قدر;
                    إذا (p_قدر != خالي_اشارو) { *p_قدر = null_قدر; }
                    have_قدر = سچ;
                }
                prev_بلاڪ = trie->معلومات_Null_وٿي;
                c = (c + UCPTRIE_CP_في_اشاريو_2_داخلا) & ~(UCPTRIE_CP_في_اشاريو_2_داخلا - 1);
                استمر;
            }
            i3 = (c >> UCPTRIE_سرڪايو_3) & UCPTRIE_اشاريو_3_نقاب;
            i3_بلاڪ_ڊيگهائي = UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي;
            معلومات_بلاڪ_ڊيگهائي = UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
        }
        // Enumerate data blocks for one index-3 block.
        افعل {
            int32_t بلاڪ;
            إذا ((i3_بلاڪ & 0x8000) == 0) {
                بلاڪ = اشاريو[i3_بلاڪ + i3];
            } وإلا {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t گروهه = (i3_بلاڪ & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                بلاڪ = (static_cast<int32_t>(اشاريو[گروهه++]) << (2 + (2 * gi))) & 0x30000;
                بلاڪ |= اشاريو[گروهه + gi];
            }
            إذا (بلاڪ == prev_بلاڪ && (c - شروعات) >= معلومات_بلاڪ_ڊيگهائي) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (معلومات_بلاڪ_ڊيگهائي - 1)) == 0);
                c += معلومات_بلاڪ_ڊيگهائي;
            } وإلا {
                int32_t معلومات_نقاب = معلومات_بلاڪ_ڊيگهائي - 1;
                prev_بلاڪ = بلاڪ;
                إذا (بلاڪ == trie->معلومات_Null_وٿي) {
                    // This is the data null block.
                    إذا (have_قدر) {
                        إذا (null_قدر != value) {
                            أرجع c - 1;
                        }
                    } وإلا {
                        trie_قدر = trie->null_قدر;
                        value = null_قدر;
                        إذا (p_قدر != خالي_اشارو) { *p_قدر = null_قدر; }
                        have_قدر = سچ;
                    }
                    c = (c + معلومات_بلاڪ_ڊيگهائي) & ~معلومات_نقاب;
                } وإلا {
                    int32_t di = بلاڪ + (c & معلومات_نقاب);
                    uint32_t trieValue2 = get_قدر(trie->معلومات, قدر_ويڪر, di);
                    إذا (have_قدر) {
                        إذا (trieValue2 != trie_قدر) {
                            إذا (filter == خالي_اشارو ||
                                    maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_قدر = trieValue2;  // may or may not help
                        }
                    } وإلا {
                        trie_قدر = trieValue2;
                        value = maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                 filter, context);
                        إذا (p_قدر != خالي_اشارو) { *p_قدر = value; }
                        have_قدر = سچ;
                    }
                    طالما ((++c & معلومات_نقاب) != 0) {
                        trieValue2 = get_قدر(trie->معلومات, قدر_ويڪر, ++di);
                        إذا (trieValue2 != trie_قدر) {
                            إذا (filter == خالي_اشارو ||
                                    maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_قدر = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } طالما (++i3 < i3_بلاڪ_ڊيگهائي);
    } طالما (c < trie->مٿاهون_شروعات);
    U_ASSERT(have_قدر);
    int32_t di = trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي;
    uint32_t مٿاهون_قدر = get_قدر(trie->معلومات, قدر_ويڪر, di);
    إذا (maybe_Filter_قدر(مٿاهون_قدر, trie->null_قدر, null_قدر,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_اندروني_Get_Range(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 شروعات,
                         UCPMapRangeOption option, uint32_t surrogate_قدر,
                         UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    إذا (option == UCPMAP_RANGE_عام) {
        أرجع getRange(trie, شروعات, filter, context, p_قدر);
    }
    uint32_t value;
    إذا (p_قدر == خالي_اشارو) {
        // We need to examine the range value even if the caller does not want it.
        p_قدر = &value;
    }
    UChar32 surr_پڇاڙي = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 پڇاڙي = getRange(trie, شروعات, filter, context, p_قدر);
    إذا (پڇاڙي < 0xd7ff || شروعات > surr_پڇاڙي) {
        أرجع پڇاڙي;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    إذا (*p_قدر == surrogate_قدر) {
        إذا (پڇاڙي >= surr_پڇاڙي) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            أرجع پڇاڙي;
        }
    } وإلا {
        إذا (شروعات <= 0xd7ff) {
            أرجع 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_قدر = surrogate_قدر;
        إذا (پڇاڙي > surr_پڇاڙي) {
            أرجع surr_پڇاڙي;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_پڇاڙي + 1, filter, context, &value2);
    إذا (value2 == surrogate_قدر) {
        أرجع end2;
    }
    أرجع surr_پڇاڙي;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 شروعات,
                 UCPMapRangeOption option, uint32_t surrogate_قدر,
                 UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع ucptrie_اندروني_Get_Range(getRange, trie, شروعات,
                                    option, surrogate_قدر,
                                    filter, context, p_قدر);
}

U_CAPI int32_t U_EXPORT2
ucptrie_منزل_ٻٽو(ثابت UCPTrie *trie,
                 فراغ *معلومات, int32_t capacity,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_قدر_ويڪر قدر_ويڪر = (UCPTrie_قدر_ويڪر)trie->قدر_ويڪر;
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قدر_ويڪر < UCPTRIE_قدر_بٽ_16 || UCPTRIE_قدر_بٽ_8 < قدر_ويڪر ||
            capacity < 0 ||
            (capacity > 0 && (معلومات == خالي_اشارو || (U_اشارو_نقاب_LSB(معلومات, 3) != 0)))) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    int32_t length = (int32_t)حجم(UCPTrie_سرو) + trie->اشاريو_ڊيگهائي * 2;
    بدل (قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        length += trie->معلومات_ڊيگهائي * 2;
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        length += trie->معلومات_ڊيگهائي * 4;
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        length += trie->معلومات_ڊيگهائي;
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إذا (capacity < length) {
        *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        أرجع length;
    }

    حرف *بائيٽون = (حرف *)معلومات;
    UCPTrie_سرو *سرو = (UCPTrie_سرو *)بائيٽون;
    سرو->signature = UCPTRIE_SIG;  // "Tri3"
    سرو->options = (uint16_t)(
        ((trie->معلومات_ڊيگهائي & 0xf0000) >> 4) |
        ((trie->معلومات_Null_وٿي & 0xf0000) >> 8) |
        (trie->type << 6) |
        قدر_ويڪر);
    سرو->اشاريو_ڊيگهائي = (uint16_t)trie->اشاريو_ڊيگهائي;
    سرو->معلومات_ڊيگهائي = (uint16_t)trie->معلومات_ڊيگهائي;
    سرو->index3_Null_وٿي = trie->index3_Null_وٿي;
    سرو->معلومات_Null_وٿي = (uint16_t)trie->معلومات_Null_وٿي;
    سرو->shifted_مٿاهون_شروعات = trie->مٿاهون_شروعات >> UCPTRIE_سرڪايو_2;
    بائيٽون += حجم(UCPTrie_سرو);

    uprv_memcpy(بائيٽون, trie->اشاريو, trie->اشاريو_ڊيگهائي * 2);
    بائيٽون += trie->اشاريو_ڊيگهائي * 2;

    بدل (قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        uprv_memcpy(بائيٽون, trie->معلومات.ptr16, trie->معلومات_ڊيگهائي * 2);
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        uprv_memcpy(بائيٽون, trie->معلومات.ptr32, trie->معلومات_ڊيگهائي * 4);
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        uprv_memcpy(بائيٽون, trie->معلومات.ptr8, trie->معلومات_ڊيگهائي);
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    أرجع length;
}

نطاق {

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
ڊگهو ڳڻپ_Null(ثابت UCPTrie *trie) {
    uint32_t null_قدر=trie->null_قدر;
    int32_t length=trie->معلومات_ڊيگهائي;
    ڊگهو count=0;
    بدل (trie->قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->معلومات.ptr16[i]==null_قدر) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->معلومات.ptr32[i]==null_قدر) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->معلومات.ptr8[i]==null_قدر) { ++count; }
        }
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    أرجع count;
}

U_CFUNC فراغ
ucptrie_printLengths(ثابت UCPTrie *trie, ثابت حرف *which) {
    ڊگهو اشاريو_ڊيگهائي=trie->اشاريو_ڊيگهائي;
    ڊگهو معلومات_ڊيگهائي=(ڊگهو)trie->معلومات_ڊيگهائي;
    ڊگهو ڪل_ڊيگهائي=(ڊگهو)حجم(UCPTrie_سرو)+اشاريو_ڊيگهائي*2+
            معلومات_ڊيگهائي*(trie->قدر_ويڪر==UCPTRIE_قدر_بٽ_16 ? 2 :
                        trie->قدر_ويڪر==UCPTRIE_قدر_بٽ_32 ? 4 : 1);
    لکو("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, اشاريو_ڊيگهائي, معلومات_ڊيگهائي, ڳڻپ_Null(trie), ڪل_ڊيگهائي);
}
#جيڪڏهن_پورو

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    أرجع ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 شروعات,
                UCPMapRangeOption option, uint32_t surrogate_قدر,
                UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), شروعات,
                            option, surrogate_قدر,
                            filter, context, p_قدر);
}
