// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#جے_تعریف_شدہ UCPTRIE_DEBUG
#   include <stdio.h>
#جے_انت

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ثنائی(UCPTrieType type,
                       UCPTrie_قدر_چوڑائی قدر_چوڑائی,
                       ثابت فراغ* انکڑے U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_لمائی,
                       UError_کوڈ* p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    إذا (length <= 0 || (U_اشارہ_گر_نقاب_LSB(انکڑے, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            قدر_چوڑائی < UCPTRIE_قدر_بٹ_ANY || UCPTRIE_قدر_بٹ_8 < قدر_چوڑائی) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    // Enough data for a trie header?
    إذا (length < (int32_t)حجم(UCPTrie_سرخی)) {
        *p_Error_کوڈ = U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    // Check the signature.
    ثابت UCPTrie_سرخی *سرخی = (ثابت UCPTrie_سرخی *)انکڑے;
    إذا (سرخی->signature != UCPTRIE_SIG) {
        *p_Error_کوڈ = U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    int32_t options = سرخی->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t قدر_چوڑائی_Int = options & UCPTRIE_OPTIONS_قدر_بٹ_نقاب;
    إذا (typeInt > UCPTRIE_TYPE_SMALL || قدر_چوڑائی_Int > UCPTRIE_قدر_بٹ_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_نقاب) != 0) {
        *p_Error_کوڈ = U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_قدر_چوڑائی actual_قدر_چوڑائی = (UCPTrie_قدر_چوڑائی)قدر_چوڑائی_Int;
    إذا (type < 0) {
        type = actualType;
    }
    إذا (قدر_چوڑائی < 0) {
        قدر_چوڑائی = actual_قدر_چوڑائی;
    }
    إذا (type != actualType || قدر_چوڑائی != actual_قدر_چوڑائی) {
        *p_Error_کوڈ = U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    // Get the length values and offsets.
    UCPTrie عارضی_Trie;
    uprv_memset(&عارضی_Trie, 0, حجم(عارضی_Trie));
    عارضی_Trie.اشاریہ_لمائی = سرخی->اشاریہ_لمائی;
    عارضی_Trie.انکڑے_لمائی =
        ((options & UCPTRIE_OPTIONS_انکڑے_لمائی_نقاب) << 4) | سرخی->انکڑے_لمائی;
    عارضی_Trie.index3_Null_وتھ = سرخی->index3_Null_وتھ;
    عارضی_Trie.انکڑے_Null_وتھ =
        ((options & UCPTRIE_OPTIONS_انکڑے_NULL_وتھ_نقاب) << 8) | سرخی->انکڑے_Null_وتھ;

    عارضی_Trie.بلند_شروع = سرخی->shifted_بلند_شروع << UCPTRIE_کھسکاؤ_2;
    عارضی_Trie.shifted12_بلند_شروع = (عارضی_Trie.بلند_شروع + 0xfff) >> 12;
    عارضی_Trie.type = type;
    عارضی_Trie.قدر_چوڑائی = قدر_چوڑائی;

    // Calculate the actual length.
    int32_t actual_لمائی = (int32_t)حجم(UCPTrie_سرخی) + عارضی_Trie.اشاریہ_لمائی * 2;
    إذا (قدر_چوڑائی == UCPTRIE_قدر_بٹ_16) {
        actual_لمائی += عارضی_Trie.انکڑے_لمائی * 2;
    } وإلا إذا (قدر_چوڑائی == UCPTRIE_قدر_بٹ_32) {
        actual_لمائی += عارضی_Trie.انکڑے_لمائی * 4;
    } وإلا {
        actual_لمائی += عارضی_Trie.انکڑے_لمائی;
    }
    إذا (length < actual_لمائی) {
        *p_Error_کوڈ = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        أرجع خالی_سوچک;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    إذا (trie == خالی_سوچک) {
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(trie, &عارضی_Trie, حجم(عارضی_Trie));
#جے_تعریف_شدہ UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#جے_انت

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(سرخی + 1);
    trie->اشاریہ = p16;
    p16 += trie->اشاریہ_لمائی;

    // Get the data.
    int32_t null_قدر_وتھ = trie->انکڑے_Null_وتھ;
    إذا (null_قدر_وتھ >= trie->انکڑے_لمائی) {
        null_قدر_وتھ = trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ;
    }
    بدل (قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        trie->انکڑے.ptr16 = p16;
        trie->null_قدر = trie->انکڑے.ptr16[null_قدر_وتھ];
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        trie->انکڑے.ptr32 = (ثابت uint32_t *)p16;
        trie->null_قدر = trie->انکڑے.ptr32[null_قدر_وتھ];
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        trie->انکڑے.ptr8 = (ثابت uint8_t *)p16;
        trie->null_قدر = trie->انکڑے.ptr8[null_قدر_وتھ];
        اكسر;
    افتراضي:
        // Unreachable because valueWidth was checked above.
        *p_Error_کوڈ = U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    إذا (p_Actual_لمائی != خالی_سوچک) {
        *p_Actual_لمائی = actual_لمائی;
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

U_CAPI UCPTrie_قدر_چوڑائی U_EXPORT2
ucptrie_get_قدر_چوڑائی(ثابت UCPTrie *trie) {
    أرجع (UCPTrie_قدر_چوڑائی)trie->قدر_چوڑائی;
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندرونی_Small_اشاریہ(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_کھسکاؤ_1;
    إذا (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->بلند_شروع);
        i1 += UCPTRIE_BMP_اشاریہ_لمائی - UCPTRIE_OMITTED_BMP_اشاریہ_1_لمائی;
    } وإلا {
        U_ASSERT((uint32_t)c < (uint32_t)trie->بلند_شروع && trie->بلند_شروع > UCPTRIE_SMALL_حد_بندی);
        i1 += UCPTRIE_SMALL_اشاریہ_لمائی;
    }
    int32_t i3_بلاک = trie->اشاریہ[
        (int32_t)trie->اشاریہ[i1] + ((c >> UCPTRIE_کھسکاؤ_2) & UCPTRIE_اشاریہ_2_نقاب)];
    int32_t i3 = (c >> UCPTRIE_کھسکاؤ_3) & UCPTRIE_اشاریہ_3_نقاب;
    int32_t انکڑے_بلاک;
    إذا ((i3_بلاک & 0x8000) == 0) {
        // 16-bit indexes
        انکڑے_بلاک = trie->اشاریہ[i3_بلاک + i3];
    } وإلا {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_بلاک = (i3_بلاک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        انکڑے_بلاک = ((int32_t)trie->اشاریہ[i3_بلاک++] << (2 + (2 * i3))) & 0x30000;
        انکڑے_بلاک |= trie->اشاریہ[i3_بلاک + i3];
    }
    أرجع انکڑے_بلاک + (c & UCPTRIE_SMALL_انکڑے_نقاب);
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندرونی_Small_U8_اشاریہ(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    إذا (c >= trie->بلند_شروع) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        أرجع trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ;
    }
    أرجع ucptrie_اندرونی_Small_اشاریہ(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندرونی_U8_Prev_اشاریہ(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *شروع, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    إذا ((src - شروع) <= 7) {
        i = length = (int32_t)(src - شروع);
    } وإلا {
        i = length = 7;
        شروع = src - 7;
    }
    c = utf8_prev_Char_محفوظ_Body(شروع, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_اشاریہ(trie, 0xffff, c);
    أرجع (idx << 3) | i;
}

نطاق {

مضمن uint32_t get_قدر(UCPTrie_انکڑے انکڑے, UCPTrie_قدر_چوڑائی قدر_چوڑائی, int32_t انکڑے_اشاریہ) {
    بدل (قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        أرجع انکڑے.ptr16[انکڑے_اشاریہ];
    حالة UCPTRIE_قدر_بٹ_32:
        أرجع انکڑے.ptr32[انکڑے_اشاریہ];
    حالة UCPTRIE_قدر_بٹ_8:
        أرجع انکڑے.ptr8[انکڑے_اشاریہ];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        أرجع 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t انکڑے_اشاریہ;
    إذا ((uint32_t)c <= 0x7f) {
        // linear ASCII
        انکڑے_اشاریہ = c;
    } وإلا {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        انکڑے_اشاریہ = UCPTRIE_CP_اشاریہ(trie, fastMax, c);
    }
    أرجع get_قدر(trie->انکڑے, (UCPTrie_قدر_چوڑائی)trie->قدر_چوڑائی, انکڑے_اشاریہ);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybe_Filter_قدر(uint32_t value, uint32_t trie_Null_قدر, uint32_t null_قدر,
                                 UCPMap_قدر_Filter *filter, ثابت فراغ *context) {
    إذا (value == trie_Null_قدر) {
        value = null_قدر;
    } وإلا إذا (filter != خالی_سوچک) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 شروع,
                 UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    إذا (static_cast<uint32_t>(شروع) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_قدر_چوڑائی قدر_چوڑائی = static_cast<UCPTrie_قدر_چوڑائی>(trie->قدر_چوڑائی);
    إذا (شروع >= trie->بلند_شروع) {
        إذا (p_قدر != خالی_سوچک) {
            int32_t di = trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ;
            uint32_t value = get_قدر(trie->انکڑے, قدر_چوڑائی, di);
            إذا (filter != خالی_سوچک) { value = filter(context, value); }
            *p_قدر = value;
        }
        أرجع MAX_UNICODE;
    }

    uint32_t null_قدر = trie->null_قدر;
    إذا (filter != خالی_سوچک) { null_قدر = filter(context, null_قدر); }
    ثابت uint16_t *اشاریہ = trie->اشاریہ;

    int32_t prev_I3_بلاک = -1;
    int32_t prev_بلاک = -1;
    UChar32 c = شروع;
    uint32_t trie_قدر, value = null_قدر;
    منطقي have_قدر = خطأ;
    افعل {
        int32_t i3_بلاک;
        int32_t i3;
        int32_t i3_بلاک_لمائی;
        int32_t انکڑے_بلاک_لمائی;
        إذا (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_بلاک = 0;
            i3 = c >> UCPTRIE_FAST_کھسکاؤ;
            i3_بلاک_لمائی = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_اشاریہ_لمائی : UCPTRIE_SMALL_اشاریہ_لمائی;
            انکڑے_بلاک_لمائی = UCPTRIE_FAST_انکڑے_بلاک_لمائی;
        } وإلا {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_کھسکاؤ_1;
            إذا (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->بلند_شروع);
                i1 += UCPTRIE_BMP_اشاریہ_لمائی - UCPTRIE_OMITTED_BMP_اشاریہ_1_لمائی;
            } وإلا {
                U_ASSERT(c < trie->بلند_شروع && trie->بلند_شروع > UCPTRIE_SMALL_حد_بندی);
                i1 += UCPTRIE_SMALL_اشاریہ_لمائی;
            }
            i3_بلاک = trie->اشاریہ[
                static_cast<int32_t>(trie->اشاریہ[i1]) + ((c >> UCPTRIE_کھسکاؤ_2) & UCPTRIE_اشاریہ_2_نقاب)];
            إذا (i3_بلاک == prev_I3_بلاک && (c - شروع) >= UCPTRIE_CP_فی_اشاریہ_2_اندراج) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1)) == 0);
                c += UCPTRIE_CP_فی_اشاریہ_2_اندراج;
                استمر;
            }
            prev_I3_بلاک = i3_بلاک;
            إذا (i3_بلاک == trie->index3_Null_وتھ) {
                // This is the index-3 null block.
                إذا (have_قدر) {
                    إذا (null_قدر != value) {
                        أرجع c - 1;
                    }
                } وإلا {
                    trie_قدر = trie->null_قدر;
                    value = null_قدر;
                    إذا (p_قدر != خالی_سوچک) { *p_قدر = null_قدر; }
                    have_قدر = سچ;
                }
                prev_بلاک = trie->انکڑے_Null_وتھ;
                c = (c + UCPTRIE_CP_فی_اشاریہ_2_اندراج) & ~(UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1);
                استمر;
            }
            i3 = (c >> UCPTRIE_کھسکاؤ_3) & UCPTRIE_اشاریہ_3_نقاب;
            i3_بلاک_لمائی = UCPTRIE_اشاریہ_3_بلاک_لمائی;
            انکڑے_بلاک_لمائی = UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
        }
        // Enumerate data blocks for one index-3 block.
        افعل {
            int32_t بلاک;
            إذا ((i3_بلاک & 0x8000) == 0) {
                بلاک = اشاریہ[i3_بلاک + i3];
            } وإلا {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t گروہ = (i3_بلاک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                بلاک = (static_cast<int32_t>(اشاریہ[گروہ++]) << (2 + (2 * gi))) & 0x30000;
                بلاک |= اشاریہ[گروہ + gi];
            }
            إذا (بلاک == prev_بلاک && (c - شروع) >= انکڑے_بلاک_لمائی) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (انکڑے_بلاک_لمائی - 1)) == 0);
                c += انکڑے_بلاک_لمائی;
            } وإلا {
                int32_t انکڑے_نقاب = انکڑے_بلاک_لمائی - 1;
                prev_بلاک = بلاک;
                إذا (بلاک == trie->انکڑے_Null_وتھ) {
                    // This is the data null block.
                    إذا (have_قدر) {
                        إذا (null_قدر != value) {
                            أرجع c - 1;
                        }
                    } وإلا {
                        trie_قدر = trie->null_قدر;
                        value = null_قدر;
                        إذا (p_قدر != خالی_سوچک) { *p_قدر = null_قدر; }
                        have_قدر = سچ;
                    }
                    c = (c + انکڑے_بلاک_لمائی) & ~انکڑے_نقاب;
                } وإلا {
                    int32_t di = بلاک + (c & انکڑے_نقاب);
                    uint32_t trieValue2 = get_قدر(trie->انکڑے, قدر_چوڑائی, di);
                    إذا (have_قدر) {
                        إذا (trieValue2 != trie_قدر) {
                            إذا (filter == خالی_سوچک ||
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
                        إذا (p_قدر != خالی_سوچک) { *p_قدر = value; }
                        have_قدر = سچ;
                    }
                    طالما ((++c & انکڑے_نقاب) != 0) {
                        trieValue2 = get_قدر(trie->انکڑے, قدر_چوڑائی, ++di);
                        إذا (trieValue2 != trie_قدر) {
                            إذا (filter == خالی_سوچک ||
                                    maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_قدر = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } طالما (++i3 < i3_بلاک_لمائی);
    } طالما (c < trie->بلند_شروع);
    U_ASSERT(have_قدر);
    int32_t di = trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ;
    uint32_t بلند_قدر = get_قدر(trie->انکڑے, قدر_چوڑائی, di);
    إذا (maybe_Filter_قدر(بلند_قدر, trie->null_قدر, null_قدر,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_اندرونی_Get_Range(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 شروع,
                         UCPMapRangeOption option, uint32_t surrogate_قدر,
                         UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    إذا (option == UCPMAP_RANGE_عام) {
        أرجع getRange(trie, شروع, filter, context, p_قدر);
    }
    uint32_t value;
    إذا (p_قدر == خالی_سوچک) {
        // We need to examine the range value even if the caller does not want it.
        p_قدر = &value;
    }
    UChar32 surr_سرا = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 سرا = getRange(trie, شروع, filter, context, p_قدر);
    إذا (سرا < 0xd7ff || شروع > surr_سرا) {
        أرجع سرا;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    إذا (*p_قدر == surrogate_قدر) {
        إذا (سرا >= surr_سرا) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            أرجع سرا;
        }
    } وإلا {
        إذا (شروع <= 0xd7ff) {
            أرجع 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_قدر = surrogate_قدر;
        إذا (سرا > surr_سرا) {
            أرجع surr_سرا;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_سرا + 1, filter, context, &value2);
    إذا (value2 == surrogate_قدر) {
        أرجع end2;
    }
    أرجع surr_سرا;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 شروع,
                 UCPMapRangeOption option, uint32_t surrogate_قدر,
                 UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع ucptrie_اندرونی_Get_Range(getRange, trie, شروع,
                                    option, surrogate_قدر,
                                    filter, context, p_قدر);
}

U_CAPI int32_t U_EXPORT2
ucptrie_منزل_ثنائی(ثابت UCPTrie *trie,
                 فراغ *انکڑے, int32_t capacity,
                 UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_قدر_چوڑائی قدر_چوڑائی = (UCPTrie_قدر_چوڑائی)trie->قدر_چوڑائی;
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قدر_چوڑائی < UCPTRIE_قدر_بٹ_16 || UCPTRIE_قدر_بٹ_8 < قدر_چوڑائی ||
            capacity < 0 ||
            (capacity > 0 && (انکڑے == خالی_سوچک || (U_اشارہ_گر_نقاب_LSB(انکڑے, 3) != 0)))) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t length = (int32_t)حجم(UCPTrie_سرخی) + trie->اشاریہ_لمائی * 2;
    بدل (قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        length += trie->انکڑے_لمائی * 2;
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        length += trie->انکڑے_لمائی * 4;
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        length += trie->انکڑے_لمائی;
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إذا (capacity < length) {
        *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        أرجع length;
    }

    حرف *بائٹاں = (حرف *)انکڑے;
    UCPTrie_سرخی *سرخی = (UCPTrie_سرخی *)بائٹاں;
    سرخی->signature = UCPTRIE_SIG;  // "Tri3"
    سرخی->options = (uint16_t)(
        ((trie->انکڑے_لمائی & 0xf0000) >> 4) |
        ((trie->انکڑے_Null_وتھ & 0xf0000) >> 8) |
        (trie->type << 6) |
        قدر_چوڑائی);
    سرخی->اشاریہ_لمائی = (uint16_t)trie->اشاریہ_لمائی;
    سرخی->انکڑے_لمائی = (uint16_t)trie->انکڑے_لمائی;
    سرخی->index3_Null_وتھ = trie->index3_Null_وتھ;
    سرخی->انکڑے_Null_وتھ = (uint16_t)trie->انکڑے_Null_وتھ;
    سرخی->shifted_بلند_شروع = trie->بلند_شروع >> UCPTRIE_کھسکاؤ_2;
    بائٹاں += حجم(UCPTrie_سرخی);

    uprv_memcpy(بائٹاں, trie->اشاریہ, trie->اشاریہ_لمائی * 2);
    بائٹاں += trie->اشاریہ_لمائی * 2;

    بدل (قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        uprv_memcpy(بائٹاں, trie->انکڑے.ptr16, trie->انکڑے_لمائی * 2);
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        uprv_memcpy(بائٹاں, trie->انکڑے.ptr32, trie->انکڑے_لمائی * 4);
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        uprv_memcpy(بائٹاں, trie->انکڑے.ptr8, trie->انکڑے_لمائی);
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    أرجع length;
}

نطاق {

#جے_تعریف_شدہ UCPTRIE_DEBUG
لما گنتی_Null(ثابت UCPTrie *trie) {
    uint32_t null_قدر=trie->null_قدر;
    int32_t length=trie->انکڑے_لمائی;
    لما count=0;
    بدل (trie->قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->انکڑے.ptr16[i]==null_قدر) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->انکڑے.ptr32[i]==null_قدر) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->انکڑے.ptr8[i]==null_قدر) { ++count; }
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
    لما اشاریہ_لمائی=trie->اشاریہ_لمائی;
    لما انکڑے_لمائی=(لما)trie->انکڑے_لمائی;
    لما کل_لمائی=(لما)حجم(UCPTrie_سرخی)+اشاریہ_لمائی*2+
            انکڑے_لمائی*(trie->قدر_چوڑائی==UCPTRIE_قدر_بٹ_16 ? 2 :
                        trie->قدر_چوڑائی==UCPTRIE_قدر_بٹ_32 ? 4 : 1);
    لکھو("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, اشاریہ_لمائی, انکڑے_لمائی, گنتی_Null(trie), کل_لمائی);
}
#جے_انت

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    أرجع ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 شروع,
                UCPMapRangeOption option, uint32_t surrogate_قدر,
                UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), شروع,
                            option, surrogate_قدر,
                            filter, context, p_قدر);
}
