// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#اگر_تعریف_شدہ UCPTRIE_DEBUG
#   include <stdio.h>
#اگر_ختم

#شامل "unicode/utypes.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ثنائی(UCPTrieType type,
                       UCPTrie_قدر_چوڑائی قدر_چوڑائی,
                       ثابت خالی* ڈیٹا U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_لمبائی,
                       UError_رمز* p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    اگر (length <= 0 || (U_اشارہ_گر_نقاب_LSB(ڈیٹا, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            قدر_چوڑائی < UCPTRIE_قدر_بٹ_ANY || UCPTRIE_قدر_بٹ_8 < قدر_چوڑائی) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    // Enough data for a trie header?
    اگر (length < (int32_t)حجم(UCPTrie_سرخی)) {
        *p_Error_رمز = U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    // Check the signature.
    ثابت UCPTrie_سرخی *سرخی = (ثابت UCPTrie_سرخی *)ڈیٹا;
    اگر (سرخی->signature != UCPTRIE_SIG) {
        *p_Error_رمز = U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    int32_t options = سرخی->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t قدر_چوڑائی_Int = options & UCPTRIE_OPTIONS_قدر_بٹ_نقاب;
    اگر (typeInt > UCPTRIE_TYPE_SMALL || قدر_چوڑائی_Int > UCPTRIE_قدر_بٹ_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_نقاب) != 0) {
        *p_Error_رمز = U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_قدر_چوڑائی actual_قدر_چوڑائی = (UCPTrie_قدر_چوڑائی)قدر_چوڑائی_Int;
    اگر (type < 0) {
        type = actualType;
    }
    اگر (قدر_چوڑائی < 0) {
        قدر_چوڑائی = actual_قدر_چوڑائی;
    }
    اگر (type != actualType || قدر_چوڑائی != actual_قدر_چوڑائی) {
        *p_Error_رمز = U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    // Get the length values and offsets.
    UCPTrie عارضی_Trie;
    uprv_memset(&عارضی_Trie, 0, حجم(عارضی_Trie));
    عارضی_Trie.اشاریہ_لمبائی = سرخی->اشاریہ_لمبائی;
    عارضی_Trie.ڈیٹا_لمبائی =
        ((options & UCPTRIE_OPTIONS_ڈیٹا_لمبائی_نقاب) << 4) | سرخی->ڈیٹا_لمبائی;
    عارضی_Trie.index3_Null_انحراف = سرخی->index3_Null_انحراف;
    عارضی_Trie.ڈیٹا_Null_انحراف =
        ((options & UCPTRIE_OPTIONS_ڈیٹا_NULL_انحراف_نقاب) << 8) | سرخی->ڈیٹا_Null_انحراف;

    عارضی_Trie.بلند_آغاز = سرخی->shifted_بلند_آغاز << UCPTRIE_سرکاؤ_2;
    عارضی_Trie.shifted12_بلند_آغاز = (عارضی_Trie.بلند_آغاز + 0xfff) >> 12;
    عارضی_Trie.type = type;
    عارضی_Trie.قدر_چوڑائی = قدر_چوڑائی;

    // Calculate the actual length.
    int32_t actual_لمبائی = (int32_t)حجم(UCPTrie_سرخی) + عارضی_Trie.اشاریہ_لمبائی * 2;
    اگر (قدر_چوڑائی == UCPTRIE_قدر_بٹ_16) {
        actual_لمبائی += عارضی_Trie.ڈیٹا_لمبائی * 2;
    } ورنہ اگر (قدر_چوڑائی == UCPTRIE_قدر_بٹ_32) {
        actual_لمبائی += عارضی_Trie.ڈیٹا_لمبائی * 4;
    } ورنہ {
        actual_لمبائی += عارضی_Trie.ڈیٹا_لمبائی;
    }
    اگر (length < actual_لمبائی) {
        *p_Error_رمز = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        واپس خالی_اشاریہ;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    اگر (trie == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(trie, &عارضی_Trie, حجم(عارضی_Trie));
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#اگر_ختم

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(سرخی + 1);
    trie->اشاریہ = p16;
    p16 += trie->اشاریہ_لمبائی;

    // Get the data.
    int32_t null_قدر_انحراف = trie->ڈیٹا_Null_انحراف;
    اگر (null_قدر_انحراف >= trie->ڈیٹا_لمبائی) {
        null_قدر_انحراف = trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف;
    }
    انتخاب (قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        trie->ڈیٹا.ptr16 = p16;
        trie->null_قدر = trie->ڈیٹا.ptr16[null_قدر_انحراف];
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        trie->ڈیٹا.ptr32 = (ثابت uint32_t *)p16;
        trie->null_قدر = trie->ڈیٹا.ptr32[null_قدر_انحراف];
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        trie->ڈیٹا.ptr8 = (ثابت uint8_t *)p16;
        trie->null_قدر = trie->ڈیٹا.ptr8[null_قدر_انحراف];
        توڑو;
    طےشدہ:
        // Unreachable because valueWidth was checked above.
        *p_Error_رمز = U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر (p_Actual_لمبائی != خالی_اشاریہ) {
        *p_Actual_لمبائی = actual_لمبائی;
    }
    واپس trie;
}

U_CAPI خالی U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ثابت UCPTrie *trie) {
    واپس (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_قدر_چوڑائی U_EXPORT2
ucptrie_get_قدر_چوڑائی(ثابت UCPTrie *trie) {
    واپس (UCPTrie_قدر_چوڑائی)trie->قدر_چوڑائی;
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندرونی_Small_اشاریہ(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_سرکاؤ_1;
    اگر (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->بلند_آغاز);
        i1 += UCPTRIE_BMP_اشاریہ_لمبائی - UCPTRIE_OMITTED_BMP_اشاریہ_1_لمبائی;
    } ورنہ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->بلند_آغاز && trie->بلند_آغاز > UCPTRIE_SMALL_حد_بندی);
        i1 += UCPTRIE_SMALL_اشاریہ_لمبائی;
    }
    int32_t i3_بلاک = trie->اشاریہ[
        (int32_t)trie->اشاریہ[i1] + ((c >> UCPTRIE_سرکاؤ_2) & UCPTRIE_اشاریہ_2_نقاب)];
    int32_t i3 = (c >> UCPTRIE_سرکاؤ_3) & UCPTRIE_اشاریہ_3_نقاب;
    int32_t ڈیٹا_بلاک;
    اگر ((i3_بلاک & 0x8000) == 0) {
        // 16-bit indexes
        ڈیٹا_بلاک = trie->اشاریہ[i3_بلاک + i3];
    } ورنہ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_بلاک = (i3_بلاک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        ڈیٹا_بلاک = ((int32_t)trie->اشاریہ[i3_بلاک++] << (2 + (2 * i3))) & 0x30000;
        ڈیٹا_بلاک |= trie->اشاریہ[i3_بلاک + i3];
    }
    واپس ڈیٹا_بلاک + (c & UCPTRIE_SMALL_ڈیٹا_نقاب);
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندرونی_Small_U8_اشاریہ(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    اگر (c >= trie->بلند_آغاز) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        واپس trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف;
    }
    واپس ucptrie_اندرونی_Small_اشاریہ(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_اندرونی_U8_پچھلا_اشاریہ(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *آغاز, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    اگر ((src - آغاز) <= 7) {
        i = length = (int32_t)(src - آغاز);
    } ورنہ {
        i = length = 7;
        آغاز = src - 7;
    }
    c = utf8_پچھلا_Char_محفوظ_Body(آغاز, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_اشاریہ(trie, 0xffff, c);
    واپس (idx << 3) | i;
}

نامخلا {

اندرونی uint32_t get_قدر(UCPTrie_ڈیٹا ڈیٹا, UCPTrie_قدر_چوڑائی قدر_چوڑائی, int32_t ڈیٹا_اشاریہ) {
    انتخاب (قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        واپس ڈیٹا.ptr16[ڈیٹا_اشاریہ];
    حالت UCPTRIE_قدر_بٹ_32:
        واپس ڈیٹا.ptr32[ڈیٹا_اشاریہ];
    حالت UCPTRIE_قدر_بٹ_8:
        واپس ڈیٹا.ptr8[ڈیٹا_اشاریہ];
    طےشدہ:
        // Unreachable if the trie is properly initialized.
        واپس 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t ڈیٹا_اشاریہ;
    اگر ((uint32_t)c <= 0x7f) {
        // linear ASCII
        ڈیٹا_اشاریہ = c;
    } ورنہ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        ڈیٹا_اشاریہ = UCPTRIE_CP_اشاریہ(trie, fastMax, c);
    }
    واپس get_قدر(trie->ڈیٹا, (UCPTrie_قدر_چوڑائی)trie->قدر_چوڑائی, ڈیٹا_اشاریہ);
}

نامخلا {

constexpr int32_t MAX_UNICODE = 0x10ffff;

اندرونی uint32_t maybe_Filter_قدر(uint32_t value, uint32_t trie_Null_قدر, uint32_t null_قدر,
                                 UCPMap_قدر_Filter *filter, ثابت خالی *context) {
    اگر (value == trie_Null_قدر) {
        value = null_قدر;
    } ورنہ اگر (filter != خالی_اشاریہ) {
        value = filter(context, value);
    }
    واپس value;
}

UChar32 getRange(ثابت خالی *t, UChar32 آغاز,
                 UCPMap_قدر_Filter *filter, ثابت خالی *context, uint32_t *p_قدر) {
    اگر (static_cast<uint32_t>(آغاز) > MAX_UNICODE) {
        واپس U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_قدر_چوڑائی قدر_چوڑائی = static_cast<UCPTrie_قدر_چوڑائی>(trie->قدر_چوڑائی);
    اگر (آغاز >= trie->بلند_آغاز) {
        اگر (p_قدر != خالی_اشاریہ) {
            int32_t di = trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف;
            uint32_t value = get_قدر(trie->ڈیٹا, قدر_چوڑائی, di);
            اگر (filter != خالی_اشاریہ) { value = filter(context, value); }
            *p_قدر = value;
        }
        واپس MAX_UNICODE;
    }

    uint32_t null_قدر = trie->null_قدر;
    اگر (filter != خالی_اشاریہ) { null_قدر = filter(context, null_قدر); }
    ثابت uint16_t *اشاریہ = trie->اشاریہ;

    int32_t پچھلا_I3_بلاک = -1;
    int32_t پچھلا_بلاک = -1;
    UChar32 c = آغاز;
    uint32_t trie_قدر, value = null_قدر;
    منطقی have_قدر = جھوٹ;
    کرو {
        int32_t i3_بلاک;
        int32_t i3;
        int32_t i3_بلاک_لمبائی;
        int32_t ڈیٹا_بلاک_لمبائی;
        اگر (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_بلاک = 0;
            i3 = c >> UCPTRIE_FAST_سرکاؤ;
            i3_بلاک_لمبائی = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_اشاریہ_لمبائی : UCPTRIE_SMALL_اشاریہ_لمبائی;
            ڈیٹا_بلاک_لمبائی = UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی;
        } ورنہ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_سرکاؤ_1;
            اگر (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->بلند_آغاز);
                i1 += UCPTRIE_BMP_اشاریہ_لمبائی - UCPTRIE_OMITTED_BMP_اشاریہ_1_لمبائی;
            } ورنہ {
                U_ASSERT(c < trie->بلند_آغاز && trie->بلند_آغاز > UCPTRIE_SMALL_حد_بندی);
                i1 += UCPTRIE_SMALL_اشاریہ_لمبائی;
            }
            i3_بلاک = trie->اشاریہ[
                static_cast<int32_t>(trie->اشاریہ[i1]) + ((c >> UCPTRIE_سرکاؤ_2) & UCPTRIE_اشاریہ_2_نقاب)];
            اگر (i3_بلاک == پچھلا_I3_بلاک && (c - آغاز) >= UCPTRIE_CP_فی_اشاریہ_2_اندراج) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1)) == 0);
                c += UCPTRIE_CP_فی_اشاریہ_2_اندراج;
                جاری;
            }
            پچھلا_I3_بلاک = i3_بلاک;
            اگر (i3_بلاک == trie->index3_Null_انحراف) {
                // This is the index-3 null block.
                اگر (have_قدر) {
                    اگر (null_قدر != value) {
                        واپس c - 1;
                    }
                } ورنہ {
                    trie_قدر = trie->null_قدر;
                    value = null_قدر;
                    اگر (p_قدر != خالی_اشاریہ) { *p_قدر = null_قدر; }
                    have_قدر = سچ;
                }
                پچھلا_بلاک = trie->ڈیٹا_Null_انحراف;
                c = (c + UCPTRIE_CP_فی_اشاریہ_2_اندراج) & ~(UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1);
                جاری;
            }
            i3 = (c >> UCPTRIE_سرکاؤ_3) & UCPTRIE_اشاریہ_3_نقاب;
            i3_بلاک_لمبائی = UCPTRIE_اشاریہ_3_بلاک_لمبائی;
            ڈیٹا_بلاک_لمبائی = UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
        }
        // Enumerate data blocks for one index-3 block.
        کرو {
            int32_t بلاک;
            اگر ((i3_بلاک & 0x8000) == 0) {
                بلاک = اشاریہ[i3_بلاک + i3];
            } ورنہ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t گروہ = (i3_بلاک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                بلاک = (static_cast<int32_t>(اشاریہ[گروہ++]) << (2 + (2 * gi))) & 0x30000;
                بلاک |= اشاریہ[گروہ + gi];
            }
            اگر (بلاک == پچھلا_بلاک && (c - آغاز) >= ڈیٹا_بلاک_لمبائی) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (ڈیٹا_بلاک_لمبائی - 1)) == 0);
                c += ڈیٹا_بلاک_لمبائی;
            } ورنہ {
                int32_t ڈیٹا_نقاب = ڈیٹا_بلاک_لمبائی - 1;
                پچھلا_بلاک = بلاک;
                اگر (بلاک == trie->ڈیٹا_Null_انحراف) {
                    // This is the data null block.
                    اگر (have_قدر) {
                        اگر (null_قدر != value) {
                            واپس c - 1;
                        }
                    } ورنہ {
                        trie_قدر = trie->null_قدر;
                        value = null_قدر;
                        اگر (p_قدر != خالی_اشاریہ) { *p_قدر = null_قدر; }
                        have_قدر = سچ;
                    }
                    c = (c + ڈیٹا_بلاک_لمبائی) & ~ڈیٹا_نقاب;
                } ورنہ {
                    int32_t di = بلاک + (c & ڈیٹا_نقاب);
                    uint32_t trieValue2 = get_قدر(trie->ڈیٹا, قدر_چوڑائی, di);
                    اگر (have_قدر) {
                        اگر (trieValue2 != trie_قدر) {
                            اگر (filter == خالی_اشاریہ ||
                                    maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                     filter, context) != value) {
                                واپس c - 1;
                            }
                            trie_قدر = trieValue2;  // may or may not help
                        }
                    } ورنہ {
                        trie_قدر = trieValue2;
                        value = maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                 filter, context);
                        اگر (p_قدر != خالی_اشاریہ) { *p_قدر = value; }
                        have_قدر = سچ;
                    }
                    جب ((++c & ڈیٹا_نقاب) != 0) {
                        trieValue2 = get_قدر(trie->ڈیٹا, قدر_چوڑائی, ++di);
                        اگر (trieValue2 != trie_قدر) {
                            اگر (filter == خالی_اشاریہ ||
                                    maybe_Filter_قدر(trieValue2, trie->null_قدر, null_قدر,
                                                     filter, context) != value) {
                                واپس c - 1;
                            }
                            trie_قدر = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } جب (++i3 < i3_بلاک_لمبائی);
    } جب (c < trie->بلند_آغاز);
    U_ASSERT(have_قدر);
    int32_t di = trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف;
    uint32_t بلند_قدر = get_قدر(trie->ڈیٹا, قدر_چوڑائی, di);
    اگر (maybe_Filter_قدر(بلند_قدر, trie->null_قدر, null_قدر,
                         filter, context) != value) {
        واپس c - 1;
    } ورنہ {
        واپس MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_اندرونی_Get_Range(UCPTrieGetRange *getRange,
                         ثابت خالی *trie, UChar32 آغاز,
                         UCPMapRangeOption option, uint32_t surrogate_قدر,
                         UCPMap_قدر_Filter *filter, ثابت خالی *context, uint32_t *p_قدر) {
    اگر (option == UCPMAP_RANGE_معمول) {
        واپس getRange(trie, آغاز, filter, context, p_قدر);
    }
    uint32_t value;
    اگر (p_قدر == خالی_اشاریہ) {
        // We need to examine the range value even if the caller does not want it.
        p_قدر = &value;
    }
    UChar32 surr_انتہا = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 انتہا = getRange(trie, آغاز, filter, context, p_قدر);
    اگر (انتہا < 0xd7ff || آغاز > surr_انتہا) {
        واپس انتہا;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    اگر (*p_قدر == surrogate_قدر) {
        اگر (انتہا >= surr_انتہا) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            واپس انتہا;
        }
    } ورنہ {
        اگر (آغاز <= 0xd7ff) {
            واپس 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_قدر = surrogate_قدر;
        اگر (انتہا > surr_انتہا) {
            واپس surr_انتہا;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_انتہا + 1, filter, context, &value2);
    اگر (value2 == surrogate_قدر) {
        واپس end2;
    }
    واپس surr_انتہا;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 آغاز,
                 UCPMapRangeOption option, uint32_t surrogate_قدر,
                 UCPMap_قدر_Filter *filter, ثابت خالی *context, uint32_t *p_قدر) {
    واپس ucptrie_اندرونی_Get_Range(getRange, trie, آغاز,
                                    option, surrogate_قدر,
                                    filter, context, p_قدر);
}

U_CAPI int32_t U_EXPORT2
ucptrie_مقصد_ثنائی(ثابت UCPTrie *trie,
                 خالی *ڈیٹا, int32_t capacity,
                 UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_قدر_چوڑائی قدر_چوڑائی = (UCPTrie_قدر_چوڑائی)trie->قدر_چوڑائی;
    اگر (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قدر_چوڑائی < UCPTRIE_قدر_بٹ_16 || UCPTRIE_قدر_بٹ_8 < قدر_چوڑائی ||
            capacity < 0 ||
            (capacity > 0 && (ڈیٹا == خالی_اشاریہ || (U_اشارہ_گر_نقاب_LSB(ڈیٹا, 3) != 0)))) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    int32_t length = (int32_t)حجم(UCPTrie_سرخی) + trie->اشاریہ_لمبائی * 2;
    انتخاب (قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        length += trie->ڈیٹا_لمبائی * 2;
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        length += trie->ڈیٹا_لمبائی * 4;
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        length += trie->ڈیٹا_لمبائی;
        توڑو;
    طےشدہ:
        // unreachable
        توڑو;
    }
    اگر (capacity < length) {
        *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        واپس length;
    }

    حرف *بائٹیں = (حرف *)ڈیٹا;
    UCPTrie_سرخی *سرخی = (UCPTrie_سرخی *)بائٹیں;
    سرخی->signature = UCPTRIE_SIG;  // "Tri3"
    سرخی->options = (uint16_t)(
        ((trie->ڈیٹا_لمبائی & 0xf0000) >> 4) |
        ((trie->ڈیٹا_Null_انحراف & 0xf0000) >> 8) |
        (trie->type << 6) |
        قدر_چوڑائی);
    سرخی->اشاریہ_لمبائی = (uint16_t)trie->اشاریہ_لمبائی;
    سرخی->ڈیٹا_لمبائی = (uint16_t)trie->ڈیٹا_لمبائی;
    سرخی->index3_Null_انحراف = trie->index3_Null_انحراف;
    سرخی->ڈیٹا_Null_انحراف = (uint16_t)trie->ڈیٹا_Null_انحراف;
    سرخی->shifted_بلند_آغاز = trie->بلند_آغاز >> UCPTRIE_سرکاؤ_2;
    بائٹیں += حجم(UCPTrie_سرخی);

    uprv_memcpy(بائٹیں, trie->اشاریہ, trie->اشاریہ_لمبائی * 2);
    بائٹیں += trie->اشاریہ_لمبائی * 2;

    انتخاب (قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        uprv_memcpy(بائٹیں, trie->ڈیٹا.ptr16, trie->ڈیٹا_لمبائی * 2);
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        uprv_memcpy(بائٹیں, trie->ڈیٹا.ptr32, trie->ڈیٹا_لمبائی * 4);
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        uprv_memcpy(بائٹیں, trie->ڈیٹا.ptr8, trie->ڈیٹا_لمبائی);
        توڑو;
    طےشدہ:
        // unreachable
        توڑو;
    }
    واپس length;
}

نامخلا {

#اگر_تعریف_شدہ UCPTRIE_DEBUG
طویل گنتی_Null(ثابت UCPTrie *trie) {
    uint32_t null_قدر=trie->null_قدر;
    int32_t length=trie->ڈیٹا_لمبائی;
    طویل count=0;
    انتخاب (trie->قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        برائے(int32_t i=0; i<length; ++i) {
            اگر(trie->ڈیٹا.ptr16[i]==null_قدر) { ++count; }
        }
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        برائے(int32_t i=0; i<length; ++i) {
            اگر(trie->ڈیٹا.ptr32[i]==null_قدر) { ++count; }
        }
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        برائے(int32_t i=0; i<length; ++i) {
            اگر(trie->ڈیٹا.ptr8[i]==null_قدر) { ++count; }
        }
        توڑو;
    طےشدہ:
        // unreachable
        توڑو;
    }
    واپس count;
}

U_CFUNC خالی
ucptrie_printLengths(ثابت UCPTrie *trie, ثابت حرف *which) {
    طویل اشاریہ_لمبائی=trie->اشاریہ_لمبائی;
    طویل ڈیٹا_لمبائی=(طویل)trie->ڈیٹا_لمبائی;
    طویل کل_لمبائی=(طویل)حجم(UCPTrie_سرخی)+اشاریہ_لمبائی*2+
            ڈیٹا_لمبائی*(trie->قدر_چوڑائی==UCPTRIE_قدر_بٹ_16 ? 2 :
                        trie->قدر_چوڑائی==UCPTRIE_قدر_بٹ_32 ? 4 : 1);
    لکھو("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, اشاریہ_لمبائی, ڈیٹا_لمبائی, گنتی_Null(trie), کل_لمبائی);
}
#اگر_ختم

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    واپس ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 آغاز,
                UCPMapRangeOption option, uint32_t surrogate_قدر,
                UCPMap_قدر_Filter *filter, ثابت خالی *context, uint32_t *p_قدر) {
    واپس ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), آغاز,
                            option, surrogate_قدر,
                            filter, context, p_قدر);
}
