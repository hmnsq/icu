// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#که_تعریف_شوی UCPTRIE_DEBUG
#   include <stdio.h>
#د_که_پای

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_دوګونی(UCPTrieType type,
                       UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی,
                       ثابت فراغ* معلومات U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_اوږدتیا,
                       UError_کوډ* p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    إذا (length <= 0 || (U_اشاره_ګر_ماسک_LSB(معلومات, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            ارزښت_پلنوالی < UCPTRIE_ارزښت_بټونه_ANY || UCPTRIE_ارزښت_بټونه_8 < ارزښت_پلنوالی) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    // Enough data for a trie header?
    إذا (length < (int32_t)حجم(UCPTrie_سرلیک)) {
        *p_Error_کوډ = U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    // Check the signature.
    ثابت UCPTrie_سرلیک *سرلیک = (ثابت UCPTrie_سرلیک *)معلومات;
    إذا (سرلیک->signature != UCPTRIE_SIG) {
        *p_Error_کوډ = U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    int32_t options = سرلیک->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t ارزښت_پلنوالی_Int = options & UCPTRIE_OPTIONS_ارزښت_بټونه_ماسک;
    إذا (typeInt > UCPTRIE_TYPE_SMALL || ارزښت_پلنوالی_Int > UCPTRIE_ارزښت_بټونه_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_ماسک) != 0) {
        *p_Error_کوډ = U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_ارزښت_پلنوالی actual_ارزښت_پلنوالی = (UCPTrie_ارزښت_پلنوالی)ارزښت_پلنوالی_Int;
    إذا (type < 0) {
        type = actualType;
    }
    إذا (ارزښت_پلنوالی < 0) {
        ارزښت_پلنوالی = actual_ارزښت_پلنوالی;
    }
    إذا (type != actualType || ارزښت_پلنوالی != actual_ارزښت_پلنوالی) {
        *p_Error_کوډ = U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    // Get the length values and offsets.
    UCPTrie لنډمهاله_Trie;
    uprv_memset(&لنډمهاله_Trie, 0, حجم(لنډمهاله_Trie));
    لنډمهاله_Trie.شاخص_اوږدتیا = سرلیک->شاخص_اوږدتیا;
    لنډمهاله_Trie.معلومات_اوږدتیا =
        ((options & UCPTRIE_OPTIONS_معلومات_اوږدتیا_ماسک) << 4) | سرلیک->معلومات_اوږدتیا;
    لنډمهاله_Trie.index3_Null_دننه_واټن = سرلیک->index3_Null_دننه_واټن;
    لنډمهاله_Trie.معلومات_Null_دننه_واټن =
        ((options & UCPTRIE_OPTIONS_معلومات_NULL_دننه_واټن_ماسک) << 8) | سرلیک->معلومات_Null_دننه_واټن;

    لنډمهاله_Trie.لوړ_پیل = سرلیک->shifted_لوړ_پیل << UCPTRIE_ښويدنه_2;
    لنډمهاله_Trie.shifted12_لوړ_پیل = (لنډمهاله_Trie.لوړ_پیل + 0xfff) >> 12;
    لنډمهاله_Trie.type = type;
    لنډمهاله_Trie.ارزښت_پلنوالی = ارزښت_پلنوالی;

    // Calculate the actual length.
    int32_t actual_اوږدتیا = (int32_t)حجم(UCPTrie_سرلیک) + لنډمهاله_Trie.شاخص_اوږدتیا * 2;
    إذا (ارزښت_پلنوالی == UCPTRIE_ارزښت_بټونه_16) {
        actual_اوږدتیا += لنډمهاله_Trie.معلومات_اوږدتیا * 2;
    } وإلا إذا (ارزښت_پلنوالی == UCPTRIE_ارزښت_بټونه_32) {
        actual_اوږدتیا += لنډمهاله_Trie.معلومات_اوږدتیا * 4;
    } وإلا {
        actual_اوږدتیا += لنډمهاله_Trie.معلومات_اوږدتیا;
    }
    إذا (length < actual_اوږدتیا) {
        *p_Error_کوډ = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        أرجع تش_اشاره;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    إذا (trie == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memcpy(trie, &لنډمهاله_Trie, حجم(لنډمهاله_Trie));
#که_تعریف_شوی UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#د_که_پای

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(سرلیک + 1);
    trie->شاخص = p16;
    p16 += trie->شاخص_اوږدتیا;

    // Get the data.
    int32_t null_ارزښت_دننه_واټن = trie->معلومات_Null_دننه_واټن;
    إذا (null_ارزښت_دننه_واټن >= trie->معلومات_اوږدتیا) {
        null_ارزښت_دننه_واټن = trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن;
    }
    بدل (ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        trie->معلومات.ptr16 = p16;
        trie->null_ارزښت = trie->معلومات.ptr16[null_ارزښت_دننه_واټن];
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        trie->معلومات.ptr32 = (ثابت uint32_t *)p16;
        trie->null_ارزښت = trie->معلومات.ptr32[null_ارزښت_دننه_واټن];
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        trie->معلومات.ptr8 = (ثابت uint8_t *)p16;
        trie->null_ارزښت = trie->معلومات.ptr8[null_ارزښت_دننه_واټن];
        اكسر;
    افتراضي:
        // Unreachable because valueWidth was checked above.
        *p_Error_کوډ = U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    إذا (p_Actual_اوږدتیا != تش_اشاره) {
        *p_Actual_اوږدتیا = actual_اوږدتیا;
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

U_CAPI UCPTrie_ارزښت_پلنوالی U_EXPORT2
ucptrie_get_ارزښت_پلنوالی(ثابت UCPTrie *trie) {
    أرجع (UCPTrie_ارزښت_پلنوالی)trie->ارزښت_پلنوالی;
}

U_CAPI int32_t U_EXPORT2
ucptrie_دننی_Small_شاخص(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_ښويدنه_1;
    إذا (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->لوړ_پیل);
        i1 += UCPTRIE_BMP_شاخص_اوږدتیا - UCPTRIE_OMITTED_BMP_شاخص_1_اوږدتیا;
    } وإلا {
        U_ASSERT((uint32_t)c < (uint32_t)trie->لوړ_پیل && trie->لوړ_پیل > UCPTRIE_SMALL_حد);
        i1 += UCPTRIE_SMALL_شاخص_اوږدتیا;
    }
    int32_t i3_بلاک = trie->شاخص[
        (int32_t)trie->شاخص[i1] + ((c >> UCPTRIE_ښويدنه_2) & UCPTRIE_شاخص_2_ماسک)];
    int32_t i3 = (c >> UCPTRIE_ښويدنه_3) & UCPTRIE_شاخص_3_ماسک;
    int32_t معلومات_بلاک;
    إذا ((i3_بلاک & 0x8000) == 0) {
        // 16-bit indexes
        معلومات_بلاک = trie->شاخص[i3_بلاک + i3];
    } وإلا {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_بلاک = (i3_بلاک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        معلومات_بلاک = ((int32_t)trie->شاخص[i3_بلاک++] << (2 + (2 * i3))) & 0x30000;
        معلومات_بلاک |= trie->شاخص[i3_بلاک + i3];
    }
    أرجع معلومات_بلاک + (c & UCPTRIE_SMALL_معلومات_ماسک);
}

U_CAPI int32_t U_EXPORT2
ucptrie_دننی_Small_U8_شاخص(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    إذا (c >= trie->لوړ_پیل) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        أرجع trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن;
    }
    أرجع ucptrie_دننی_Small_شاخص(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_دننی_U8_Prev_شاخص(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *پیل, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    إذا ((src - پیل) <= 7) {
        i = length = (int32_t)(src - پیل);
    } وإلا {
        i = length = 7;
        پیل = src - 7;
    }
    c = utf8_prev_Char_خوندي_Body(پیل, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_شاخص(trie, 0xffff, c);
    أرجع (idx << 3) | i;
}

نطاق {

مضمن uint32_t get_ارزښت(UCPTrie_معلومات معلومات, UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی, int32_t معلومات_شاخص) {
    بدل (ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        أرجع معلومات.ptr16[معلومات_شاخص];
    حالة UCPTRIE_ارزښت_بټونه_32:
        أرجع معلومات.ptr32[معلومات_شاخص];
    حالة UCPTRIE_ارزښت_بټونه_8:
        أرجع معلومات.ptr8[معلومات_شاخص];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        أرجع 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t معلومات_شاخص;
    إذا ((uint32_t)c <= 0x7f) {
        // linear ASCII
        معلومات_شاخص = c;
    } وإلا {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        معلومات_شاخص = UCPTRIE_CP_شاخص(trie, fastMax, c);
    }
    أرجع get_ارزښت(trie->معلومات, (UCPTrie_ارزښت_پلنوالی)trie->ارزښت_پلنوالی, معلومات_شاخص);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybe_Filter_ارزښت(uint32_t value, uint32_t trie_Null_ارزښت, uint32_t null_ارزښت,
                                 UCPMap_ارزښت_Filter *filter, ثابت فراغ *context) {
    إذا (value == trie_Null_ارزښت) {
        value = null_ارزښت;
    } وإلا إذا (filter != تش_اشاره) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 پیل,
                 UCPMap_ارزښت_Filter *filter, ثابت فراغ *context, uint32_t *p_ارزښت) {
    إذا (static_cast<uint32_t>(پیل) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی = static_cast<UCPTrie_ارزښت_پلنوالی>(trie->ارزښت_پلنوالی);
    إذا (پیل >= trie->لوړ_پیل) {
        إذا (p_ارزښت != تش_اشاره) {
            int32_t di = trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن;
            uint32_t value = get_ارزښت(trie->معلومات, ارزښت_پلنوالی, di);
            إذا (filter != تش_اشاره) { value = filter(context, value); }
            *p_ارزښت = value;
        }
        أرجع MAX_UNICODE;
    }

    uint32_t null_ارزښت = trie->null_ارزښت;
    إذا (filter != تش_اشاره) { null_ارزښت = filter(context, null_ارزښت); }
    ثابت uint16_t *شاخص = trie->شاخص;

    int32_t prev_I3_بلاک = -1;
    int32_t prev_بلاک = -1;
    UChar32 c = پیل;
    uint32_t trie_ارزښت, value = null_ارزښت;
    منطقي have_ارزښت = خطأ;
    افعل {
        int32_t i3_بلاک;
        int32_t i3;
        int32_t i3_بلاک_اوږدتیا;
        int32_t معلومات_بلاک_اوږدتیا;
        إذا (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_بلاک = 0;
            i3 = c >> UCPTRIE_FAST_ښويدنه;
            i3_بلاک_اوږدتیا = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_شاخص_اوږدتیا : UCPTRIE_SMALL_شاخص_اوږدتیا;
            معلومات_بلاک_اوږدتیا = UCPTRIE_FAST_معلومات_بلاک_اوږدتیا;
        } وإلا {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_ښويدنه_1;
            إذا (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->لوړ_پیل);
                i1 += UCPTRIE_BMP_شاخص_اوږدتیا - UCPTRIE_OMITTED_BMP_شاخص_1_اوږدتیا;
            } وإلا {
                U_ASSERT(c < trie->لوړ_پیل && trie->لوړ_پیل > UCPTRIE_SMALL_حد);
                i1 += UCPTRIE_SMALL_شاخص_اوږدتیا;
            }
            i3_بلاک = trie->شاخص[
                static_cast<int32_t>(trie->شاخص[i1]) + ((c >> UCPTRIE_ښويدنه_2) & UCPTRIE_شاخص_2_ماسک)];
            إذا (i3_بلاک == prev_I3_بلاک && (c - پیل) >= UCPTRIE_CP_فی_شاخص_2_ثبت) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_فی_شاخص_2_ثبت - 1)) == 0);
                c += UCPTRIE_CP_فی_شاخص_2_ثبت;
                استمر;
            }
            prev_I3_بلاک = i3_بلاک;
            إذا (i3_بلاک == trie->index3_Null_دننه_واټن) {
                // This is the index-3 null block.
                إذا (have_ارزښت) {
                    إذا (null_ارزښت != value) {
                        أرجع c - 1;
                    }
                } وإلا {
                    trie_ارزښت = trie->null_ارزښت;
                    value = null_ارزښت;
                    إذا (p_ارزښت != تش_اشاره) { *p_ارزښت = null_ارزښت; }
                    have_ارزښت = ریښتیا;
                }
                prev_بلاک = trie->معلومات_Null_دننه_واټن;
                c = (c + UCPTRIE_CP_فی_شاخص_2_ثبت) & ~(UCPTRIE_CP_فی_شاخص_2_ثبت - 1);
                استمر;
            }
            i3 = (c >> UCPTRIE_ښويدنه_3) & UCPTRIE_شاخص_3_ماسک;
            i3_بلاک_اوږدتیا = UCPTRIE_شاخص_3_بلاک_اوږدتیا;
            معلومات_بلاک_اوږدتیا = UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
        }
        // Enumerate data blocks for one index-3 block.
        افعل {
            int32_t بلاک;
            إذا ((i3_بلاک & 0x8000) == 0) {
                بلاک = شاخص[i3_بلاک + i3];
            } وإلا {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t ډله = (i3_بلاک & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                بلاک = (static_cast<int32_t>(شاخص[ډله++]) << (2 + (2 * gi))) & 0x30000;
                بلاک |= شاخص[ډله + gi];
            }
            إذا (بلاک == prev_بلاک && (c - پیل) >= معلومات_بلاک_اوږدتیا) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (معلومات_بلاک_اوږدتیا - 1)) == 0);
                c += معلومات_بلاک_اوږدتیا;
            } وإلا {
                int32_t معلومات_ماسک = معلومات_بلاک_اوږدتیا - 1;
                prev_بلاک = بلاک;
                إذا (بلاک == trie->معلومات_Null_دننه_واټن) {
                    // This is the data null block.
                    إذا (have_ارزښت) {
                        إذا (null_ارزښت != value) {
                            أرجع c - 1;
                        }
                    } وإلا {
                        trie_ارزښت = trie->null_ارزښت;
                        value = null_ارزښت;
                        إذا (p_ارزښت != تش_اشاره) { *p_ارزښت = null_ارزښت; }
                        have_ارزښت = ریښتیا;
                    }
                    c = (c + معلومات_بلاک_اوږدتیا) & ~معلومات_ماسک;
                } وإلا {
                    int32_t di = بلاک + (c & معلومات_ماسک);
                    uint32_t trieValue2 = get_ارزښت(trie->معلومات, ارزښت_پلنوالی, di);
                    إذا (have_ارزښت) {
                        إذا (trieValue2 != trie_ارزښت) {
                            إذا (filter == تش_اشاره ||
                                    maybe_Filter_ارزښت(trieValue2, trie->null_ارزښت, null_ارزښت,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_ارزښت = trieValue2;  // may or may not help
                        }
                    } وإلا {
                        trie_ارزښت = trieValue2;
                        value = maybe_Filter_ارزښت(trieValue2, trie->null_ارزښت, null_ارزښت,
                                                 filter, context);
                        إذا (p_ارزښت != تش_اشاره) { *p_ارزښت = value; }
                        have_ارزښت = ریښتیا;
                    }
                    طالما ((++c & معلومات_ماسک) != 0) {
                        trieValue2 = get_ارزښت(trie->معلومات, ارزښت_پلنوالی, ++di);
                        إذا (trieValue2 != trie_ارزښت) {
                            إذا (filter == تش_اشاره ||
                                    maybe_Filter_ارزښت(trieValue2, trie->null_ارزښت, null_ارزښت,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trie_ارزښت = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } طالما (++i3 < i3_بلاک_اوږدتیا);
    } طالما (c < trie->لوړ_پیل);
    U_ASSERT(have_ارزښت);
    int32_t di = trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن;
    uint32_t لوړ_ارزښت = get_ارزښت(trie->معلومات, ارزښت_پلنوالی, di);
    إذا (maybe_Filter_ارزښت(لوړ_ارزښت, trie->null_ارزښت, null_ارزښت,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_دننی_Get_Range(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 پیل,
                         UCPMapRangeOption option, uint32_t surrogate_ارزښت,
                         UCPMap_ارزښت_Filter *filter, ثابت فراغ *context, uint32_t *p_ارزښت) {
    إذا (option == UCPMAP_RANGE_نورمال) {
        أرجع getRange(trie, پیل, filter, context, p_ارزښت);
    }
    uint32_t value;
    إذا (p_ارزښت == تش_اشاره) {
        // We need to examine the range value even if the caller does not want it.
        p_ارزښت = &value;
    }
    UChar32 surr_پای_ټکی = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 پای_ټکی = getRange(trie, پیل, filter, context, p_ارزښت);
    إذا (پای_ټکی < 0xd7ff || پیل > surr_پای_ټکی) {
        أرجع پای_ټکی;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    إذا (*p_ارزښت == surrogate_ارزښت) {
        إذا (پای_ټکی >= surr_پای_ټکی) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            أرجع پای_ټکی;
        }
    } وإلا {
        إذا (پیل <= 0xd7ff) {
            أرجع 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_ارزښت = surrogate_ارزښت;
        إذا (پای_ټکی > surr_پای_ټکی) {
            أرجع surr_پای_ټکی;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_پای_ټکی + 1, filter, context, &value2);
    إذا (value2 == surrogate_ارزښت) {
        أرجع end2;
    }
    أرجع surr_پای_ټکی;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 پیل,
                 UCPMapRangeOption option, uint32_t surrogate_ارزښت,
                 UCPMap_ارزښت_Filter *filter, ثابت فراغ *context, uint32_t *p_ارزښت) {
    أرجع ucptrie_دننی_Get_Range(getRange, trie, پیل,
                                    option, surrogate_ارزښت,
                                    filter, context, p_ارزښت);
}

U_CAPI int32_t U_EXPORT2
ucptrie_منزل_دوګونی(ثابت UCPTrie *trie,
                 فراغ *معلومات, int32_t capacity,
                 UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی = (UCPTrie_ارزښت_پلنوالی)trie->ارزښت_پلنوالی;
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ارزښت_پلنوالی < UCPTRIE_ارزښت_بټونه_16 || UCPTRIE_ارزښت_بټونه_8 < ارزښت_پلنوالی ||
            capacity < 0 ||
            (capacity > 0 && (معلومات == تش_اشاره || (U_اشاره_ګر_ماسک_LSB(معلومات, 3) != 0)))) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t length = (int32_t)حجم(UCPTrie_سرلیک) + trie->شاخص_اوږدتیا * 2;
    بدل (ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        length += trie->معلومات_اوږدتیا * 2;
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        length += trie->معلومات_اوږدتیا * 4;
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        length += trie->معلومات_اوږدتیا;
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إذا (capacity < length) {
        *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        أرجع length;
    }

    حرف *بایټونه = (حرف *)معلومات;
    UCPTrie_سرلیک *سرلیک = (UCPTrie_سرلیک *)بایټونه;
    سرلیک->signature = UCPTRIE_SIG;  // "Tri3"
    سرلیک->options = (uint16_t)(
        ((trie->معلومات_اوږدتیا & 0xf0000) >> 4) |
        ((trie->معلومات_Null_دننه_واټن & 0xf0000) >> 8) |
        (trie->type << 6) |
        ارزښت_پلنوالی);
    سرلیک->شاخص_اوږدتیا = (uint16_t)trie->شاخص_اوږدتیا;
    سرلیک->معلومات_اوږدتیا = (uint16_t)trie->معلومات_اوږدتیا;
    سرلیک->index3_Null_دننه_واټن = trie->index3_Null_دننه_واټن;
    سرلیک->معلومات_Null_دننه_واټن = (uint16_t)trie->معلومات_Null_دننه_واټن;
    سرلیک->shifted_لوړ_پیل = trie->لوړ_پیل >> UCPTRIE_ښويدنه_2;
    بایټونه += حجم(UCPTrie_سرلیک);

    uprv_memcpy(بایټونه, trie->شاخص, trie->شاخص_اوږدتیا * 2);
    بایټونه += trie->شاخص_اوږدتیا * 2;

    بدل (ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        uprv_memcpy(بایټونه, trie->معلومات.ptr16, trie->معلومات_اوږدتیا * 2);
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        uprv_memcpy(بایټونه, trie->معلومات.ptr32, trie->معلومات_اوږدتیا * 4);
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        uprv_memcpy(بایټونه, trie->معلومات.ptr8, trie->معلومات_اوږدتیا);
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    أرجع length;
}

نطاق {

#که_تعریف_شوی UCPTRIE_DEBUG
اوږد شمېر_Null(ثابت UCPTrie *trie) {
    uint32_t null_ارزښت=trie->null_ارزښت;
    int32_t length=trie->معلومات_اوږدتیا;
    اوږد count=0;
    بدل (trie->ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->معلومات.ptr16[i]==null_ارزښت) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->معلومات.ptr32[i]==null_ارزښت) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->معلومات.ptr8[i]==null_ارزښت) { ++count; }
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
    اوږد شاخص_اوږدتیا=trie->شاخص_اوږدتیا;
    اوږد معلومات_اوږدتیا=(اوږد)trie->معلومات_اوږدتیا;
    اوږد ټول_اوږدتیا=(اوږد)حجم(UCPTrie_سرلیک)+شاخص_اوږدتیا*2+
            معلومات_اوږدتیا*(trie->ارزښت_پلنوالی==UCPTRIE_ارزښت_بټونه_16 ? 2 :
                        trie->ارزښت_پلنوالی==UCPTRIE_ارزښت_بټونه_32 ? 4 : 1);
    چاپ_کړه("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, شاخص_اوږدتیا, معلومات_اوږدتیا, شمېر_Null(trie), ټول_اوږدتیا);
}
#د_که_پای

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    أرجع ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 پیل,
                UCPMapRangeOption option, uint32_t surrogate_ارزښت,
                UCPMap_ارزښت_Filter *filter, ثابت فراغ *context, uint32_t *p_ارزښت) {
    أرجع ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), پیل,
                            option, surrogate_ارزښت,
                            filter, context, p_ارزښت);
}
