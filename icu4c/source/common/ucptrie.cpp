// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#اگر_تعریف_شده UCPTRIE_DEBUG
#   include <stdio.h>
#پایان_اگر

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_دودویی(UCPTrieType type,
                       UCPTrie_مقدار_عرض مقدار_عرض,
                       ثابت فراغ* داده U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_درازی,
                       UError_کد* p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    إذا (length <= 0 || (U_اشاره_گر_ماسک_LSB(داده, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            مقدار_عرض < UCPTRIE_مقدار_بیتها_ANY || UCPTRIE_مقدار_بیتها_8 < مقدار_عرض) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    // Enough data for a trie header?
    إذا (length < (int32_t)حجم(UCPTrie_سرآیند)) {
        *p_Error_کد = U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    // Check the signature.
    ثابت UCPTrie_سرآیند *سرآیند = (ثابت UCPTrie_سرآیند *)داده;
    إذا (سرآیند->signature != UCPTRIE_SIG) {
        *p_Error_کد = U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    int32_t options = سرآیند->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t مقدار_عرض_Int = options & UCPTRIE_OPTIONS_مقدار_بیتها_ماسک;
    إذا (typeInt > UCPTRIE_TYPE_SMALL || مقدار_عرض_Int > UCPTRIE_مقدار_بیتها_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_ماسک) != 0) {
        *p_Error_کد = U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_مقدار_عرض actual_مقدار_عرض = (UCPTrie_مقدار_عرض)مقدار_عرض_Int;
    إذا (type < 0) {
        type = actualType;
    }
    إذا (مقدار_عرض < 0) {
        مقدار_عرض = actual_مقدار_عرض;
    }
    إذا (type != actualType || مقدار_عرض != actual_مقدار_عرض) {
        *p_Error_کد = U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    // Get the length values and offsets.
    UCPTrie موقت_Trie;
    uprv_memset(&موقت_Trie, 0, حجم(موقت_Trie));
    موقت_Trie.شاخص_درازی = سرآیند->شاخص_درازی;
    موقت_Trie.داده_درازی =
        ((options & UCPTRIE_OPTIONS_داده_درازی_ماسک) << 4) | سرآیند->داده_درازی;
    موقت_Trie.index3_Null_جابه_جایی = سرآیند->index3_Null_جابه_جایی;
    موقت_Trie.داده_Null_جابه_جایی =
        ((options & UCPTRIE_OPTIONS_داده_NULL_جابه_جایی_ماسک) << 8) | سرآیند->داده_Null_جابه_جایی;

    موقت_Trie.بالا_شروع = سرآیند->shifted_بالا_شروع << UCPTRIE_لغزش_2;
    موقت_Trie.shifted12_بالا_شروع = (موقت_Trie.بالا_شروع + 0xfff) >> 12;
    موقت_Trie.type = type;
    موقت_Trie.مقدار_عرض = مقدار_عرض;

    // Calculate the actual length.
    int32_t actual_درازی = (int32_t)حجم(UCPTrie_سرآیند) + موقت_Trie.شاخص_درازی * 2;
    إذا (مقدار_عرض == UCPTRIE_مقدار_بیتها_16) {
        actual_درازی += موقت_Trie.داده_درازی * 2;
    } وإلا إذا (مقدار_عرض == UCPTRIE_مقدار_بیتها_32) {
        actual_درازی += موقت_Trie.داده_درازی * 4;
    } وإلا {
        actual_درازی += موقت_Trie.داده_درازی;
    }
    إذا (length < actual_درازی) {
        *p_Error_کد = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        أرجع اشاره_تهی;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    إذا (trie == اشاره_تهی) {
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memcpy(trie, &موقت_Trie, حجم(موقت_Trie));
#اگر_تعریف_شده UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#پایان_اگر

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(سرآیند + 1);
    trie->شاخص = p16;
    p16 += trie->شاخص_درازی;

    // Get the data.
    int32_t null_مقدار_جابه_جایی = trie->داده_Null_جابه_جایی;
    إذا (null_مقدار_جابه_جایی >= trie->داده_درازی) {
        null_مقدار_جابه_جایی = trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی;
    }
    بدل (مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        trie->داده.ptr16 = p16;
        trie->null_مقدار = trie->داده.ptr16[null_مقدار_جابه_جایی];
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        trie->داده.ptr32 = (ثابت uint32_t *)p16;
        trie->null_مقدار = trie->داده.ptr32[null_مقدار_جابه_جایی];
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        trie->داده.ptr8 = (ثابت uint8_t *)p16;
        trie->null_مقدار = trie->داده.ptr8[null_مقدار_جابه_جایی];
        اكسر;
    افتراضي:
        // Unreachable because valueWidth was checked above.
        *p_Error_کد = U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    إذا (p_Actual_درازی != اشاره_تهی) {
        *p_Actual_درازی = actual_درازی;
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

U_CAPI UCPTrie_مقدار_عرض U_EXPORT2
ucptrie_get_مقدار_عرض(ثابت UCPTrie *trie) {
    أرجع (UCPTrie_مقدار_عرض)trie->مقدار_عرض;
}

U_CAPI int32_t U_EXPORT2
ucptrie_داخلی_Small_شاخص(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_لغزش_1;
    إذا (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->بالا_شروع);
        i1 += UCPTRIE_BMP_شاخص_درازی - UCPTRIE_OMITTED_BMP_شاخص_1_درازی;
    } وإلا {
        U_ASSERT((uint32_t)c < (uint32_t)trie->بالا_شروع && trie->بالا_شروع > UCPTRIE_SMALL_حد);
        i1 += UCPTRIE_SMALL_شاخص_درازی;
    }
    int32_t i3_بلوک = trie->شاخص[
        (int32_t)trie->شاخص[i1] + ((c >> UCPTRIE_لغزش_2) & UCPTRIE_شاخص_2_ماسک)];
    int32_t i3 = (c >> UCPTRIE_لغزش_3) & UCPTRIE_شاخص_3_ماسک;
    int32_t داده_بلوک;
    إذا ((i3_بلوک & 0x8000) == 0) {
        // 16-bit indexes
        داده_بلوک = trie->شاخص[i3_بلوک + i3];
    } وإلا {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_بلوک = (i3_بلوک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        داده_بلوک = ((int32_t)trie->شاخص[i3_بلوک++] << (2 + (2 * i3))) & 0x30000;
        داده_بلوک |= trie->شاخص[i3_بلوک + i3];
    }
    أرجع داده_بلوک + (c & UCPTRIE_SMALL_داده_ماسک);
}

U_CAPI int32_t U_EXPORT2
ucptrie_داخلی_Small_U8_شاخص(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    إذا (c >= trie->بالا_شروع) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        أرجع trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی;
    }
    أرجع ucptrie_داخلی_Small_شاخص(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_داخلی_U8_Prev_شاخص(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *شروع, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    إذا ((src - شروع) <= 7) {
        i = length = (int32_t)(src - شروع);
    } وإلا {
        i = length = 7;
        شروع = src - 7;
    }
    c = utf8_prev_Char_امن_Body(شروع, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_شاخص(trie, 0xffff, c);
    أرجع (idx << 3) | i;
}

نطاق {

مضمن uint32_t get_مقدار(UCPTrie_داده داده, UCPTrie_مقدار_عرض مقدار_عرض, int32_t داده_شاخص) {
    بدل (مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        أرجع داده.ptr16[داده_شاخص];
    حالة UCPTRIE_مقدار_بیتها_32:
        أرجع داده.ptr32[داده_شاخص];
    حالة UCPTRIE_مقدار_بیتها_8:
        أرجع داده.ptr8[داده_شاخص];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        أرجع 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t داده_شاخص;
    إذا ((uint32_t)c <= 0x7f) {
        // linear ASCII
        داده_شاخص = c;
    } وإلا {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        داده_شاخص = UCPTRIE_CP_شاخص(trie, fastMax, c);
    }
    أرجع get_مقدار(trie->داده, (UCPTrie_مقدار_عرض)trie->مقدار_عرض, داده_شاخص);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybe_Filter_مقدار(uint32_t value, uint32_t trie_Null_مقدار, uint32_t null_مقدار,
                                 UCPMap_مقدار_Filter *filter, ثابت فراغ *context) {
    إذا (value == trie_Null_مقدار) {
        value = null_مقدار;
    } وإلا إذا (filter != اشاره_تهی) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 شروع,
                 UCPMap_مقدار_Filter *filter, ثابت فراغ *context, uint32_t *p_مقدار) {
    إذا (static_cast<uint32_t>(شروع) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_مقدار_عرض مقدار_عرض = static_cast<UCPTrie_مقدار_عرض>(trie->مقدار_عرض);
    إذا (شروع >= trie->بالا_شروع) {
        إذا (p_مقدار != اشاره_تهی) {
            int32_t di = trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی;
            uint32_t value = get_مقدار(trie->داده, مقدار_عرض, di);
            إذا (filter != اشاره_تهی) { value = filter(context, value); }
            *p_مقدار = value;
        }
        أرجع MAX_UNICODE;
    }

    uint32_t null_مقدار = trie->null_مقدار;
    إذا (filter != اشاره_تهی) { null_مقدار = filter(context, null_مقدار); }
    ثابت uint16_t *شاخص = trie->شاخص;

    int32_t prev_I3_بلوک = -1;
    int32_t prev_بلوک = -1;
    UChar32 c = شروع;
    uint32_t trie_مقدار, value = null_مقدار;
    منطقي have_مقدار = خطأ;
    افعل {
        int32_t i3_بلوک;
        int32_t i3;
        int32_t i3_بلوک_درازی;
        int32_t داده_بلوک_درازی;
        إذا (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_بلوک = 0;
            i3 = c >> UCPTRIE_FAST_لغزش;
            i3_بلوک_درازی = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_شاخص_درازی : UCPTRIE_SMALL_شاخص_درازی;
            داده_بلوک_درازی = UCPTRIE_FAST_داده_بلوک_درازی;
        } وإلا {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_لغزش_1;
            إذا (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->بالا_شروع);
                i1 += UCPTRIE_BMP_شاخص_درازی - UCPTRIE_OMITTED_BMP_شاخص_1_درازی;
            } وإلا {
                U_ASSERT(c < trie->بالا_شروع && trie->بالا_شروع > UCPTRIE_SMALL_حد);
                i1 += UCPTRIE_SMALL_شاخص_درازی;
            }
            i3_بلوک = trie->شاخص[
                static_cast<int32_t>(trie->شاخص[i1]) + ((c >> UCPTRIE_لغزش_2) & UCPTRIE_شاخص_2_ماسک)];
            إذا (i3_بلوک == prev_I3_بلوک && (c - شروع) >= UCPTRIE_CP_در_هر_شاخص_2_مدخل) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_در_هر_شاخص_2_مدخل - 1)) == 0);
                c += UCPTRIE_CP_در_هر_شاخص_2_مدخل;
                استمر;
            }
            prev_I3_بلوک = i3_بلوک;
            إذا (i3_بلوک == trie->index3_Null_جابه_جایی) {
                // This is the index-3 null block.
                إذا (have_مقدار) {
                    إذا (null_مقدار != value) {
                        أرجع c - 1;
                    }
                } وإلا {
                    trie_مقدار = trie->null_مقدار;
                    value = null_مقدار;
                    إذا (p_مقدار != اشاره_تهی) { *p_مقدار = null_مقدار; }
                    have_مقدار = درست;
                }
                prev_بلوک = trie->داده_Null_جابه_جایی;
                c = (c + UCPTRIE_CP_در_هر_شاخص_2_مدخل) & ~(UCPTRIE_CP_در_هر_شاخص_2_مدخل - 1);
                استمر;
            }
            i3 = (c >> UCPTRIE_لغزش_3) & UCPTRIE_شاخص_3_ماسک;
            i3_بلوک_درازی = UCPTRIE_شاخص_3_بلوک_درازی;
            داده_بلوک_درازی = UCPTRIE_SMALL_داده_بلوک_درازی;
        }
        // Enumerate data blocks for one index-3 block.
        افعل {
            int32_t بلوک;
            إذا ((i3_بلوک & 0x8000) == 0) {
                بلوک = شاخص[i3_بلوک + i3];
            } وإلا {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t گروه = (i3_بلوک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                بلوک = (static_cast<int32_t>(شاخص[گروه++]) << (2 + (2 * gi))) & 0x30000;
                بلوک |= شاخص[گروه + gi];
            }
            إذا (بلوک == prev_بلوک && (c - شروع) >= داده_بلوک_درازی) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (داده_بلوک_درازی - 1)) == 0);
                c += داده_بلوک_درازی;
            } وإلا {
                int32_t داده_ماسک = داده_بلوک_درازی - 1;
                prev_بلوک = بلوک;
                إذا (بلوک == trie->داده_Null_جابه_جایی) {
                    // This is the data null block.
                    إذا (have_مقدار) {
                        إذا (null_مقدار != value) {
                            أرجع c - 1;
                        }
                    } وإلا {
                        trie_مقدار = trie->null_مقدار;
                        value = null_مقدار;
                        إذا (p_مقدار != اشاره_تهی) { *p_مقدار = null_مقدار; }
                        have_مقدار = درست;
                    }
                    c = (c + داده_بلوک_درازی) & ~داده_ماسک;
                } وإلا {
                    int32_t di = بلوک + (c & داده_ماسک);
                    uint32_t trieValue2 = get_مقدار(trie->داده, مقدار_عرض, di);
                    إذا (have_مقدار) {
                        إذا (trieValue2 != trie_مقدار) {
                            إذا (filter == اشاره_تهی ||
                                    maybe_Filter_مقدار(trieValue2, trie->null_مقدار, null_مقدار,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_مقدار = trieValue2;  // may or may not help
                        }
                    } وإلا {
                        trie_مقدار = trieValue2;
                        value = maybe_Filter_مقدار(trieValue2, trie->null_مقدار, null_مقدار,
                                                 filter, context);
                        إذا (p_مقدار != اشاره_تهی) { *p_مقدار = value; }
                        have_مقدار = درست;
                    }
                    طالما ((++c & داده_ماسک) != 0) {
                        trieValue2 = get_مقدار(trie->داده, مقدار_عرض, ++di);
                        إذا (trieValue2 != trie_مقدار) {
                            إذا (filter == اشاره_تهی ||
                                    maybe_Filter_مقدار(trieValue2, trie->null_مقدار, null_مقدار,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_مقدار = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } طالما (++i3 < i3_بلوک_درازی);
    } طالما (c < trie->بالا_شروع);
    U_ASSERT(have_مقدار);
    int32_t di = trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی;
    uint32_t بالا_مقدار = get_مقدار(trie->داده, مقدار_عرض, di);
    إذا (maybe_Filter_مقدار(بالا_مقدار, trie->null_مقدار, null_مقدار,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_داخلی_Get_Range(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 شروع,
                         UCPMapRangeOption option, uint32_t surrogate_مقدار,
                         UCPMap_مقدار_Filter *filter, ثابت فراغ *context, uint32_t *p_مقدار) {
    إذا (option == UCPMAP_RANGE_نرمال) {
        أرجع getRange(trie, شروع, filter, context, p_مقدار);
    }
    uint32_t value;
    إذا (p_مقدار == اشاره_تهی) {
        // We need to examine the range value even if the caller does not want it.
        p_مقدار = &value;
    }
    UChar32 surr_انتها = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 انتها = getRange(trie, شروع, filter, context, p_مقدار);
    إذا (انتها < 0xd7ff || شروع > surr_انتها) {
        أرجع انتها;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    إذا (*p_مقدار == surrogate_مقدار) {
        إذا (انتها >= surr_انتها) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            أرجع انتها;
        }
    } وإلا {
        إذا (شروع <= 0xd7ff) {
            أرجع 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_مقدار = surrogate_مقدار;
        إذا (انتها > surr_انتها) {
            أرجع surr_انتها;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_انتها + 1, filter, context, &value2);
    إذا (value2 == surrogate_مقدار) {
        أرجع end2;
    }
    أرجع surr_انتها;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 شروع,
                 UCPMapRangeOption option, uint32_t surrogate_مقدار,
                 UCPMap_مقدار_Filter *filter, ثابت فراغ *context, uint32_t *p_مقدار) {
    أرجع ucptrie_داخلی_Get_Range(getRange, trie, شروع,
                                    option, surrogate_مقدار,
                                    filter, context, p_مقدار);
}

U_CAPI int32_t U_EXPORT2
ucptrie_مقصد_دودویی(ثابت UCPTrie *trie,
                 فراغ *داده, int32_t capacity,
                 UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_مقدار_عرض مقدار_عرض = (UCPTrie_مقدار_عرض)trie->مقدار_عرض;
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            مقدار_عرض < UCPTRIE_مقدار_بیتها_16 || UCPTRIE_مقدار_بیتها_8 < مقدار_عرض ||
            capacity < 0 ||
            (capacity > 0 && (داده == اشاره_تهی || (U_اشاره_گر_ماسک_LSB(داده, 3) != 0)))) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    int32_t length = (int32_t)حجم(UCPTrie_سرآیند) + trie->شاخص_درازی * 2;
    بدل (مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        length += trie->داده_درازی * 2;
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        length += trie->داده_درازی * 4;
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        length += trie->داده_درازی;
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إذا (capacity < length) {
        *p_Error_کد = U_میانگیر_سرریز_ERROR;
        أرجع length;
    }

    حرف *بایتها = (حرف *)داده;
    UCPTrie_سرآیند *سرآیند = (UCPTrie_سرآیند *)بایتها;
    سرآیند->signature = UCPTRIE_SIG;  // "Tri3"
    سرآیند->options = (uint16_t)(
        ((trie->داده_درازی & 0xf0000) >> 4) |
        ((trie->داده_Null_جابه_جایی & 0xf0000) >> 8) |
        (trie->type << 6) |
        مقدار_عرض);
    سرآیند->شاخص_درازی = (uint16_t)trie->شاخص_درازی;
    سرآیند->داده_درازی = (uint16_t)trie->داده_درازی;
    سرآیند->index3_Null_جابه_جایی = trie->index3_Null_جابه_جایی;
    سرآیند->داده_Null_جابه_جایی = (uint16_t)trie->داده_Null_جابه_جایی;
    سرآیند->shifted_بالا_شروع = trie->بالا_شروع >> UCPTRIE_لغزش_2;
    بایتها += حجم(UCPTrie_سرآیند);

    uprv_memcpy(بایتها, trie->شاخص, trie->شاخص_درازی * 2);
    بایتها += trie->شاخص_درازی * 2;

    بدل (مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        uprv_memcpy(بایتها, trie->داده.ptr16, trie->داده_درازی * 2);
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        uprv_memcpy(بایتها, trie->داده.ptr32, trie->داده_درازی * 4);
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        uprv_memcpy(بایتها, trie->داده.ptr8, trie->داده_درازی);
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    أرجع length;
}

نطاق {

#اگر_تعریف_شده UCPTRIE_DEBUG
دراز شمار_Null(ثابت UCPTrie *trie) {
    uint32_t null_مقدار=trie->null_مقدار;
    int32_t length=trie->داده_درازی;
    دراز count=0;
    بدل (trie->مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->داده.ptr16[i]==null_مقدار) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->داده.ptr32[i]==null_مقدار) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->داده.ptr8[i]==null_مقدار) { ++count; }
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
    دراز شاخص_درازی=trie->شاخص_درازی;
    دراز داده_درازی=(دراز)trie->داده_درازی;
    دراز مجموع_درازی=(دراز)حجم(UCPTrie_سرآیند)+شاخص_درازی*2+
            داده_درازی*(trie->مقدار_عرض==UCPTRIE_مقدار_بیتها_16 ? 2 :
                        trie->مقدار_عرض==UCPTRIE_مقدار_بیتها_32 ? 4 : 1);
    چاپ_کن("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, شاخص_درازی, داده_درازی, شمار_Null(trie), مجموع_درازی);
}
#پایان_اگر

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    أرجع ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 شروع,
                UCPMapRangeOption option, uint32_t surrogate_مقدار,
                UCPMap_مقدار_Filter *filter, ثابت فراغ *context, uint32_t *p_مقدار) {
    أرجع ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), شروع,
                            option, surrogate_مقدار,
                            filter, context, p_مقدار);
}
