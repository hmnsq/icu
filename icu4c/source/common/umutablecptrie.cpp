// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#جے_تعریف_شدہ UCPTRIE_DEBUG
#   include <stdio.h>
#جے_انت

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#جے_تعریف_شدہ حد_توں_ودھ_2
#تعریف_رد حد_توں_ودھ_2
#جے_انت

U_NAMESPACE_BEGIN

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_حد_بندی = 0x110000;
constexpr int32_t BMP_حد_بندی = 0x10000;
constexpr int32_t ASCII_حد_بندی = 0x80;

constexpr int32_t I_حد_بندی = UNICODE_حد_بندی >> UCPTRIE_کھسکاؤ_3;
constexpr int32_t BMP_I_حد_بندی = BMP_حد_بندی >> UCPTRIE_کھسکاؤ_3;
constexpr int32_t ASCII_I_حد_بندی = ASCII_حد_بندی >> UCPTRIE_کھسکاؤ_3;

constexpr int32_t SMALL_انکڑے_بلاکاں_فی_BMP_بلاک = (1 << (UCPTRIE_FAST_کھسکاؤ - UCPTRIE_کھسکاؤ_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_انکڑے_لمائی = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_انکڑے_لمائی = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_انکڑے_لمائی = UNICODE_حد_بندی;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی = UCPTRIE_اشاریہ_3_بلاک_لمائی + UCPTRIE_اشاریہ_3_بلاک_لمائی / 8;

صنف All_Same_بلاکاں;
صنف Mixed_بلاکاں;

صنف Mutable_کوڈ_نقطہ_Trie : عام UMemory {
عام:
    Mutable_کوڈ_نقطہ_Trie(uint32_t initial_قدر, uint32_t error_قدر, UError_کوڈ &error_کوڈ);
    Mutable_کوڈ_نقطہ_Trie(ثابت Mutable_کوڈ_نقطہ_Trie &other, UError_کوڈ &error_کوڈ);
    Mutable_کوڈ_نقطہ_Trie(ثابت Mutable_کوڈ_نقطہ_Trie &other) = مٹاؤ;
    ~Mutable_کوڈ_نقطہ_Trie();

    Mutable_کوڈ_نقطہ_Trie &عامل=(ثابت Mutable_کوڈ_نقطہ_Trie &other) = مٹاؤ;

    ساكن Mutable_کوڈ_نقطہ_Trie *fromUCPMap(ثابت UCPMap *map, UError_کوڈ &error_کوڈ);
    ساكن Mutable_کوڈ_نقطہ_Trie *fromUCPTrie(ثابت UCPTrie *trie, UError_کوڈ &error_کوڈ);

    uint32_t get(UChar32 c) ثابت;
    int32_t getRange(UChar32 شروع, UCPMap_قدر_Filter *filter, ثابت فراغ *context,
                     uint32_t *p_قدر) ثابت;

    فراغ سیٹ(UChar32 c, uint32_t value, UError_کوڈ &error_کوڈ);
    فراغ سیٹ_Range(UChar32 شروع, UChar32 سرا, uint32_t value, UError_کوڈ &error_کوڈ);

    UCPTrie *build(UCPTrieType type, UCPTrie_قدر_چوڑائی قدر_چوڑائی, UError_کوڈ &error_کوڈ);

خاص:
    فراغ clear();

    منطقي ensure_بلند_شروع(UChar32 c);
    int32_t alloc_انکڑے_بلاک(int32_t بلاک_لمائی);
    int32_t get_انکڑے_بلاک(int32_t i);

    فراغ نقاب_اقدار(uint32_t mask);
    UChar32 لبھو_بلند_شروع() ثابت;
    int32_t compact_سارا_انکڑے_بلاکاں(int32_t fastILimit, All_Same_بلاکاں &all_Same_بلاکاں);
    int32_t compact_انکڑے(
            int32_t fastILimit, uint32_t *new_انکڑے, int32_t new_انکڑے_سمرتھا,
            int32_t انکڑے_Null_اشاریہ, Mixed_بلاکاں &mixed_بلاکاں, UError_کوڈ &error_کوڈ);
    int32_t compact_اشاریہ(int32_t fastILimit, Mixed_بلاکاں &mixed_بلاکاں, UError_کوڈ &error_کوڈ);
    int32_t compactTrie(int32_t fastILimit, UError_کوڈ &error_کوڈ);

    uint32_t *اشاریہ = خالی_سوچک;
    int32_t اشاریہ_سمرتھا = 0;
    int32_t index3_Null_وتھ = -1;
    uint32_t *انکڑے = خالی_سوچک;
    int32_t انکڑے_سمرتھا = 0;
    int32_t انکڑے_لمائی = 0;
    int32_t انکڑے_Null_وتھ = -1;

    uint32_t orig_Initial_قدر;
    uint32_t initial_قدر;
    uint32_t error_قدر;
    UChar32 بلند_شروع;
    uint32_t بلند_قدر;
#جے_تعریف_شدہ UCPTRIE_DEBUG
عام:
    ثابت حرف *name;
#جے_انت
خاص:
    /** Temporary array while building the final data. */
    uint16_t *index16 = خالی_سوچک;
    uint8_t flags[UNICODE_حد_بندی >> UCPTRIE_کھسکاؤ_3];
};

Mutable_کوڈ_نقطہ_Trie::Mutable_کوڈ_نقطہ_Trie(uint32_t ini_قدر, uint32_t err_قدر, UError_کوڈ &error_کوڈ) :
        orig_Initial_قدر(ini_قدر), initial_قدر(ini_قدر), error_قدر(err_قدر),
        بلند_شروع(0), بلند_قدر(initial_قدر)
#جے_تعریف_شدہ UCPTRIE_DEBUG
        , name("open")
#جے_انت
        {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    اشاریہ = static_cast<uint32_t*>(uprv_malloc(BMP_I_حد_بندی * 4));
    انکڑے = static_cast<uint32_t*>(uprv_malloc(INITIAL_انکڑے_لمائی * 4));
    إذا (اشاریہ == خالی_سوچک || انکڑے == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    اشاریہ_سمرتھا = BMP_I_حد_بندی;
    انکڑے_سمرتھا = INITIAL_انکڑے_لمائی;
}

Mutable_کوڈ_نقطہ_Trie::Mutable_کوڈ_نقطہ_Trie(ثابت Mutable_کوڈ_نقطہ_Trie &other, UError_کوڈ &error_کوڈ) :
        index3_Null_وتھ(other.index3_Null_وتھ),
        انکڑے_Null_وتھ(other.انکڑے_Null_وتھ),
        orig_Initial_قدر(other.orig_Initial_قدر), initial_قدر(other.initial_قدر),
        error_قدر(other.error_قدر),
        بلند_شروع(other.بلند_شروع), بلند_قدر(other.بلند_قدر)
#جے_تعریف_شدہ UCPTRIE_DEBUG
        , name("mutable clone")
#جے_انت
        {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    int32_t i_سمرتھا = بلند_شروع <= BMP_حد_بندی ? BMP_I_حد_بندی : I_حد_بندی;
    اشاریہ = static_cast<uint32_t*>(uprv_malloc(i_سمرتھا * 4));
    انکڑے = static_cast<uint32_t*>(uprv_malloc(other.انکڑے_سمرتھا * 4));
    إذا (اشاریہ == خالی_سوچک || انکڑے == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    اشاریہ_سمرتھا = i_سمرتھا;
    انکڑے_سمرتھا = other.انکڑے_سمرتھا;

    int32_t i_حد_بندی = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
    uprv_memcpy(flags, other.flags, i_حد_بندی);
    uprv_memcpy(اشاریہ, other.اشاریہ, i_حد_بندی * 4);
    uprv_memcpy(انکڑے, other.انکڑے, (size_t)other.انکڑے_لمائی * 4);
    انکڑے_لمائی = other.انکڑے_لمائی;
    U_ASSERT(other.index16 == خالی_سوچک);
}

Mutable_کوڈ_نقطہ_Trie::~Mutable_کوڈ_نقطہ_Trie() {
    uprv_free(اشاریہ);
    uprv_free(انکڑے);
    uprv_free(index16);
}

Mutable_کوڈ_نقطہ_Trie *Mutable_کوڈ_نقطہ_Trie::fromUCPMap(ثابت UCPMap *map, UError_کوڈ &error_کوڈ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_قدر = ucpmap_get(map, -1);
    uint32_t initial_قدر = ucpmap_get(map, 0x10ffff);
    Local_اشارہ_گر<Mutable_کوڈ_نقطہ_Trie> mutableTrie(
        جديد Mutable_کوڈ_نقطہ_Trie(initial_قدر, error_قدر, error_کوڈ),
        error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    UChar32 شروع = 0, سرا;
    uint32_t value;
    طالما ((سرا = ucpmap_getRange(map, شروع, UCPMAP_RANGE_عام, 0,
                                  خالی_سوچک, خالی_سوچک, &value)) >= 0) {
        إذا (value != initial_قدر) {
            إذا (شروع == سرا) {
                mutableTrie->سیٹ(شروع, value, error_کوڈ);
            } وإلا {
                mutableTrie->سیٹ_Range(شروع, سرا, value, error_کوڈ);
            }
        }
        شروع = سرا + 1;
    }
    إذا (U_SUCCESS(error_کوڈ)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع خالی_سوچک;
    }
}

Mutable_کوڈ_نقطہ_Trie *Mutable_کوڈ_نقطہ_Trie::fromUCPTrie(ثابت UCPTrie *trie, UError_کوڈ &error_کوڈ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_قدر;
    uint32_t initial_قدر;
    بدل (trie->قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        error_قدر = trie->انکڑے.ptr16[trie->انکڑے_لمائی - UCPTRIE_ERROR_قدر_NEG_انکڑے_وتھ];
        initial_قدر = trie->انکڑے.ptr16[trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ];
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        error_قدر = trie->انکڑے.ptr32[trie->انکڑے_لمائی - UCPTRIE_ERROR_قدر_NEG_انکڑے_وتھ];
        initial_قدر = trie->انکڑے.ptr32[trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ];
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        error_قدر = trie->انکڑے.ptr8[trie->انکڑے_لمائی - UCPTRIE_ERROR_قدر_NEG_انکڑے_وتھ];
        initial_قدر = trie->انکڑے.ptr8[trie->انکڑے_لمائی - UCPTRIE_بلند_قدر_NEG_انکڑے_وتھ];
        اكسر;
    افتراضي:
        // Unreachable if the trie is properly initialized.
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    Local_اشارہ_گر<Mutable_کوڈ_نقطہ_Trie> mutableTrie(
        جديد Mutable_کوڈ_نقطہ_Trie(initial_قدر, error_قدر, error_کوڈ),
        error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    UChar32 شروع = 0, سرا;
    uint32_t value;
    طالما ((سرا = ucptrie_getRange(trie, شروع, UCPMAP_RANGE_عام, 0,
                                   خالی_سوچک, خالی_سوچک, &value)) >= 0) {
        إذا (value != initial_قدر) {
            إذا (شروع == سرا) {
                mutableTrie->سیٹ(شروع, value, error_کوڈ);
            } وإلا {
                mutableTrie->سیٹ_Range(شروع, سرا, value, error_کوڈ);
            }
        }
        شروع = سرا + 1;
    }
    إذا (U_SUCCESS(error_کوڈ)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع خالی_سوچک;
    }
}

فراغ Mutable_کوڈ_نقطہ_Trie::clear() {
    index3_Null_وتھ = انکڑے_Null_وتھ = -1;
    انکڑے_لمائی = 0;
    بلند_قدر = initial_قدر = orig_Initial_قدر;
    بلند_شروع = 0;
    uprv_free(index16);
    index16 = خالی_سوچک;
}

uint32_t Mutable_کوڈ_نقطہ_Trie::get(UChar32 c) ثابت {
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        أرجع error_قدر;
    }
    إذا (c >= بلند_شروع) {
        أرجع بلند_قدر;
    }
    int32_t i = c >> UCPTRIE_کھسکاؤ_3;
    إذا (flags[i] == ALL_SAME) {
        أرجع اشاریہ[i];
    } وإلا {
        أرجع انکڑے[اشاریہ[i] + (c & UCPTRIE_SMALL_انکڑے_نقاب)];
    }
}

مضمن uint32_t maybe_Filter_قدر(uint32_t value, uint32_t initial_قدر, uint32_t null_قدر,
                                 UCPMap_قدر_Filter *filter, ثابت فراغ *context) {
    إذا (value == initial_قدر) {
        value = null_قدر;
    } وإلا إذا (filter != خالی_سوچک) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 Mutable_کوڈ_نقطہ_Trie::getRange(
        UChar32 شروع, UCPMap_قدر_Filter *filter, ثابت فراغ *context,
        uint32_t *p_قدر) ثابت {
    إذا (static_cast<uint32_t>(شروع) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    إذا (شروع >= بلند_شروع) {
        إذا (p_قدر != خالی_سوچک) {
            uint32_t value = بلند_قدر;
            إذا (filter != خالی_سوچک) { value = filter(context, value); }
            *p_قدر = value;
        }
        أرجع MAX_UNICODE;
    }
    uint32_t null_قدر = initial_قدر;
    إذا (filter != خالی_سوچک) { null_قدر = filter(context, null_قدر); }
    UChar32 c = شروع;
    uint32_t trie_قدر, value;
    منطقي have_قدر = خطأ;
    int32_t i = c >> UCPTRIE_کھسکاؤ_3;
    افعل {
        إذا (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = اشاریہ[i];
            إذا (have_قدر) {
                إذا (trieValue2 != trie_قدر) {
                    إذا (filter == خالی_سوچک ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_قدر = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_قدر = trieValue2;
                value = maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر, filter, context);
                إذا (p_قدر != خالی_سوچک) { *p_قدر = value; }
                have_قدر = سچ;
            }
            c = (c + UCPTRIE_SMALL_انکڑے_بلاک_لمائی) & ~UCPTRIE_SMALL_انکڑے_نقاب;
        } وإلا /* MIXED */ {
            int32_t di = اشاریہ[i] + (c & UCPTRIE_SMALL_انکڑے_نقاب);
            uint32_t trieValue2 = انکڑے[di];
            إذا (have_قدر) {
                إذا (trieValue2 != trie_قدر) {
                    إذا (filter == خالی_سوچک ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_قدر = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_قدر = trieValue2;
                value = maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر, filter, context);
                إذا (p_قدر != خالی_سوچک) { *p_قدر = value; }
                have_قدر = سچ;
            }
            طالما ((++c & UCPTRIE_SMALL_انکڑے_نقاب) != 0) {
                trieValue2 = انکڑے[++di];
                إذا (trieValue2 != trie_قدر) {
                    إذا (filter == خالی_سوچک ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                }
                trie_قدر = trieValue2;  // may or may not help
            }
        }
        ++i;
    } طالما (c < بلند_شروع);
    U_ASSERT(have_قدر);
    إذا (maybe_Filter_قدر(بلند_قدر, initial_قدر, null_قدر,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

فراغ
write_بلاک(uint32_t *بلاک, uint32_t value) {
    uint32_t *حد_بندی = بلاک + UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
    طالما (بلاک < حد_بندی) {
        *بلاک++ = value;
    }
}

منطقي Mutable_کوڈ_نقطہ_Trie::ensure_بلند_شروع(UChar32 c) {
    إذا (c >= بلند_شروع) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_فی_اشاریہ_2_اندراج) & ~(UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1);
        int32_t i = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
        int32_t i_حد_بندی = c >> UCPTRIE_کھسکاؤ_3;
        إذا (i_حد_بندی > اشاریہ_سمرتھا) {
            uint32_t* new_اشاریہ = static_cast<uint32_t*>(uprv_malloc(I_حد_بندی * 4));
            إذا (new_اشاریہ == خالی_سوچک) { أرجع خطأ; }
            uprv_memcpy(new_اشاریہ, اشاریہ, i * 4);
            uprv_free(اشاریہ);
            اشاریہ = new_اشاریہ;
            اشاریہ_سمرتھا = I_حد_بندی;
        }
        افعل {
            flags[i] = ALL_SAME;
            اشاریہ[i] = initial_قدر;
        } طالما(++i < i_حد_بندی);
        بلند_شروع = c;
    }
    أرجع سچ;
}

int32_t Mutable_کوڈ_نقطہ_Trie::alloc_انکڑے_بلاک(int32_t بلاک_لمائی) {
    int32_t new_بلاک = انکڑے_لمائی;
    int32_t new_چوٹی = new_بلاک + بلاک_لمائی;
    إذا (new_چوٹی > انکڑے_سمرتھا) {
        int32_t capacity;
        إذا (انکڑے_سمرتھا < MEDIUM_انکڑے_لمائی) {
            capacity = MEDIUM_انکڑے_لمائی;
        } وإلا إذا (انکڑے_سمرتھا < MAX_انکڑے_لمائی) {
            capacity = MAX_انکڑے_لمائی;
        } وإلا {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            أرجع -1;
        }
        uint32_t* new_انکڑے = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        إذا (new_انکڑے == خالی_سوچک) {
            أرجع -1;
        }
        uprv_memcpy(new_انکڑے, انکڑے, (size_t)انکڑے_لمائی * 4);
        uprv_free(انکڑے);
        انکڑے = new_انکڑے;
        انکڑے_سمرتھا = capacity;
    }
    انکڑے_لمائی = new_چوٹی;
    أرجع new_بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_کوڈ_نقطہ_Trie::get_انکڑے_بلاک(int32_t i) {
    إذا (flags[i] == MIXED) {
        أرجع اشاریہ[i];
    }
    إذا (i < BMP_I_حد_بندی) {
        int32_t new_بلاک = alloc_انکڑے_بلاک(UCPTRIE_FAST_انکڑے_بلاک_لمائی);
        إذا (new_بلاک < 0) { أرجع new_بلاک; }
        int32_t i_شروع = i & ~(SMALL_انکڑے_بلاکاں_فی_BMP_بلاک -1);
        int32_t i_حد_بندی = i_شروع + SMALL_انکڑے_بلاکاں_فی_BMP_بلاک;
        افعل {
            U_ASSERT(flags[i_شروع] == ALL_SAME);
            write_بلاک(انکڑے + new_بلاک, اشاریہ[i_شروع]);
            flags[i_شروع] = MIXED;
            اشاریہ[i_شروع++] = new_بلاک;
            new_بلاک += UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
        } طالما (i_شروع < i_حد_بندی);
        أرجع اشاریہ[i];
    } وإلا {
        int32_t new_بلاک = alloc_انکڑے_بلاک(UCPTRIE_SMALL_انکڑے_بلاک_لمائی);
        إذا (new_بلاک < 0) { أرجع new_بلاک; }
        write_بلاک(انکڑے + new_بلاک, اشاریہ[i]);
        flags[i] = MIXED;
        اشاریہ[i] = new_بلاک;
        أرجع new_بلاک;
    }
}

فراغ Mutable_کوڈ_نقطہ_Trie::سیٹ(UChar32 c, uint32_t value, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    int32_t بلاک;
    إذا (!ensure_بلند_شروع(c) || (بلاک = get_انکڑے_بلاک(c >> UCPTRIE_کھسکاؤ_3)) < 0) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    انکڑے[بلاک + (c & UCPTRIE_SMALL_انکڑے_نقاب)] = value;
}

فراغ
fill_بلاک(uint32_t *بلاک, UChar32 شروع, UChar32 حد_بندی, uint32_t value) {
    uint32_t *p_حد_بندی = بلاک + حد_بندی;
    بلاک += شروع;
    طالما (بلاک < p_حد_بندی) {
        *بلاک++ = value;
    }
}

فراغ Mutable_کوڈ_نقطہ_Trie::سیٹ_Range(UChar32 شروع, UChar32 سرا, uint32_t value, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(شروع) > MAX_UNICODE || static_cast<uint32_t>(سرا) > MAX_UNICODE || شروع > سرا) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (!ensure_بلند_شروع(سرا)) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    UChar32 حد_بندی = سرا + 1;
    إذا (شروع & UCPTRIE_SMALL_انکڑے_نقاب) {
        // Set partial block at [start..following block boundary[.
        int32_t بلاک = get_انکڑے_بلاک(شروع >> UCPTRIE_کھسکاؤ_3);
        إذا (بلاک < 0) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }

        UChar32 اگلا_شروع = (شروع + UCPTRIE_SMALL_انکڑے_نقاب) & ~UCPTRIE_SMALL_انکڑے_نقاب;
        إذا (اگلا_شروع <= حد_بندی) {
            fill_بلاک(انکڑے + بلاک, شروع & UCPTRIE_SMALL_انکڑے_نقاب, UCPTRIE_SMALL_انکڑے_بلاک_لمائی,
                      value);
            شروع = اگلا_شروع;
        } وإلا {
            fill_بلاک(انکڑے + بلاک, شروع & UCPTRIE_SMALL_انکڑے_نقاب, حد_بندی & UCPTRIE_SMALL_انکڑے_نقاب,
                      value);
            أرجع;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = حد_بندی & UCPTRIE_SMALL_انکڑے_نقاب;

    // Round down limit to a block boundary.
    حد_بندی &= ~UCPTRIE_SMALL_انکڑے_نقاب;

    // Iterate over all-value blocks.
    طالما (شروع < حد_بندی) {
        int32_t i = شروع >> UCPTRIE_کھسکاؤ_3;
        إذا (flags[i] == ALL_SAME) {
            اشاریہ[i] = value;
        } وإلا /* MIXED */ {
            fill_بلاک(انکڑے + اشاریہ[i], 0, UCPTRIE_SMALL_انکڑے_بلاک_لمائی, value);
        }
        شروع += UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
    }

    إذا (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t بلاک = get_انکڑے_بلاک(شروع >> UCPTRIE_کھسکاؤ_3);
        إذا (بلاک < 0) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }

        fill_بلاک(انکڑے + بلاک, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

فراغ Mutable_کوڈ_نقطہ_Trie::نقاب_اقدار(uint32_t mask) {
    initial_قدر &= mask;
    error_قدر &= mask;
    بلند_قدر &= mask;
    int32_t i_حد_بندی = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
    لكل (int32_t i = 0; i < i_حد_بندی; ++i) {
        إذا (flags[i] == ALL_SAME) {
            اشاریہ[i] &= mask;
        }
    }
    لكل (int32_t i = 0; i < انکڑے_لمائی; ++i) {
        انکڑے[i] &= mask;
    }
}

سانچا<قسم_ناں UIntA, قسم_ناں UIntB>
منطقي equal_بلاکاں(ثابت UIntA *s, ثابت UIntB *t, int32_t length) {
    طالما (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

منطقي all_اقدار_Same_As(ثابت uint32_t *p, int32_t length, uint32_t value) {
    ثابت uint32_t *p_حد_بندی = p + length;
    طالما (p < p_حد_بندی && *p == value) { ++p; }
    أرجع p == p_حد_بندی;
}

/** Search for an identical block. */
int32_t لبھو_Same_بلاک(ثابت uint16_t *p, int32_t p_شروع, int32_t length,
                      ثابت uint16_t *q, int32_t q_شروع, int32_t بلاک_لمائی) {
    // Ensure that we do not even partially get past length.
    length -= بلاک_لمائی;

    q += q_شروع;
    طالما (p_شروع <= length) {
        إذا (equal_بلاکاں(p + p_شروع, q, بلاک_لمائی)) {
            أرجع p_شروع;
        }
        ++p_شروع;
    }
    أرجع -1;
}

int32_t لبھو_All_Same_بلاک(ثابت uint32_t *p, int32_t شروع, int32_t حد_بندی,
                         uint32_t value, int32_t بلاک_لمائی) {
    // Ensure that we do not even partially get past limit.
    حد_بندی -= بلاک_لمائی;

    لكل (int32_t بلاک = شروع; بلاک <= حد_بندی; ++بلاک) {
        إذا (p[بلاک] == value) {
            لكل (int32_t i = 1;; ++i) {
                إذا (i == بلاک_لمائی) {
                    أرجع بلاک;
                }
                إذا (p[بلاک + i] != value) {
                    بلاک += i;
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
سانچا<قسم_ناں UIntA, قسم_ناں UIntB>
int32_t getOverlap(ثابت UIntA *p, int32_t length,
                   ثابت UIntB *q, int32_t q_شروع, int32_t بلاک_لمائی) {
    int32_t overlap = بلاک_لمائی - 1;
    U_ASSERT(overlap <= length);
    q += q_شروع;
    طالما (overlap > 0 && !equal_بلاکاں(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    أرجع overlap;
}

int32_t getAllSameOverlap(ثابت uint32_t *p, int32_t length, uint32_t value,
                          int32_t بلاک_لمائی) {
    int32_t min = length - (بلاک_لمائی - 1);
    int32_t i = length;
    طالما (min < i && p[i - 1] == value) { --i; }
    أرجع length - i;
}

منطقي is_شروع_دا_Some_Fast_بلاک(uint32_t انکڑے_وتھ, ثابت uint32_t اشاریہ[], int32_t fastILimit) {
    لكل (int32_t i = 0; i < fastILimit; i += SMALL_انکڑے_بلاکاں_فی_BMP_بلاک) {
        إذا (اشاریہ[i] == انکڑے_وتھ) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_کوڈ_نقطہ_Trie::لبھو_بلند_شروع() ثابت {
    int32_t i = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
    طالما (i > 0) {
        منطقي میل;
        إذا (flags[--i] == ALL_SAME) {
            میل = اشاریہ[i] == بلند_قدر;
        } وإلا /* MIXED */ {
            ثابت uint32_t *p = انکڑے + اشاریہ[i];
            لكل (int32_t j = 0;; ++j) {
                إذا (j == UCPTRIE_SMALL_انکڑے_بلاک_لمائی) {
                    میل = سچ;
                    اكسر;
                }
                إذا (p[j] != بلند_قدر) {
                    میل = خطأ;
                    اكسر;
                }
            }
        }
        إذا (!میل) {
            أرجع (i + 1) << UCPTRIE_کھسکاؤ_3;
        }
    }
    أرجع 0;
}

صنف All_Same_بلاکاں {
عام:
    ساكن constexpr int32_t NEW_UNIQUE = -1;
    ساكن constexpr int32_t حد_توں_ودھ_2 = -2;

    All_Same_بلاکاں() : length(0), mostRecent(-1) {}

    int32_t لبھو_Or_جوڑو(int32_t اشاریہ, int32_t count, uint32_t value) {
        إذا (mostRecent >= 0 && اقدار[mostRecent] == value) {
            refCounts[mostRecent] += count;
            أرجع indexes[mostRecent];
        }
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (اقدار[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                أرجع indexes[i];
            }
        }
        إذا (length == سمرتھا_3) {
            أرجع حد_توں_ودھ_2;
        }
        mostRecent = length;
        indexes[length] = اشاریہ;
        اقدار[length] = value;
        refCounts[length++] = count;
        أرجع NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    فراغ جوڑو(int32_t اشاریہ, int32_t count, uint32_t value) {
        U_ASSERT(length == سمرتھا_3);
        int32_t least = -1;
        int32_t گھٹ_توں_گھٹ_گنتی = I_حد_بندی;
        لكل (int32_t i = 0; i < length; ++i) {
            U_ASSERT(اقدار[i] != value);
            إذا (refCounts[i] < گھٹ_توں_گھٹ_گنتی) {
                least = i;
                گھٹ_توں_گھٹ_گنتی = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = اشاریہ;
        اقدار[least] = value;
        refCounts[least] = count;
    }

    int32_t لبھو_Most_ورتیا() ثابت {
        إذا (length == 0) { أرجع -1; }
        int32_t max = -1;
        int32_t max_گنتی = 0;
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (refCounts[i] > max_گنتی) {
                max = i;
                max_گنتی = refCounts[i];
            }
        }
        أرجع indexes[max];
    }

خاص:
    ساكن constexpr int32_t سمرتھا_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[سمرتھا_3];
    uint32_t اقدار[سمرتھا_3];
    int32_t refCounts[سمرتھا_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
صنف Mixed_بلاکاں {
عام:
    Mixed_بلاکاں() {}
    ~Mixed_بلاکاں() {
        uprv_free(جدول);
    }

    منطقي init(int32_t max_لمائی_2, int32_t new_بلاک_لمائی) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_انکڑے_اشاریہ = max_لمائی_2 - new_بلاک_لمائی + 1;
        int32_t new_لمائی;
        إذا (max_انکڑے_اشاریہ <= 0xfff) {  // 4k
            new_لمائی = 6007;
            کھسکاؤ = 12;
            mask = 0xfff;
        } وإلا إذا (max_انکڑے_اشاریہ <= 0x7fff) {  // 32k
            new_لمائی = 50021;
            کھسکاؤ = 15;
            mask = 0x7fff;
        } وإلا إذا (max_انکڑے_اشاریہ <= 0x1ffff) {  // 128k
            new_لمائی = 200003;
            کھسکاؤ = 17;
            mask = 0x1ffff;
        } وإلا {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_لمائی = 1500007;
            کھسکاؤ = 21;
            mask = 0x1fffff;
        }
        إذا (new_لمائی > capacity) {
            uprv_free(جدول);
            جدول = static_cast<uint32_t*>(uprv_malloc(new_لمائی * 4));
            إذا (جدول == خالی_سوچک) {
                أرجع خطأ;
            }
            capacity = new_لمائی;
        }
        length = new_لمائی;
        uprv_memset(جدول, 0, length * 4);

        بلاک_لمائی = new_بلاک_لمائی;
        أرجع سچ;
    }

    سانچا<قسم_ناں UInt>
    فراغ extend(ثابت UInt *انکڑے, int32_t min_شروع, int32_t prev_انکڑے_لمائی, int32_t new_انکڑے_لمائی) {
        int32_t شروع = prev_انکڑے_لمائی - بلاک_لمائی;
        إذا (شروع >= min_شروع) {
            ++شروع;  // Skip the last block that we added last time.
        } وإلا {
            شروع = min_شروع;  // Begin with the first full block.
        }
        لكل (int32_t سرا = new_انکڑے_لمائی - بلاک_لمائی; شروع <= سرا; ++شروع) {
            uint32_t hash_کوڈ = make_Hash_کوڈ(انکڑے, شروع);
            جوڑو_اندراج(انکڑے, شروع, hash_کوڈ, شروع);
        }
    }

    سانچا<قسم_ناں UIntA, قسم_ناں UIntB>
    int32_t لبھو_بلاک(ثابت UIntA *انکڑے, ثابت UIntB *بلاک_انکڑے, int32_t بلاک_شروع) ثابت {
        uint32_t hash_کوڈ = make_Hash_کوڈ(بلاک_انکڑے, بلاک_شروع);
        int32_t اندراج_اشاریہ = لبھو_اندراج(انکڑے, بلاک_انکڑے, بلاک_شروع, hash_کوڈ);
        إذا (اندراج_اشاریہ >= 0) {
            أرجع (جدول[اندراج_اشاریہ] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

    int32_t لبھو_All_Same_بلاک(ثابت uint32_t *انکڑے, uint32_t بلاک_قدر) ثابت {
        uint32_t hash_کوڈ = make_Hash_کوڈ(بلاک_قدر);
        int32_t اندراج_اشاریہ = لبھو_اندراج(انکڑے, بلاک_قدر, hash_کوڈ);
        إذا (اندراج_اشاریہ >= 0) {
            أرجع (جدول[اندراج_اشاریہ] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

خاص:
    سانچا<قسم_ناں UInt>
    uint32_t make_Hash_کوڈ(ثابت UInt *بلاک_انکڑے, int32_t بلاک_شروع) ثابت {
        int32_t بلاک_حد_بندی = بلاک_شروع + بلاک_لمائی;
        uint32_t hash_کوڈ = بلاک_انکڑے[بلاک_شروع++];
        افعل {
            hash_کوڈ = 37 * hash_کوڈ + بلاک_انکڑے[بلاک_شروع++];
        } طالما (بلاک_شروع < بلاک_حد_بندی);
        أرجع hash_کوڈ;
    }

    uint32_t make_Hash_کوڈ(uint32_t بلاک_قدر) ثابت {
        uint32_t hash_کوڈ = بلاک_قدر;
        لكل (int32_t i = 1; i < بلاک_لمائی; ++i) {
            hash_کوڈ = 37 * hash_کوڈ + بلاک_قدر;
        }
        أرجع hash_کوڈ;
    }

    سانچا<قسم_ناں UInt>
    فراغ جوڑو_اندراج(ثابت UInt *انکڑے, int32_t بلاک_شروع, uint32_t hash_کوڈ, int32_t انکڑے_اشاریہ) {
        U_ASSERT(0 <= انکڑے_اشاریہ && انکڑے_اشاریہ < (int32_t)mask);
        int32_t اندراج_اشاریہ = لبھو_اندراج(انکڑے, انکڑے, بلاک_شروع, hash_کوڈ);
        إذا (اندراج_اشاریہ < 0) {
            جدول[~اندراج_اشاریہ] = (hash_کوڈ << کھسکاؤ) | (انکڑے_اشاریہ + 1);
        }
    }

    سانچا<قسم_ناں UIntA, قسم_ناں UIntB>
    int32_t لبھو_اندراج(ثابت UIntA *انکڑے, ثابت UIntB *بلاک_انکڑے, int32_t بلاک_شروع,
                      uint32_t hash_کوڈ) ثابت {
        uint32_t shifted_Hash_کوڈ = hash_کوڈ << کھسکاؤ;
        int32_t initial_اندراج_اشاریہ = (hash_کوڈ % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t اندراج_اشاریہ = initial_اندراج_اشاریہ;;) {
            uint32_t اندراج = جدول[اندراج_اشاریہ];
            إذا (اندراج == 0) {
                أرجع ~اندراج_اشاریہ;
            }
            إذا ((اندراج & ~mask) == shifted_Hash_کوڈ) {
                int32_t انکڑے_اشاریہ = (اندراج & mask) - 1;
                إذا (equal_بلاکاں(انکڑے + انکڑے_اشاریہ, بلاک_انکڑے + بلاک_شروع, بلاک_لمائی)) {
                    أرجع اندراج_اشاریہ;
                }
            }
            اندراج_اشاریہ = اگلا_اشاریہ(initial_اندراج_اشاریہ, اندراج_اشاریہ);
        }
    }

    int32_t لبھو_اندراج(ثابت uint32_t *انکڑے, uint32_t بلاک_قدر, uint32_t hash_کوڈ) ثابت {
        uint32_t shifted_Hash_کوڈ = hash_کوڈ << کھسکاؤ;
        int32_t initial_اندراج_اشاریہ = (hash_کوڈ % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t اندراج_اشاریہ = initial_اندراج_اشاریہ;;) {
            uint32_t اندراج = جدول[اندراج_اشاریہ];
            إذا (اندراج == 0) {
                أرجع ~اندراج_اشاریہ;
            }
            إذا ((اندراج & ~mask) == shifted_Hash_کوڈ) {
                int32_t انکڑے_اشاریہ = (اندراج & mask) - 1;
                إذا (all_اقدار_Same_As(انکڑے + انکڑے_اشاریہ, بلاک_لمائی, بلاک_قدر)) {
                    أرجع اندراج_اشاریہ;
                }
            }
            اندراج_اشاریہ = اگلا_اشاریہ(initial_اندراج_اشاریہ, اندراج_اشاریہ);
        }
    }

    مضمن int32_t اگلا_اشاریہ(int32_t initial_اندراج_اشاریہ, int32_t اندراج_اشاریہ) ثابت {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        أرجع (اندراج_اشاریہ + initial_اندراج_اشاریہ) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *جدول = خالی_سوچک;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t کھسکاؤ = 0;
    uint32_t mask = 0;

    int32_t بلاک_لمائی = 0;
};

int32_t Mutable_کوڈ_نقطہ_Trie::compact_سارا_انکڑے_بلاکاں(int32_t fastILimit, All_Same_بلاکاں &all_Same_بلاکاں) {
#جے_تعریف_شدہ UCPTRIE_DEBUG
    منطقي حد_توں_ودھ = خطأ;
#جے_انت

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_انکڑے_سمرتھا = ASCII_حد_بندی;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_انکڑے_سمرتھا += UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
    // Add room for special values (errorValue, highValue) and padding.
    new_انکڑے_سمرتھا += 4;
    int32_t i_حد_بندی = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
    int32_t بلاک_لمائی = UCPTRIE_FAST_انکڑے_بلاک_لمائی;
    int32_t inc = SMALL_انکڑے_بلاکاں_فی_BMP_بلاک;
    لكل (int32_t i = 0; i < i_حد_بندی; i += inc) {
        إذا (i == fastILimit) {
            بلاک_لمائی = UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
            inc = 1;
        }
        uint32_t value = اشاریہ[i];
        إذا (flags[i] == MIXED) {
            // Really mixed?
            ثابت uint32_t *p = انکڑے + value;
            value = *p;
            إذا (all_اقدار_Same_As(p + 1, بلاک_لمائی - 1, value)) {
                flags[i] = ALL_SAME;
                اشاریہ[i] = value;
                // Fall through to ALL_SAME handling.
            } وإلا {
                new_انکڑے_سمرتھا += بلاک_لمائی;
                استمر;
            }
        } وإلا {
            U_ASSERT(flags[i] == ALL_SAME);
            إذا (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                منطقي allSame = سچ;
                int32_t اگلا_i = i + inc;
                لكل (int32_t j = i + 1; j < اگلا_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    إذا (اشاریہ[j] != value) {
                        allSame = خطأ;
                        اكسر;
                    }
                }
                إذا (!allSame) {
                    // Turn it into a MIXED block.
                    إذا (get_انکڑے_بلاک(i) < 0) {
                        أرجع -1;
                    }
                    new_انکڑے_سمرتھا += بلاک_لمائی;
                    استمر;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_بلاکاں.لبھو_Or_جوڑو(i, inc, value);
        إذا (other == All_Same_بلاکاں::حد_توں_ودھ_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#جے_تعریف_شدہ UCPTRIE_DEBUG
            إذا (!حد_توں_ودھ) {
                puts("UCPTrie AllSameBlocks overflow");
                حد_توں_ودھ = سچ;
            }
#جے_انت
            int32_t jInc = SMALL_انکڑے_بلاکاں_فی_BMP_بلاک;
            لكل (int32_t j = 0;; j += jInc) {
                إذا (j == i) {
                    all_Same_بلاکاں.جوڑو(i, inc, value);
                    اكسر;
                }
                إذا (j == fastILimit) {
                    jInc = 1;
                }
                إذا (flags[j] == ALL_SAME && اشاریہ[j] == value) {
                    all_Same_بلاکاں.جوڑو(j, jInc + inc, value);
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
            اشاریہ[i] = other;
        } وإلا {
            // New unique same-value block.
            new_انکڑے_سمرتھا += بلاک_لمائی;
        }
    }
    أرجع new_انکڑے_سمرتھا;
}

#جے_تعریف_شدہ UCPTRIE_DEBUG
#   define DEBUG_DO(اظہاریہ) اظہاریہ
#وإلا
#   define DEBUG_DO(اظہاریہ)
#جے_انت

#جے_تعریف_شدہ UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_قدر(حرف s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (حرف)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (حرف)(0x80 + (value & 0x3F));
    أرجع length + 3;
}

فراغ print_بلاک(ثابت uint32_t *بلاک, int32_t بلاک_لمائی, uint32_t value,
                UChar32 شروع, int32_t overlap, uint32_t initial_قدر) {
    حرف s[UCPTRIE_FAST_انکڑے_بلاک_لمائی * 3 + 3];
    int32_t length = 0;
    int32_t i;
    لكل (i = 0; i < overlap; ++i) {
        length = append_قدر(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    لكل (; i < بلاک_لمائی; ++i) {
        إذا (بلاک != خالی_سوچک) {
            value = بلاک[i];
        }
        إذا (value == initial_قدر) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_قدر(s, length, value);
    }
    s[length] = 0;
    شروع += overlap;
    إذا (شروع <= 0xffff) {
        لکھو("    %04lX  %s|\n", (لما)شروع, s);
    } وإلا إذا (شروع <= 0xfffff) {
        لکھو("   %5lX  %s|\n", (لما)شروع, s);
    } وإلا {
        لکھو("  %6lX  %s|\n", (لما)شروع, s);
    }
}
#جے_انت

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
int32_t Mutable_کوڈ_نقطہ_Trie::compact_انکڑے(
        int32_t fastILimit, uint32_t *new_انکڑے, int32_t new_انکڑے_سمرتھا,
        int32_t انکڑے_Null_اشاریہ, Mixed_بلاکاں &mixed_بلاکاں, UError_کوڈ &error_کوڈ) {
#جے_تعریف_شدہ UCPTRIE_DEBUG
    int32_t گنتی_Same=0, جوڑ_Overlaps=0;
    منطقي print_انکڑے = انکڑے_لمائی == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        انکڑے_لمائی == 50400 /* zh.txt~stroke */;
#جے_انت

    // The linear ASCII data has been copied into newData already.
    int32_t new_انکڑے_لمائی = 0;
    لكل (int32_t i = 0; new_انکڑے_لمائی < ASCII_حد_بندی;
            new_انکڑے_لمائی += UCPTRIE_FAST_انکڑے_بلاک_لمائی, i += SMALL_انکڑے_بلاکاں_فی_BMP_بلاک) {
        اشاریہ[i] = new_انکڑے_لمائی;
#جے_تعریف_شدہ UCPTRIE_DEBUG
        إذا (print_انکڑے) {
            print_بلاک(new_انکڑے + new_انکڑے_لمائی, UCPTRIE_FAST_انکڑے_بلاک_لمائی, 0, new_انکڑے_لمائی, 0, initial_قدر);
        }
#جے_انت
    }

    int32_t بلاک_لمائی = UCPTRIE_FAST_انکڑے_بلاک_لمائی;
    إذا (!mixed_بلاکاں.init(new_انکڑے_سمرتھا, بلاک_لمائی)) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    mixed_بلاکاں.extend(new_انکڑے, 0, 0, new_انکڑے_لمائی);

    int32_t i_حد_بندی = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
    int32_t inc = SMALL_انکڑے_بلاکاں_فی_BMP_بلاک;
    int32_t fast_لمائی = 0;
    لكل (int32_t i = ASCII_I_حد_بندی; i < i_حد_بندی; i += inc) {
        إذا (i == fastILimit) {
            بلاک_لمائی = UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
            inc = 1;
            fast_لمائی = new_انکڑے_لمائی;
            إذا (!mixed_بلاکاں.init(new_انکڑے_سمرتھا, بلاک_لمائی)) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع 0;
            }
            mixed_بلاکاں.extend(new_انکڑے, 0, 0, new_انکڑے_لمائی);
        }
        إذا (flags[i] == ALL_SAME) {
            uint32_t value = اشاریہ[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_بلاکاں.لبھو_All_Same_بلاک(new_انکڑے, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            طالما (n >= 0 && i == انکڑے_Null_اشاریہ && i >= fastILimit && n < fast_لمائی &&
                    is_شروع_دا_Some_Fast_بلاک(n, اشاریہ, fastILimit)) {
                n = لبھو_All_Same_بلاک(new_انکڑے, n + 1, new_انکڑے_لمائی, value, بلاک_لمائی);
            }
            إذا (n >= 0) {
                DEBUG_DO(++گنتی_Same);
                اشاریہ[i] = n;
            } وإلا {
                n = getAllSameOverlap(new_انکڑے, new_انکڑے_لمائی, value, بلاک_لمائی);
                DEBUG_DO(جوڑ_Overlaps += n);
#جے_تعریف_شدہ UCPTRIE_DEBUG
                إذا (print_انکڑے) {
                    print_بلاک(خالی_سوچک, بلاک_لمائی, value, i << UCPTRIE_کھسکاؤ_3, n, initial_قدر);
                }
#جے_انت
                اشاریہ[i] = new_انکڑے_لمائی - n;
                int32_t prev_انکڑے_لمائی = new_انکڑے_لمائی;
                طالما (n < بلاک_لمائی) {
                    new_انکڑے[new_انکڑے_لمائی++] = value;
                    ++n;
                }
                mixed_بلاکاں.extend(new_انکڑے, 0, prev_انکڑے_لمائی, new_انکڑے_لمائی);
            }
        } وإلا إذا (flags[i] == MIXED) {
            ثابت uint32_t *بلاک = انکڑے + اشاریہ[i];
            int32_t n = mixed_بلاکاں.لبھو_بلاک(new_انکڑے, بلاک, 0);
            إذا (n >= 0) {
                DEBUG_DO(++گنتی_Same);
                اشاریہ[i] = n;
            } وإلا {
                n = getOverlap(new_انکڑے, new_انکڑے_لمائی, بلاک, 0, بلاک_لمائی);
                DEBUG_DO(جوڑ_Overlaps += n);
#جے_تعریف_شدہ UCPTRIE_DEBUG
                إذا (print_انکڑے) {
                    print_بلاک(بلاک, بلاک_لمائی, 0, i << UCPTRIE_کھسکاؤ_3, n, initial_قدر);
                }
#جے_انت
                اشاریہ[i] = new_انکڑے_لمائی - n;
                int32_t prev_انکڑے_لمائی = new_انکڑے_لمائی;
                طالما (n < بلاک_لمائی) {
                    new_انکڑے[new_انکڑے_لمائی++] = بلاک[n++];
                }
                mixed_بلاکاں.extend(new_انکڑے, 0, prev_انکڑے_لمائی, new_انکڑے_لمائی);
            }
        } وإلا /* SAME_AS */ {
            uint32_t j = اشاریہ[i];
            اشاریہ[i] = اشاریہ[j];
        }
    }

#جے_تعریف_شدہ UCPTRIE_DEBUG
    /* we saved some space */
    لکھو("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (لما)انکڑے_لمائی, (لما)new_انکڑے_لمائی, (لما)گنتی_Same, (لما)جوڑ_Overlaps);
#جے_انت
    أرجع new_انکڑے_لمائی;
}

int32_t Mutable_کوڈ_نقطہ_Trie::compact_اشاریہ(int32_t fastILimit, Mixed_بلاکاں &mixed_بلاکاں,
                                           UError_کوڈ &error_کوڈ) {
    int32_t fast_اشاریہ_لمائی = fastILimit >> (UCPTRIE_FAST_کھسکاؤ - UCPTRIE_کھسکاؤ_3);
    إذا ((بلند_شروع >> UCPTRIE_FAST_کھسکاؤ) <= fast_اشاریہ_لمائی) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_وتھ = UCPTRIE_NO_INDEX3_NULL_وتھ;
        أرجع fast_اشاریہ_لمائی;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_اشاریہ[UCPTRIE_BMP_اشاریہ_لمائی];  // fastIndexLength
    int32_t i3_پہلا_Null = -1;
    لكل (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = اشاریہ[i];
        fast_اشاریہ[j] = static_cast<uint16_t>(i3);
        إذا (i3 == static_cast<uint32_t>(انکڑے_Null_وتھ)) {
            إذا (i3_پہلا_Null < 0) {
                i3_پہلا_Null = j;
            } وإلا إذا (index3_Null_وتھ < 0 &&
                    (j - i3_پہلا_Null + 1) == UCPTRIE_اشاریہ_3_بلاک_لمائی) {
                index3_Null_وتھ = i3_پہلا_Null;
            }
        } وإلا {
            i3_پہلا_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_اگلا = i + SMALL_انکڑے_بلاکاں_فی_BMP_بلاک;
        طالما (++i < i_اگلا) {
            i3 += UCPTRIE_SMALL_انکڑے_بلاک_لمائی;
            اشاریہ[i] = i3;
        }
    }

    إذا (!mixed_بلاکاں.init(fast_اشاریہ_لمائی, UCPTRIE_اشاریہ_3_بلاک_لمائی)) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    mixed_بلاکاں.extend(fast_اشاریہ, 0, 0, fast_اشاریہ_لمائی);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_سمرتھا = 0;
    i3_پہلا_Null = index3_Null_وتھ;
    منطقي has_Long_I3_بلاکاں = خطأ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_شروع = fastILimit < BMP_I_حد_بندی ? 0 : BMP_I_حد_بندی;
    int32_t i_حد_بندی = بلند_شروع >> UCPTRIE_کھسکاؤ_3;
    لكل (int32_t i = i_شروع; i < i_حد_بندی;) {
        int32_t j = i;
        int32_t j_حد_بندی = i + UCPTRIE_اشاریہ_3_بلاک_لمائی;
        uint32_t oredI3 = 0;
        منطقي isNull = سچ;
        افعل {
            uint32_t i3 = اشاریہ[j];
            oredI3 |= i3;
            إذا (i3 != static_cast<uint32_t>(انکڑے_Null_وتھ)) {
                isNull = خطأ;
            }
        } طالما (++j < j_حد_بندی);
        إذا (isNull) {
            flags[i] = I3_NULL;
            إذا (i3_پہلا_Null < 0) {
                إذا (oredI3 <= 0xffff) {
                    index3_سمرتھا += UCPTRIE_اشاریہ_3_بلاک_لمائی;
                } وإلا {
                    index3_سمرتھا += اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی;
                    has_Long_I3_بلاکاں = سچ;
                }
                i3_پہلا_Null = 0;
            }
        } وإلا {
            إذا (oredI3 <= 0xffff) {
                int32_t n = mixed_بلاکاں.لبھو_بلاک(fast_اشاریہ, اشاریہ, i);
                إذا (n >= 0) {
                    flags[i] = I3_BMP;
                    اشاریہ[i] = n;
                } وإلا {
                    flags[i] = I3_16;
                    index3_سمرتھا += UCPTRIE_اشاریہ_3_بلاک_لمائی;
                }
            } وإلا {
                flags[i] = I3_18;
                index3_سمرتھا += اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی;
                has_Long_I3_بلاکاں = سچ;
            }
        }
        i = j;
    }

    int32_t index2_سمرتھا = (i_حد_بندی - i_شروع) >> UCPTRIE_کھسکاؤ_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_لمائی = (index2_سمرتھا + UCPTRIE_اشاریہ_2_نقاب) >> UCPTRIE_کھسکاؤ_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_سمرتھا = fast_اشاریہ_لمائی + index1_لمائی + index3_سمرتھا + index2_سمرتھا + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_سمرتھا * 2));
    إذا (index16 == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    uprv_memcpy(index16, fast_اشاریہ, fast_اشاریہ_لمائی * 2);

    إذا (!mixed_بلاکاں.init(index16_سمرتھا, UCPTRIE_اشاریہ_3_بلاک_لمائی)) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    Mixed_بلاکاں long_I3_بلاکاں;
    إذا (has_Long_I3_بلاکاں) {
        إذا (!long_I3_بلاکاں.init(index16_سمرتھا, اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی)) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_حد_بندی >> UCPTRIE_کھسکاؤ_2];  // index2Capacity
    int32_t i2_لمائی = 0;
    i3_پہلا_Null = index3_Null_وتھ;
    int32_t index3_شروع = fast_اشاریہ_لمائی + index1_لمائی;
    int32_t اشاریہ_لمائی = index3_شروع;
    لكل (int32_t i = i_شروع; i < i_حد_بندی; i += UCPTRIE_اشاریہ_3_بلاک_لمائی) {
        int32_t i3;
        uint8_t f = flags[i];
        إذا (f == I3_NULL && i3_پہلا_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = انکڑے_Null_وتھ <= 0xffff ? I3_16 : I3_18;
            i3_پہلا_Null = 0;
        }
        إذا (f == I3_NULL) {
            i3 = index3_Null_وتھ;
        } وإلا إذا (f == I3_BMP) {
            i3 = اشاریہ[i];
        } وإلا إذا (f == I3_16) {
            int32_t n = mixed_بلاکاں.لبھو_بلاک(index16, اشاریہ, i);
            إذا (n >= 0) {
                i3 = n;
            } وإلا {
                إذا (اشاریہ_لمائی == index3_شروع) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, اشاریہ_لمائی,
                                   اشاریہ, i, UCPTRIE_اشاریہ_3_بلاک_لمائی);
                }
                i3 = اشاریہ_لمائی - n;
                int32_t prev_اشاریہ_لمائی = اشاریہ_لمائی;
                طالما (n < UCPTRIE_اشاریہ_3_بلاک_لمائی) {
                    index16[اشاریہ_لمائی++] = اشاریہ[i + n++];
                }
                mixed_بلاکاں.extend(index16, index3_شروع, prev_اشاریہ_لمائی, اشاریہ_لمائی);
                إذا (has_Long_I3_بلاکاں) {
                    long_I3_بلاکاں.extend(index16, index3_شروع, prev_اشاریہ_لمائی, اشاریہ_لمائی);
                }
            }
        } وإلا {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_بلاکاں);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_حد_بندی = i + UCPTRIE_اشاریہ_3_بلاک_لمائی;
            int32_t k = اشاریہ_لمائی;
            افعل {
                ++k;
                uint32_t v = اشاریہ[j++];
                uint32_t اتلا_بٹ = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = اشاریہ[j++];
                اتلا_بٹ |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = اتلا_بٹ;
            } طالما (j < j_حد_بندی);
            int32_t n = long_I3_بلاکاں.لبھو_بلاک(index16, index16, اشاریہ_لمائی);
            إذا (n >= 0) {
                i3 = n | 0x8000;
            } وإلا {
                إذا (اشاریہ_لمائی == index3_شروع) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, اشاریہ_لمائی,
                                   index16, اشاریہ_لمائی, اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی);
                }
                i3 = (اشاریہ_لمائی - n) | 0x8000;
                int32_t prev_اشاریہ_لمائی = اشاریہ_لمائی;
                إذا (n > 0) {
                    int32_t شروع = اشاریہ_لمائی;
                    طالما (n < اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی) {
                        index16[اشاریہ_لمائی++] = index16[شروع + n++];
                    }
                } وإلا {
                    اشاریہ_لمائی += اشاریہ_3_18_ثنائی_ہندسہ_بلاک_لمائی;
                }
                mixed_بلاکاں.extend(index16, index3_شروع, prev_اشاریہ_لمائی, اشاریہ_لمائی);
                إذا (has_Long_I3_بلاکاں) {
                    long_I3_بلاکاں.extend(index16, index3_شروع, prev_اشاریہ_لمائی, اشاریہ_لمائی);
                }
            }
        }
        إذا (index3_Null_وتھ < 0 && i3_پہلا_Null >= 0) {
            index3_Null_وتھ = i3;
        }
        // Set the index-2 table entry.
        index2[i2_لمائی++] = i3;
    }
    U_ASSERT(i2_لمائی == index2_سمرتھا);
    U_ASSERT(اشاریہ_لمائی <= index3_شروع + index3_سمرتھا);

    إذا (index3_Null_وتھ < 0) {
        index3_Null_وتھ = UCPTRIE_NO_INDEX3_NULL_وتھ;
    }
    إذا (اشاریہ_لمائی >= (UCPTRIE_NO_INDEX3_NULL_وتھ + UCPTRIE_اشاریہ_3_بلاک_لمائی)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_اشاریہ_2_بلاک_لمائی == UCPTRIE_اشاریہ_3_بلاک_لمائی,
                  "must re-init mixedBlocks");
    int32_t بلاک_لمائی = UCPTRIE_اشاریہ_2_بلاک_لمائی;
    int32_t i1 = fast_اشاریہ_لمائی;
    لكل (int32_t i = 0; i < i2_لمائی; i += بلاک_لمائی) {
        int32_t n;
        إذا ((i2_لمائی - i) >= بلاک_لمائی) {
            // normal block
            U_ASSERT(بلاک_لمائی == UCPTRIE_اشاریہ_2_بلاک_لمائی);
            n = mixed_بلاکاں.لبھو_بلاک(index16, index2, i);
        } وإلا {
            // highStart is inside the last index-2 block. Shorten it.
            بلاک_لمائی = i2_لمائی - i;
            n = لبھو_Same_بلاک(index16, index3_شروع, اشاریہ_لمائی,
                              index2, i, بلاک_لمائی);
        }
        int32_t i2;
        إذا (n >= 0) {
            i2 = n;
        } وإلا {
            إذا (اشاریہ_لمائی == index3_شروع) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } وإلا {
                n = getOverlap(index16, اشاریہ_لمائی, index2, i, بلاک_لمائی);
            }
            i2 = اشاریہ_لمائی - n;
            int32_t prev_اشاریہ_لمائی = اشاریہ_لمائی;
            طالما (n < بلاک_لمائی) {
                index16[اشاریہ_لمائی++] = index2[i + n++];
            }
            mixed_بلاکاں.extend(index16, index3_شروع, prev_اشاریہ_لمائی, اشاریہ_لمائی);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_شروع);
    U_ASSERT(اشاریہ_لمائی <= index16_سمرتھا);

#جے_تعریف_شدہ UCPTRIE_DEBUG
    /* we saved some space */
    لکھو("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (لما)i_حد_بندی, (لما)اشاریہ_لمائی);
#جے_انت

    أرجع اشاریہ_لمائی;
}

int32_t Mutable_کوڈ_نقطہ_Trie::compactTrie(int32_t fastILimit, UError_کوڈ &error_کوڈ) {
    // Find the real highStart and round it up.
    U_ASSERT((بلند_شروع & (UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1)) == 0);
    بلند_قدر = get(MAX_UNICODE);
    int32_t real_بلند_شروع = لبھو_بلند_شروع();
    real_بلند_شروع = (real_بلند_شروع + (UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1)) &
        ~(UCPTRIE_CP_فی_اشاریہ_2_اندراج - 1);
    إذا (real_بلند_شروع == UNICODE_حد_بندی) {
        بلند_قدر = initial_قدر;
    }

#جے_تعریف_شدہ UCPTRIE_DEBUG
    لکھو("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (لما)real_بلند_شروع, (لما)بلند_قدر, (لما)initial_قدر);
#جے_انت

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_حد_بندی = fastILimit << UCPTRIE_کھسکاؤ_3;
    إذا (real_بلند_شروع < fast_حد_بندی) {
        لكل (int32_t i = (real_بلند_شروع >> UCPTRIE_کھسکاؤ_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            اشاریہ[i] = بلند_قدر;
        }
        بلند_شروع = fast_حد_بندی;
    } وإلا {
        بلند_شروع = real_بلند_شروع;
    }

    uint32_t ascii_انکڑے[ASCII_حد_بندی];
    لكل (int32_t i = 0; i < ASCII_حد_بندی; ++i) {
        ascii_انکڑے[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_بلاکاں all_Same_بلاکاں;
    int32_t new_انکڑے_سمرتھا = compact_سارا_انکڑے_بلاکاں(fastILimit, all_Same_بلاکاں);
    إذا (new_انکڑے_سمرتھا < 0) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    uint32_t* new_انکڑے = static_cast<uint32_t*>(uprv_malloc(new_انکڑے_سمرتھا * 4));
    إذا (new_انکڑے == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    uprv_memcpy(new_انکڑے, ascii_انکڑے, حجم(ascii_انکڑے));

    int32_t انکڑے_Null_اشاریہ = all_Same_بلاکاں.لبھو_Most_ورتیا();

    Mixed_بلاکاں mixed_بلاکاں;
    int32_t new_انکڑے_لمائی = compact_انکڑے(fastILimit, new_انکڑے, new_انکڑے_سمرتھا,
                                        انکڑے_Null_اشاریہ, mixed_بلاکاں, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع 0; }
    U_ASSERT(new_انکڑے_لمائی <= new_انکڑے_سمرتھا);
    uprv_free(انکڑے);
    انکڑے = new_انکڑے;
    انکڑے_سمرتھا = new_انکڑے_سمرتھا;
    انکڑے_لمائی = new_انکڑے_لمائی;
    إذا (انکڑے_لمائی > (0x3ffff + UCPTRIE_SMALL_انکڑے_بلاک_لمائی)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا (انکڑے_Null_اشاریہ >= 0) {
        انکڑے_Null_وتھ = اشاریہ[انکڑے_Null_اشاریہ];
#جے_تعریف_شدہ UCPTRIE_DEBUG
        إذا (انکڑے[انکڑے_Null_وتھ] != initial_قدر) {
            لکھو("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (لما)initial_قدر, (لما)انکڑے[انکڑے_Null_وتھ]);
        }
#جے_انت
        initial_قدر = انکڑے[انکڑے_Null_وتھ];
    } وإلا {
        انکڑے_Null_وتھ = UCPTRIE_NO_انکڑے_NULL_وتھ;
    }

    int32_t اشاریہ_لمائی = compact_اشاریہ(fastILimit, mixed_بلاکاں, error_کوڈ);
    بلند_شروع = real_بلند_شروع;
    أرجع اشاریہ_لمائی;
}

UCPTrie *Mutable_کوڈ_نقطہ_Trie::build(UCPTrieType type, UCPTrie_قدر_چوڑائی قدر_چوڑائی, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قدر_چوڑائی < UCPTRIE_قدر_بٹ_16 || UCPTRIE_قدر_بٹ_8 < قدر_چوڑائی) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    بدل (قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_32:
        اكسر;
    حالة UCPTRIE_قدر_بٹ_16:
        نقاب_اقدار(0xffff);
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        نقاب_اقدار(0xff);
        اكسر;
    افتراضي:
        اكسر;
    }

    UChar32 fast_حد_بندی = type == UCPTRIE_TYPE_FAST ? BMP_حد_بندی : UCPTRIE_SMALL_حد_بندی;
    int32_t اشاریہ_لمائی = compactTrie(fast_حد_بندی >> UCPTRIE_کھسکاؤ_3, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) {
        clear();
        أرجع خالی_سوچک;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    إذا (قدر_چوڑائی == UCPTRIE_قدر_بٹ_32 && (اشاریہ_لمائی & 1) != 0) {
        index16[اشاریہ_لمائی++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = اشاریہ_لمائی * 2;
    إذا (قدر_چوڑائی == UCPTRIE_قدر_بٹ_16) {
        إذا (((اشاریہ_لمائی ^ انکڑے_لمائی) & 1) != 0) {
            // padding
            انکڑے[انکڑے_لمائی++] = error_قدر;
        }
        إذا (انکڑے[انکڑے_لمائی - 1] != error_قدر || انکڑے[انکڑے_لمائی - 2] != بلند_قدر) {
            انکڑے[انکڑے_لمائی++] = بلند_قدر;
            انکڑے[انکڑے_لمائی++] = error_قدر;
        }
        length += انکڑے_لمائی * 2;
    } وإلا إذا (قدر_چوڑائی == UCPTRIE_قدر_بٹ_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        إذا (انکڑے[انکڑے_لمائی - 1] != error_قدر || انکڑے[انکڑے_لمائی - 2] != بلند_قدر) {
            إذا (انکڑے[انکڑے_لمائی - 1] != بلند_قدر) {
                انکڑے[انکڑے_لمائی++] = بلند_قدر;
            }
            انکڑے[انکڑے_لمائی++] = error_قدر;
        }
        length += انکڑے_لمائی * 4;
    } وإلا {
        int32_t and3 = (length + انکڑے_لمائی) & 3;
        إذا (and3 == 0 && انکڑے[انکڑے_لمائی - 1] == error_قدر && انکڑے[انکڑے_لمائی - 2] == بلند_قدر) {
            // all set
        } وإلا إذا(and3 == 3 && انکڑے[انکڑے_لمائی - 1] == بلند_قدر) {
            انکڑے[انکڑے_لمائی++] = error_قدر;
        } وإلا {
            طالما (and3 != 2) {
                انکڑے[انکڑے_لمائی++] = بلند_قدر;
                and3 = (and3 + 1) & 3;
            }
            انکڑے[انکڑے_لمائی++] = بلند_قدر;
            انکڑے[انکڑے_لمائی++] = error_قدر;
        }
        length += انکڑے_لمائی;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += حجم(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* بائٹاں = static_cast<uint8_t*>(uprv_malloc(length));
    إذا (بائٹاں == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        clear();
        أرجع خالی_سوچک;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(بائٹاں);
    uprv_memset(trie, 0, حجم(UCPTrie));
    trie->اشاریہ_لمائی = اشاریہ_لمائی;
    trie->انکڑے_لمائی = انکڑے_لمائی;

    trie->بلند_شروع = بلند_شروع;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_بلند_شروع = (بلند_شروع + 0xfff) >> 12;
    trie->type = type;
    trie->قدر_چوڑائی = قدر_چوڑائی;

    trie->index3_Null_وتھ = index3_Null_وتھ;
    trie->انکڑے_Null_وتھ = انکڑے_Null_وتھ;
    trie->null_قدر = initial_قدر;

    بائٹاں += حجم(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(بائٹاں);
    trie->اشاریہ = dest16;

    إذا (بلند_شروع <= fast_حد_بندی) {
        // Condense only the fast index from the mutable-trie index.
        لكل (int32_t i = 0, j = 0; j < اشاریہ_لمائی; i += SMALL_انکڑے_بلاکاں_فی_BMP_بلاک, ++j) {
            *dest16++ = static_cast<uint16_t>(اشاریہ[i]); // dest16[j]
        }
    } وإلا {
        uprv_memcpy(dest16, index16, اشاریہ_لمائی * 2);
        dest16 += اشاریہ_لمائی;
    }
    بائٹاں += اشاریہ_لمائی * 2;

    // Write the data array.
    ثابت uint32_t *p = انکڑے;
    بدل (قدر_چوڑائی) {
    حالة UCPTRIE_قدر_بٹ_16:
        // Write 16-bit data values.
        trie->انکڑے.ptr16 = dest16;
        لكل (int32_t i = انکڑے_لمائی; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        اكسر;
    حالة UCPTRIE_قدر_بٹ_32:
        // Write 32-bit data values.
        trie->انکڑے.ptr32 = reinterpret_cast<uint32_t*>(بائٹاں);
        uprv_memcpy(بائٹاں, p, (size_t)انکڑے_لمائی * 4);
        اكسر;
    حالة UCPTRIE_قدر_بٹ_8:
        // Write 8-bit data values.
        trie->انکڑے.ptr8 = بائٹاں;
        لكل (int32_t i = انکڑے_لمائی; i > 0; --i) {
            *بائٹاں++ = static_cast<uint8_t>(*p++);
        }
        اكسر;
    افتراضي:
        // Will not occur, valueWidth checked at the beginning.
        اكسر;
    }

#جے_تعریف_شدہ UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#جے_انت

    clear();
    أرجع trie;
}

}  // namespace

U_NAMESPACE_سرا

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_قدر, uint32_t error_قدر, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    Local_اشارہ_گر<Mutable_کوڈ_نقطہ_Trie> trie(
        جديد Mutable_کوڈ_نقطہ_Trie(initial_قدر, error_قدر, *p_Error_کوڈ), *p_Error_کوڈ);
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ثابت UMutableCPTrie *other, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا (other == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    Local_اشارہ_گر<Mutable_کوڈ_نقطہ_Trie> clone(
        جديد Mutable_کوڈ_نقطہ_Trie(*reinterpret_cast<ثابت Mutable_کوڈ_نقطہ_Trie *>(other), *p_Error_کوڈ), *p_Error_کوڈ);
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    مٹاؤ reinterpret_cast<Mutable_کوڈ_نقطہ_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ثابت UCPMap *map, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا (map == خالی_سوچک) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_کوڈ_نقطہ_Trie::fromUCPMap(map, *p_Error_کوڈ));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ثابت UCPTrie *trie, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا (trie == خالی_سوچک) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_کوڈ_نقطہ_Trie::fromUCPTrie(trie, *p_Error_کوڈ));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ثابت UMutableCPTrie *trie, UChar32 c) {
    أرجع reinterpret_cast<ثابت Mutable_کوڈ_نقطہ_Trie *>(trie)->get(c);
}

نطاق {

UChar32 getRange(ثابت فراغ *trie, UChar32 شروع,
                 UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع reinterpret_cast<ثابت Mutable_کوڈ_نقطہ_Trie *>(trie)->
        getRange(شروع, filter, context, p_قدر);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ثابت UMutableCPTrie *trie, UChar32 شروع,
                        UCPMapRangeOption option, uint32_t surrogate_قدر,
                        UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع ucptrie_اندرونی_Get_Range(getRange, trie, شروع,
                                    option, surrogate_قدر,
                                    filter, context, p_قدر);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_سیٹ(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    reinterpret_cast<Mutable_کوڈ_نقطہ_Trie *>(trie)->سیٹ(c, value, *p_Error_کوڈ);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_سیٹ_Range(UMutableCPTrie *trie, UChar32 شروع, UChar32 سرا,
                   uint32_t value, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    reinterpret_cast<Mutable_کوڈ_نقطہ_Trie *>(trie)->سیٹ_Range(شروع, سرا, value, *p_Error_کوڈ);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_قدر_چوڑائی قدر_چوڑائی,
                              UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<Mutable_کوڈ_نقطہ_Trie *>(trie)->build(type, قدر_چوڑائی, *p_Error_کوڈ);
}

#جے_تعریف_شدہ UCPTRIE_DEBUG
U_CFUNC فراغ umutablecptrie_سیٹ_Name(UMutableCPTrie *trie, ثابت حرف *name) {
    reinterpret_cast<Mutable_کوڈ_نقطہ_Trie *>(trie)->name = name;
}
#جے_انت
