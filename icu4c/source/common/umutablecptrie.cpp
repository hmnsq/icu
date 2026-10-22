// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#اگر_تعریف_شدہ UCPTRIE_DEBUG
#   include <stdio.h>
#اگر_ختم

#شامل "unicode/utypes.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/umutablecptrie.h"
#شامل "unicode/uobject.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#اگر_تعریف_شدہ حد_سے_تجاوز_2
#تعریف_منسوخ حد_سے_تجاوز_2
#اگر_ختم

U_NAMESPACE_BEGIN

نامخلا {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_حد_بندی = 0x110000;
constexpr int32_t BMP_حد_بندی = 0x10000;
constexpr int32_t ASCII_حد_بندی = 0x80;

constexpr int32_t I_حد_بندی = UNICODE_حد_بندی >> UCPTRIE_سرکاؤ_3;
constexpr int32_t BMP_I_حد_بندی = BMP_حد_بندی >> UCPTRIE_سرکاؤ_3;
constexpr int32_t ASCII_I_حد_بندی = ASCII_حد_بندی >> UCPTRIE_سرکاؤ_3;

constexpr int32_t SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک = (1 << (UCPTRIE_FAST_سرکاؤ - UCPTRIE_سرکاؤ_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_ڈیٹا_لمبائی = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_ڈیٹا_لمبائی = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_ڈیٹا_لمبائی = UNICODE_حد_بندی;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی = UCPTRIE_اشاریہ_3_بلاک_لمبائی + UCPTRIE_اشاریہ_3_بلاک_لمبائی / 8;

جماعت All_Same_بلاکیں;
جماعت Mixed_بلاکیں;

جماعت Mutable_رمز_نقطہ_Trie : عوامی UMemory {
عوامی:
    Mutable_رمز_نقطہ_Trie(uint32_t initial_قدر, uint32_t error_قدر, UError_رمز &error_رمز);
    Mutable_رمز_نقطہ_Trie(ثابت Mutable_رمز_نقطہ_Trie &other, UError_رمز &error_رمز);
    Mutable_رمز_نقطہ_Trie(ثابت Mutable_رمز_نقطہ_Trie &other) = حذف;
    ~Mutable_رمز_نقطہ_Trie();

    Mutable_رمز_نقطہ_Trie &عامل=(ثابت Mutable_رمز_نقطہ_Trie &other) = حذف;

    ساکن Mutable_رمز_نقطہ_Trie *fromUCPMap(ثابت UCPMap *map, UError_رمز &error_رمز);
    ساکن Mutable_رمز_نقطہ_Trie *fromUCPTrie(ثابت UCPTrie *trie, UError_رمز &error_رمز);

    uint32_t get(UChar32 c) ثابت;
    int32_t getRange(UChar32 آغاز, UCPMap_قدر_Filter *filter, ثابت خالی *context,
                     uint32_t *p_قدر) ثابت;

    خالی سیٹ(UChar32 c, uint32_t value, UError_رمز &error_رمز);
    خالی سیٹ_Range(UChar32 آغاز, UChar32 انتہا, uint32_t value, UError_رمز &error_رمز);

    UCPTrie *build(UCPTrieType type, UCPTrie_قدر_چوڑائی قدر_چوڑائی, UError_رمز &error_رمز);

نجی:
    خالی clear();

    منطقی ensure_بلند_آغاز(UChar32 c);
    int32_t alloc_ڈیٹا_بلاک(int32_t بلاک_لمبائی);
    int32_t get_ڈیٹا_بلاک(int32_t i);

    خالی نقاب_اقدار(uint32_t mask);
    UChar32 ڈھونڈو_بلند_آغاز() ثابت;
    int32_t compact_پورا_ڈیٹا_بلاکیں(int32_t fastILimit, All_Same_بلاکیں &all_Same_بلاکیں);
    int32_t compact_ڈیٹا(
            int32_t fastILimit, uint32_t *new_ڈیٹا, int32_t new_ڈیٹا_استعداد,
            int32_t ڈیٹا_Null_اشاریہ, Mixed_بلاکیں &mixed_بلاکیں, UError_رمز &error_رمز);
    int32_t compact_اشاریہ(int32_t fastILimit, Mixed_بلاکیں &mixed_بلاکیں, UError_رمز &error_رمز);
    int32_t compactTrie(int32_t fastILimit, UError_رمز &error_رمز);

    uint32_t *اشاریہ = خالی_اشاریہ;
    int32_t اشاریہ_استعداد = 0;
    int32_t index3_Null_انحراف = -1;
    uint32_t *ڈیٹا = خالی_اشاریہ;
    int32_t ڈیٹا_استعداد = 0;
    int32_t ڈیٹا_لمبائی = 0;
    int32_t ڈیٹا_Null_انحراف = -1;

    uint32_t orig_Initial_قدر;
    uint32_t initial_قدر;
    uint32_t error_قدر;
    UChar32 بلند_آغاز;
    uint32_t بلند_قدر;
#اگر_تعریف_شدہ UCPTRIE_DEBUG
عوامی:
    ثابت حرف *name;
#اگر_ختم
نجی:
    /** Temporary array while building the final data. */
    uint16_t *index16 = خالی_اشاریہ;
    uint8_t flags[UNICODE_حد_بندی >> UCPTRIE_سرکاؤ_3];
};

Mutable_رمز_نقطہ_Trie::Mutable_رمز_نقطہ_Trie(uint32_t ini_قدر, uint32_t err_قدر, UError_رمز &error_رمز) :
        orig_Initial_قدر(ini_قدر), initial_قدر(ini_قدر), error_قدر(err_قدر),
        بلند_آغاز(0), بلند_قدر(initial_قدر)
#اگر_تعریف_شدہ UCPTRIE_DEBUG
        , name("open")
#اگر_ختم
        {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    اشاریہ = static_cast<uint32_t*>(uprv_malloc(BMP_I_حد_بندی * 4));
    ڈیٹا = static_cast<uint32_t*>(uprv_malloc(INITIAL_ڈیٹا_لمبائی * 4));
    اگر (اشاریہ == خالی_اشاریہ || ڈیٹا == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    اشاریہ_استعداد = BMP_I_حد_بندی;
    ڈیٹا_استعداد = INITIAL_ڈیٹا_لمبائی;
}

Mutable_رمز_نقطہ_Trie::Mutable_رمز_نقطہ_Trie(ثابت Mutable_رمز_نقطہ_Trie &other, UError_رمز &error_رمز) :
        index3_Null_انحراف(other.index3_Null_انحراف),
        ڈیٹا_Null_انحراف(other.ڈیٹا_Null_انحراف),
        orig_Initial_قدر(other.orig_Initial_قدر), initial_قدر(other.initial_قدر),
        error_قدر(other.error_قدر),
        بلند_آغاز(other.بلند_آغاز), بلند_قدر(other.بلند_قدر)
#اگر_تعریف_شدہ UCPTRIE_DEBUG
        , name("mutable clone")
#اگر_ختم
        {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    int32_t i_استعداد = بلند_آغاز <= BMP_حد_بندی ? BMP_I_حد_بندی : I_حد_بندی;
    اشاریہ = static_cast<uint32_t*>(uprv_malloc(i_استعداد * 4));
    ڈیٹا = static_cast<uint32_t*>(uprv_malloc(other.ڈیٹا_استعداد * 4));
    اگر (اشاریہ == خالی_اشاریہ || ڈیٹا == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    اشاریہ_استعداد = i_استعداد;
    ڈیٹا_استعداد = other.ڈیٹا_استعداد;

    int32_t i_حد_بندی = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
    uprv_memcpy(flags, other.flags, i_حد_بندی);
    uprv_memcpy(اشاریہ, other.اشاریہ, i_حد_بندی * 4);
    uprv_memcpy(ڈیٹا, other.ڈیٹا, (size_t)other.ڈیٹا_لمبائی * 4);
    ڈیٹا_لمبائی = other.ڈیٹا_لمبائی;
    U_ASSERT(other.index16 == خالی_اشاریہ);
}

Mutable_رمز_نقطہ_Trie::~Mutable_رمز_نقطہ_Trie() {
    uprv_free(اشاریہ);
    uprv_free(ڈیٹا);
    uprv_free(index16);
}

Mutable_رمز_نقطہ_Trie *Mutable_رمز_نقطہ_Trie::fromUCPMap(ثابت UCPMap *map, UError_رمز &error_رمز) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_قدر = ucpmap_get(map, -1);
    uint32_t initial_قدر = ucpmap_get(map, 0x10ffff);
    Local_اشارہ_گر<Mutable_رمز_نقطہ_Trie> mutableTrie(
        نیا Mutable_رمز_نقطہ_Trie(initial_قدر, error_قدر, error_رمز),
        error_رمز);
    اگر (U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    UChar32 آغاز = 0, انتہا;
    uint32_t value;
    جب ((انتہا = ucpmap_getRange(map, آغاز, UCPMAP_RANGE_معمول, 0,
                                  خالی_اشاریہ, خالی_اشاریہ, &value)) >= 0) {
        اگر (value != initial_قدر) {
            اگر (آغاز == انتہا) {
                mutableTrie->سیٹ(آغاز, value, error_رمز);
            } ورنہ {
                mutableTrie->سیٹ_Range(آغاز, انتہا, value, error_رمز);
            }
        }
        آغاز = انتہا + 1;
    }
    اگر (U_SUCCESS(error_رمز)) {
        واپس mutableTrie.orphan();
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

Mutable_رمز_نقطہ_Trie *Mutable_رمز_نقطہ_Trie::fromUCPTrie(ثابت UCPTrie *trie, UError_رمز &error_رمز) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_قدر;
    uint32_t initial_قدر;
    انتخاب (trie->قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        error_قدر = trie->ڈیٹا.ptr16[trie->ڈیٹا_لمبائی - UCPTRIE_ERROR_قدر_NEG_ڈیٹا_انحراف];
        initial_قدر = trie->ڈیٹا.ptr16[trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف];
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        error_قدر = trie->ڈیٹا.ptr32[trie->ڈیٹا_لمبائی - UCPTRIE_ERROR_قدر_NEG_ڈیٹا_انحراف];
        initial_قدر = trie->ڈیٹا.ptr32[trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف];
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        error_قدر = trie->ڈیٹا.ptr8[trie->ڈیٹا_لمبائی - UCPTRIE_ERROR_قدر_NEG_ڈیٹا_انحراف];
        initial_قدر = trie->ڈیٹا.ptr8[trie->ڈیٹا_لمبائی - UCPTRIE_بلند_قدر_NEG_ڈیٹا_انحراف];
        توڑو;
    طےشدہ:
        // Unreachable if the trie is properly initialized.
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    Local_اشارہ_گر<Mutable_رمز_نقطہ_Trie> mutableTrie(
        نیا Mutable_رمز_نقطہ_Trie(initial_قدر, error_قدر, error_رمز),
        error_رمز);
    اگر (U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    UChar32 آغاز = 0, انتہا;
    uint32_t value;
    جب ((انتہا = ucptrie_getRange(trie, آغاز, UCPMAP_RANGE_معمول, 0,
                                   خالی_اشاریہ, خالی_اشاریہ, &value)) >= 0) {
        اگر (value != initial_قدر) {
            اگر (آغاز == انتہا) {
                mutableTrie->سیٹ(آغاز, value, error_رمز);
            } ورنہ {
                mutableTrie->سیٹ_Range(آغاز, انتہا, value, error_رمز);
            }
        }
        آغاز = انتہا + 1;
    }
    اگر (U_SUCCESS(error_رمز)) {
        واپس mutableTrie.orphan();
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

خالی Mutable_رمز_نقطہ_Trie::clear() {
    index3_Null_انحراف = ڈیٹا_Null_انحراف = -1;
    ڈیٹا_لمبائی = 0;
    بلند_قدر = initial_قدر = orig_Initial_قدر;
    بلند_آغاز = 0;
    uprv_free(index16);
    index16 = خالی_اشاریہ;
}

uint32_t Mutable_رمز_نقطہ_Trie::get(UChar32 c) ثابت {
    اگر (static_cast<uint32_t>(c) > MAX_UNICODE) {
        واپس error_قدر;
    }
    اگر (c >= بلند_آغاز) {
        واپس بلند_قدر;
    }
    int32_t i = c >> UCPTRIE_سرکاؤ_3;
    اگر (flags[i] == ALL_SAME) {
        واپس اشاریہ[i];
    } ورنہ {
        واپس ڈیٹا[اشاریہ[i] + (c & UCPTRIE_SMALL_ڈیٹا_نقاب)];
    }
}

اندرونی uint32_t maybe_Filter_قدر(uint32_t value, uint32_t initial_قدر, uint32_t null_قدر,
                                 UCPMap_قدر_Filter *filter, ثابت خالی *context) {
    اگر (value == initial_قدر) {
        value = null_قدر;
    } ورنہ اگر (filter != خالی_اشاریہ) {
        value = filter(context, value);
    }
    واپس value;
}

UChar32 Mutable_رمز_نقطہ_Trie::getRange(
        UChar32 آغاز, UCPMap_قدر_Filter *filter, ثابت خالی *context,
        uint32_t *p_قدر) ثابت {
    اگر (static_cast<uint32_t>(آغاز) > MAX_UNICODE) {
        واپس U_SENTINEL;
    }
    اگر (آغاز >= بلند_آغاز) {
        اگر (p_قدر != خالی_اشاریہ) {
            uint32_t value = بلند_قدر;
            اگر (filter != خالی_اشاریہ) { value = filter(context, value); }
            *p_قدر = value;
        }
        واپس MAX_UNICODE;
    }
    uint32_t null_قدر = initial_قدر;
    اگر (filter != خالی_اشاریہ) { null_قدر = filter(context, null_قدر); }
    UChar32 c = آغاز;
    uint32_t trie_قدر, value;
    منطقی have_قدر = جھوٹ;
    int32_t i = c >> UCPTRIE_سرکاؤ_3;
    کرو {
        اگر (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = اشاریہ[i];
            اگر (have_قدر) {
                اگر (trieValue2 != trie_قدر) {
                    اگر (filter == خالی_اشاریہ ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        واپس c - 1;
                    }
                    trie_قدر = trieValue2;  // may or may not help
                }
            } ورنہ {
                trie_قدر = trieValue2;
                value = maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر, filter, context);
                اگر (p_قدر != خالی_اشاریہ) { *p_قدر = value; }
                have_قدر = سچ;
            }
            c = (c + UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی) & ~UCPTRIE_SMALL_ڈیٹا_نقاب;
        } ورنہ /* MIXED */ {
            int32_t di = اشاریہ[i] + (c & UCPTRIE_SMALL_ڈیٹا_نقاب);
            uint32_t trieValue2 = ڈیٹا[di];
            اگر (have_قدر) {
                اگر (trieValue2 != trie_قدر) {
                    اگر (filter == خالی_اشاریہ ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        واپس c - 1;
                    }
                    trie_قدر = trieValue2;  // may or may not help
                }
            } ورنہ {
                trie_قدر = trieValue2;
                value = maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر, filter, context);
                اگر (p_قدر != خالی_اشاریہ) { *p_قدر = value; }
                have_قدر = سچ;
            }
            جب ((++c & UCPTRIE_SMALL_ڈیٹا_نقاب) != 0) {
                trieValue2 = ڈیٹا[++di];
                اگر (trieValue2 != trie_قدر) {
                    اگر (filter == خالی_اشاریہ ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        واپس c - 1;
                    }
                }
                trie_قدر = trieValue2;  // may or may not help
            }
        }
        ++i;
    } جب (c < بلند_آغاز);
    U_ASSERT(have_قدر);
    اگر (maybe_Filter_قدر(بلند_قدر, initial_قدر, null_قدر,
                         filter, context) != value) {
        واپس c - 1;
    } ورنہ {
        واپس MAX_UNICODE;
    }
}

خالی
write_بلاک(uint32_t *بلاک, uint32_t value) {
    uint32_t *حد_بندی = بلاک + UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
    جب (بلاک < حد_بندی) {
        *بلاک++ = value;
    }
}

منطقی Mutable_رمز_نقطہ_Trie::ensure_بلند_آغاز(UChar32 c) {
    اگر (c >= بلند_آغاز) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_فی_اشاریہ_2_اندراج) & ~(UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1);
        int32_t i = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
        int32_t i_حد_بندی = c >> UCPTRIE_سرکاؤ_3;
        اگر (i_حد_بندی > اشاریہ_استعداد) {
            uint32_t* new_اشاریہ = static_cast<uint32_t*>(uprv_malloc(I_حد_بندی * 4));
            اگر (new_اشاریہ == خالی_اشاریہ) { واپس جھوٹ; }
            uprv_memcpy(new_اشاریہ, اشاریہ, i * 4);
            uprv_free(اشاریہ);
            اشاریہ = new_اشاریہ;
            اشاریہ_استعداد = I_حد_بندی;
        }
        کرو {
            flags[i] = ALL_SAME;
            اشاریہ[i] = initial_قدر;
        } جب(++i < i_حد_بندی);
        بلند_آغاز = c;
    }
    واپس سچ;
}

int32_t Mutable_رمز_نقطہ_Trie::alloc_ڈیٹا_بلاک(int32_t بلاک_لمبائی) {
    int32_t new_بلاک = ڈیٹا_لمبائی;
    int32_t new_چوٹی = new_بلاک + بلاک_لمبائی;
    اگر (new_چوٹی > ڈیٹا_استعداد) {
        int32_t capacity;
        اگر (ڈیٹا_استعداد < MEDIUM_ڈیٹا_لمبائی) {
            capacity = MEDIUM_ڈیٹا_لمبائی;
        } ورنہ اگر (ڈیٹا_استعداد < MAX_ڈیٹا_لمبائی) {
            capacity = MAX_ڈیٹا_لمبائی;
        } ورنہ {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            واپس -1;
        }
        uint32_t* new_ڈیٹا = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        اگر (new_ڈیٹا == خالی_اشاریہ) {
            واپس -1;
        }
        uprv_memcpy(new_ڈیٹا, ڈیٹا, (size_t)ڈیٹا_لمبائی * 4);
        uprv_free(ڈیٹا);
        ڈیٹا = new_ڈیٹا;
        ڈیٹا_استعداد = capacity;
    }
    ڈیٹا_لمبائی = new_چوٹی;
    واپس new_بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_رمز_نقطہ_Trie::get_ڈیٹا_بلاک(int32_t i) {
    اگر (flags[i] == MIXED) {
        واپس اشاریہ[i];
    }
    اگر (i < BMP_I_حد_بندی) {
        int32_t new_بلاک = alloc_ڈیٹا_بلاک(UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی);
        اگر (new_بلاک < 0) { واپس new_بلاک; }
        int32_t i_آغاز = i & ~(SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک -1);
        int32_t i_حد_بندی = i_آغاز + SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک;
        کرو {
            U_ASSERT(flags[i_آغاز] == ALL_SAME);
            write_بلاک(ڈیٹا + new_بلاک, اشاریہ[i_آغاز]);
            flags[i_آغاز] = MIXED;
            اشاریہ[i_آغاز++] = new_بلاک;
            new_بلاک += UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
        } جب (i_آغاز < i_حد_بندی);
        واپس اشاریہ[i];
    } ورنہ {
        int32_t new_بلاک = alloc_ڈیٹا_بلاک(UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی);
        اگر (new_بلاک < 0) { واپس new_بلاک; }
        write_بلاک(ڈیٹا + new_بلاک, اشاریہ[i]);
        flags[i] = MIXED;
        اشاریہ[i] = new_بلاک;
        واپس new_بلاک;
    }
}

خالی Mutable_رمز_نقطہ_Trie::سیٹ(UChar32 c, uint32_t value, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    int32_t بلاک;
    اگر (!ensure_بلند_آغاز(c) || (بلاک = get_ڈیٹا_بلاک(c >> UCPTRIE_سرکاؤ_3)) < 0) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    ڈیٹا[بلاک + (c & UCPTRIE_SMALL_ڈیٹا_نقاب)] = value;
}

خالی
fill_بلاک(uint32_t *بلاک, UChar32 آغاز, UChar32 حد_بندی, uint32_t value) {
    uint32_t *p_حد_بندی = بلاک + حد_بندی;
    بلاک += آغاز;
    جب (بلاک < p_حد_بندی) {
        *بلاک++ = value;
    }
}

خالی Mutable_رمز_نقطہ_Trie::سیٹ_Range(UChar32 آغاز, UChar32 انتہا, uint32_t value, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر (static_cast<uint32_t>(آغاز) > MAX_UNICODE || static_cast<uint32_t>(انتہا) > MAX_UNICODE || آغاز > انتہا) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر (!ensure_بلند_آغاز(انتہا)) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    UChar32 حد_بندی = انتہا + 1;
    اگر (آغاز & UCPTRIE_SMALL_ڈیٹا_نقاب) {
        // Set partial block at [start..following block boundary[.
        int32_t بلاک = get_ڈیٹا_بلاک(آغاز >> UCPTRIE_سرکاؤ_3);
        اگر (بلاک < 0) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس;
        }

        UChar32 اگلا_آغاز = (آغاز + UCPTRIE_SMALL_ڈیٹا_نقاب) & ~UCPTRIE_SMALL_ڈیٹا_نقاب;
        اگر (اگلا_آغاز <= حد_بندی) {
            fill_بلاک(ڈیٹا + بلاک, آغاز & UCPTRIE_SMALL_ڈیٹا_نقاب, UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی,
                      value);
            آغاز = اگلا_آغاز;
        } ورنہ {
            fill_بلاک(ڈیٹا + بلاک, آغاز & UCPTRIE_SMALL_ڈیٹا_نقاب, حد_بندی & UCPTRIE_SMALL_ڈیٹا_نقاب,
                      value);
            واپس;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = حد_بندی & UCPTRIE_SMALL_ڈیٹا_نقاب;

    // Round down limit to a block boundary.
    حد_بندی &= ~UCPTRIE_SMALL_ڈیٹا_نقاب;

    // Iterate over all-value blocks.
    جب (آغاز < حد_بندی) {
        int32_t i = آغاز >> UCPTRIE_سرکاؤ_3;
        اگر (flags[i] == ALL_SAME) {
            اشاریہ[i] = value;
        } ورنہ /* MIXED */ {
            fill_بلاک(ڈیٹا + اشاریہ[i], 0, UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی, value);
        }
        آغاز += UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
    }

    اگر (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t بلاک = get_ڈیٹا_بلاک(آغاز >> UCPTRIE_سرکاؤ_3);
        اگر (بلاک < 0) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس;
        }

        fill_بلاک(ڈیٹا + بلاک, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

خالی Mutable_رمز_نقطہ_Trie::نقاب_اقدار(uint32_t mask) {
    initial_قدر &= mask;
    error_قدر &= mask;
    بلند_قدر &= mask;
    int32_t i_حد_بندی = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
    برائے (int32_t i = 0; i < i_حد_بندی; ++i) {
        اگر (flags[i] == ALL_SAME) {
            اشاریہ[i] &= mask;
        }
    }
    برائے (int32_t i = 0; i < ڈیٹا_لمبائی; ++i) {
        ڈیٹا[i] &= mask;
    }
}

سانچہ<قسم_نام UIntA, قسم_نام UIntB>
منطقی equal_بلاکیں(ثابت UIntA *s, ثابت UIntB *t, int32_t length) {
    جب (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    واپس length == 0;
}

منطقی all_اقدار_Same_As(ثابت uint32_t *p, int32_t length, uint32_t value) {
    ثابت uint32_t *p_حد_بندی = p + length;
    جب (p < p_حد_بندی && *p == value) { ++p; }
    واپس p == p_حد_بندی;
}

/** Search for an identical block. */
int32_t ڈھونڈو_Same_بلاک(ثابت uint16_t *p, int32_t p_آغاز, int32_t length,
                      ثابت uint16_t *q, int32_t q_آغاز, int32_t بلاک_لمبائی) {
    // Ensure that we do not even partially get past length.
    length -= بلاک_لمبائی;

    q += q_آغاز;
    جب (p_آغاز <= length) {
        اگر (equal_بلاکیں(p + p_آغاز, q, بلاک_لمبائی)) {
            واپس p_آغاز;
        }
        ++p_آغاز;
    }
    واپس -1;
}

int32_t ڈھونڈو_All_Same_بلاک(ثابت uint32_t *p, int32_t آغاز, int32_t حد_بندی,
                         uint32_t value, int32_t بلاک_لمبائی) {
    // Ensure that we do not even partially get past limit.
    حد_بندی -= بلاک_لمبائی;

    برائے (int32_t بلاک = آغاز; بلاک <= حد_بندی; ++بلاک) {
        اگر (p[بلاک] == value) {
            برائے (int32_t i = 1;; ++i) {
                اگر (i == بلاک_لمبائی) {
                    واپس بلاک;
                }
                اگر (p[بلاک + i] != value) {
                    بلاک += i;
                    توڑو;
                }
            }
        }
    }
    واپس -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
سانچہ<قسم_نام UIntA, قسم_نام UIntB>
int32_t getOverlap(ثابت UIntA *p, int32_t length,
                   ثابت UIntB *q, int32_t q_آغاز, int32_t بلاک_لمبائی) {
    int32_t overlap = بلاک_لمبائی - 1;
    U_ASSERT(overlap <= length);
    q += q_آغاز;
    جب (overlap > 0 && !equal_بلاکیں(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    واپس overlap;
}

int32_t getAllSameOverlap(ثابت uint32_t *p, int32_t length, uint32_t value,
                          int32_t بلاک_لمبائی) {
    int32_t min = length - (بلاک_لمبائی - 1);
    int32_t i = length;
    جب (min < i && p[i - 1] == value) { --i; }
    واپس length - i;
}

منطقی is_آغاز_کا_Some_Fast_بلاک(uint32_t ڈیٹا_انحراف, ثابت uint32_t اشاریہ[], int32_t fastILimit) {
    برائے (int32_t i = 0; i < fastILimit; i += SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک) {
        اگر (اشاریہ[i] == ڈیٹا_انحراف) {
            واپس سچ;
        }
    }
    واپس جھوٹ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_رمز_نقطہ_Trie::ڈھونڈو_بلند_آغاز() ثابت {
    int32_t i = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
    جب (i > 0) {
        منطقی مطابقت;
        اگر (flags[--i] == ALL_SAME) {
            مطابقت = اشاریہ[i] == بلند_قدر;
        } ورنہ /* MIXED */ {
            ثابت uint32_t *p = ڈیٹا + اشاریہ[i];
            برائے (int32_t j = 0;; ++j) {
                اگر (j == UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی) {
                    مطابقت = سچ;
                    توڑو;
                }
                اگر (p[j] != بلند_قدر) {
                    مطابقت = جھوٹ;
                    توڑو;
                }
            }
        }
        اگر (!مطابقت) {
            واپس (i + 1) << UCPTRIE_سرکاؤ_3;
        }
    }
    واپس 0;
}

جماعت All_Same_بلاکیں {
عوامی:
    ساکن constexpr int32_t NEW_UNIQUE = -1;
    ساکن constexpr int32_t حد_سے_تجاوز_2 = -2;

    All_Same_بلاکیں() : length(0), mostRecent(-1) {}

    int32_t ڈھونڈو_Or_جوڑو(int32_t اشاریہ, int32_t count, uint32_t value) {
        اگر (mostRecent >= 0 && اقدار[mostRecent] == value) {
            refCounts[mostRecent] += count;
            واپس indexes[mostRecent];
        }
        برائے (int32_t i = 0; i < length; ++i) {
            اگر (اقدار[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                واپس indexes[i];
            }
        }
        اگر (length == استعداد_3) {
            واپس حد_سے_تجاوز_2;
        }
        mostRecent = length;
        indexes[length] = اشاریہ;
        اقدار[length] = value;
        refCounts[length++] = count;
        واپس NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    خالی جوڑو(int32_t اشاریہ, int32_t count, uint32_t value) {
        U_ASSERT(length == استعداد_3);
        int32_t least = -1;
        int32_t کم_سے_کم_گنتی = I_حد_بندی;
        برائے (int32_t i = 0; i < length; ++i) {
            U_ASSERT(اقدار[i] != value);
            اگر (refCounts[i] < کم_سے_کم_گنتی) {
                least = i;
                کم_سے_کم_گنتی = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = اشاریہ;
        اقدار[least] = value;
        refCounts[least] = count;
    }

    int32_t ڈھونڈو_Most_مستعمل() ثابت {
        اگر (length == 0) { واپس -1; }
        int32_t max = -1;
        int32_t max_گنتی = 0;
        برائے (int32_t i = 0; i < length; ++i) {
            اگر (refCounts[i] > max_گنتی) {
                max = i;
                max_گنتی = refCounts[i];
            }
        }
        واپس indexes[max];
    }

نجی:
    ساکن constexpr int32_t استعداد_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[استعداد_3];
    uint32_t اقدار[استعداد_3];
    int32_t refCounts[استعداد_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
جماعت Mixed_بلاکیں {
عوامی:
    Mixed_بلاکیں() {}
    ~Mixed_بلاکیں() {
        uprv_free(جدول);
    }

    منطقی init(int32_t max_لمبائی_2, int32_t new_بلاک_لمبائی) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_ڈیٹا_اشاریہ = max_لمبائی_2 - new_بلاک_لمبائی + 1;
        int32_t new_لمبائی;
        اگر (max_ڈیٹا_اشاریہ <= 0xfff) {  // 4k
            new_لمبائی = 6007;
            سرکاؤ = 12;
            mask = 0xfff;
        } ورنہ اگر (max_ڈیٹا_اشاریہ <= 0x7fff) {  // 32k
            new_لمبائی = 50021;
            سرکاؤ = 15;
            mask = 0x7fff;
        } ورنہ اگر (max_ڈیٹا_اشاریہ <= 0x1ffff) {  // 128k
            new_لمبائی = 200003;
            سرکاؤ = 17;
            mask = 0x1ffff;
        } ورنہ {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_لمبائی = 1500007;
            سرکاؤ = 21;
            mask = 0x1fffff;
        }
        اگر (new_لمبائی > capacity) {
            uprv_free(جدول);
            جدول = static_cast<uint32_t*>(uprv_malloc(new_لمبائی * 4));
            اگر (جدول == خالی_اشاریہ) {
                واپس جھوٹ;
            }
            capacity = new_لمبائی;
        }
        length = new_لمبائی;
        uprv_memset(جدول, 0, length * 4);

        بلاک_لمبائی = new_بلاک_لمبائی;
        واپس سچ;
    }

    سانچہ<قسم_نام UInt>
    خالی extend(ثابت UInt *ڈیٹا, int32_t min_آغاز, int32_t پچھلا_ڈیٹا_لمبائی, int32_t new_ڈیٹا_لمبائی) {
        int32_t آغاز = پچھلا_ڈیٹا_لمبائی - بلاک_لمبائی;
        اگر (آغاز >= min_آغاز) {
            ++آغاز;  // Skip the last block that we added last time.
        } ورنہ {
            آغاز = min_آغاز;  // Begin with the first full block.
        }
        برائے (int32_t انتہا = new_ڈیٹا_لمبائی - بلاک_لمبائی; آغاز <= انتہا; ++آغاز) {
            uint32_t hash_رمز = make_Hash_رمز(ڈیٹا, آغاز);
            جوڑو_اندراج(ڈیٹا, آغاز, hash_رمز, آغاز);
        }
    }

    سانچہ<قسم_نام UIntA, قسم_نام UIntB>
    int32_t ڈھونڈو_بلاک(ثابت UIntA *ڈیٹا, ثابت UIntB *بلاک_ڈیٹا, int32_t بلاک_آغاز) ثابت {
        uint32_t hash_رمز = make_Hash_رمز(بلاک_ڈیٹا, بلاک_آغاز);
        int32_t اندراج_اشاریہ = ڈھونڈو_اندراج(ڈیٹا, بلاک_ڈیٹا, بلاک_آغاز, hash_رمز);
        اگر (اندراج_اشاریہ >= 0) {
            واپس (جدول[اندراج_اشاریہ] & mask) - 1;
        } ورنہ {
            واپس -1;
        }
    }

    int32_t ڈھونڈو_All_Same_بلاک(ثابت uint32_t *ڈیٹا, uint32_t بلاک_قدر) ثابت {
        uint32_t hash_رمز = make_Hash_رمز(بلاک_قدر);
        int32_t اندراج_اشاریہ = ڈھونڈو_اندراج(ڈیٹا, بلاک_قدر, hash_رمز);
        اگر (اندراج_اشاریہ >= 0) {
            واپس (جدول[اندراج_اشاریہ] & mask) - 1;
        } ورنہ {
            واپس -1;
        }
    }

نجی:
    سانچہ<قسم_نام UInt>
    uint32_t make_Hash_رمز(ثابت UInt *بلاک_ڈیٹا, int32_t بلاک_آغاز) ثابت {
        int32_t بلاک_حد_بندی = بلاک_آغاز + بلاک_لمبائی;
        uint32_t hash_رمز = بلاک_ڈیٹا[بلاک_آغاز++];
        کرو {
            hash_رمز = 37 * hash_رمز + بلاک_ڈیٹا[بلاک_آغاز++];
        } جب (بلاک_آغاز < بلاک_حد_بندی);
        واپس hash_رمز;
    }

    uint32_t make_Hash_رمز(uint32_t بلاک_قدر) ثابت {
        uint32_t hash_رمز = بلاک_قدر;
        برائے (int32_t i = 1; i < بلاک_لمبائی; ++i) {
            hash_رمز = 37 * hash_رمز + بلاک_قدر;
        }
        واپس hash_رمز;
    }

    سانچہ<قسم_نام UInt>
    خالی جوڑو_اندراج(ثابت UInt *ڈیٹا, int32_t بلاک_آغاز, uint32_t hash_رمز, int32_t ڈیٹا_اشاریہ) {
        U_ASSERT(0 <= ڈیٹا_اشاریہ && ڈیٹا_اشاریہ < (int32_t)mask);
        int32_t اندراج_اشاریہ = ڈھونڈو_اندراج(ڈیٹا, ڈیٹا, بلاک_آغاز, hash_رمز);
        اگر (اندراج_اشاریہ < 0) {
            جدول[~اندراج_اشاریہ] = (hash_رمز << سرکاؤ) | (ڈیٹا_اشاریہ + 1);
        }
    }

    سانچہ<قسم_نام UIntA, قسم_نام UIntB>
    int32_t ڈھونڈو_اندراج(ثابت UIntA *ڈیٹا, ثابت UIntB *بلاک_ڈیٹا, int32_t بلاک_آغاز,
                      uint32_t hash_رمز) ثابت {
        uint32_t shifted_Hash_رمز = hash_رمز << سرکاؤ;
        int32_t initial_اندراج_اشاریہ = (hash_رمز % (length - 1)) + 1;  // 1..length-1
        برائے (int32_t اندراج_اشاریہ = initial_اندراج_اشاریہ;;) {
            uint32_t اندراج_2 = جدول[اندراج_اشاریہ];
            اگر (اندراج_2 == 0) {
                واپس ~اندراج_اشاریہ;
            }
            اگر ((اندراج_2 & ~mask) == shifted_Hash_رمز) {
                int32_t ڈیٹا_اشاریہ = (اندراج_2 & mask) - 1;
                اگر (equal_بلاکیں(ڈیٹا + ڈیٹا_اشاریہ, بلاک_ڈیٹا + بلاک_آغاز, بلاک_لمبائی)) {
                    واپس اندراج_اشاریہ;
                }
            }
            اندراج_اشاریہ = اگلا_اشاریہ(initial_اندراج_اشاریہ, اندراج_اشاریہ);
        }
    }

    int32_t ڈھونڈو_اندراج(ثابت uint32_t *ڈیٹا, uint32_t بلاک_قدر, uint32_t hash_رمز) ثابت {
        uint32_t shifted_Hash_رمز = hash_رمز << سرکاؤ;
        int32_t initial_اندراج_اشاریہ = (hash_رمز % (length - 1)) + 1;  // 1..length-1
        برائے (int32_t اندراج_اشاریہ = initial_اندراج_اشاریہ;;) {
            uint32_t اندراج_2 = جدول[اندراج_اشاریہ];
            اگر (اندراج_2 == 0) {
                واپس ~اندراج_اشاریہ;
            }
            اگر ((اندراج_2 & ~mask) == shifted_Hash_رمز) {
                int32_t ڈیٹا_اشاریہ = (اندراج_2 & mask) - 1;
                اگر (all_اقدار_Same_As(ڈیٹا + ڈیٹا_اشاریہ, بلاک_لمبائی, بلاک_قدر)) {
                    واپس اندراج_اشاریہ;
                }
            }
            اندراج_اشاریہ = اگلا_اشاریہ(initial_اندراج_اشاریہ, اندراج_اشاریہ);
        }
    }

    اندرونی int32_t اگلا_اشاریہ(int32_t initial_اندراج_اشاریہ, int32_t اندراج_اشاریہ) ثابت {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        واپس (اندراج_اشاریہ + initial_اندراج_اشاریہ) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *جدول = خالی_اشاریہ;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t سرکاؤ = 0;
    uint32_t mask = 0;

    int32_t بلاک_لمبائی = 0;
};

int32_t Mutable_رمز_نقطہ_Trie::compact_پورا_ڈیٹا_بلاکیں(int32_t fastILimit, All_Same_بلاکیں &all_Same_بلاکیں) {
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    منطقی حد_سے_تجاوز = جھوٹ;
#اگر_ختم

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_ڈیٹا_استعداد = ASCII_حد_بندی;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_ڈیٹا_استعداد += UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
    // Add room for special values (errorValue, highValue) and padding.
    new_ڈیٹا_استعداد += 4;
    int32_t i_حد_بندی = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
    int32_t بلاک_لمبائی = UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی;
    int32_t inc = SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک;
    برائے (int32_t i = 0; i < i_حد_بندی; i += inc) {
        اگر (i == fastILimit) {
            بلاک_لمبائی = UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
            inc = 1;
        }
        uint32_t value = اشاریہ[i];
        اگر (flags[i] == MIXED) {
            // Really mixed?
            ثابت uint32_t *p = ڈیٹا + value;
            value = *p;
            اگر (all_اقدار_Same_As(p + 1, بلاک_لمبائی - 1, value)) {
                flags[i] = ALL_SAME;
                اشاریہ[i] = value;
                // Fall through to ALL_SAME handling.
            } ورنہ {
                new_ڈیٹا_استعداد += بلاک_لمبائی;
                جاری;
            }
        } ورنہ {
            U_ASSERT(flags[i] == ALL_SAME);
            اگر (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                منطقی allSame = سچ;
                int32_t اگلا_i = i + inc;
                برائے (int32_t j = i + 1; j < اگلا_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    اگر (اشاریہ[j] != value) {
                        allSame = جھوٹ;
                        توڑو;
                    }
                }
                اگر (!allSame) {
                    // Turn it into a MIXED block.
                    اگر (get_ڈیٹا_بلاک(i) < 0) {
                        واپس -1;
                    }
                    new_ڈیٹا_استعداد += بلاک_لمبائی;
                    جاری;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_بلاکیں.ڈھونڈو_Or_جوڑو(i, inc, value);
        اگر (other == All_Same_بلاکیں::حد_سے_تجاوز_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#اگر_تعریف_شدہ UCPTRIE_DEBUG
            اگر (!حد_سے_تجاوز) {
                puts("UCPTrie AllSameBlocks overflow");
                حد_سے_تجاوز = سچ;
            }
#اگر_ختم
            int32_t jInc = SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک;
            برائے (int32_t j = 0;; j += jInc) {
                اگر (j == i) {
                    all_Same_بلاکیں.جوڑو(i, inc, value);
                    توڑو;
                }
                اگر (j == fastILimit) {
                    jInc = 1;
                }
                اگر (flags[j] == ALL_SAME && اشاریہ[j] == value) {
                    all_Same_بلاکیں.جوڑو(j, jInc + inc, value);
                    other = j;
                    توڑو;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        اگر (other >= 0) {
            flags[i] = SAME_AS;
            اشاریہ[i] = other;
        } ورنہ {
            // New unique same-value block.
            new_ڈیٹا_استعداد += بلاک_لمبائی;
        }
    }
    واپس new_ڈیٹا_استعداد;
}

#اگر_تعریف_شدہ UCPTRIE_DEBUG
#   define DEBUG_DO(اظہاریہ) اظہاریہ
#ورنہ
#   define DEBUG_DO(اظہاریہ)
#اگر_ختم

#اگر_تعریف_شدہ UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_قدر(حرف s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (حرف)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (حرف)(0x80 + (value & 0x3F));
    واپس length + 3;
}

خالی print_بلاک(ثابت uint32_t *بلاک, int32_t بلاک_لمبائی, uint32_t value,
                UChar32 آغاز, int32_t overlap, uint32_t initial_قدر) {
    حرف s[UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی * 3 + 3];
    int32_t length = 0;
    int32_t i;
    برائے (i = 0; i < overlap; ++i) {
        length = append_قدر(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    برائے (; i < بلاک_لمبائی; ++i) {
        اگر (بلاک != خالی_اشاریہ) {
            value = بلاک[i];
        }
        اگر (value == initial_قدر) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_قدر(s, length, value);
    }
    s[length] = 0;
    آغاز += overlap;
    اگر (آغاز <= 0xffff) {
        لکھو("    %04lX  %s|\n", (طویل)آغاز, s);
    } ورنہ اگر (آغاز <= 0xfffff) {
        لکھو("   %5lX  %s|\n", (طویل)آغاز, s);
    } ورنہ {
        لکھو("  %6lX  %s|\n", (طویل)آغاز, s);
    }
}
#اگر_ختم

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
int32_t Mutable_رمز_نقطہ_Trie::compact_ڈیٹا(
        int32_t fastILimit, uint32_t *new_ڈیٹا, int32_t new_ڈیٹا_استعداد,
        int32_t ڈیٹا_Null_اشاریہ, Mixed_بلاکیں &mixed_بلاکیں, UError_رمز &error_رمز) {
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    int32_t گنتی_Same=0, مجموع_Overlaps=0;
    منطقی print_ڈیٹا = ڈیٹا_لمبائی == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        ڈیٹا_لمبائی == 50400 /* zh.txt~stroke */;
#اگر_ختم

    // The linear ASCII data has been copied into newData already.
    int32_t new_ڈیٹا_لمبائی = 0;
    برائے (int32_t i = 0; new_ڈیٹا_لمبائی < ASCII_حد_بندی;
            new_ڈیٹا_لمبائی += UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی, i += SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک) {
        اشاریہ[i] = new_ڈیٹا_لمبائی;
#اگر_تعریف_شدہ UCPTRIE_DEBUG
        اگر (print_ڈیٹا) {
            print_بلاک(new_ڈیٹا + new_ڈیٹا_لمبائی, UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی, 0, new_ڈیٹا_لمبائی, 0, initial_قدر);
        }
#اگر_ختم
    }

    int32_t بلاک_لمبائی = UCPTRIE_FAST_ڈیٹا_بلاک_لمبائی;
    اگر (!mixed_بلاکیں.init(new_ڈیٹا_استعداد, بلاک_لمبائی)) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    mixed_بلاکیں.extend(new_ڈیٹا, 0, 0, new_ڈیٹا_لمبائی);

    int32_t i_حد_بندی = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
    int32_t inc = SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک;
    int32_t fast_لمبائی = 0;
    برائے (int32_t i = ASCII_I_حد_بندی; i < i_حد_بندی; i += inc) {
        اگر (i == fastILimit) {
            بلاک_لمبائی = UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
            inc = 1;
            fast_لمبائی = new_ڈیٹا_لمبائی;
            اگر (!mixed_بلاکیں.init(new_ڈیٹا_استعداد, بلاک_لمبائی)) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس 0;
            }
            mixed_بلاکیں.extend(new_ڈیٹا, 0, 0, new_ڈیٹا_لمبائی);
        }
        اگر (flags[i] == ALL_SAME) {
            uint32_t value = اشاریہ[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_بلاکیں.ڈھونڈو_All_Same_بلاک(new_ڈیٹا, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            جب (n >= 0 && i == ڈیٹا_Null_اشاریہ && i >= fastILimit && n < fast_لمبائی &&
                    is_آغاز_کا_Some_Fast_بلاک(n, اشاریہ, fastILimit)) {
                n = ڈھونڈو_All_Same_بلاک(new_ڈیٹا, n + 1, new_ڈیٹا_لمبائی, value, بلاک_لمبائی);
            }
            اگر (n >= 0) {
                DEBUG_DO(++گنتی_Same);
                اشاریہ[i] = n;
            } ورنہ {
                n = getAllSameOverlap(new_ڈیٹا, new_ڈیٹا_لمبائی, value, بلاک_لمبائی);
                DEBUG_DO(مجموع_Overlaps += n);
#اگر_تعریف_شدہ UCPTRIE_DEBUG
                اگر (print_ڈیٹا) {
                    print_بلاک(خالی_اشاریہ, بلاک_لمبائی, value, i << UCPTRIE_سرکاؤ_3, n, initial_قدر);
                }
#اگر_ختم
                اشاریہ[i] = new_ڈیٹا_لمبائی - n;
                int32_t پچھلا_ڈیٹا_لمبائی = new_ڈیٹا_لمبائی;
                جب (n < بلاک_لمبائی) {
                    new_ڈیٹا[new_ڈیٹا_لمبائی++] = value;
                    ++n;
                }
                mixed_بلاکیں.extend(new_ڈیٹا, 0, پچھلا_ڈیٹا_لمبائی, new_ڈیٹا_لمبائی);
            }
        } ورنہ اگر (flags[i] == MIXED) {
            ثابت uint32_t *بلاک = ڈیٹا + اشاریہ[i];
            int32_t n = mixed_بلاکیں.ڈھونڈو_بلاک(new_ڈیٹا, بلاک, 0);
            اگر (n >= 0) {
                DEBUG_DO(++گنتی_Same);
                اشاریہ[i] = n;
            } ورنہ {
                n = getOverlap(new_ڈیٹا, new_ڈیٹا_لمبائی, بلاک, 0, بلاک_لمبائی);
                DEBUG_DO(مجموع_Overlaps += n);
#اگر_تعریف_شدہ UCPTRIE_DEBUG
                اگر (print_ڈیٹا) {
                    print_بلاک(بلاک, بلاک_لمبائی, 0, i << UCPTRIE_سرکاؤ_3, n, initial_قدر);
                }
#اگر_ختم
                اشاریہ[i] = new_ڈیٹا_لمبائی - n;
                int32_t پچھلا_ڈیٹا_لمبائی = new_ڈیٹا_لمبائی;
                جب (n < بلاک_لمبائی) {
                    new_ڈیٹا[new_ڈیٹا_لمبائی++] = بلاک[n++];
                }
                mixed_بلاکیں.extend(new_ڈیٹا, 0, پچھلا_ڈیٹا_لمبائی, new_ڈیٹا_لمبائی);
            }
        } ورنہ /* SAME_AS */ {
            uint32_t j = اشاریہ[i];
            اشاریہ[i] = اشاریہ[j];
        }
    }

#اگر_تعریف_شدہ UCPTRIE_DEBUG
    /* we saved some space */
    لکھو("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (طویل)ڈیٹا_لمبائی, (طویل)new_ڈیٹا_لمبائی, (طویل)گنتی_Same, (طویل)مجموع_Overlaps);
#اگر_ختم
    واپس new_ڈیٹا_لمبائی;
}

int32_t Mutable_رمز_نقطہ_Trie::compact_اشاریہ(int32_t fastILimit, Mixed_بلاکیں &mixed_بلاکیں,
                                           UError_رمز &error_رمز) {
    int32_t fast_اشاریہ_لمبائی = fastILimit >> (UCPTRIE_FAST_سرکاؤ - UCPTRIE_سرکاؤ_3);
    اگر ((بلند_آغاز >> UCPTRIE_FAST_سرکاؤ) <= fast_اشاریہ_لمبائی) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_انحراف = UCPTRIE_NO_INDEX3_NULL_انحراف;
        واپس fast_اشاریہ_لمبائی;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_اشاریہ[UCPTRIE_BMP_اشاریہ_لمبائی];  // fastIndexLength
    int32_t i3_پہلا_Null = -1;
    برائے (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = اشاریہ[i];
        fast_اشاریہ[j] = static_cast<uint16_t>(i3);
        اگر (i3 == static_cast<uint32_t>(ڈیٹا_Null_انحراف)) {
            اگر (i3_پہلا_Null < 0) {
                i3_پہلا_Null = j;
            } ورنہ اگر (index3_Null_انحراف < 0 &&
                    (j - i3_پہلا_Null + 1) == UCPTRIE_اشاریہ_3_بلاک_لمبائی) {
                index3_Null_انحراف = i3_پہلا_Null;
            }
        } ورنہ {
            i3_پہلا_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_اگلا = i + SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک;
        جب (++i < i_اگلا) {
            i3 += UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی;
            اشاریہ[i] = i3;
        }
    }

    اگر (!mixed_بلاکیں.init(fast_اشاریہ_لمبائی, UCPTRIE_اشاریہ_3_بلاک_لمبائی)) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    mixed_بلاکیں.extend(fast_اشاریہ, 0, 0, fast_اشاریہ_لمبائی);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_استعداد = 0;
    i3_پہلا_Null = index3_Null_انحراف;
    منطقی has_Long_I3_بلاکیں = جھوٹ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_آغاز = fastILimit < BMP_I_حد_بندی ? 0 : BMP_I_حد_بندی;
    int32_t i_حد_بندی = بلند_آغاز >> UCPTRIE_سرکاؤ_3;
    برائے (int32_t i = i_آغاز; i < i_حد_بندی;) {
        int32_t j = i;
        int32_t j_حد_بندی = i + UCPTRIE_اشاریہ_3_بلاک_لمبائی;
        uint32_t oredI3 = 0;
        منطقی isNull = سچ;
        کرو {
            uint32_t i3 = اشاریہ[j];
            oredI3 |= i3;
            اگر (i3 != static_cast<uint32_t>(ڈیٹا_Null_انحراف)) {
                isNull = جھوٹ;
            }
        } جب (++j < j_حد_بندی);
        اگر (isNull) {
            flags[i] = I3_NULL;
            اگر (i3_پہلا_Null < 0) {
                اگر (oredI3 <= 0xffff) {
                    index3_استعداد += UCPTRIE_اشاریہ_3_بلاک_لمبائی;
                } ورنہ {
                    index3_استعداد += اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی;
                    has_Long_I3_بلاکیں = سچ;
                }
                i3_پہلا_Null = 0;
            }
        } ورنہ {
            اگر (oredI3 <= 0xffff) {
                int32_t n = mixed_بلاکیں.ڈھونڈو_بلاک(fast_اشاریہ, اشاریہ, i);
                اگر (n >= 0) {
                    flags[i] = I3_BMP;
                    اشاریہ[i] = n;
                } ورنہ {
                    flags[i] = I3_16;
                    index3_استعداد += UCPTRIE_اشاریہ_3_بلاک_لمبائی;
                }
            } ورنہ {
                flags[i] = I3_18;
                index3_استعداد += اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی;
                has_Long_I3_بلاکیں = سچ;
            }
        }
        i = j;
    }

    int32_t index2_استعداد = (i_حد_بندی - i_آغاز) >> UCPTRIE_سرکاؤ_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_لمبائی = (index2_استعداد + UCPTRIE_اشاریہ_2_نقاب) >> UCPTRIE_سرکاؤ_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_استعداد = fast_اشاریہ_لمبائی + index1_لمبائی + index3_استعداد + index2_استعداد + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_استعداد * 2));
    اگر (index16 == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    uprv_memcpy(index16, fast_اشاریہ, fast_اشاریہ_لمبائی * 2);

    اگر (!mixed_بلاکیں.init(index16_استعداد, UCPTRIE_اشاریہ_3_بلاک_لمبائی)) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    Mixed_بلاکیں long_I3_بلاکیں;
    اگر (has_Long_I3_بلاکیں) {
        اگر (!long_I3_بلاکیں.init(index16_استعداد, اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی)) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_حد_بندی >> UCPTRIE_سرکاؤ_2];  // index2Capacity
    int32_t i2_لمبائی = 0;
    i3_پہلا_Null = index3_Null_انحراف;
    int32_t index3_آغاز = fast_اشاریہ_لمبائی + index1_لمبائی;
    int32_t اشاریہ_لمبائی = index3_آغاز;
    برائے (int32_t i = i_آغاز; i < i_حد_بندی; i += UCPTRIE_اشاریہ_3_بلاک_لمبائی) {
        int32_t i3;
        uint8_t f = flags[i];
        اگر (f == I3_NULL && i3_پہلا_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = ڈیٹا_Null_انحراف <= 0xffff ? I3_16 : I3_18;
            i3_پہلا_Null = 0;
        }
        اگر (f == I3_NULL) {
            i3 = index3_Null_انحراف;
        } ورنہ اگر (f == I3_BMP) {
            i3 = اشاریہ[i];
        } ورنہ اگر (f == I3_16) {
            int32_t n = mixed_بلاکیں.ڈھونڈو_بلاک(index16, اشاریہ, i);
            اگر (n >= 0) {
                i3 = n;
            } ورنہ {
                اگر (اشاریہ_لمبائی == index3_آغاز) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ورنہ {
                    n = getOverlap(index16, اشاریہ_لمبائی,
                                   اشاریہ, i, UCPTRIE_اشاریہ_3_بلاک_لمبائی);
                }
                i3 = اشاریہ_لمبائی - n;
                int32_t پچھلا_اشاریہ_لمبائی = اشاریہ_لمبائی;
                جب (n < UCPTRIE_اشاریہ_3_بلاک_لمبائی) {
                    index16[اشاریہ_لمبائی++] = اشاریہ[i + n++];
                }
                mixed_بلاکیں.extend(index16, index3_آغاز, پچھلا_اشاریہ_لمبائی, اشاریہ_لمبائی);
                اگر (has_Long_I3_بلاکیں) {
                    long_I3_بلاکیں.extend(index16, index3_آغاز, پچھلا_اشاریہ_لمبائی, اشاریہ_لمبائی);
                }
            }
        } ورنہ {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_بلاکیں);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_حد_بندی = i + UCPTRIE_اشاریہ_3_بلاک_لمبائی;
            int32_t k = اشاریہ_لمبائی;
            کرو {
                ++k;
                uint32_t v = اشاریہ[j++];
                uint32_t بالائی_بٹ = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = اشاریہ[j++];
                بالائی_بٹ |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = بالائی_بٹ;
            } جب (j < j_حد_بندی);
            int32_t n = long_I3_بلاکیں.ڈھونڈو_بلاک(index16, index16, اشاریہ_لمبائی);
            اگر (n >= 0) {
                i3 = n | 0x8000;
            } ورنہ {
                اگر (اشاریہ_لمبائی == index3_آغاز) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ورنہ {
                    n = getOverlap(index16, اشاریہ_لمبائی,
                                   index16, اشاریہ_لمبائی, اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی);
                }
                i3 = (اشاریہ_لمبائی - n) | 0x8000;
                int32_t پچھلا_اشاریہ_لمبائی = اشاریہ_لمبائی;
                اگر (n > 0) {
                    int32_t آغاز = اشاریہ_لمبائی;
                    جب (n < اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی) {
                        index16[اشاریہ_لمبائی++] = index16[آغاز + n++];
                    }
                } ورنہ {
                    اشاریہ_لمبائی += اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمبائی;
                }
                mixed_بلاکیں.extend(index16, index3_آغاز, پچھلا_اشاریہ_لمبائی, اشاریہ_لمبائی);
                اگر (has_Long_I3_بلاکیں) {
                    long_I3_بلاکیں.extend(index16, index3_آغاز, پچھلا_اشاریہ_لمبائی, اشاریہ_لمبائی);
                }
            }
        }
        اگر (index3_Null_انحراف < 0 && i3_پہلا_Null >= 0) {
            index3_Null_انحراف = i3;
        }
        // Set the index-2 table entry.
        index2[i2_لمبائی++] = i3;
    }
    U_ASSERT(i2_لمبائی == index2_استعداد);
    U_ASSERT(اشاریہ_لمبائی <= index3_آغاز + index3_استعداد);

    اگر (index3_Null_انحراف < 0) {
        index3_Null_انحراف = UCPTRIE_NO_INDEX3_NULL_انحراف;
    }
    اگر (اشاریہ_لمبائی >= (UCPTRIE_NO_INDEX3_NULL_انحراف + UCPTRIE_اشاریہ_3_بلاک_لمبائی)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_اشاریہ_2_بلاک_لمبائی == UCPTRIE_اشاریہ_3_بلاک_لمبائی,
                  "must re-init mixedBlocks");
    int32_t بلاک_لمبائی = UCPTRIE_اشاریہ_2_بلاک_لمبائی;
    int32_t i1 = fast_اشاریہ_لمبائی;
    برائے (int32_t i = 0; i < i2_لمبائی; i += بلاک_لمبائی) {
        int32_t n;
        اگر ((i2_لمبائی - i) >= بلاک_لمبائی) {
            // normal block
            U_ASSERT(بلاک_لمبائی == UCPTRIE_اشاریہ_2_بلاک_لمبائی);
            n = mixed_بلاکیں.ڈھونڈو_بلاک(index16, index2, i);
        } ورنہ {
            // highStart is inside the last index-2 block. Shorten it.
            بلاک_لمبائی = i2_لمبائی - i;
            n = ڈھونڈو_Same_بلاک(index16, index3_آغاز, اشاریہ_لمبائی,
                              index2, i, بلاک_لمبائی);
        }
        int32_t i2;
        اگر (n >= 0) {
            i2 = n;
        } ورنہ {
            اگر (اشاریہ_لمبائی == index3_آغاز) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } ورنہ {
                n = getOverlap(index16, اشاریہ_لمبائی, index2, i, بلاک_لمبائی);
            }
            i2 = اشاریہ_لمبائی - n;
            int32_t پچھلا_اشاریہ_لمبائی = اشاریہ_لمبائی;
            جب (n < بلاک_لمبائی) {
                index16[اشاریہ_لمبائی++] = index2[i + n++];
            }
            mixed_بلاکیں.extend(index16, index3_آغاز, پچھلا_اشاریہ_لمبائی, اشاریہ_لمبائی);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_آغاز);
    U_ASSERT(اشاریہ_لمبائی <= index16_استعداد);

#اگر_تعریف_شدہ UCPTRIE_DEBUG
    /* we saved some space */
    لکھو("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (طویل)i_حد_بندی, (طویل)اشاریہ_لمبائی);
#اگر_ختم

    واپس اشاریہ_لمبائی;
}

int32_t Mutable_رمز_نقطہ_Trie::compactTrie(int32_t fastILimit, UError_رمز &error_رمز) {
    // Find the real highStart and round it up.
    U_ASSERT((بلند_آغاز & (UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1)) == 0);
    بلند_قدر = get(MAX_UNICODE);
    int32_t real_بلند_آغاز = ڈھونڈو_بلند_آغاز();
    real_بلند_آغاز = (real_بلند_آغاز + (UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1)) &
        ~(UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1);
    اگر (real_بلند_آغاز == UNICODE_حد_بندی) {
        بلند_قدر = initial_قدر;
    }

#اگر_تعریف_شدہ UCPTRIE_DEBUG
    لکھو("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (طویل)real_بلند_آغاز, (طویل)بلند_قدر, (طویل)initial_قدر);
#اگر_ختم

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_حد_بندی = fastILimit << UCPTRIE_سرکاؤ_3;
    اگر (real_بلند_آغاز < fast_حد_بندی) {
        برائے (int32_t i = (real_بلند_آغاز >> UCPTRIE_سرکاؤ_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            اشاریہ[i] = بلند_قدر;
        }
        بلند_آغاز = fast_حد_بندی;
    } ورنہ {
        بلند_آغاز = real_بلند_آغاز;
    }

    uint32_t ascii_ڈیٹا[ASCII_حد_بندی];
    برائے (int32_t i = 0; i < ASCII_حد_بندی; ++i) {
        ascii_ڈیٹا[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_بلاکیں all_Same_بلاکیں;
    int32_t new_ڈیٹا_استعداد = compact_پورا_ڈیٹا_بلاکیں(fastILimit, all_Same_بلاکیں);
    اگر (new_ڈیٹا_استعداد < 0) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    uint32_t* new_ڈیٹا = static_cast<uint32_t*>(uprv_malloc(new_ڈیٹا_استعداد * 4));
    اگر (new_ڈیٹا == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    uprv_memcpy(new_ڈیٹا, ascii_ڈیٹا, حجم(ascii_ڈیٹا));

    int32_t ڈیٹا_Null_اشاریہ = all_Same_بلاکیں.ڈھونڈو_Most_مستعمل();

    Mixed_بلاکیں mixed_بلاکیں;
    int32_t new_ڈیٹا_لمبائی = compact_ڈیٹا(fastILimit, new_ڈیٹا, new_ڈیٹا_استعداد,
                                        ڈیٹا_Null_اشاریہ, mixed_بلاکیں, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس 0; }
    U_ASSERT(new_ڈیٹا_لمبائی <= new_ڈیٹا_استعداد);
    uprv_free(ڈیٹا);
    ڈیٹا = new_ڈیٹا;
    ڈیٹا_استعداد = new_ڈیٹا_استعداد;
    ڈیٹا_لمبائی = new_ڈیٹا_لمبائی;
    اگر (ڈیٹا_لمبائی > (0x3ffff + UCPTRIE_SMALL_ڈیٹا_بلاک_لمبائی)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اگر (ڈیٹا_Null_اشاریہ >= 0) {
        ڈیٹا_Null_انحراف = اشاریہ[ڈیٹا_Null_اشاریہ];
#اگر_تعریف_شدہ UCPTRIE_DEBUG
        اگر (ڈیٹا[ڈیٹا_Null_انحراف] != initial_قدر) {
            لکھو("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (طویل)initial_قدر, (طویل)ڈیٹا[ڈیٹا_Null_انحراف]);
        }
#اگر_ختم
        initial_قدر = ڈیٹا[ڈیٹا_Null_انحراف];
    } ورنہ {
        ڈیٹا_Null_انحراف = UCPTRIE_NO_ڈیٹا_NULL_انحراف;
    }

    int32_t اشاریہ_لمبائی = compact_اشاریہ(fastILimit, mixed_بلاکیں, error_رمز);
    بلند_آغاز = real_بلند_آغاز;
    واپس اشاریہ_لمبائی;
}

UCPTrie *Mutable_رمز_نقطہ_Trie::build(UCPTrieType type, UCPTrie_قدر_چوڑائی قدر_چوڑائی, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قدر_چوڑائی < UCPTRIE_قدر_بٹ_16 || UCPTRIE_قدر_بٹ_8 < قدر_چوڑائی) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    انتخاب (قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_32:
        توڑو;
    حالت UCPTRIE_قدر_بٹ_16:
        نقاب_اقدار(0xffff);
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        نقاب_اقدار(0xff);
        توڑو;
    طےشدہ:
        توڑو;
    }

    UChar32 fast_حد_بندی = type == UCPTRIE_TYPE_FAST ? BMP_حد_بندی : UCPTRIE_SMALL_حد_بندی;
    int32_t اشاریہ_لمبائی = compactTrie(fast_حد_بندی >> UCPTRIE_سرکاؤ_3, error_رمز);
    اگر (U_FAILURE(error_رمز)) {
        clear();
        واپس خالی_اشاریہ;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    اگر (قدر_چوڑائی == UCPTRIE_قدر_بٹ_32 && (اشاریہ_لمبائی & 1) != 0) {
        index16[اشاریہ_لمبائی++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = اشاریہ_لمبائی * 2;
    اگر (قدر_چوڑائی == UCPTRIE_قدر_بٹ_16) {
        اگر (((اشاریہ_لمبائی ^ ڈیٹا_لمبائی) & 1) != 0) {
            // padding
            ڈیٹا[ڈیٹا_لمبائی++] = error_قدر;
        }
        اگر (ڈیٹا[ڈیٹا_لمبائی - 1] != error_قدر || ڈیٹا[ڈیٹا_لمبائی - 2] != بلند_قدر) {
            ڈیٹا[ڈیٹا_لمبائی++] = بلند_قدر;
            ڈیٹا[ڈیٹا_لمبائی++] = error_قدر;
        }
        length += ڈیٹا_لمبائی * 2;
    } ورنہ اگر (قدر_چوڑائی == UCPTRIE_قدر_بٹ_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        اگر (ڈیٹا[ڈیٹا_لمبائی - 1] != error_قدر || ڈیٹا[ڈیٹا_لمبائی - 2] != بلند_قدر) {
            اگر (ڈیٹا[ڈیٹا_لمبائی - 1] != بلند_قدر) {
                ڈیٹا[ڈیٹا_لمبائی++] = بلند_قدر;
            }
            ڈیٹا[ڈیٹا_لمبائی++] = error_قدر;
        }
        length += ڈیٹا_لمبائی * 4;
    } ورنہ {
        int32_t and3 = (length + ڈیٹا_لمبائی) & 3;
        اگر (and3 == 0 && ڈیٹا[ڈیٹا_لمبائی - 1] == error_قدر && ڈیٹا[ڈیٹا_لمبائی - 2] == بلند_قدر) {
            // all set
        } ورنہ اگر(and3 == 3 && ڈیٹا[ڈیٹا_لمبائی - 1] == بلند_قدر) {
            ڈیٹا[ڈیٹا_لمبائی++] = error_قدر;
        } ورنہ {
            جب (and3 != 2) {
                ڈیٹا[ڈیٹا_لمبائی++] = بلند_قدر;
                and3 = (and3 + 1) & 3;
            }
            ڈیٹا[ڈیٹا_لمبائی++] = بلند_قدر;
            ڈیٹا[ڈیٹا_لمبائی++] = error_قدر;
        }
        length += ڈیٹا_لمبائی;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += حجم(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* بائٹیں = static_cast<uint8_t*>(uprv_malloc(length));
    اگر (بائٹیں == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        clear();
        واپس خالی_اشاریہ;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(بائٹیں);
    uprv_memset(trie, 0, حجم(UCPTrie));
    trie->اشاریہ_لمبائی = اشاریہ_لمبائی;
    trie->ڈیٹا_لمبائی = ڈیٹا_لمبائی;

    trie->بلند_آغاز = بلند_آغاز;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_بلند_آغاز = (بلند_آغاز + 0xfff) >> 12;
    trie->type = type;
    trie->قدر_چوڑائی = قدر_چوڑائی;

    trie->index3_Null_انحراف = index3_Null_انحراف;
    trie->ڈیٹا_Null_انحراف = ڈیٹا_Null_انحراف;
    trie->null_قدر = initial_قدر;

    بائٹیں += حجم(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(بائٹیں);
    trie->اشاریہ = dest16;

    اگر (بلند_آغاز <= fast_حد_بندی) {
        // Condense only the fast index from the mutable-trie index.
        برائے (int32_t i = 0, j = 0; j < اشاریہ_لمبائی; i += SMALL_ڈیٹا_بلاکیں_فی_BMP_بلاک, ++j) {
            *dest16++ = static_cast<uint16_t>(اشاریہ[i]); // dest16[j]
        }
    } ورنہ {
        uprv_memcpy(dest16, index16, اشاریہ_لمبائی * 2);
        dest16 += اشاریہ_لمبائی;
    }
    بائٹیں += اشاریہ_لمبائی * 2;

    // Write the data array.
    ثابت uint32_t *p = ڈیٹا;
    انتخاب (قدر_چوڑائی) {
    حالت UCPTRIE_قدر_بٹ_16:
        // Write 16-bit data values.
        trie->ڈیٹا.ptr16 = dest16;
        برائے (int32_t i = ڈیٹا_لمبائی; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        توڑو;
    حالت UCPTRIE_قدر_بٹ_32:
        // Write 32-bit data values.
        trie->ڈیٹا.ptr32 = reinterpret_cast<uint32_t*>(بائٹیں);
        uprv_memcpy(بائٹیں, p, (size_t)ڈیٹا_لمبائی * 4);
        توڑو;
    حالت UCPTRIE_قدر_بٹ_8:
        // Write 8-bit data values.
        trie->ڈیٹا.ptr8 = بائٹیں;
        برائے (int32_t i = ڈیٹا_لمبائی; i > 0; --i) {
            *بائٹیں++ = static_cast<uint8_t>(*p++);
        }
        توڑو;
    طےشدہ:
        // Will not occur, valueWidth checked at the beginning.
        توڑو;
    }

#اگر_تعریف_شدہ UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#اگر_ختم

    clear();
    واپس trie;
}

}  // namespace

U_NAMESPACE_انتہا

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_قدر, uint32_t error_قدر, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    Local_اشارہ_گر<Mutable_رمز_نقطہ_Trie> trie(
        نیا Mutable_رمز_نقطہ_Trie(initial_قدر, error_قدر, *p_Error_رمز), *p_Error_رمز);
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ثابت UMutableCPTrie *other, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر (other == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    Local_اشارہ_گر<Mutable_رمز_نقطہ_Trie> clone(
        نیا Mutable_رمز_نقطہ_Trie(*reinterpret_cast<ثابت Mutable_رمز_نقطہ_Trie *>(other), *p_Error_رمز), *p_Error_رمز);
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI خالی U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    حذف reinterpret_cast<Mutable_رمز_نقطہ_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ثابت UCPMap *map, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر (map == خالی_اشاریہ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<UMutableCPTrie *>(Mutable_رمز_نقطہ_Trie::fromUCPMap(map, *p_Error_رمز));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ثابت UCPTrie *trie, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر (trie == خالی_اشاریہ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<UMutableCPTrie *>(Mutable_رمز_نقطہ_Trie::fromUCPTrie(trie, *p_Error_رمز));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ثابت UMutableCPTrie *trie, UChar32 c) {
    واپس reinterpret_cast<ثابت Mutable_رمز_نقطہ_Trie *>(trie)->get(c);
}

نامخلا {

UChar32 getRange(ثابت خالی *trie, UChar32 آغاز,
                 UCPMap_قدر_Filter *filter, ثابت خالی *context, uint32_t *p_قدر) {
    واپس reinterpret_cast<ثابت Mutable_رمز_نقطہ_Trie *>(trie)->
        getRange(آغاز, filter, context, p_قدر);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ثابت UMutableCPTrie *trie, UChar32 آغاز,
                        UCPMapRangeOption option, uint32_t surrogate_قدر,
                        UCPMap_قدر_Filter *filter, ثابت خالی *context, uint32_t *p_قدر) {
    واپس ucptrie_اندرونی_Get_Range(getRange, trie, آغاز,
                                    option, surrogate_قدر,
                                    filter, context, p_قدر);
}

U_CAPI خالی U_EXPORT2
umutablecptrie_سیٹ(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    reinterpret_cast<Mutable_رمز_نقطہ_Trie *>(trie)->سیٹ(c, value, *p_Error_رمز);
}

U_CAPI خالی U_EXPORT2
umutablecptrie_سیٹ_Range(UMutableCPTrie *trie, UChar32 آغاز, UChar32 انتہا,
                   uint32_t value, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    reinterpret_cast<Mutable_رمز_نقطہ_Trie *>(trie)->سیٹ_Range(آغاز, انتہا, value, *p_Error_رمز);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_قدر_چوڑائی قدر_چوڑائی,
                              UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<Mutable_رمز_نقطہ_Trie *>(trie)->build(type, قدر_چوڑائی, *p_Error_رمز);
}

#اگر_تعریف_شدہ UCPTRIE_DEBUG
U_CFUNC خالی umutablecptrie_سیٹ_Name(UMutableCPTrie *trie, ثابت حرف *name) {
    reinterpret_cast<Mutable_رمز_نقطہ_Trie *>(trie)->name = name;
}
#اگر_ختم
