// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#اگر_تعریف_شده UCPTRIE_DEBUG
#   include <stdio.h>
#پایان_اگر

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#اگر_تعریف_شده سرریز_2
#لغو_تعریف سرریز_2
#پایان_اگر

U_NAMESPACE_BEGIN

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_حد = 0x110000;
constexpr int32_t BMP_حد = 0x10000;
constexpr int32_t ASCII_حد = 0x80;

constexpr int32_t I_حد = UNICODE_حد >> UCPTRIE_لغزش_3;
constexpr int32_t BMP_I_حد = BMP_حد >> UCPTRIE_لغزش_3;
constexpr int32_t ASCII_I_حد = ASCII_حد >> UCPTRIE_لغزش_3;

constexpr int32_t SMALL_داده_بلوکها_در_هر_BMP_بلوک = (1 << (UCPTRIE_FAST_لغزش - UCPTRIE_لغزش_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_داده_درازی = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_داده_درازی = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_داده_درازی = UNICODE_حد;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t شاخص_3_18_بیت_بلوک_درازی = UCPTRIE_شاخص_3_بلوک_درازی + UCPTRIE_شاخص_3_بلوک_درازی / 8;

صنف All_Same_بلوکها;
صنف Mixed_بلوکها;

صنف Mutable_کد_نقطه_Trie : عام UMemory {
عام:
    Mutable_کد_نقطه_Trie(uint32_t initial_مقدار, uint32_t error_مقدار, UError_کد &error_کد);
    Mutable_کد_نقطه_Trie(ثابت Mutable_کد_نقطه_Trie &other, UError_کد &error_کد);
    Mutable_کد_نقطه_Trie(ثابت Mutable_کد_نقطه_Trie &other) = حذف;
    ~Mutable_کد_نقطه_Trie();

    Mutable_کد_نقطه_Trie &عملگر=(ثابت Mutable_کد_نقطه_Trie &other) = حذف;

    ساكن Mutable_کد_نقطه_Trie *fromUCPMap(ثابت UCPMap *map, UError_کد &error_کد);
    ساكن Mutable_کد_نقطه_Trie *fromUCPTrie(ثابت UCPTrie *trie, UError_کد &error_کد);

    uint32_t get(UChar32 c) ثابت;
    int32_t getRange(UChar32 شروع, UCPMap_مقدار_Filter *filter, ثابت فراغ *context,
                     uint32_t *p_مقدار) ثابت;

    فراغ مجموعه(UChar32 c, uint32_t value, UError_کد &error_کد);
    فراغ مجموعه_Range(UChar32 شروع, UChar32 انتها, uint32_t value, UError_کد &error_کد);

    UCPTrie *build(UCPTrieType type, UCPTrie_مقدار_عرض مقدار_عرض, UError_کد &error_کد);

خاص:
    فراغ clear();

    منطقي ensure_بالا_شروع(UChar32 c);
    int32_t alloc_داده_بلوک(int32_t بلوک_درازی);
    int32_t get_داده_بلوک(int32_t i);

    فراغ ماسک_مقادیر(uint32_t mask);
    UChar32 پیدا_کن_بالا_شروع() ثابت;
    int32_t compact_کامل_داده_بلوکها(int32_t fastILimit, All_Same_بلوکها &all_Same_بلوکها);
    int32_t compact_داده(
            int32_t fastILimit, uint32_t *new_داده, int32_t new_داده_ظرفیت,
            int32_t داده_Null_شاخص, Mixed_بلوکها &mixed_بلوکها, UError_کد &error_کد);
    int32_t compact_شاخص(int32_t fastILimit, Mixed_بلوکها &mixed_بلوکها, UError_کد &error_کد);
    int32_t compactTrie(int32_t fastILimit, UError_کد &error_کد);

    uint32_t *شاخص = اشاره_تهی;
    int32_t شاخص_ظرفیت = 0;
    int32_t index3_Null_جابه_جایی = -1;
    uint32_t *داده = اشاره_تهی;
    int32_t داده_ظرفیت = 0;
    int32_t داده_درازی = 0;
    int32_t داده_Null_جابه_جایی = -1;

    uint32_t orig_Initial_مقدار;
    uint32_t initial_مقدار;
    uint32_t error_مقدار;
    UChar32 بالا_شروع;
    uint32_t بالا_مقدار;
#اگر_تعریف_شده UCPTRIE_DEBUG
عام:
    ثابت حرف *name;
#پایان_اگر
خاص:
    /** Temporary array while building the final data. */
    uint16_t *index16 = اشاره_تهی;
    uint8_t flags[UNICODE_حد >> UCPTRIE_لغزش_3];
};

Mutable_کد_نقطه_Trie::Mutable_کد_نقطه_Trie(uint32_t ini_مقدار, uint32_t err_مقدار, UError_کد &error_کد) :
        orig_Initial_مقدار(ini_مقدار), initial_مقدار(ini_مقدار), error_مقدار(err_مقدار),
        بالا_شروع(0), بالا_مقدار(initial_مقدار)
#اگر_تعریف_شده UCPTRIE_DEBUG
        , name("open")
#پایان_اگر
        {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    شاخص = static_cast<uint32_t*>(uprv_malloc(BMP_I_حد * 4));
    داده = static_cast<uint32_t*>(uprv_malloc(INITIAL_داده_درازی * 4));
    إذا (شاخص == اشاره_تهی || داده == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    شاخص_ظرفیت = BMP_I_حد;
    داده_ظرفیت = INITIAL_داده_درازی;
}

Mutable_کد_نقطه_Trie::Mutable_کد_نقطه_Trie(ثابت Mutable_کد_نقطه_Trie &other, UError_کد &error_کد) :
        index3_Null_جابه_جایی(other.index3_Null_جابه_جایی),
        داده_Null_جابه_جایی(other.داده_Null_جابه_جایی),
        orig_Initial_مقدار(other.orig_Initial_مقدار), initial_مقدار(other.initial_مقدار),
        error_مقدار(other.error_مقدار),
        بالا_شروع(other.بالا_شروع), بالا_مقدار(other.بالا_مقدار)
#اگر_تعریف_شده UCPTRIE_DEBUG
        , name("mutable clone")
#پایان_اگر
        {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    int32_t i_ظرفیت = بالا_شروع <= BMP_حد ? BMP_I_حد : I_حد;
    شاخص = static_cast<uint32_t*>(uprv_malloc(i_ظرفیت * 4));
    داده = static_cast<uint32_t*>(uprv_malloc(other.داده_ظرفیت * 4));
    إذا (شاخص == اشاره_تهی || داده == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    شاخص_ظرفیت = i_ظرفیت;
    داده_ظرفیت = other.داده_ظرفیت;

    int32_t i_حد = بالا_شروع >> UCPTRIE_لغزش_3;
    uprv_memcpy(flags, other.flags, i_حد);
    uprv_memcpy(شاخص, other.شاخص, i_حد * 4);
    uprv_memcpy(داده, other.داده, (size_t)other.داده_درازی * 4);
    داده_درازی = other.داده_درازی;
    U_ASSERT(other.index16 == اشاره_تهی);
}

Mutable_کد_نقطه_Trie::~Mutable_کد_نقطه_Trie() {
    uprv_free(شاخص);
    uprv_free(داده);
    uprv_free(index16);
}

Mutable_کد_نقطه_Trie *Mutable_کد_نقطه_Trie::fromUCPMap(ثابت UCPMap *map, UError_کد &error_کد) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_مقدار = ucpmap_get(map, -1);
    uint32_t initial_مقدار = ucpmap_get(map, 0x10ffff);
    Local_اشاره_گر<Mutable_کد_نقطه_Trie> mutableTrie(
        جديد Mutable_کد_نقطه_Trie(initial_مقدار, error_مقدار, error_کد),
        error_کد);
    إذا (U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    UChar32 شروع = 0, انتها;
    uint32_t value;
    طالما ((انتها = ucpmap_getRange(map, شروع, UCPMAP_RANGE_نرمال, 0,
                                  اشاره_تهی, اشاره_تهی, &value)) >= 0) {
        إذا (value != initial_مقدار) {
            إذا (شروع == انتها) {
                mutableTrie->مجموعه(شروع, value, error_کد);
            } وإلا {
                mutableTrie->مجموعه_Range(شروع, انتها, value, error_کد);
            }
        }
        شروع = انتها + 1;
    }
    إذا (U_SUCCESS(error_کد)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع اشاره_تهی;
    }
}

Mutable_کد_نقطه_Trie *Mutable_کد_نقطه_Trie::fromUCPTrie(ثابت UCPTrie *trie, UError_کد &error_کد) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_مقدار;
    uint32_t initial_مقدار;
    بدل (trie->مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        error_مقدار = trie->داده.ptr16[trie->داده_درازی - UCPTRIE_ERROR_مقدار_NEG_داده_جابه_جایی];
        initial_مقدار = trie->داده.ptr16[trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی];
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        error_مقدار = trie->داده.ptr32[trie->داده_درازی - UCPTRIE_ERROR_مقدار_NEG_داده_جابه_جایی];
        initial_مقدار = trie->داده.ptr32[trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی];
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        error_مقدار = trie->داده.ptr8[trie->داده_درازی - UCPTRIE_ERROR_مقدار_NEG_داده_جابه_جایی];
        initial_مقدار = trie->داده.ptr8[trie->داده_درازی - UCPTRIE_بالا_مقدار_NEG_داده_جابه_جایی];
        اكسر;
    افتراضي:
        // Unreachable if the trie is properly initialized.
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    Local_اشاره_گر<Mutable_کد_نقطه_Trie> mutableTrie(
        جديد Mutable_کد_نقطه_Trie(initial_مقدار, error_مقدار, error_کد),
        error_کد);
    إذا (U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    UChar32 شروع = 0, انتها;
    uint32_t value;
    طالما ((انتها = ucptrie_getRange(trie, شروع, UCPMAP_RANGE_نرمال, 0,
                                   اشاره_تهی, اشاره_تهی, &value)) >= 0) {
        إذا (value != initial_مقدار) {
            إذا (شروع == انتها) {
                mutableTrie->مجموعه(شروع, value, error_کد);
            } وإلا {
                mutableTrie->مجموعه_Range(شروع, انتها, value, error_کد);
            }
        }
        شروع = انتها + 1;
    }
    إذا (U_SUCCESS(error_کد)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع اشاره_تهی;
    }
}

فراغ Mutable_کد_نقطه_Trie::clear() {
    index3_Null_جابه_جایی = داده_Null_جابه_جایی = -1;
    داده_درازی = 0;
    بالا_مقدار = initial_مقدار = orig_Initial_مقدار;
    بالا_شروع = 0;
    uprv_free(index16);
    index16 = اشاره_تهی;
}

uint32_t Mutable_کد_نقطه_Trie::get(UChar32 c) ثابت {
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        أرجع error_مقدار;
    }
    إذا (c >= بالا_شروع) {
        أرجع بالا_مقدار;
    }
    int32_t i = c >> UCPTRIE_لغزش_3;
    إذا (flags[i] == ALL_SAME) {
        أرجع شاخص[i];
    } وإلا {
        أرجع داده[شاخص[i] + (c & UCPTRIE_SMALL_داده_ماسک)];
    }
}

مضمن uint32_t maybe_Filter_مقدار(uint32_t value, uint32_t initial_مقدار, uint32_t null_مقدار,
                                 UCPMap_مقدار_Filter *filter, ثابت فراغ *context) {
    إذا (value == initial_مقدار) {
        value = null_مقدار;
    } وإلا إذا (filter != اشاره_تهی) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 Mutable_کد_نقطه_Trie::getRange(
        UChar32 شروع, UCPMap_مقدار_Filter *filter, ثابت فراغ *context,
        uint32_t *p_مقدار) ثابت {
    إذا (static_cast<uint32_t>(شروع) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    إذا (شروع >= بالا_شروع) {
        إذا (p_مقدار != اشاره_تهی) {
            uint32_t value = بالا_مقدار;
            إذا (filter != اشاره_تهی) { value = filter(context, value); }
            *p_مقدار = value;
        }
        أرجع MAX_UNICODE;
    }
    uint32_t null_مقدار = initial_مقدار;
    إذا (filter != اشاره_تهی) { null_مقدار = filter(context, null_مقدار); }
    UChar32 c = شروع;
    uint32_t trie_مقدار, value;
    منطقي have_مقدار = خطأ;
    int32_t i = c >> UCPTRIE_لغزش_3;
    افعل {
        إذا (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = شاخص[i];
            إذا (have_مقدار) {
                إذا (trieValue2 != trie_مقدار) {
                    إذا (filter == اشاره_تهی ||
                            maybe_Filter_مقدار(trieValue2, initial_مقدار, null_مقدار,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_مقدار = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_مقدار = trieValue2;
                value = maybe_Filter_مقدار(trieValue2, initial_مقدار, null_مقدار, filter, context);
                إذا (p_مقدار != اشاره_تهی) { *p_مقدار = value; }
                have_مقدار = درست;
            }
            c = (c + UCPTRIE_SMALL_داده_بلوک_درازی) & ~UCPTRIE_SMALL_داده_ماسک;
        } وإلا /* MIXED */ {
            int32_t di = شاخص[i] + (c & UCPTRIE_SMALL_داده_ماسک);
            uint32_t trieValue2 = داده[di];
            إذا (have_مقدار) {
                إذا (trieValue2 != trie_مقدار) {
                    إذا (filter == اشاره_تهی ||
                            maybe_Filter_مقدار(trieValue2, initial_مقدار, null_مقدار,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_مقدار = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_مقدار = trieValue2;
                value = maybe_Filter_مقدار(trieValue2, initial_مقدار, null_مقدار, filter, context);
                إذا (p_مقدار != اشاره_تهی) { *p_مقدار = value; }
                have_مقدار = درست;
            }
            طالما ((++c & UCPTRIE_SMALL_داده_ماسک) != 0) {
                trieValue2 = داده[++di];
                إذا (trieValue2 != trie_مقدار) {
                    إذا (filter == اشاره_تهی ||
                            maybe_Filter_مقدار(trieValue2, initial_مقدار, null_مقدار,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                }
                trie_مقدار = trieValue2;  // may or may not help
            }
        }
        ++i;
    } طالما (c < بالا_شروع);
    U_ASSERT(have_مقدار);
    إذا (maybe_Filter_مقدار(بالا_مقدار, initial_مقدار, null_مقدار,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

فراغ
write_بلوک(uint32_t *بلوک, uint32_t value) {
    uint32_t *حد = بلوک + UCPTRIE_SMALL_داده_بلوک_درازی;
    طالما (بلوک < حد) {
        *بلوک++ = value;
    }
}

منطقي Mutable_کد_نقطه_Trie::ensure_بالا_شروع(UChar32 c) {
    إذا (c >= بالا_شروع) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_در_هر_شاخص_2_مدخل) & ~(UCPTRIE_CP_در_هر_شاخص_2_مدخل - 1);
        int32_t i = بالا_شروع >> UCPTRIE_لغزش_3;
        int32_t i_حد = c >> UCPTRIE_لغزش_3;
        إذا (i_حد > شاخص_ظرفیت) {
            uint32_t* new_شاخص = static_cast<uint32_t*>(uprv_malloc(I_حد * 4));
            إذا (new_شاخص == اشاره_تهی) { أرجع خطأ; }
            uprv_memcpy(new_شاخص, شاخص, i * 4);
            uprv_free(شاخص);
            شاخص = new_شاخص;
            شاخص_ظرفیت = I_حد;
        }
        افعل {
            flags[i] = ALL_SAME;
            شاخص[i] = initial_مقدار;
        } طالما(++i < i_حد);
        بالا_شروع = c;
    }
    أرجع درست;
}

int32_t Mutable_کد_نقطه_Trie::alloc_داده_بلوک(int32_t بلوک_درازی) {
    int32_t new_بلوک = داده_درازی;
    int32_t new_بالاترین = new_بلوک + بلوک_درازی;
    إذا (new_بالاترین > داده_ظرفیت) {
        int32_t capacity;
        إذا (داده_ظرفیت < MEDIUM_داده_درازی) {
            capacity = MEDIUM_داده_درازی;
        } وإلا إذا (داده_ظرفیت < MAX_داده_درازی) {
            capacity = MAX_داده_درازی;
        } وإلا {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            أرجع -1;
        }
        uint32_t* new_داده = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        إذا (new_داده == اشاره_تهی) {
            أرجع -1;
        }
        uprv_memcpy(new_داده, داده, (size_t)داده_درازی * 4);
        uprv_free(داده);
        داده = new_داده;
        داده_ظرفیت = capacity;
    }
    داده_درازی = new_بالاترین;
    أرجع new_بلوک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_کد_نقطه_Trie::get_داده_بلوک(int32_t i) {
    إذا (flags[i] == MIXED) {
        أرجع شاخص[i];
    }
    إذا (i < BMP_I_حد) {
        int32_t new_بلوک = alloc_داده_بلوک(UCPTRIE_FAST_داده_بلوک_درازی);
        إذا (new_بلوک < 0) { أرجع new_بلوک; }
        int32_t i_شروع = i & ~(SMALL_داده_بلوکها_در_هر_BMP_بلوک -1);
        int32_t i_حد = i_شروع + SMALL_داده_بلوکها_در_هر_BMP_بلوک;
        افعل {
            U_ASSERT(flags[i_شروع] == ALL_SAME);
            write_بلوک(داده + new_بلوک, شاخص[i_شروع]);
            flags[i_شروع] = MIXED;
            شاخص[i_شروع++] = new_بلوک;
            new_بلوک += UCPTRIE_SMALL_داده_بلوک_درازی;
        } طالما (i_شروع < i_حد);
        أرجع شاخص[i];
    } وإلا {
        int32_t new_بلوک = alloc_داده_بلوک(UCPTRIE_SMALL_داده_بلوک_درازی);
        إذا (new_بلوک < 0) { أرجع new_بلوک; }
        write_بلوک(داده + new_بلوک, شاخص[i]);
        flags[i] = MIXED;
        شاخص[i] = new_بلوک;
        أرجع new_بلوک;
    }
}

فراغ Mutable_کد_نقطه_Trie::مجموعه(UChar32 c, uint32_t value, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    int32_t بلوک;
    إذا (!ensure_بالا_شروع(c) || (بلوک = get_داده_بلوک(c >> UCPTRIE_لغزش_3)) < 0) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    داده[بلوک + (c & UCPTRIE_SMALL_داده_ماسک)] = value;
}

فراغ
fill_بلوک(uint32_t *بلوک, UChar32 شروع, UChar32 حد, uint32_t value) {
    uint32_t *p_حد = بلوک + حد;
    بلوک += شروع;
    طالما (بلوک < p_حد) {
        *بلوک++ = value;
    }
}

فراغ Mutable_کد_نقطه_Trie::مجموعه_Range(UChar32 شروع, UChar32 انتها, uint32_t value, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(شروع) > MAX_UNICODE || static_cast<uint32_t>(انتها) > MAX_UNICODE || شروع > انتها) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا (!ensure_بالا_شروع(انتها)) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    UChar32 حد = انتها + 1;
    إذا (شروع & UCPTRIE_SMALL_داده_ماسک) {
        // Set partial block at [start..following block boundary[.
        int32_t بلوک = get_داده_بلوک(شروع >> UCPTRIE_لغزش_3);
        إذا (بلوک < 0) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع;
        }

        UChar32 بعدی_شروع = (شروع + UCPTRIE_SMALL_داده_ماسک) & ~UCPTRIE_SMALL_داده_ماسک;
        إذا (بعدی_شروع <= حد) {
            fill_بلوک(داده + بلوک, شروع & UCPTRIE_SMALL_داده_ماسک, UCPTRIE_SMALL_داده_بلوک_درازی,
                      value);
            شروع = بعدی_شروع;
        } وإلا {
            fill_بلوک(داده + بلوک, شروع & UCPTRIE_SMALL_داده_ماسک, حد & UCPTRIE_SMALL_داده_ماسک,
                      value);
            أرجع;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = حد & UCPTRIE_SMALL_داده_ماسک;

    // Round down limit to a block boundary.
    حد &= ~UCPTRIE_SMALL_داده_ماسک;

    // Iterate over all-value blocks.
    طالما (شروع < حد) {
        int32_t i = شروع >> UCPTRIE_لغزش_3;
        إذا (flags[i] == ALL_SAME) {
            شاخص[i] = value;
        } وإلا /* MIXED */ {
            fill_بلوک(داده + شاخص[i], 0, UCPTRIE_SMALL_داده_بلوک_درازی, value);
        }
        شروع += UCPTRIE_SMALL_داده_بلوک_درازی;
    }

    إذا (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t بلوک = get_داده_بلوک(شروع >> UCPTRIE_لغزش_3);
        إذا (بلوک < 0) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع;
        }

        fill_بلوک(داده + بلوک, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

فراغ Mutable_کد_نقطه_Trie::ماسک_مقادیر(uint32_t mask) {
    initial_مقدار &= mask;
    error_مقدار &= mask;
    بالا_مقدار &= mask;
    int32_t i_حد = بالا_شروع >> UCPTRIE_لغزش_3;
    لكل (int32_t i = 0; i < i_حد; ++i) {
        إذا (flags[i] == ALL_SAME) {
            شاخص[i] &= mask;
        }
    }
    لكل (int32_t i = 0; i < داده_درازی; ++i) {
        داده[i] &= mask;
    }
}

قالب<نام_نوع UIntA, نام_نوع UIntB>
منطقي equal_بلوکها(ثابت UIntA *s, ثابت UIntB *t, int32_t length) {
    طالما (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

منطقي all_مقادیر_Same_As(ثابت uint32_t *p, int32_t length, uint32_t value) {
    ثابت uint32_t *p_حد = p + length;
    طالما (p < p_حد && *p == value) { ++p; }
    أرجع p == p_حد;
}

/** Search for an identical block. */
int32_t پیدا_کن_Same_بلوک(ثابت uint16_t *p, int32_t p_شروع, int32_t length,
                      ثابت uint16_t *q, int32_t q_شروع, int32_t بلوک_درازی) {
    // Ensure that we do not even partially get past length.
    length -= بلوک_درازی;

    q += q_شروع;
    طالما (p_شروع <= length) {
        إذا (equal_بلوکها(p + p_شروع, q, بلوک_درازی)) {
            أرجع p_شروع;
        }
        ++p_شروع;
    }
    أرجع -1;
}

int32_t پیدا_کن_All_Same_بلوک(ثابت uint32_t *p, int32_t شروع, int32_t حد,
                         uint32_t value, int32_t بلوک_درازی) {
    // Ensure that we do not even partially get past limit.
    حد -= بلوک_درازی;

    لكل (int32_t بلوک = شروع; بلوک <= حد; ++بلوک) {
        إذا (p[بلوک] == value) {
            لكل (int32_t i = 1;; ++i) {
                إذا (i == بلوک_درازی) {
                    أرجع بلوک;
                }
                إذا (p[بلوک + i] != value) {
                    بلوک += i;
                    اكسر;
                }
            }
        }
    }
    أرجع -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
قالب<نام_نوع UIntA, نام_نوع UIntB>
int32_t getOverlap(ثابت UIntA *p, int32_t length,
                   ثابت UIntB *q, int32_t q_شروع, int32_t بلوک_درازی) {
    int32_t overlap = بلوک_درازی - 1;
    U_ASSERT(overlap <= length);
    q += q_شروع;
    طالما (overlap > 0 && !equal_بلوکها(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    أرجع overlap;
}

int32_t getAllSameOverlap(ثابت uint32_t *p, int32_t length, uint32_t value,
                          int32_t بلوک_درازی) {
    int32_t min = length - (بلوک_درازی - 1);
    int32_t i = length;
    طالما (min < i && p[i - 1] == value) { --i; }
    أرجع length - i;
}

منطقي is_شروع_از_Some_Fast_بلوک(uint32_t داده_جابه_جایی, ثابت uint32_t شاخص[], int32_t fastILimit) {
    لكل (int32_t i = 0; i < fastILimit; i += SMALL_داده_بلوکها_در_هر_BMP_بلوک) {
        إذا (شاخص[i] == داده_جابه_جایی) {
            أرجع درست;
        }
    }
    أرجع خطأ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_کد_نقطه_Trie::پیدا_کن_بالا_شروع() ثابت {
    int32_t i = بالا_شروع >> UCPTRIE_لغزش_3;
    طالما (i > 0) {
        منطقي تطابق;
        إذا (flags[--i] == ALL_SAME) {
            تطابق = شاخص[i] == بالا_مقدار;
        } وإلا /* MIXED */ {
            ثابت uint32_t *p = داده + شاخص[i];
            لكل (int32_t j = 0;; ++j) {
                إذا (j == UCPTRIE_SMALL_داده_بلوک_درازی) {
                    تطابق = درست;
                    اكسر;
                }
                إذا (p[j] != بالا_مقدار) {
                    تطابق = خطأ;
                    اكسر;
                }
            }
        }
        إذا (!تطابق) {
            أرجع (i + 1) << UCPTRIE_لغزش_3;
        }
    }
    أرجع 0;
}

صنف All_Same_بلوکها {
عام:
    ساكن constexpr int32_t NEW_UNIQUE = -1;
    ساكن constexpr int32_t سرریز_2 = -2;

    All_Same_بلوکها() : length(0), mostRecent(-1) {}

    int32_t پیدا_کن_Or_جمع_کن(int32_t شاخص, int32_t count, uint32_t value) {
        إذا (mostRecent >= 0 && مقادیر[mostRecent] == value) {
            refCounts[mostRecent] += count;
            أرجع indexes[mostRecent];
        }
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (مقادیر[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                أرجع indexes[i];
            }
        }
        إذا (length == ظرفیت_3) {
            أرجع سرریز_2;
        }
        mostRecent = length;
        indexes[length] = شاخص;
        مقادیر[length] = value;
        refCounts[length++] = count;
        أرجع NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    فراغ جمع_کن(int32_t شاخص, int32_t count, uint32_t value) {
        U_ASSERT(length == ظرفیت_3);
        int32_t least = -1;
        int32_t کمترین_شمار = I_حد;
        لكل (int32_t i = 0; i < length; ++i) {
            U_ASSERT(مقادیر[i] != value);
            إذا (refCounts[i] < کمترین_شمار) {
                least = i;
                کمترین_شمار = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = شاخص;
        مقادیر[least] = value;
        refCounts[least] = count;
    }

    int32_t پیدا_کن_Most_استفاده_شده() ثابت {
        إذا (length == 0) { أرجع -1; }
        int32_t max = -1;
        int32_t max_شمار = 0;
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (refCounts[i] > max_شمار) {
                max = i;
                max_شمار = refCounts[i];
            }
        }
        أرجع indexes[max];
    }

خاص:
    ساكن constexpr int32_t ظرفیت_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[ظرفیت_3];
    uint32_t مقادیر[ظرفیت_3];
    int32_t refCounts[ظرفیت_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
صنف Mixed_بلوکها {
عام:
    Mixed_بلوکها() {}
    ~Mixed_بلوکها() {
        uprv_free(جدول);
    }

    منطقي init(int32_t max_درازی_2, int32_t new_بلوک_درازی) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_داده_شاخص = max_درازی_2 - new_بلوک_درازی + 1;
        int32_t new_درازی;
        إذا (max_داده_شاخص <= 0xfff) {  // 4k
            new_درازی = 6007;
            لغزش = 12;
            mask = 0xfff;
        } وإلا إذا (max_داده_شاخص <= 0x7fff) {  // 32k
            new_درازی = 50021;
            لغزش = 15;
            mask = 0x7fff;
        } وإلا إذا (max_داده_شاخص <= 0x1ffff) {  // 128k
            new_درازی = 200003;
            لغزش = 17;
            mask = 0x1ffff;
        } وإلا {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_درازی = 1500007;
            لغزش = 21;
            mask = 0x1fffff;
        }
        إذا (new_درازی > capacity) {
            uprv_free(جدول);
            جدول = static_cast<uint32_t*>(uprv_malloc(new_درازی * 4));
            إذا (جدول == اشاره_تهی) {
                أرجع خطأ;
            }
            capacity = new_درازی;
        }
        length = new_درازی;
        uprv_memset(جدول, 0, length * 4);

        بلوک_درازی = new_بلوک_درازی;
        أرجع درست;
    }

    قالب<نام_نوع UInt>
    فراغ extend(ثابت UInt *داده, int32_t min_شروع, int32_t prev_داده_درازی, int32_t new_داده_درازی) {
        int32_t شروع = prev_داده_درازی - بلوک_درازی;
        إذا (شروع >= min_شروع) {
            ++شروع;  // Skip the last block that we added last time.
        } وإلا {
            شروع = min_شروع;  // Begin with the first full block.
        }
        لكل (int32_t انتها = new_داده_درازی - بلوک_درازی; شروع <= انتها; ++شروع) {
            uint32_t hash_کد = make_Hash_کد(داده, شروع);
            جمع_کن_مدخل(داده, شروع, hash_کد, شروع);
        }
    }

    قالب<نام_نوع UIntA, نام_نوع UIntB>
    int32_t پیدا_کن_بلوک(ثابت UIntA *داده, ثابت UIntB *بلوک_داده, int32_t بلوک_شروع) ثابت {
        uint32_t hash_کد = make_Hash_کد(بلوک_داده, بلوک_شروع);
        int32_t مدخل_شاخص = پیدا_کن_مدخل(داده, بلوک_داده, بلوک_شروع, hash_کد);
        إذا (مدخل_شاخص >= 0) {
            أرجع (جدول[مدخل_شاخص] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

    int32_t پیدا_کن_All_Same_بلوک(ثابت uint32_t *داده, uint32_t بلوک_مقدار) ثابت {
        uint32_t hash_کد = make_Hash_کد(بلوک_مقدار);
        int32_t مدخل_شاخص = پیدا_کن_مدخل(داده, بلوک_مقدار, hash_کد);
        إذا (مدخل_شاخص >= 0) {
            أرجع (جدول[مدخل_شاخص] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

خاص:
    قالب<نام_نوع UInt>
    uint32_t make_Hash_کد(ثابت UInt *بلوک_داده, int32_t بلوک_شروع) ثابت {
        int32_t بلوک_حد = بلوک_شروع + بلوک_درازی;
        uint32_t hash_کد = بلوک_داده[بلوک_شروع++];
        افعل {
            hash_کد = 37 * hash_کد + بلوک_داده[بلوک_شروع++];
        } طالما (بلوک_شروع < بلوک_حد);
        أرجع hash_کد;
    }

    uint32_t make_Hash_کد(uint32_t بلوک_مقدار) ثابت {
        uint32_t hash_کد = بلوک_مقدار;
        لكل (int32_t i = 1; i < بلوک_درازی; ++i) {
            hash_کد = 37 * hash_کد + بلوک_مقدار;
        }
        أرجع hash_کد;
    }

    قالب<نام_نوع UInt>
    فراغ جمع_کن_مدخل(ثابت UInt *داده, int32_t بلوک_شروع, uint32_t hash_کد, int32_t داده_شاخص) {
        U_ASSERT(0 <= داده_شاخص && داده_شاخص < (int32_t)mask);
        int32_t مدخل_شاخص = پیدا_کن_مدخل(داده, داده, بلوک_شروع, hash_کد);
        إذا (مدخل_شاخص < 0) {
            جدول[~مدخل_شاخص] = (hash_کد << لغزش) | (داده_شاخص + 1);
        }
    }

    قالب<نام_نوع UIntA, نام_نوع UIntB>
    int32_t پیدا_کن_مدخل(ثابت UIntA *داده, ثابت UIntB *بلوک_داده, int32_t بلوک_شروع,
                      uint32_t hash_کد) ثابت {
        uint32_t shifted_Hash_کد = hash_کد << لغزش;
        int32_t initial_مدخل_شاخص = (hash_کد % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t مدخل_شاخص = initial_مدخل_شاخص;;) {
            uint32_t مدخل = جدول[مدخل_شاخص];
            إذا (مدخل == 0) {
                أرجع ~مدخل_شاخص;
            }
            إذا ((مدخل & ~mask) == shifted_Hash_کد) {
                int32_t داده_شاخص = (مدخل & mask) - 1;
                إذا (equal_بلوکها(داده + داده_شاخص, بلوک_داده + بلوک_شروع, بلوک_درازی)) {
                    أرجع مدخل_شاخص;
                }
            }
            مدخل_شاخص = بعدی_شاخص(initial_مدخل_شاخص, مدخل_شاخص);
        }
    }

    int32_t پیدا_کن_مدخل(ثابت uint32_t *داده, uint32_t بلوک_مقدار, uint32_t hash_کد) ثابت {
        uint32_t shifted_Hash_کد = hash_کد << لغزش;
        int32_t initial_مدخل_شاخص = (hash_کد % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t مدخل_شاخص = initial_مدخل_شاخص;;) {
            uint32_t مدخل = جدول[مدخل_شاخص];
            إذا (مدخل == 0) {
                أرجع ~مدخل_شاخص;
            }
            إذا ((مدخل & ~mask) == shifted_Hash_کد) {
                int32_t داده_شاخص = (مدخل & mask) - 1;
                إذا (all_مقادیر_Same_As(داده + داده_شاخص, بلوک_درازی, بلوک_مقدار)) {
                    أرجع مدخل_شاخص;
                }
            }
            مدخل_شاخص = بعدی_شاخص(initial_مدخل_شاخص, مدخل_شاخص);
        }
    }

    مضمن int32_t بعدی_شاخص(int32_t initial_مدخل_شاخص, int32_t مدخل_شاخص) ثابت {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        أرجع (مدخل_شاخص + initial_مدخل_شاخص) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *جدول = اشاره_تهی;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t لغزش = 0;
    uint32_t mask = 0;

    int32_t بلوک_درازی = 0;
};

int32_t Mutable_کد_نقطه_Trie::compact_کامل_داده_بلوکها(int32_t fastILimit, All_Same_بلوکها &all_Same_بلوکها) {
#اگر_تعریف_شده UCPTRIE_DEBUG
    منطقي سرریز = خطأ;
#پایان_اگر

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_داده_ظرفیت = ASCII_حد;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_داده_ظرفیت += UCPTRIE_SMALL_داده_بلوک_درازی;
    // Add room for special values (errorValue, highValue) and padding.
    new_داده_ظرفیت += 4;
    int32_t i_حد = بالا_شروع >> UCPTRIE_لغزش_3;
    int32_t بلوک_درازی = UCPTRIE_FAST_داده_بلوک_درازی;
    int32_t inc = SMALL_داده_بلوکها_در_هر_BMP_بلوک;
    لكل (int32_t i = 0; i < i_حد; i += inc) {
        إذا (i == fastILimit) {
            بلوک_درازی = UCPTRIE_SMALL_داده_بلوک_درازی;
            inc = 1;
        }
        uint32_t value = شاخص[i];
        إذا (flags[i] == MIXED) {
            // Really mixed?
            ثابت uint32_t *p = داده + value;
            value = *p;
            إذا (all_مقادیر_Same_As(p + 1, بلوک_درازی - 1, value)) {
                flags[i] = ALL_SAME;
                شاخص[i] = value;
                // Fall through to ALL_SAME handling.
            } وإلا {
                new_داده_ظرفیت += بلوک_درازی;
                استمر;
            }
        } وإلا {
            U_ASSERT(flags[i] == ALL_SAME);
            إذا (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                منطقي allSame = درست;
                int32_t بعدی_i = i + inc;
                لكل (int32_t j = i + 1; j < بعدی_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    إذا (شاخص[j] != value) {
                        allSame = خطأ;
                        اكسر;
                    }
                }
                إذا (!allSame) {
                    // Turn it into a MIXED block.
                    إذا (get_داده_بلوک(i) < 0) {
                        أرجع -1;
                    }
                    new_داده_ظرفیت += بلوک_درازی;
                    استمر;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_بلوکها.پیدا_کن_Or_جمع_کن(i, inc, value);
        إذا (other == All_Same_بلوکها::سرریز_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#اگر_تعریف_شده UCPTRIE_DEBUG
            إذا (!سرریز) {
                puts("UCPTrie AllSameBlocks overflow");
                سرریز = درست;
            }
#پایان_اگر
            int32_t jInc = SMALL_داده_بلوکها_در_هر_BMP_بلوک;
            لكل (int32_t j = 0;; j += jInc) {
                إذا (j == i) {
                    all_Same_بلوکها.جمع_کن(i, inc, value);
                    اكسر;
                }
                إذا (j == fastILimit) {
                    jInc = 1;
                }
                إذا (flags[j] == ALL_SAME && شاخص[j] == value) {
                    all_Same_بلوکها.جمع_کن(j, jInc + inc, value);
                    other = j;
                    اكسر;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        إذا (other >= 0) {
            flags[i] = SAME_AS;
            شاخص[i] = other;
        } وإلا {
            // New unique same-value block.
            new_داده_ظرفیت += بلوک_درازی;
        }
    }
    أرجع new_داده_ظرفیت;
}

#اگر_تعریف_شده UCPTRIE_DEBUG
#   define DEBUG_DO(عبارت) عبارت
#وإلا
#   define DEBUG_DO(عبارت)
#پایان_اگر

#اگر_تعریف_شده UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_مقدار(حرف s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (حرف)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (حرف)(0x80 + (value & 0x3F));
    أرجع length + 3;
}

فراغ print_بلوک(ثابت uint32_t *بلوک, int32_t بلوک_درازی, uint32_t value,
                UChar32 شروع, int32_t overlap, uint32_t initial_مقدار) {
    حرف s[UCPTRIE_FAST_داده_بلوک_درازی * 3 + 3];
    int32_t length = 0;
    int32_t i;
    لكل (i = 0; i < overlap; ++i) {
        length = append_مقدار(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    لكل (; i < بلوک_درازی; ++i) {
        إذا (بلوک != اشاره_تهی) {
            value = بلوک[i];
        }
        إذا (value == initial_مقدار) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_مقدار(s, length, value);
    }
    s[length] = 0;
    شروع += overlap;
    إذا (شروع <= 0xffff) {
        چاپ_کن("    %04lX  %s|\n", (دراز)شروع, s);
    } وإلا إذا (شروع <= 0xfffff) {
        چاپ_کن("   %5lX  %s|\n", (دراز)شروع, s);
    } وإلا {
        چاپ_کن("  %6lX  %s|\n", (دراز)شروع, s);
    }
}
#پایان_اگر

/**
 * Compacts a build-time trie.
 *
 * The compaction
 * - removes blocks that are identical with earlier ones
 * - overlaps each new non-duplicate block as much as possible with the previously-written one
 * - works with fast-range data blocks whose length is a multiple of that of
 *   higher-code-point data blocks
 *
 * It does not try to find an optimal order of writing, deduplicating, and overlapping blocks.
 */
int32_t Mutable_کد_نقطه_Trie::compact_داده(
        int32_t fastILimit, uint32_t *new_داده, int32_t new_داده_ظرفیت,
        int32_t داده_Null_شاخص, Mixed_بلوکها &mixed_بلوکها, UError_کد &error_کد) {
#اگر_تعریف_شده UCPTRIE_DEBUG
    int32_t شمار_Same=0, جمع_Overlaps=0;
    منطقي print_داده = داده_درازی == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        داده_درازی == 50400 /* zh.txt~stroke */;
#پایان_اگر

    // The linear ASCII data has been copied into newData already.
    int32_t new_داده_درازی = 0;
    لكل (int32_t i = 0; new_داده_درازی < ASCII_حد;
            new_داده_درازی += UCPTRIE_FAST_داده_بلوک_درازی, i += SMALL_داده_بلوکها_در_هر_BMP_بلوک) {
        شاخص[i] = new_داده_درازی;
#اگر_تعریف_شده UCPTRIE_DEBUG
        إذا (print_داده) {
            print_بلوک(new_داده + new_داده_درازی, UCPTRIE_FAST_داده_بلوک_درازی, 0, new_داده_درازی, 0, initial_مقدار);
        }
#پایان_اگر
    }

    int32_t بلوک_درازی = UCPTRIE_FAST_داده_بلوک_درازی;
    إذا (!mixed_بلوکها.init(new_داده_ظرفیت, بلوک_درازی)) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    mixed_بلوکها.extend(new_داده, 0, 0, new_داده_درازی);

    int32_t i_حد = بالا_شروع >> UCPTRIE_لغزش_3;
    int32_t inc = SMALL_داده_بلوکها_در_هر_BMP_بلوک;
    int32_t fast_درازی = 0;
    لكل (int32_t i = ASCII_I_حد; i < i_حد; i += inc) {
        إذا (i == fastILimit) {
            بلوک_درازی = UCPTRIE_SMALL_داده_بلوک_درازی;
            inc = 1;
            fast_درازی = new_داده_درازی;
            إذا (!mixed_بلوکها.init(new_داده_ظرفیت, بلوک_درازی)) {
                error_کد = U_حافظه_تخصیص_ERROR;
                أرجع 0;
            }
            mixed_بلوکها.extend(new_داده, 0, 0, new_داده_درازی);
        }
        إذا (flags[i] == ALL_SAME) {
            uint32_t value = شاخص[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_بلوکها.پیدا_کن_All_Same_بلوک(new_داده, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            طالما (n >= 0 && i == داده_Null_شاخص && i >= fastILimit && n < fast_درازی &&
                    is_شروع_از_Some_Fast_بلوک(n, شاخص, fastILimit)) {
                n = پیدا_کن_All_Same_بلوک(new_داده, n + 1, new_داده_درازی, value, بلوک_درازی);
            }
            إذا (n >= 0) {
                DEBUG_DO(++شمار_Same);
                شاخص[i] = n;
            } وإلا {
                n = getAllSameOverlap(new_داده, new_داده_درازی, value, بلوک_درازی);
                DEBUG_DO(جمع_Overlaps += n);
#اگر_تعریف_شده UCPTRIE_DEBUG
                إذا (print_داده) {
                    print_بلوک(اشاره_تهی, بلوک_درازی, value, i << UCPTRIE_لغزش_3, n, initial_مقدار);
                }
#پایان_اگر
                شاخص[i] = new_داده_درازی - n;
                int32_t prev_داده_درازی = new_داده_درازی;
                طالما (n < بلوک_درازی) {
                    new_داده[new_داده_درازی++] = value;
                    ++n;
                }
                mixed_بلوکها.extend(new_داده, 0, prev_داده_درازی, new_داده_درازی);
            }
        } وإلا إذا (flags[i] == MIXED) {
            ثابت uint32_t *بلوک = داده + شاخص[i];
            int32_t n = mixed_بلوکها.پیدا_کن_بلوک(new_داده, بلوک, 0);
            إذا (n >= 0) {
                DEBUG_DO(++شمار_Same);
                شاخص[i] = n;
            } وإلا {
                n = getOverlap(new_داده, new_داده_درازی, بلوک, 0, بلوک_درازی);
                DEBUG_DO(جمع_Overlaps += n);
#اگر_تعریف_شده UCPTRIE_DEBUG
                إذا (print_داده) {
                    print_بلوک(بلوک, بلوک_درازی, 0, i << UCPTRIE_لغزش_3, n, initial_مقدار);
                }
#پایان_اگر
                شاخص[i] = new_داده_درازی - n;
                int32_t prev_داده_درازی = new_داده_درازی;
                طالما (n < بلوک_درازی) {
                    new_داده[new_داده_درازی++] = بلوک[n++];
                }
                mixed_بلوکها.extend(new_داده, 0, prev_داده_درازی, new_داده_درازی);
            }
        } وإلا /* SAME_AS */ {
            uint32_t j = شاخص[i];
            شاخص[i] = شاخص[j];
        }
    }

#اگر_تعریف_شده UCPTRIE_DEBUG
    /* we saved some space */
    چاپ_کن("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (دراز)داده_درازی, (دراز)new_داده_درازی, (دراز)شمار_Same, (دراز)جمع_Overlaps);
#پایان_اگر
    أرجع new_داده_درازی;
}

int32_t Mutable_کد_نقطه_Trie::compact_شاخص(int32_t fastILimit, Mixed_بلوکها &mixed_بلوکها,
                                           UError_کد &error_کد) {
    int32_t fast_شاخص_درازی = fastILimit >> (UCPTRIE_FAST_لغزش - UCPTRIE_لغزش_3);
    إذا ((بالا_شروع >> UCPTRIE_FAST_لغزش) <= fast_شاخص_درازی) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_جابه_جایی = UCPTRIE_NO_INDEX3_NULL_جابه_جایی;
        أرجع fast_شاخص_درازی;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_شاخص[UCPTRIE_BMP_شاخص_درازی];  // fastIndexLength
    int32_t i3_اولی_Null = -1;
    لكل (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = شاخص[i];
        fast_شاخص[j] = static_cast<uint16_t>(i3);
        إذا (i3 == static_cast<uint32_t>(داده_Null_جابه_جایی)) {
            إذا (i3_اولی_Null < 0) {
                i3_اولی_Null = j;
            } وإلا إذا (index3_Null_جابه_جایی < 0 &&
                    (j - i3_اولی_Null + 1) == UCPTRIE_شاخص_3_بلوک_درازی) {
                index3_Null_جابه_جایی = i3_اولی_Null;
            }
        } وإلا {
            i3_اولی_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_بعدی = i + SMALL_داده_بلوکها_در_هر_BMP_بلوک;
        طالما (++i < i_بعدی) {
            i3 += UCPTRIE_SMALL_داده_بلوک_درازی;
            شاخص[i] = i3;
        }
    }

    إذا (!mixed_بلوکها.init(fast_شاخص_درازی, UCPTRIE_شاخص_3_بلوک_درازی)) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    mixed_بلوکها.extend(fast_شاخص, 0, 0, fast_شاخص_درازی);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_ظرفیت = 0;
    i3_اولی_Null = index3_Null_جابه_جایی;
    منطقي has_Long_I3_بلوکها = خطأ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_شروع = fastILimit < BMP_I_حد ? 0 : BMP_I_حد;
    int32_t i_حد = بالا_شروع >> UCPTRIE_لغزش_3;
    لكل (int32_t i = i_شروع; i < i_حد;) {
        int32_t j = i;
        int32_t j_حد = i + UCPTRIE_شاخص_3_بلوک_درازی;
        uint32_t oredI3 = 0;
        منطقي isNull = درست;
        افعل {
            uint32_t i3 = شاخص[j];
            oredI3 |= i3;
            إذا (i3 != static_cast<uint32_t>(داده_Null_جابه_جایی)) {
                isNull = خطأ;
            }
        } طالما (++j < j_حد);
        إذا (isNull) {
            flags[i] = I3_NULL;
            إذا (i3_اولی_Null < 0) {
                إذا (oredI3 <= 0xffff) {
                    index3_ظرفیت += UCPTRIE_شاخص_3_بلوک_درازی;
                } وإلا {
                    index3_ظرفیت += شاخص_3_18_بیت_بلوک_درازی;
                    has_Long_I3_بلوکها = درست;
                }
                i3_اولی_Null = 0;
            }
        } وإلا {
            إذا (oredI3 <= 0xffff) {
                int32_t n = mixed_بلوکها.پیدا_کن_بلوک(fast_شاخص, شاخص, i);
                إذا (n >= 0) {
                    flags[i] = I3_BMP;
                    شاخص[i] = n;
                } وإلا {
                    flags[i] = I3_16;
                    index3_ظرفیت += UCPTRIE_شاخص_3_بلوک_درازی;
                }
            } وإلا {
                flags[i] = I3_18;
                index3_ظرفیت += شاخص_3_18_بیت_بلوک_درازی;
                has_Long_I3_بلوکها = درست;
            }
        }
        i = j;
    }

    int32_t index2_ظرفیت = (i_حد - i_شروع) >> UCPTRIE_لغزش_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_درازی = (index2_ظرفیت + UCPTRIE_شاخص_2_ماسک) >> UCPTRIE_لغزش_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_ظرفیت = fast_شاخص_درازی + index1_درازی + index3_ظرفیت + index2_ظرفیت + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_ظرفیت * 2));
    إذا (index16 == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    uprv_memcpy(index16, fast_شاخص, fast_شاخص_درازی * 2);

    إذا (!mixed_بلوکها.init(index16_ظرفیت, UCPTRIE_شاخص_3_بلوک_درازی)) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    Mixed_بلوکها long_I3_بلوکها;
    إذا (has_Long_I3_بلوکها) {
        إذا (!long_I3_بلوکها.init(index16_ظرفیت, شاخص_3_18_بیت_بلوک_درازی)) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_حد >> UCPTRIE_لغزش_2];  // index2Capacity
    int32_t i2_درازی = 0;
    i3_اولی_Null = index3_Null_جابه_جایی;
    int32_t index3_شروع = fast_شاخص_درازی + index1_درازی;
    int32_t شاخص_درازی = index3_شروع;
    لكل (int32_t i = i_شروع; i < i_حد; i += UCPTRIE_شاخص_3_بلوک_درازی) {
        int32_t i3;
        uint8_t f = flags[i];
        إذا (f == I3_NULL && i3_اولی_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = داده_Null_جابه_جایی <= 0xffff ? I3_16 : I3_18;
            i3_اولی_Null = 0;
        }
        إذا (f == I3_NULL) {
            i3 = index3_Null_جابه_جایی;
        } وإلا إذا (f == I3_BMP) {
            i3 = شاخص[i];
        } وإلا إذا (f == I3_16) {
            int32_t n = mixed_بلوکها.پیدا_کن_بلوک(index16, شاخص, i);
            إذا (n >= 0) {
                i3 = n;
            } وإلا {
                إذا (شاخص_درازی == index3_شروع) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, شاخص_درازی,
                                   شاخص, i, UCPTRIE_شاخص_3_بلوک_درازی);
                }
                i3 = شاخص_درازی - n;
                int32_t prev_شاخص_درازی = شاخص_درازی;
                طالما (n < UCPTRIE_شاخص_3_بلوک_درازی) {
                    index16[شاخص_درازی++] = شاخص[i + n++];
                }
                mixed_بلوکها.extend(index16, index3_شروع, prev_شاخص_درازی, شاخص_درازی);
                إذا (has_Long_I3_بلوکها) {
                    long_I3_بلوکها.extend(index16, index3_شروع, prev_شاخص_درازی, شاخص_درازی);
                }
            }
        } وإلا {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_بلوکها);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_حد = i + UCPTRIE_شاخص_3_بلوک_درازی;
            int32_t k = شاخص_درازی;
            افعل {
                ++k;
                uint32_t v = شاخص[j++];
                uint32_t فوقانی_بیتها = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = شاخص[j++];
                فوقانی_بیتها |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = فوقانی_بیتها;
            } طالما (j < j_حد);
            int32_t n = long_I3_بلوکها.پیدا_کن_بلوک(index16, index16, شاخص_درازی);
            إذا (n >= 0) {
                i3 = n | 0x8000;
            } وإلا {
                إذا (شاخص_درازی == index3_شروع) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, شاخص_درازی,
                                   index16, شاخص_درازی, شاخص_3_18_بیت_بلوک_درازی);
                }
                i3 = (شاخص_درازی - n) | 0x8000;
                int32_t prev_شاخص_درازی = شاخص_درازی;
                إذا (n > 0) {
                    int32_t شروع = شاخص_درازی;
                    طالما (n < شاخص_3_18_بیت_بلوک_درازی) {
                        index16[شاخص_درازی++] = index16[شروع + n++];
                    }
                } وإلا {
                    شاخص_درازی += شاخص_3_18_بیت_بلوک_درازی;
                }
                mixed_بلوکها.extend(index16, index3_شروع, prev_شاخص_درازی, شاخص_درازی);
                إذا (has_Long_I3_بلوکها) {
                    long_I3_بلوکها.extend(index16, index3_شروع, prev_شاخص_درازی, شاخص_درازی);
                }
            }
        }
        إذا (index3_Null_جابه_جایی < 0 && i3_اولی_Null >= 0) {
            index3_Null_جابه_جایی = i3;
        }
        // Set the index-2 table entry.
        index2[i2_درازی++] = i3;
    }
    U_ASSERT(i2_درازی == index2_ظرفیت);
    U_ASSERT(شاخص_درازی <= index3_شروع + index3_ظرفیت);

    إذا (index3_Null_جابه_جایی < 0) {
        index3_Null_جابه_جایی = UCPTRIE_NO_INDEX3_NULL_جابه_جایی;
    }
    إذا (شاخص_درازی >= (UCPTRIE_NO_INDEX3_NULL_جابه_جایی + UCPTRIE_شاخص_3_بلوک_درازی)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_شاخص_2_بلوک_درازی == UCPTRIE_شاخص_3_بلوک_درازی,
                  "must re-init mixedBlocks");
    int32_t بلوک_درازی = UCPTRIE_شاخص_2_بلوک_درازی;
    int32_t i1 = fast_شاخص_درازی;
    لكل (int32_t i = 0; i < i2_درازی; i += بلوک_درازی) {
        int32_t n;
        إذا ((i2_درازی - i) >= بلوک_درازی) {
            // normal block
            U_ASSERT(بلوک_درازی == UCPTRIE_شاخص_2_بلوک_درازی);
            n = mixed_بلوکها.پیدا_کن_بلوک(index16, index2, i);
        } وإلا {
            // highStart is inside the last index-2 block. Shorten it.
            بلوک_درازی = i2_درازی - i;
            n = پیدا_کن_Same_بلوک(index16, index3_شروع, شاخص_درازی,
                              index2, i, بلوک_درازی);
        }
        int32_t i2;
        إذا (n >= 0) {
            i2 = n;
        } وإلا {
            إذا (شاخص_درازی == index3_شروع) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } وإلا {
                n = getOverlap(index16, شاخص_درازی, index2, i, بلوک_درازی);
            }
            i2 = شاخص_درازی - n;
            int32_t prev_شاخص_درازی = شاخص_درازی;
            طالما (n < بلوک_درازی) {
                index16[شاخص_درازی++] = index2[i + n++];
            }
            mixed_بلوکها.extend(index16, index3_شروع, prev_شاخص_درازی, شاخص_درازی);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_شروع);
    U_ASSERT(شاخص_درازی <= index16_ظرفیت);

#اگر_تعریف_شده UCPTRIE_DEBUG
    /* we saved some space */
    چاپ_کن("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (دراز)i_حد, (دراز)شاخص_درازی);
#پایان_اگر

    أرجع شاخص_درازی;
}

int32_t Mutable_کد_نقطه_Trie::compactTrie(int32_t fastILimit, UError_کد &error_کد) {
    // Find the real highStart and round it up.
    U_ASSERT((بالا_شروع & (UCPTRIE_CP_در_هر_شاخص_2_مدخل - 1)) == 0);
    بالا_مقدار = get(MAX_UNICODE);
    int32_t real_بالا_شروع = پیدا_کن_بالا_شروع();
    real_بالا_شروع = (real_بالا_شروع + (UCPTRIE_CP_در_هر_شاخص_2_مدخل - 1)) &
        ~(UCPTRIE_CP_در_هر_شاخص_2_مدخل - 1);
    إذا (real_بالا_شروع == UNICODE_حد) {
        بالا_مقدار = initial_مقدار;
    }

#اگر_تعریف_شده UCPTRIE_DEBUG
    چاپ_کن("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (دراز)real_بالا_شروع, (دراز)بالا_مقدار, (دراز)initial_مقدار);
#پایان_اگر

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_حد = fastILimit << UCPTRIE_لغزش_3;
    إذا (real_بالا_شروع < fast_حد) {
        لكل (int32_t i = (real_بالا_شروع >> UCPTRIE_لغزش_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            شاخص[i] = بالا_مقدار;
        }
        بالا_شروع = fast_حد;
    } وإلا {
        بالا_شروع = real_بالا_شروع;
    }

    uint32_t ascii_داده[ASCII_حد];
    لكل (int32_t i = 0; i < ASCII_حد; ++i) {
        ascii_داده[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_بلوکها all_Same_بلوکها;
    int32_t new_داده_ظرفیت = compact_کامل_داده_بلوکها(fastILimit, all_Same_بلوکها);
    إذا (new_داده_ظرفیت < 0) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    uint32_t* new_داده = static_cast<uint32_t*>(uprv_malloc(new_داده_ظرفیت * 4));
    إذا (new_داده == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    uprv_memcpy(new_داده, ascii_داده, حجم(ascii_داده));

    int32_t داده_Null_شاخص = all_Same_بلوکها.پیدا_کن_Most_استفاده_شده();

    Mixed_بلوکها mixed_بلوکها;
    int32_t new_داده_درازی = compact_داده(fastILimit, new_داده, new_داده_ظرفیت,
                                        داده_Null_شاخص, mixed_بلوکها, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع 0; }
    U_ASSERT(new_داده_درازی <= new_داده_ظرفیت);
    uprv_free(داده);
    داده = new_داده;
    داده_ظرفیت = new_داده_ظرفیت;
    داده_درازی = new_داده_درازی;
    إذا (داده_درازی > (0x3ffff + UCPTRIE_SMALL_داده_بلوک_درازی)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا (داده_Null_شاخص >= 0) {
        داده_Null_جابه_جایی = شاخص[داده_Null_شاخص];
#اگر_تعریف_شده UCPTRIE_DEBUG
        إذا (داده[داده_Null_جابه_جایی] != initial_مقدار) {
            چاپ_کن("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (دراز)initial_مقدار, (دراز)داده[داده_Null_جابه_جایی]);
        }
#پایان_اگر
        initial_مقدار = داده[داده_Null_جابه_جایی];
    } وإلا {
        داده_Null_جابه_جایی = UCPTRIE_NO_داده_NULL_جابه_جایی;
    }

    int32_t شاخص_درازی = compact_شاخص(fastILimit, mixed_بلوکها, error_کد);
    بالا_شروع = real_بالا_شروع;
    أرجع شاخص_درازی;
}

UCPTrie *Mutable_کد_نقطه_Trie::build(UCPTrieType type, UCPTrie_مقدار_عرض مقدار_عرض, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            مقدار_عرض < UCPTRIE_مقدار_بیتها_16 || UCPTRIE_مقدار_بیتها_8 < مقدار_عرض) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    بدل (مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_32:
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_16:
        ماسک_مقادیر(0xffff);
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        ماسک_مقادیر(0xff);
        اكسر;
    افتراضي:
        اكسر;
    }

    UChar32 fast_حد = type == UCPTRIE_TYPE_FAST ? BMP_حد : UCPTRIE_SMALL_حد;
    int32_t شاخص_درازی = compactTrie(fast_حد >> UCPTRIE_لغزش_3, error_کد);
    إذا (U_FAILURE(error_کد)) {
        clear();
        أرجع اشاره_تهی;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    إذا (مقدار_عرض == UCPTRIE_مقدار_بیتها_32 && (شاخص_درازی & 1) != 0) {
        index16[شاخص_درازی++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = شاخص_درازی * 2;
    إذا (مقدار_عرض == UCPTRIE_مقدار_بیتها_16) {
        إذا (((شاخص_درازی ^ داده_درازی) & 1) != 0) {
            // padding
            داده[داده_درازی++] = error_مقدار;
        }
        إذا (داده[داده_درازی - 1] != error_مقدار || داده[داده_درازی - 2] != بالا_مقدار) {
            داده[داده_درازی++] = بالا_مقدار;
            داده[داده_درازی++] = error_مقدار;
        }
        length += داده_درازی * 2;
    } وإلا إذا (مقدار_عرض == UCPTRIE_مقدار_بیتها_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        إذا (داده[داده_درازی - 1] != error_مقدار || داده[داده_درازی - 2] != بالا_مقدار) {
            إذا (داده[داده_درازی - 1] != بالا_مقدار) {
                داده[داده_درازی++] = بالا_مقدار;
            }
            داده[داده_درازی++] = error_مقدار;
        }
        length += داده_درازی * 4;
    } وإلا {
        int32_t and3 = (length + داده_درازی) & 3;
        إذا (and3 == 0 && داده[داده_درازی - 1] == error_مقدار && داده[داده_درازی - 2] == بالا_مقدار) {
            // all set
        } وإلا إذا(and3 == 3 && داده[داده_درازی - 1] == بالا_مقدار) {
            داده[داده_درازی++] = error_مقدار;
        } وإلا {
            طالما (and3 != 2) {
                داده[داده_درازی++] = بالا_مقدار;
                and3 = (and3 + 1) & 3;
            }
            داده[داده_درازی++] = بالا_مقدار;
            داده[داده_درازی++] = error_مقدار;
        }
        length += داده_درازی;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += حجم(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* بایتها = static_cast<uint8_t*>(uprv_malloc(length));
    إذا (بایتها == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        clear();
        أرجع اشاره_تهی;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(بایتها);
    uprv_memset(trie, 0, حجم(UCPTrie));
    trie->شاخص_درازی = شاخص_درازی;
    trie->داده_درازی = داده_درازی;

    trie->بالا_شروع = بالا_شروع;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_بالا_شروع = (بالا_شروع + 0xfff) >> 12;
    trie->type = type;
    trie->مقدار_عرض = مقدار_عرض;

    trie->index3_Null_جابه_جایی = index3_Null_جابه_جایی;
    trie->داده_Null_جابه_جایی = داده_Null_جابه_جایی;
    trie->null_مقدار = initial_مقدار;

    بایتها += حجم(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(بایتها);
    trie->شاخص = dest16;

    إذا (بالا_شروع <= fast_حد) {
        // Condense only the fast index from the mutable-trie index.
        لكل (int32_t i = 0, j = 0; j < شاخص_درازی; i += SMALL_داده_بلوکها_در_هر_BMP_بلوک, ++j) {
            *dest16++ = static_cast<uint16_t>(شاخص[i]); // dest16[j]
        }
    } وإلا {
        uprv_memcpy(dest16, index16, شاخص_درازی * 2);
        dest16 += شاخص_درازی;
    }
    بایتها += شاخص_درازی * 2;

    // Write the data array.
    ثابت uint32_t *p = داده;
    بدل (مقدار_عرض) {
    حالة UCPTRIE_مقدار_بیتها_16:
        // Write 16-bit data values.
        trie->داده.ptr16 = dest16;
        لكل (int32_t i = داده_درازی; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_32:
        // Write 32-bit data values.
        trie->داده.ptr32 = reinterpret_cast<uint32_t*>(بایتها);
        uprv_memcpy(بایتها, p, (size_t)داده_درازی * 4);
        اكسر;
    حالة UCPTRIE_مقدار_بیتها_8:
        // Write 8-bit data values.
        trie->داده.ptr8 = بایتها;
        لكل (int32_t i = داده_درازی; i > 0; --i) {
            *بایتها++ = static_cast<uint8_t>(*p++);
        }
        اكسر;
    افتراضي:
        // Will not occur, valueWidth checked at the beginning.
        اكسر;
    }

#اگر_تعریف_شده UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#پایان_اگر

    clear();
    أرجع trie;
}

}  // namespace

U_NAMESPACE_انتها

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_مقدار, uint32_t error_مقدار, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    Local_اشاره_گر<Mutable_کد_نقطه_Trie> trie(
        جديد Mutable_کد_نقطه_Trie(initial_مقدار, error_مقدار, *p_Error_کد), *p_Error_کد);
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ثابت UMutableCPTrie *other, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا (other == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    Local_اشاره_گر<Mutable_کد_نقطه_Trie> clone(
        جديد Mutable_کد_نقطه_Trie(*reinterpret_cast<ثابت Mutable_کد_نقطه_Trie *>(other), *p_Error_کد), *p_Error_کد);
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    حذف reinterpret_cast<Mutable_کد_نقطه_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ثابت UCPMap *map, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا (map == اشاره_تهی) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_کد_نقطه_Trie::fromUCPMap(map, *p_Error_کد));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ثابت UCPTrie *trie, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا (trie == اشاره_تهی) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_کد_نقطه_Trie::fromUCPTrie(trie, *p_Error_کد));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ثابت UMutableCPTrie *trie, UChar32 c) {
    أرجع reinterpret_cast<ثابت Mutable_کد_نقطه_Trie *>(trie)->get(c);
}

نطاق {

UChar32 getRange(ثابت فراغ *trie, UChar32 شروع,
                 UCPMap_مقدار_Filter *filter, ثابت فراغ *context, uint32_t *p_مقدار) {
    أرجع reinterpret_cast<ثابت Mutable_کد_نقطه_Trie *>(trie)->
        getRange(شروع, filter, context, p_مقدار);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ثابت UMutableCPTrie *trie, UChar32 شروع,
                        UCPMapRangeOption option, uint32_t surrogate_مقدار,
                        UCPMap_مقدار_Filter *filter, ثابت فراغ *context, uint32_t *p_مقدار) {
    أرجع ucptrie_داخلی_Get_Range(getRange, trie, شروع,
                                    option, surrogate_مقدار,
                                    filter, context, p_مقدار);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_مجموعه(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    reinterpret_cast<Mutable_کد_نقطه_Trie *>(trie)->مجموعه(c, value, *p_Error_کد);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_مجموعه_Range(UMutableCPTrie *trie, UChar32 شروع, UChar32 انتها,
                   uint32_t value, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    reinterpret_cast<Mutable_کد_نقطه_Trie *>(trie)->مجموعه_Range(شروع, انتها, value, *p_Error_کد);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_مقدار_عرض مقدار_عرض,
                              UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<Mutable_کد_نقطه_Trie *>(trie)->build(type, مقدار_عرض, *p_Error_کد);
}

#اگر_تعریف_شده UCPTRIE_DEBUG
U_CFUNC فراغ umutablecptrie_مجموعه_Name(UMutableCPTrie *trie, ثابت حرف *name) {
    reinterpret_cast<Mutable_کد_نقطه_Trie *>(trie)->name = name;
}
#پایان_اگر
