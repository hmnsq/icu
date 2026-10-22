// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
#   include <stdio.h>
#جيڪڏهن_پورو

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#جيڪڏهن_وصف_ٿيل حد_کان_مٿي_2
#وصف_رد حد_کان_مٿي_2
#جيڪڏهن_پورو

U_NAMESPACE_BEGIN

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_حد_بندي = 0x110000;
constexpr int32_t BMP_حد_بندي = 0x10000;
constexpr int32_t ASCII_حد_بندي = 0x80;

constexpr int32_t I_حد_بندي = UNICODE_حد_بندي >> UCPTRIE_سرڪايو_3;
constexpr int32_t BMP_I_حد_بندي = BMP_حد_بندي >> UCPTRIE_سرڪايو_3;
constexpr int32_t ASCII_I_حد_بندي = ASCII_حد_بندي >> UCPTRIE_سرڪايو_3;

constexpr int32_t SMALL_معلومات_بلاڪون_في_BMP_بلاڪ = (1 << (UCPTRIE_FAST_سرڪايو - UCPTRIE_سرڪايو_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_معلومات_ڊيگهائي = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_معلومات_ڊيگهائي = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_معلومات_ڊيگهائي = UNICODE_حد_بندي;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي = UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي + UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي / 8;

صنف All_Same_بلاڪون;
صنف Mixed_بلاڪون;

صنف Mutable_ڪوڊ_نقطو_Trie : عام UMemory {
عام:
    Mutable_ڪوڊ_نقطو_Trie(uint32_t initial_قدر, uint32_t error_قدر, UError_ڪوڊ &error_ڪوڊ);
    Mutable_ڪوڊ_نقطو_Trie(ثابت Mutable_ڪوڊ_نقطو_Trie &other, UError_ڪوڊ &error_ڪوڊ);
    Mutable_ڪوڊ_نقطو_Trie(ثابت Mutable_ڪوڊ_نقطو_Trie &other) = ميٽيو;
    ~Mutable_ڪوڊ_نقطو_Trie();

    Mutable_ڪوڊ_نقطو_Trie &عامل=(ثابت Mutable_ڪوڊ_نقطو_Trie &other) = ميٽيو;

    ساكن Mutable_ڪوڊ_نقطو_Trie *fromUCPMap(ثابت UCPMap *map, UError_ڪوڊ &error_ڪوڊ);
    ساكن Mutable_ڪوڊ_نقطو_Trie *fromUCPTrie(ثابت UCPTrie *trie, UError_ڪوڊ &error_ڪوڊ);

    uint32_t get(UChar32 c) ثابت;
    int32_t getRange(UChar32 شروعات, UCPMap_قدر_Filter *filter, ثابت فراغ *context,
                     uint32_t *p_قدر) ثابت;

    فراغ سيٽ(UChar32 c, uint32_t value, UError_ڪوڊ &error_ڪوڊ);
    فراغ سيٽ_Range(UChar32 شروعات, UChar32 پڇاڙي, uint32_t value, UError_ڪوڊ &error_ڪوڊ);

    UCPTrie *build(UCPTrieType type, UCPTrie_قدر_ويڪر قدر_ويڪر, UError_ڪوڊ &error_ڪوڊ);

خاص:
    فراغ clear();

    منطقي ensure_مٿاهون_شروعات(UChar32 c);
    int32_t alloc_معلومات_بلاڪ(int32_t بلاڪ_ڊيگهائي);
    int32_t get_معلومات_بلاڪ(int32_t i);

    فراغ نقاب_قدرون(uint32_t mask);
    UChar32 ڳوليو_مٿاهون_شروعات() ثابت;
    int32_t compact_سڄو_معلومات_بلاڪون(int32_t fastILimit, All_Same_بلاڪون &all_Same_بلاڪون);
    int32_t compact_معلومات(
            int32_t fastILimit, uint32_t *new_معلومات, int32_t new_معلومات_صلاحيت,
            int32_t معلومات_Null_اشاريو, Mixed_بلاڪون &mixed_بلاڪون, UError_ڪوڊ &error_ڪوڊ);
    int32_t compact_اشاريو(int32_t fastILimit, Mixed_بلاڪون &mixed_بلاڪون, UError_ڪوڊ &error_ڪوڊ);
    int32_t compactTrie(int32_t fastILimit, UError_ڪوڊ &error_ڪوڊ);

    uint32_t *اشاريو = خالي_اشارو;
    int32_t اشاريو_صلاحيت = 0;
    int32_t index3_Null_وٿي = -1;
    uint32_t *معلومات = خالي_اشارو;
    int32_t معلومات_صلاحيت = 0;
    int32_t معلومات_ڊيگهائي = 0;
    int32_t معلومات_Null_وٿي = -1;

    uint32_t orig_Initial_قدر;
    uint32_t initial_قدر;
    uint32_t error_قدر;
    UChar32 مٿاهون_شروعات;
    uint32_t مٿاهون_قدر;
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
عام:
    ثابت حرف *name;
#جيڪڏهن_پورو
خاص:
    /** Temporary array while building the final data. */
    uint16_t *index16 = خالي_اشارو;
    uint8_t flags[UNICODE_حد_بندي >> UCPTRIE_سرڪايو_3];
};

Mutable_ڪوڊ_نقطو_Trie::Mutable_ڪوڊ_نقطو_Trie(uint32_t ini_قدر, uint32_t err_قدر, UError_ڪوڊ &error_ڪوڊ) :
        orig_Initial_قدر(ini_قدر), initial_قدر(ini_قدر), error_قدر(err_قدر),
        مٿاهون_شروعات(0), مٿاهون_قدر(initial_قدر)
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
        , name("open")
#جيڪڏهن_پورو
        {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    اشاريو = static_cast<uint32_t*>(uprv_malloc(BMP_I_حد_بندي * 4));
    معلومات = static_cast<uint32_t*>(uprv_malloc(INITIAL_معلومات_ڊيگهائي * 4));
    إذا (اشاريو == خالي_اشارو || معلومات == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    اشاريو_صلاحيت = BMP_I_حد_بندي;
    معلومات_صلاحيت = INITIAL_معلومات_ڊيگهائي;
}

Mutable_ڪوڊ_نقطو_Trie::Mutable_ڪوڊ_نقطو_Trie(ثابت Mutable_ڪوڊ_نقطو_Trie &other, UError_ڪوڊ &error_ڪوڊ) :
        index3_Null_وٿي(other.index3_Null_وٿي),
        معلومات_Null_وٿي(other.معلومات_Null_وٿي),
        orig_Initial_قدر(other.orig_Initial_قدر), initial_قدر(other.initial_قدر),
        error_قدر(other.error_قدر),
        مٿاهون_شروعات(other.مٿاهون_شروعات), مٿاهون_قدر(other.مٿاهون_قدر)
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
        , name("mutable clone")
#جيڪڏهن_پورو
        {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    int32_t i_صلاحيت = مٿاهون_شروعات <= BMP_حد_بندي ? BMP_I_حد_بندي : I_حد_بندي;
    اشاريو = static_cast<uint32_t*>(uprv_malloc(i_صلاحيت * 4));
    معلومات = static_cast<uint32_t*>(uprv_malloc(other.معلومات_صلاحيت * 4));
    إذا (اشاريو == خالي_اشارو || معلومات == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    اشاريو_صلاحيت = i_صلاحيت;
    معلومات_صلاحيت = other.معلومات_صلاحيت;

    int32_t i_حد_بندي = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
    uprv_memcpy(flags, other.flags, i_حد_بندي);
    uprv_memcpy(اشاريو, other.اشاريو, i_حد_بندي * 4);
    uprv_memcpy(معلومات, other.معلومات, (size_t)other.معلومات_ڊيگهائي * 4);
    معلومات_ڊيگهائي = other.معلومات_ڊيگهائي;
    U_ASSERT(other.index16 == خالي_اشارو);
}

Mutable_ڪوڊ_نقطو_Trie::~Mutable_ڪوڊ_نقطو_Trie() {
    uprv_free(اشاريو);
    uprv_free(معلومات);
    uprv_free(index16);
}

Mutable_ڪوڊ_نقطو_Trie *Mutable_ڪوڊ_نقطو_Trie::fromUCPMap(ثابت UCPMap *map, UError_ڪوڊ &error_ڪوڊ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_قدر = ucpmap_get(map, -1);
    uint32_t initial_قدر = ucpmap_get(map, 0x10ffff);
    Local_اشارو<Mutable_ڪوڊ_نقطو_Trie> mutableTrie(
        جديد Mutable_ڪوڊ_نقطو_Trie(initial_قدر, error_قدر, error_ڪوڊ),
        error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    UChar32 شروعات = 0, پڇاڙي;
    uint32_t value;
    طالما ((پڇاڙي = ucpmap_getRange(map, شروعات, UCPMAP_RANGE_عام, 0,
                                  خالي_اشارو, خالي_اشارو, &value)) >= 0) {
        إذا (value != initial_قدر) {
            إذا (شروعات == پڇاڙي) {
                mutableTrie->سيٽ(شروعات, value, error_ڪوڊ);
            } وإلا {
                mutableTrie->سيٽ_Range(شروعات, پڇاڙي, value, error_ڪوڊ);
            }
        }
        شروعات = پڇاڙي + 1;
    }
    إذا (U_SUCCESS(error_ڪوڊ)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع خالي_اشارو;
    }
}

Mutable_ڪوڊ_نقطو_Trie *Mutable_ڪوڊ_نقطو_Trie::fromUCPTrie(ثابت UCPTrie *trie, UError_ڪوڊ &error_ڪوڊ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_قدر;
    uint32_t initial_قدر;
    بدل (trie->قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        error_قدر = trie->معلومات.ptr16[trie->معلومات_ڊيگهائي - UCPTRIE_ERROR_قدر_NEG_معلومات_وٿي];
        initial_قدر = trie->معلومات.ptr16[trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي];
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        error_قدر = trie->معلومات.ptr32[trie->معلومات_ڊيگهائي - UCPTRIE_ERROR_قدر_NEG_معلومات_وٿي];
        initial_قدر = trie->معلومات.ptr32[trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي];
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        error_قدر = trie->معلومات.ptr8[trie->معلومات_ڊيگهائي - UCPTRIE_ERROR_قدر_NEG_معلومات_وٿي];
        initial_قدر = trie->معلومات.ptr8[trie->معلومات_ڊيگهائي - UCPTRIE_مٿاهون_قدر_NEG_معلومات_وٿي];
        اكسر;
    افتراضي:
        // Unreachable if the trie is properly initialized.
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    Local_اشارو<Mutable_ڪوڊ_نقطو_Trie> mutableTrie(
        جديد Mutable_ڪوڊ_نقطو_Trie(initial_قدر, error_قدر, error_ڪوڊ),
        error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    UChar32 شروعات = 0, پڇاڙي;
    uint32_t value;
    طالما ((پڇاڙي = ucptrie_getRange(trie, شروعات, UCPMAP_RANGE_عام, 0,
                                   خالي_اشارو, خالي_اشارو, &value)) >= 0) {
        إذا (value != initial_قدر) {
            إذا (شروعات == پڇاڙي) {
                mutableTrie->سيٽ(شروعات, value, error_ڪوڊ);
            } وإلا {
                mutableTrie->سيٽ_Range(شروعات, پڇاڙي, value, error_ڪوڊ);
            }
        }
        شروعات = پڇاڙي + 1;
    }
    إذا (U_SUCCESS(error_ڪوڊ)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع خالي_اشارو;
    }
}

فراغ Mutable_ڪوڊ_نقطو_Trie::clear() {
    index3_Null_وٿي = معلومات_Null_وٿي = -1;
    معلومات_ڊيگهائي = 0;
    مٿاهون_قدر = initial_قدر = orig_Initial_قدر;
    مٿاهون_شروعات = 0;
    uprv_free(index16);
    index16 = خالي_اشارو;
}

uint32_t Mutable_ڪوڊ_نقطو_Trie::get(UChar32 c) ثابت {
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        أرجع error_قدر;
    }
    إذا (c >= مٿاهون_شروعات) {
        أرجع مٿاهون_قدر;
    }
    int32_t i = c >> UCPTRIE_سرڪايو_3;
    إذا (flags[i] == ALL_SAME) {
        أرجع اشاريو[i];
    } وإلا {
        أرجع معلومات[اشاريو[i] + (c & UCPTRIE_SMALL_معلومات_نقاب)];
    }
}

مضمن uint32_t maybe_Filter_قدر(uint32_t value, uint32_t initial_قدر, uint32_t null_قدر,
                                 UCPMap_قدر_Filter *filter, ثابت فراغ *context) {
    إذا (value == initial_قدر) {
        value = null_قدر;
    } وإلا إذا (filter != خالي_اشارو) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 Mutable_ڪوڊ_نقطو_Trie::getRange(
        UChar32 شروعات, UCPMap_قدر_Filter *filter, ثابت فراغ *context,
        uint32_t *p_قدر) ثابت {
    إذا (static_cast<uint32_t>(شروعات) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    إذا (شروعات >= مٿاهون_شروعات) {
        إذا (p_قدر != خالي_اشارو) {
            uint32_t value = مٿاهون_قدر;
            إذا (filter != خالي_اشارو) { value = filter(context, value); }
            *p_قدر = value;
        }
        أرجع MAX_UNICODE;
    }
    uint32_t null_قدر = initial_قدر;
    إذا (filter != خالي_اشارو) { null_قدر = filter(context, null_قدر); }
    UChar32 c = شروعات;
    uint32_t trie_قدر, value;
    منطقي have_قدر = خطأ;
    int32_t i = c >> UCPTRIE_سرڪايو_3;
    افعل {
        إذا (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = اشاريو[i];
            إذا (have_قدر) {
                إذا (trieValue2 != trie_قدر) {
                    إذا (filter == خالي_اشارو ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_قدر = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_قدر = trieValue2;
                value = maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر, filter, context);
                إذا (p_قدر != خالي_اشارو) { *p_قدر = value; }
                have_قدر = سچ;
            }
            c = (c + UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي) & ~UCPTRIE_SMALL_معلومات_نقاب;
        } وإلا /* MIXED */ {
            int32_t di = اشاريو[i] + (c & UCPTRIE_SMALL_معلومات_نقاب);
            uint32_t trieValue2 = معلومات[di];
            إذا (have_قدر) {
                إذا (trieValue2 != trie_قدر) {
                    إذا (filter == خالي_اشارو ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_قدر = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_قدر = trieValue2;
                value = maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر, filter, context);
                إذا (p_قدر != خالي_اشارو) { *p_قدر = value; }
                have_قدر = سچ;
            }
            طالما ((++c & UCPTRIE_SMALL_معلومات_نقاب) != 0) {
                trieValue2 = معلومات[++di];
                إذا (trieValue2 != trie_قدر) {
                    إذا (filter == خالي_اشارو ||
                            maybe_Filter_قدر(trieValue2, initial_قدر, null_قدر,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                }
                trie_قدر = trieValue2;  // may or may not help
            }
        }
        ++i;
    } طالما (c < مٿاهون_شروعات);
    U_ASSERT(have_قدر);
    إذا (maybe_Filter_قدر(مٿاهون_قدر, initial_قدر, null_قدر,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

فراغ
write_بلاڪ(uint32_t *بلاڪ, uint32_t value) {
    uint32_t *حد_بندي = بلاڪ + UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
    طالما (بلاڪ < حد_بندي) {
        *بلاڪ++ = value;
    }
}

منطقي Mutable_ڪوڊ_نقطو_Trie::ensure_مٿاهون_شروعات(UChar32 c) {
    إذا (c >= مٿاهون_شروعات) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_في_اشاريو_2_داخلا) & ~(UCPTRIE_CP_في_اشاريو_2_داخلا - 1);
        int32_t i = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
        int32_t i_حد_بندي = c >> UCPTRIE_سرڪايو_3;
        إذا (i_حد_بندي > اشاريو_صلاحيت) {
            uint32_t* new_اشاريو = static_cast<uint32_t*>(uprv_malloc(I_حد_بندي * 4));
            إذا (new_اشاريو == خالي_اشارو) { أرجع خطأ; }
            uprv_memcpy(new_اشاريو, اشاريو, i * 4);
            uprv_free(اشاريو);
            اشاريو = new_اشاريو;
            اشاريو_صلاحيت = I_حد_بندي;
        }
        افعل {
            flags[i] = ALL_SAME;
            اشاريو[i] = initial_قدر;
        } طالما(++i < i_حد_بندي);
        مٿاهون_شروعات = c;
    }
    أرجع سچ;
}

int32_t Mutable_ڪوڊ_نقطو_Trie::alloc_معلومات_بلاڪ(int32_t بلاڪ_ڊيگهائي) {
    int32_t new_بلاڪ = معلومات_ڊيگهائي;
    int32_t new_چوٽي = new_بلاڪ + بلاڪ_ڊيگهائي;
    إذا (new_چوٽي > معلومات_صلاحيت) {
        int32_t capacity;
        إذا (معلومات_صلاحيت < MEDIUM_معلومات_ڊيگهائي) {
            capacity = MEDIUM_معلومات_ڊيگهائي;
        } وإلا إذا (معلومات_صلاحيت < MAX_معلومات_ڊيگهائي) {
            capacity = MAX_معلومات_ڊيگهائي;
        } وإلا {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            أرجع -1;
        }
        uint32_t* new_معلومات = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        إذا (new_معلومات == خالي_اشارو) {
            أرجع -1;
        }
        uprv_memcpy(new_معلومات, معلومات, (size_t)معلومات_ڊيگهائي * 4);
        uprv_free(معلومات);
        معلومات = new_معلومات;
        معلومات_صلاحيت = capacity;
    }
    معلومات_ڊيگهائي = new_چوٽي;
    أرجع new_بلاڪ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_ڪوڊ_نقطو_Trie::get_معلومات_بلاڪ(int32_t i) {
    إذا (flags[i] == MIXED) {
        أرجع اشاريو[i];
    }
    إذا (i < BMP_I_حد_بندي) {
        int32_t new_بلاڪ = alloc_معلومات_بلاڪ(UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي);
        إذا (new_بلاڪ < 0) { أرجع new_بلاڪ; }
        int32_t i_شروعات = i & ~(SMALL_معلومات_بلاڪون_في_BMP_بلاڪ -1);
        int32_t i_حد_بندي = i_شروعات + SMALL_معلومات_بلاڪون_في_BMP_بلاڪ;
        افعل {
            U_ASSERT(flags[i_شروعات] == ALL_SAME);
            write_بلاڪ(معلومات + new_بلاڪ, اشاريو[i_شروعات]);
            flags[i_شروعات] = MIXED;
            اشاريو[i_شروعات++] = new_بلاڪ;
            new_بلاڪ += UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
        } طالما (i_شروعات < i_حد_بندي);
        أرجع اشاريو[i];
    } وإلا {
        int32_t new_بلاڪ = alloc_معلومات_بلاڪ(UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي);
        إذا (new_بلاڪ < 0) { أرجع new_بلاڪ; }
        write_بلاڪ(معلومات + new_بلاڪ, اشاريو[i]);
        flags[i] = MIXED;
        اشاريو[i] = new_بلاڪ;
        أرجع new_بلاڪ;
    }
}

فراغ Mutable_ڪوڊ_نقطو_Trie::سيٽ(UChar32 c, uint32_t value, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    int32_t بلاڪ;
    إذا (!ensure_مٿاهون_شروعات(c) || (بلاڪ = get_معلومات_بلاڪ(c >> UCPTRIE_سرڪايو_3)) < 0) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    معلومات[بلاڪ + (c & UCPTRIE_SMALL_معلومات_نقاب)] = value;
}

فراغ
fill_بلاڪ(uint32_t *بلاڪ, UChar32 شروعات, UChar32 حد_بندي, uint32_t value) {
    uint32_t *p_حد_بندي = بلاڪ + حد_بندي;
    بلاڪ += شروعات;
    طالما (بلاڪ < p_حد_بندي) {
        *بلاڪ++ = value;
    }
}

فراغ Mutable_ڪوڊ_نقطو_Trie::سيٽ_Range(UChar32 شروعات, UChar32 پڇاڙي, uint32_t value, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(شروعات) > MAX_UNICODE || static_cast<uint32_t>(پڇاڙي) > MAX_UNICODE || شروعات > پڇاڙي) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا (!ensure_مٿاهون_شروعات(پڇاڙي)) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    UChar32 حد_بندي = پڇاڙي + 1;
    إذا (شروعات & UCPTRIE_SMALL_معلومات_نقاب) {
        // Set partial block at [start..following block boundary[.
        int32_t بلاڪ = get_معلومات_بلاڪ(شروعات >> UCPTRIE_سرڪايو_3);
        إذا (بلاڪ < 0) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }

        UChar32 ايندڙ_شروعات = (شروعات + UCPTRIE_SMALL_معلومات_نقاب) & ~UCPTRIE_SMALL_معلومات_نقاب;
        إذا (ايندڙ_شروعات <= حد_بندي) {
            fill_بلاڪ(معلومات + بلاڪ, شروعات & UCPTRIE_SMALL_معلومات_نقاب, UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي,
                      value);
            شروعات = ايندڙ_شروعات;
        } وإلا {
            fill_بلاڪ(معلومات + بلاڪ, شروعات & UCPTRIE_SMALL_معلومات_نقاب, حد_بندي & UCPTRIE_SMALL_معلومات_نقاب,
                      value);
            أرجع;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = حد_بندي & UCPTRIE_SMALL_معلومات_نقاب;

    // Round down limit to a block boundary.
    حد_بندي &= ~UCPTRIE_SMALL_معلومات_نقاب;

    // Iterate over all-value blocks.
    طالما (شروعات < حد_بندي) {
        int32_t i = شروعات >> UCPTRIE_سرڪايو_3;
        إذا (flags[i] == ALL_SAME) {
            اشاريو[i] = value;
        } وإلا /* MIXED */ {
            fill_بلاڪ(معلومات + اشاريو[i], 0, UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي, value);
        }
        شروعات += UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
    }

    إذا (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t بلاڪ = get_معلومات_بلاڪ(شروعات >> UCPTRIE_سرڪايو_3);
        إذا (بلاڪ < 0) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }

        fill_بلاڪ(معلومات + بلاڪ, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

فراغ Mutable_ڪوڊ_نقطو_Trie::نقاب_قدرون(uint32_t mask) {
    initial_قدر &= mask;
    error_قدر &= mask;
    مٿاهون_قدر &= mask;
    int32_t i_حد_بندي = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
    لكل (int32_t i = 0; i < i_حد_بندي; ++i) {
        إذا (flags[i] == ALL_SAME) {
            اشاريو[i] &= mask;
        }
    }
    لكل (int32_t i = 0; i < معلومات_ڊيگهائي; ++i) {
        معلومات[i] &= mask;
    }
}

سانچو<قسم_نالو UIntA, قسم_نالو UIntB>
منطقي equal_بلاڪون(ثابت UIntA *s, ثابت UIntB *t, int32_t length) {
    طالما (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

منطقي all_قدرون_Same_As(ثابت uint32_t *p, int32_t length, uint32_t value) {
    ثابت uint32_t *p_حد_بندي = p + length;
    طالما (p < p_حد_بندي && *p == value) { ++p; }
    أرجع p == p_حد_بندي;
}

/** Search for an identical block. */
int32_t ڳوليو_Same_بلاڪ(ثابت uint16_t *p, int32_t p_شروعات, int32_t length,
                      ثابت uint16_t *q, int32_t q_شروعات, int32_t بلاڪ_ڊيگهائي) {
    // Ensure that we do not even partially get past length.
    length -= بلاڪ_ڊيگهائي;

    q += q_شروعات;
    طالما (p_شروعات <= length) {
        إذا (equal_بلاڪون(p + p_شروعات, q, بلاڪ_ڊيگهائي)) {
            أرجع p_شروعات;
        }
        ++p_شروعات;
    }
    أرجع -1;
}

int32_t ڳوليو_All_Same_بلاڪ(ثابت uint32_t *p, int32_t شروعات, int32_t حد_بندي,
                         uint32_t value, int32_t بلاڪ_ڊيگهائي) {
    // Ensure that we do not even partially get past limit.
    حد_بندي -= بلاڪ_ڊيگهائي;

    لكل (int32_t بلاڪ = شروعات; بلاڪ <= حد_بندي; ++بلاڪ) {
        إذا (p[بلاڪ] == value) {
            لكل (int32_t i = 1;; ++i) {
                إذا (i == بلاڪ_ڊيگهائي) {
                    أرجع بلاڪ;
                }
                إذا (p[بلاڪ + i] != value) {
                    بلاڪ += i;
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
سانچو<قسم_نالو UIntA, قسم_نالو UIntB>
int32_t getOverlap(ثابت UIntA *p, int32_t length,
                   ثابت UIntB *q, int32_t q_شروعات, int32_t بلاڪ_ڊيگهائي) {
    int32_t overlap = بلاڪ_ڊيگهائي - 1;
    U_ASSERT(overlap <= length);
    q += q_شروعات;
    طالما (overlap > 0 && !equal_بلاڪون(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    أرجع overlap;
}

int32_t getAllSameOverlap(ثابت uint32_t *p, int32_t length, uint32_t value,
                          int32_t بلاڪ_ڊيگهائي) {
    int32_t min = length - (بلاڪ_ڊيگهائي - 1);
    int32_t i = length;
    طالما (min < i && p[i - 1] == value) { --i; }
    أرجع length - i;
}

منطقي is_شروعات_جو_Some_Fast_بلاڪ(uint32_t معلومات_وٿي, ثابت uint32_t اشاريو[], int32_t fastILimit) {
    لكل (int32_t i = 0; i < fastILimit; i += SMALL_معلومات_بلاڪون_في_BMP_بلاڪ) {
        إذا (اشاريو[i] == معلومات_وٿي) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_ڪوڊ_نقطو_Trie::ڳوليو_مٿاهون_شروعات() ثابت {
    int32_t i = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
    طالما (i > 0) {
        منطقي ميلاپ;
        إذا (flags[--i] == ALL_SAME) {
            ميلاپ = اشاريو[i] == مٿاهون_قدر;
        } وإلا /* MIXED */ {
            ثابت uint32_t *p = معلومات + اشاريو[i];
            لكل (int32_t j = 0;; ++j) {
                إذا (j == UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي) {
                    ميلاپ = سچ;
                    اكسر;
                }
                إذا (p[j] != مٿاهون_قدر) {
                    ميلاپ = خطأ;
                    اكسر;
                }
            }
        }
        إذا (!ميلاپ) {
            أرجع (i + 1) << UCPTRIE_سرڪايو_3;
        }
    }
    أرجع 0;
}

صنف All_Same_بلاڪون {
عام:
    ساكن constexpr int32_t NEW_UNIQUE = -1;
    ساكن constexpr int32_t حد_کان_مٿي_2 = -2;

    All_Same_بلاڪون() : length(0), mostRecent(-1) {}

    int32_t ڳوليو_Or_جوڙيو(int32_t اشاريو, int32_t count, uint32_t value) {
        إذا (mostRecent >= 0 && قدرون[mostRecent] == value) {
            refCounts[mostRecent] += count;
            أرجع indexes[mostRecent];
        }
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (قدرون[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                أرجع indexes[i];
            }
        }
        إذا (length == صلاحيت_3) {
            أرجع حد_کان_مٿي_2;
        }
        mostRecent = length;
        indexes[length] = اشاريو;
        قدرون[length] = value;
        refCounts[length++] = count;
        أرجع NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    فراغ جوڙيو(int32_t اشاريو, int32_t count, uint32_t value) {
        U_ASSERT(length == صلاحيت_3);
        int32_t least = -1;
        int32_t گهٽ_کان_گهٽ_ڳڻپ = I_حد_بندي;
        لكل (int32_t i = 0; i < length; ++i) {
            U_ASSERT(قدرون[i] != value);
            إذا (refCounts[i] < گهٽ_کان_گهٽ_ڳڻپ) {
                least = i;
                گهٽ_کان_گهٽ_ڳڻپ = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = اشاريو;
        قدرون[least] = value;
        refCounts[least] = count;
    }

    int32_t ڳوليو_Most_استعمال_ٿيل() ثابت {
        إذا (length == 0) { أرجع -1; }
        int32_t max = -1;
        int32_t max_ڳڻپ = 0;
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (refCounts[i] > max_ڳڻپ) {
                max = i;
                max_ڳڻپ = refCounts[i];
            }
        }
        أرجع indexes[max];
    }

خاص:
    ساكن constexpr int32_t صلاحيت_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[صلاحيت_3];
    uint32_t قدرون[صلاحيت_3];
    int32_t refCounts[صلاحيت_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
صنف Mixed_بلاڪون {
عام:
    Mixed_بلاڪون() {}
    ~Mixed_بلاڪون() {
        uprv_free(جدول);
    }

    منطقي init(int32_t max_ڊيگهائي_2, int32_t new_بلاڪ_ڊيگهائي) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_معلومات_اشاريو = max_ڊيگهائي_2 - new_بلاڪ_ڊيگهائي + 1;
        int32_t new_ڊيگهائي;
        إذا (max_معلومات_اشاريو <= 0xfff) {  // 4k
            new_ڊيگهائي = 6007;
            سرڪايو = 12;
            mask = 0xfff;
        } وإلا إذا (max_معلومات_اشاريو <= 0x7fff) {  // 32k
            new_ڊيگهائي = 50021;
            سرڪايو = 15;
            mask = 0x7fff;
        } وإلا إذا (max_معلومات_اشاريو <= 0x1ffff) {  // 128k
            new_ڊيگهائي = 200003;
            سرڪايو = 17;
            mask = 0x1ffff;
        } وإلا {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_ڊيگهائي = 1500007;
            سرڪايو = 21;
            mask = 0x1fffff;
        }
        إذا (new_ڊيگهائي > capacity) {
            uprv_free(جدول);
            جدول = static_cast<uint32_t*>(uprv_malloc(new_ڊيگهائي * 4));
            إذا (جدول == خالي_اشارو) {
                أرجع خطأ;
            }
            capacity = new_ڊيگهائي;
        }
        length = new_ڊيگهائي;
        uprv_memset(جدول, 0, length * 4);

        بلاڪ_ڊيگهائي = new_بلاڪ_ڊيگهائي;
        أرجع سچ;
    }

    سانچو<قسم_نالو UInt>
    فراغ extend(ثابت UInt *معلومات, int32_t min_شروعات, int32_t prev_معلومات_ڊيگهائي, int32_t new_معلومات_ڊيگهائي) {
        int32_t شروعات = prev_معلومات_ڊيگهائي - بلاڪ_ڊيگهائي;
        إذا (شروعات >= min_شروعات) {
            ++شروعات;  // Skip the last block that we added last time.
        } وإلا {
            شروعات = min_شروعات;  // Begin with the first full block.
        }
        لكل (int32_t پڇاڙي = new_معلومات_ڊيگهائي - بلاڪ_ڊيگهائي; شروعات <= پڇاڙي; ++شروعات) {
            uint32_t hash_ڪوڊ = make_Hash_ڪوڊ(معلومات, شروعات);
            جوڙيو_داخلا(معلومات, شروعات, hash_ڪوڊ, شروعات);
        }
    }

    سانچو<قسم_نالو UIntA, قسم_نالو UIntB>
    int32_t ڳوليو_بلاڪ(ثابت UIntA *معلومات, ثابت UIntB *بلاڪ_معلومات, int32_t بلاڪ_شروعات) ثابت {
        uint32_t hash_ڪوڊ = make_Hash_ڪوڊ(بلاڪ_معلومات, بلاڪ_شروعات);
        int32_t داخلا_اشاريو = ڳوليو_داخلا(معلومات, بلاڪ_معلومات, بلاڪ_شروعات, hash_ڪوڊ);
        إذا (داخلا_اشاريو >= 0) {
            أرجع (جدول[داخلا_اشاريو] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

    int32_t ڳوليو_All_Same_بلاڪ(ثابت uint32_t *معلومات, uint32_t بلاڪ_قدر) ثابت {
        uint32_t hash_ڪوڊ = make_Hash_ڪوڊ(بلاڪ_قدر);
        int32_t داخلا_اشاريو = ڳوليو_داخلا(معلومات, بلاڪ_قدر, hash_ڪوڊ);
        إذا (داخلا_اشاريو >= 0) {
            أرجع (جدول[داخلا_اشاريو] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

خاص:
    سانچو<قسم_نالو UInt>
    uint32_t make_Hash_ڪوڊ(ثابت UInt *بلاڪ_معلومات, int32_t بلاڪ_شروعات) ثابت {
        int32_t بلاڪ_حد_بندي = بلاڪ_شروعات + بلاڪ_ڊيگهائي;
        uint32_t hash_ڪوڊ = بلاڪ_معلومات[بلاڪ_شروعات++];
        افعل {
            hash_ڪوڊ = 37 * hash_ڪوڊ + بلاڪ_معلومات[بلاڪ_شروعات++];
        } طالما (بلاڪ_شروعات < بلاڪ_حد_بندي);
        أرجع hash_ڪوڊ;
    }

    uint32_t make_Hash_ڪوڊ(uint32_t بلاڪ_قدر) ثابت {
        uint32_t hash_ڪوڊ = بلاڪ_قدر;
        لكل (int32_t i = 1; i < بلاڪ_ڊيگهائي; ++i) {
            hash_ڪوڊ = 37 * hash_ڪوڊ + بلاڪ_قدر;
        }
        أرجع hash_ڪوڊ;
    }

    سانچو<قسم_نالو UInt>
    فراغ جوڙيو_داخلا(ثابت UInt *معلومات, int32_t بلاڪ_شروعات, uint32_t hash_ڪوڊ, int32_t معلومات_اشاريو) {
        U_ASSERT(0 <= معلومات_اشاريو && معلومات_اشاريو < (int32_t)mask);
        int32_t داخلا_اشاريو = ڳوليو_داخلا(معلومات, معلومات, بلاڪ_شروعات, hash_ڪوڊ);
        إذا (داخلا_اشاريو < 0) {
            جدول[~داخلا_اشاريو] = (hash_ڪوڊ << سرڪايو) | (معلومات_اشاريو + 1);
        }
    }

    سانچو<قسم_نالو UIntA, قسم_نالو UIntB>
    int32_t ڳوليو_داخلا(ثابت UIntA *معلومات, ثابت UIntB *بلاڪ_معلومات, int32_t بلاڪ_شروعات,
                      uint32_t hash_ڪوڊ) ثابت {
        uint32_t shifted_Hash_ڪوڊ = hash_ڪوڊ << سرڪايو;
        int32_t initial_داخلا_اشاريو = (hash_ڪوڊ % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t داخلا_اشاريو = initial_داخلا_اشاريو;;) {
            uint32_t داخلا_2 = جدول[داخلا_اشاريو];
            إذا (داخلا_2 == 0) {
                أرجع ~داخلا_اشاريو;
            }
            إذا ((داخلا_2 & ~mask) == shifted_Hash_ڪوڊ) {
                int32_t معلومات_اشاريو = (داخلا_2 & mask) - 1;
                إذا (equal_بلاڪون(معلومات + معلومات_اشاريو, بلاڪ_معلومات + بلاڪ_شروعات, بلاڪ_ڊيگهائي)) {
                    أرجع داخلا_اشاريو;
                }
            }
            داخلا_اشاريو = ايندڙ_اشاريو(initial_داخلا_اشاريو, داخلا_اشاريو);
        }
    }

    int32_t ڳوليو_داخلا(ثابت uint32_t *معلومات, uint32_t بلاڪ_قدر, uint32_t hash_ڪوڊ) ثابت {
        uint32_t shifted_Hash_ڪوڊ = hash_ڪوڊ << سرڪايو;
        int32_t initial_داخلا_اشاريو = (hash_ڪوڊ % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t داخلا_اشاريو = initial_داخلا_اشاريو;;) {
            uint32_t داخلا_2 = جدول[داخلا_اشاريو];
            إذا (داخلا_2 == 0) {
                أرجع ~داخلا_اشاريو;
            }
            إذا ((داخلا_2 & ~mask) == shifted_Hash_ڪوڊ) {
                int32_t معلومات_اشاريو = (داخلا_2 & mask) - 1;
                إذا (all_قدرون_Same_As(معلومات + معلومات_اشاريو, بلاڪ_ڊيگهائي, بلاڪ_قدر)) {
                    أرجع داخلا_اشاريو;
                }
            }
            داخلا_اشاريو = ايندڙ_اشاريو(initial_داخلا_اشاريو, داخلا_اشاريو);
        }
    }

    مضمن int32_t ايندڙ_اشاريو(int32_t initial_داخلا_اشاريو, int32_t داخلا_اشاريو) ثابت {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        أرجع (داخلا_اشاريو + initial_داخلا_اشاريو) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *جدول = خالي_اشارو;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t سرڪايو = 0;
    uint32_t mask = 0;

    int32_t بلاڪ_ڊيگهائي = 0;
};

int32_t Mutable_ڪوڊ_نقطو_Trie::compact_سڄو_معلومات_بلاڪون(int32_t fastILimit, All_Same_بلاڪون &all_Same_بلاڪون) {
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    منطقي حد_کان_مٿي = خطأ;
#جيڪڏهن_پورو

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_معلومات_صلاحيت = ASCII_حد_بندي;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_معلومات_صلاحيت += UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
    // Add room for special values (errorValue, highValue) and padding.
    new_معلومات_صلاحيت += 4;
    int32_t i_حد_بندي = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
    int32_t بلاڪ_ڊيگهائي = UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي;
    int32_t inc = SMALL_معلومات_بلاڪون_في_BMP_بلاڪ;
    لكل (int32_t i = 0; i < i_حد_بندي; i += inc) {
        إذا (i == fastILimit) {
            بلاڪ_ڊيگهائي = UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
            inc = 1;
        }
        uint32_t value = اشاريو[i];
        إذا (flags[i] == MIXED) {
            // Really mixed?
            ثابت uint32_t *p = معلومات + value;
            value = *p;
            إذا (all_قدرون_Same_As(p + 1, بلاڪ_ڊيگهائي - 1, value)) {
                flags[i] = ALL_SAME;
                اشاريو[i] = value;
                // Fall through to ALL_SAME handling.
            } وإلا {
                new_معلومات_صلاحيت += بلاڪ_ڊيگهائي;
                استمر;
            }
        } وإلا {
            U_ASSERT(flags[i] == ALL_SAME);
            إذا (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                منطقي allSame = سچ;
                int32_t ايندڙ_i = i + inc;
                لكل (int32_t j = i + 1; j < ايندڙ_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    إذا (اشاريو[j] != value) {
                        allSame = خطأ;
                        اكسر;
                    }
                }
                إذا (!allSame) {
                    // Turn it into a MIXED block.
                    إذا (get_معلومات_بلاڪ(i) < 0) {
                        أرجع -1;
                    }
                    new_معلومات_صلاحيت += بلاڪ_ڊيگهائي;
                    استمر;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_بلاڪون.ڳوليو_Or_جوڙيو(i, inc, value);
        إذا (other == All_Same_بلاڪون::حد_کان_مٿي_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
            إذا (!حد_کان_مٿي) {
                puts("UCPTrie AllSameBlocks overflow");
                حد_کان_مٿي = سچ;
            }
#جيڪڏهن_پورو
            int32_t jInc = SMALL_معلومات_بلاڪون_في_BMP_بلاڪ;
            لكل (int32_t j = 0;; j += jInc) {
                إذا (j == i) {
                    all_Same_بلاڪون.جوڙيو(i, inc, value);
                    اكسر;
                }
                إذا (j == fastILimit) {
                    jInc = 1;
                }
                إذا (flags[j] == ALL_SAME && اشاريو[j] == value) {
                    all_Same_بلاڪون.جوڙيو(j, jInc + inc, value);
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
            اشاريو[i] = other;
        } وإلا {
            // New unique same-value block.
            new_معلومات_صلاحيت += بلاڪ_ڊيگهائي;
        }
    }
    أرجع new_معلومات_صلاحيت;
}

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
#   define DEBUG_DO(اظهار) اظهار
#وإلا
#   define DEBUG_DO(اظهار)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_قدر(حرف s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (حرف)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (حرف)(0x80 + (value & 0x3F));
    أرجع length + 3;
}

فراغ print_بلاڪ(ثابت uint32_t *بلاڪ, int32_t بلاڪ_ڊيگهائي, uint32_t value,
                UChar32 شروعات, int32_t overlap, uint32_t initial_قدر) {
    حرف s[UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي * 3 + 3];
    int32_t length = 0;
    int32_t i;
    لكل (i = 0; i < overlap; ++i) {
        length = append_قدر(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    لكل (; i < بلاڪ_ڊيگهائي; ++i) {
        إذا (بلاڪ != خالي_اشارو) {
            value = بلاڪ[i];
        }
        إذا (value == initial_قدر) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_قدر(s, length, value);
    }
    s[length] = 0;
    شروعات += overlap;
    إذا (شروعات <= 0xffff) {
        لکو("    %04lX  %s|\n", (ڊگهو)شروعات, s);
    } وإلا إذا (شروعات <= 0xfffff) {
        لکو("   %5lX  %s|\n", (ڊگهو)شروعات, s);
    } وإلا {
        لکو("  %6lX  %s|\n", (ڊگهو)شروعات, s);
    }
}
#جيڪڏهن_پورو

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
int32_t Mutable_ڪوڊ_نقطو_Trie::compact_معلومات(
        int32_t fastILimit, uint32_t *new_معلومات, int32_t new_معلومات_صلاحيت,
        int32_t معلومات_Null_اشاريو, Mixed_بلاڪون &mixed_بلاڪون, UError_ڪوڊ &error_ڪوڊ) {
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    int32_t ڳڻپ_Same=0, جوڙ_Overlaps=0;
    منطقي print_معلومات = معلومات_ڊيگهائي == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        معلومات_ڊيگهائي == 50400 /* zh.txt~stroke */;
#جيڪڏهن_پورو

    // The linear ASCII data has been copied into newData already.
    int32_t new_معلومات_ڊيگهائي = 0;
    لكل (int32_t i = 0; new_معلومات_ڊيگهائي < ASCII_حد_بندي;
            new_معلومات_ڊيگهائي += UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي, i += SMALL_معلومات_بلاڪون_في_BMP_بلاڪ) {
        اشاريو[i] = new_معلومات_ڊيگهائي;
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
        إذا (print_معلومات) {
            print_بلاڪ(new_معلومات + new_معلومات_ڊيگهائي, UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي, 0, new_معلومات_ڊيگهائي, 0, initial_قدر);
        }
#جيڪڏهن_پورو
    }

    int32_t بلاڪ_ڊيگهائي = UCPTRIE_FAST_معلومات_بلاڪ_ڊيگهائي;
    إذا (!mixed_بلاڪون.init(new_معلومات_صلاحيت, بلاڪ_ڊيگهائي)) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    mixed_بلاڪون.extend(new_معلومات, 0, 0, new_معلومات_ڊيگهائي);

    int32_t i_حد_بندي = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
    int32_t inc = SMALL_معلومات_بلاڪون_في_BMP_بلاڪ;
    int32_t fast_ڊيگهائي = 0;
    لكل (int32_t i = ASCII_I_حد_بندي; i < i_حد_بندي; i += inc) {
        إذا (i == fastILimit) {
            بلاڪ_ڊيگهائي = UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
            inc = 1;
            fast_ڊيگهائي = new_معلومات_ڊيگهائي;
            إذا (!mixed_بلاڪون.init(new_معلومات_صلاحيت, بلاڪ_ڊيگهائي)) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع 0;
            }
            mixed_بلاڪون.extend(new_معلومات, 0, 0, new_معلومات_ڊيگهائي);
        }
        إذا (flags[i] == ALL_SAME) {
            uint32_t value = اشاريو[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_بلاڪون.ڳوليو_All_Same_بلاڪ(new_معلومات, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            طالما (n >= 0 && i == معلومات_Null_اشاريو && i >= fastILimit && n < fast_ڊيگهائي &&
                    is_شروعات_جو_Some_Fast_بلاڪ(n, اشاريو, fastILimit)) {
                n = ڳوليو_All_Same_بلاڪ(new_معلومات, n + 1, new_معلومات_ڊيگهائي, value, بلاڪ_ڊيگهائي);
            }
            إذا (n >= 0) {
                DEBUG_DO(++ڳڻپ_Same);
                اشاريو[i] = n;
            } وإلا {
                n = getAllSameOverlap(new_معلومات, new_معلومات_ڊيگهائي, value, بلاڪ_ڊيگهائي);
                DEBUG_DO(جوڙ_Overlaps += n);
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
                إذا (print_معلومات) {
                    print_بلاڪ(خالي_اشارو, بلاڪ_ڊيگهائي, value, i << UCPTRIE_سرڪايو_3, n, initial_قدر);
                }
#جيڪڏهن_پورو
                اشاريو[i] = new_معلومات_ڊيگهائي - n;
                int32_t prev_معلومات_ڊيگهائي = new_معلومات_ڊيگهائي;
                طالما (n < بلاڪ_ڊيگهائي) {
                    new_معلومات[new_معلومات_ڊيگهائي++] = value;
                    ++n;
                }
                mixed_بلاڪون.extend(new_معلومات, 0, prev_معلومات_ڊيگهائي, new_معلومات_ڊيگهائي);
            }
        } وإلا إذا (flags[i] == MIXED) {
            ثابت uint32_t *بلاڪ = معلومات + اشاريو[i];
            int32_t n = mixed_بلاڪون.ڳوليو_بلاڪ(new_معلومات, بلاڪ, 0);
            إذا (n >= 0) {
                DEBUG_DO(++ڳڻپ_Same);
                اشاريو[i] = n;
            } وإلا {
                n = getOverlap(new_معلومات, new_معلومات_ڊيگهائي, بلاڪ, 0, بلاڪ_ڊيگهائي);
                DEBUG_DO(جوڙ_Overlaps += n);
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
                إذا (print_معلومات) {
                    print_بلاڪ(بلاڪ, بلاڪ_ڊيگهائي, 0, i << UCPTRIE_سرڪايو_3, n, initial_قدر);
                }
#جيڪڏهن_پورو
                اشاريو[i] = new_معلومات_ڊيگهائي - n;
                int32_t prev_معلومات_ڊيگهائي = new_معلومات_ڊيگهائي;
                طالما (n < بلاڪ_ڊيگهائي) {
                    new_معلومات[new_معلومات_ڊيگهائي++] = بلاڪ[n++];
                }
                mixed_بلاڪون.extend(new_معلومات, 0, prev_معلومات_ڊيگهائي, new_معلومات_ڊيگهائي);
            }
        } وإلا /* SAME_AS */ {
            uint32_t j = اشاريو[i];
            اشاريو[i] = اشاريو[j];
        }
    }

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    /* we saved some space */
    لکو("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (ڊگهو)معلومات_ڊيگهائي, (ڊگهو)new_معلومات_ڊيگهائي, (ڊگهو)ڳڻپ_Same, (ڊگهو)جوڙ_Overlaps);
#جيڪڏهن_پورو
    أرجع new_معلومات_ڊيگهائي;
}

int32_t Mutable_ڪوڊ_نقطو_Trie::compact_اشاريو(int32_t fastILimit, Mixed_بلاڪون &mixed_بلاڪون,
                                           UError_ڪوڊ &error_ڪوڊ) {
    int32_t fast_اشاريو_ڊيگهائي = fastILimit >> (UCPTRIE_FAST_سرڪايو - UCPTRIE_سرڪايو_3);
    إذا ((مٿاهون_شروعات >> UCPTRIE_FAST_سرڪايو) <= fast_اشاريو_ڊيگهائي) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_وٿي = UCPTRIE_NO_INDEX3_NULL_وٿي;
        أرجع fast_اشاريو_ڊيگهائي;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_اشاريو[UCPTRIE_BMP_اشاريو_ڊيگهائي];  // fastIndexLength
    int32_t i3_پهريون_Null = -1;
    لكل (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = اشاريو[i];
        fast_اشاريو[j] = static_cast<uint16_t>(i3);
        إذا (i3 == static_cast<uint32_t>(معلومات_Null_وٿي)) {
            إذا (i3_پهريون_Null < 0) {
                i3_پهريون_Null = j;
            } وإلا إذا (index3_Null_وٿي < 0 &&
                    (j - i3_پهريون_Null + 1) == UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي) {
                index3_Null_وٿي = i3_پهريون_Null;
            }
        } وإلا {
            i3_پهريون_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_ايندڙ = i + SMALL_معلومات_بلاڪون_في_BMP_بلاڪ;
        طالما (++i < i_ايندڙ) {
            i3 += UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي;
            اشاريو[i] = i3;
        }
    }

    إذا (!mixed_بلاڪون.init(fast_اشاريو_ڊيگهائي, UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي)) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    mixed_بلاڪون.extend(fast_اشاريو, 0, 0, fast_اشاريو_ڊيگهائي);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_صلاحيت = 0;
    i3_پهريون_Null = index3_Null_وٿي;
    منطقي has_Long_I3_بلاڪون = خطأ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_شروعات = fastILimit < BMP_I_حد_بندي ? 0 : BMP_I_حد_بندي;
    int32_t i_حد_بندي = مٿاهون_شروعات >> UCPTRIE_سرڪايو_3;
    لكل (int32_t i = i_شروعات; i < i_حد_بندي;) {
        int32_t j = i;
        int32_t j_حد_بندي = i + UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي;
        uint32_t oredI3 = 0;
        منطقي isNull = سچ;
        افعل {
            uint32_t i3 = اشاريو[j];
            oredI3 |= i3;
            إذا (i3 != static_cast<uint32_t>(معلومات_Null_وٿي)) {
                isNull = خطأ;
            }
        } طالما (++j < j_حد_بندي);
        إذا (isNull) {
            flags[i] = I3_NULL;
            إذا (i3_پهريون_Null < 0) {
                إذا (oredI3 <= 0xffff) {
                    index3_صلاحيت += UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي;
                } وإلا {
                    index3_صلاحيت += اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي;
                    has_Long_I3_بلاڪون = سچ;
                }
                i3_پهريون_Null = 0;
            }
        } وإلا {
            إذا (oredI3 <= 0xffff) {
                int32_t n = mixed_بلاڪون.ڳوليو_بلاڪ(fast_اشاريو, اشاريو, i);
                إذا (n >= 0) {
                    flags[i] = I3_BMP;
                    اشاريو[i] = n;
                } وإلا {
                    flags[i] = I3_16;
                    index3_صلاحيت += UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي;
                }
            } وإلا {
                flags[i] = I3_18;
                index3_صلاحيت += اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي;
                has_Long_I3_بلاڪون = سچ;
            }
        }
        i = j;
    }

    int32_t index2_صلاحيت = (i_حد_بندي - i_شروعات) >> UCPTRIE_سرڪايو_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_ڊيگهائي = (index2_صلاحيت + UCPTRIE_اشاريو_2_نقاب) >> UCPTRIE_سرڪايو_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_صلاحيت = fast_اشاريو_ڊيگهائي + index1_ڊيگهائي + index3_صلاحيت + index2_صلاحيت + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_صلاحيت * 2));
    إذا (index16 == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    uprv_memcpy(index16, fast_اشاريو, fast_اشاريو_ڊيگهائي * 2);

    إذا (!mixed_بلاڪون.init(index16_صلاحيت, UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي)) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    Mixed_بلاڪون long_I3_بلاڪون;
    إذا (has_Long_I3_بلاڪون) {
        إذا (!long_I3_بلاڪون.init(index16_صلاحيت, اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي)) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_حد_بندي >> UCPTRIE_سرڪايو_2];  // index2Capacity
    int32_t i2_ڊيگهائي = 0;
    i3_پهريون_Null = index3_Null_وٿي;
    int32_t index3_شروعات = fast_اشاريو_ڊيگهائي + index1_ڊيگهائي;
    int32_t اشاريو_ڊيگهائي = index3_شروعات;
    لكل (int32_t i = i_شروعات; i < i_حد_بندي; i += UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي) {
        int32_t i3;
        uint8_t f = flags[i];
        إذا (f == I3_NULL && i3_پهريون_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = معلومات_Null_وٿي <= 0xffff ? I3_16 : I3_18;
            i3_پهريون_Null = 0;
        }
        إذا (f == I3_NULL) {
            i3 = index3_Null_وٿي;
        } وإلا إذا (f == I3_BMP) {
            i3 = اشاريو[i];
        } وإلا إذا (f == I3_16) {
            int32_t n = mixed_بلاڪون.ڳوليو_بلاڪ(index16, اشاريو, i);
            إذا (n >= 0) {
                i3 = n;
            } وإلا {
                إذا (اشاريو_ڊيگهائي == index3_شروعات) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, اشاريو_ڊيگهائي,
                                   اشاريو, i, UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي);
                }
                i3 = اشاريو_ڊيگهائي - n;
                int32_t prev_اشاريو_ڊيگهائي = اشاريو_ڊيگهائي;
                طالما (n < UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي) {
                    index16[اشاريو_ڊيگهائي++] = اشاريو[i + n++];
                }
                mixed_بلاڪون.extend(index16, index3_شروعات, prev_اشاريو_ڊيگهائي, اشاريو_ڊيگهائي);
                إذا (has_Long_I3_بلاڪون) {
                    long_I3_بلاڪون.extend(index16, index3_شروعات, prev_اشاريو_ڊيگهائي, اشاريو_ڊيگهائي);
                }
            }
        } وإلا {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_بلاڪون);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_حد_بندي = i + UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي;
            int32_t k = اشاريو_ڊيگهائي;
            افعل {
                ++k;
                uint32_t v = اشاريو[j++];
                uint32_t مٿانهون_بٽ = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = اشاريو[j++];
                مٿانهون_بٽ |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = مٿانهون_بٽ;
            } طالما (j < j_حد_بندي);
            int32_t n = long_I3_بلاڪون.ڳوليو_بلاڪ(index16, index16, اشاريو_ڊيگهائي);
            إذا (n >= 0) {
                i3 = n | 0x8000;
            } وإلا {
                إذا (اشاريو_ڊيگهائي == index3_شروعات) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, اشاريو_ڊيگهائي,
                                   index16, اشاريو_ڊيگهائي, اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي);
                }
                i3 = (اشاريو_ڊيگهائي - n) | 0x8000;
                int32_t prev_اشاريو_ڊيگهائي = اشاريو_ڊيگهائي;
                إذا (n > 0) {
                    int32_t شروعات = اشاريو_ڊيگهائي;
                    طالما (n < اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي) {
                        index16[اشاريو_ڊيگهائي++] = index16[شروعات + n++];
                    }
                } وإلا {
                    اشاريو_ڊيگهائي += اشاريو_3_18_ٻٽو_انگ_بلاڪ_ڊيگهائي;
                }
                mixed_بلاڪون.extend(index16, index3_شروعات, prev_اشاريو_ڊيگهائي, اشاريو_ڊيگهائي);
                إذا (has_Long_I3_بلاڪون) {
                    long_I3_بلاڪون.extend(index16, index3_شروعات, prev_اشاريو_ڊيگهائي, اشاريو_ڊيگهائي);
                }
            }
        }
        إذا (index3_Null_وٿي < 0 && i3_پهريون_Null >= 0) {
            index3_Null_وٿي = i3;
        }
        // Set the index-2 table entry.
        index2[i2_ڊيگهائي++] = i3;
    }
    U_ASSERT(i2_ڊيگهائي == index2_صلاحيت);
    U_ASSERT(اشاريو_ڊيگهائي <= index3_شروعات + index3_صلاحيت);

    إذا (index3_Null_وٿي < 0) {
        index3_Null_وٿي = UCPTRIE_NO_INDEX3_NULL_وٿي;
    }
    إذا (اشاريو_ڊيگهائي >= (UCPTRIE_NO_INDEX3_NULL_وٿي + UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_اشاريو_2_بلاڪ_ڊيگهائي == UCPTRIE_اشاريو_3_بلاڪ_ڊيگهائي,
                  "must re-init mixedBlocks");
    int32_t بلاڪ_ڊيگهائي = UCPTRIE_اشاريو_2_بلاڪ_ڊيگهائي;
    int32_t i1 = fast_اشاريو_ڊيگهائي;
    لكل (int32_t i = 0; i < i2_ڊيگهائي; i += بلاڪ_ڊيگهائي) {
        int32_t n;
        إذا ((i2_ڊيگهائي - i) >= بلاڪ_ڊيگهائي) {
            // normal block
            U_ASSERT(بلاڪ_ڊيگهائي == UCPTRIE_اشاريو_2_بلاڪ_ڊيگهائي);
            n = mixed_بلاڪون.ڳوليو_بلاڪ(index16, index2, i);
        } وإلا {
            // highStart is inside the last index-2 block. Shorten it.
            بلاڪ_ڊيگهائي = i2_ڊيگهائي - i;
            n = ڳوليو_Same_بلاڪ(index16, index3_شروعات, اشاريو_ڊيگهائي,
                              index2, i, بلاڪ_ڊيگهائي);
        }
        int32_t i2;
        إذا (n >= 0) {
            i2 = n;
        } وإلا {
            إذا (اشاريو_ڊيگهائي == index3_شروعات) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } وإلا {
                n = getOverlap(index16, اشاريو_ڊيگهائي, index2, i, بلاڪ_ڊيگهائي);
            }
            i2 = اشاريو_ڊيگهائي - n;
            int32_t prev_اشاريو_ڊيگهائي = اشاريو_ڊيگهائي;
            طالما (n < بلاڪ_ڊيگهائي) {
                index16[اشاريو_ڊيگهائي++] = index2[i + n++];
            }
            mixed_بلاڪون.extend(index16, index3_شروعات, prev_اشاريو_ڊيگهائي, اشاريو_ڊيگهائي);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_شروعات);
    U_ASSERT(اشاريو_ڊيگهائي <= index16_صلاحيت);

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    /* we saved some space */
    لکو("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (ڊگهو)i_حد_بندي, (ڊگهو)اشاريو_ڊيگهائي);
#جيڪڏهن_پورو

    أرجع اشاريو_ڊيگهائي;
}

int32_t Mutable_ڪوڊ_نقطو_Trie::compactTrie(int32_t fastILimit, UError_ڪوڊ &error_ڪوڊ) {
    // Find the real highStart and round it up.
    U_ASSERT((مٿاهون_شروعات & (UCPTRIE_CP_في_اشاريو_2_داخلا - 1)) == 0);
    مٿاهون_قدر = get(MAX_UNICODE);
    int32_t real_مٿاهون_شروعات = ڳوليو_مٿاهون_شروعات();
    real_مٿاهون_شروعات = (real_مٿاهون_شروعات + (UCPTRIE_CP_في_اشاريو_2_داخلا - 1)) &
        ~(UCPTRIE_CP_في_اشاريو_2_داخلا - 1);
    إذا (real_مٿاهون_شروعات == UNICODE_حد_بندي) {
        مٿاهون_قدر = initial_قدر;
    }

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    لکو("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (ڊگهو)real_مٿاهون_شروعات, (ڊگهو)مٿاهون_قدر, (ڊگهو)initial_قدر);
#جيڪڏهن_پورو

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_حد_بندي = fastILimit << UCPTRIE_سرڪايو_3;
    إذا (real_مٿاهون_شروعات < fast_حد_بندي) {
        لكل (int32_t i = (real_مٿاهون_شروعات >> UCPTRIE_سرڪايو_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            اشاريو[i] = مٿاهون_قدر;
        }
        مٿاهون_شروعات = fast_حد_بندي;
    } وإلا {
        مٿاهون_شروعات = real_مٿاهون_شروعات;
    }

    uint32_t ascii_معلومات[ASCII_حد_بندي];
    لكل (int32_t i = 0; i < ASCII_حد_بندي; ++i) {
        ascii_معلومات[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_بلاڪون all_Same_بلاڪون;
    int32_t new_معلومات_صلاحيت = compact_سڄو_معلومات_بلاڪون(fastILimit, all_Same_بلاڪون);
    إذا (new_معلومات_صلاحيت < 0) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    uint32_t* new_معلومات = static_cast<uint32_t*>(uprv_malloc(new_معلومات_صلاحيت * 4));
    إذا (new_معلومات == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    uprv_memcpy(new_معلومات, ascii_معلومات, حجم(ascii_معلومات));

    int32_t معلومات_Null_اشاريو = all_Same_بلاڪون.ڳوليو_Most_استعمال_ٿيل();

    Mixed_بلاڪون mixed_بلاڪون;
    int32_t new_معلومات_ڊيگهائي = compact_معلومات(fastILimit, new_معلومات, new_معلومات_صلاحيت,
                                        معلومات_Null_اشاريو, mixed_بلاڪون, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع 0; }
    U_ASSERT(new_معلومات_ڊيگهائي <= new_معلومات_صلاحيت);
    uprv_free(معلومات);
    معلومات = new_معلومات;
    معلومات_صلاحيت = new_معلومات_صلاحيت;
    معلومات_ڊيگهائي = new_معلومات_ڊيگهائي;
    إذا (معلومات_ڊيگهائي > (0x3ffff + UCPTRIE_SMALL_معلومات_بلاڪ_ڊيگهائي)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا (معلومات_Null_اشاريو >= 0) {
        معلومات_Null_وٿي = اشاريو[معلومات_Null_اشاريو];
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
        إذا (معلومات[معلومات_Null_وٿي] != initial_قدر) {
            لکو("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (ڊگهو)initial_قدر, (ڊگهو)معلومات[معلومات_Null_وٿي]);
        }
#جيڪڏهن_پورو
        initial_قدر = معلومات[معلومات_Null_وٿي];
    } وإلا {
        معلومات_Null_وٿي = UCPTRIE_NO_معلومات_NULL_وٿي;
    }

    int32_t اشاريو_ڊيگهائي = compact_اشاريو(fastILimit, mixed_بلاڪون, error_ڪوڊ);
    مٿاهون_شروعات = real_مٿاهون_شروعات;
    أرجع اشاريو_ڊيگهائي;
}

UCPTrie *Mutable_ڪوڊ_نقطو_Trie::build(UCPTrieType type, UCPTrie_قدر_ويڪر قدر_ويڪر, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قدر_ويڪر < UCPTRIE_قدر_بٽ_16 || UCPTRIE_قدر_بٽ_8 < قدر_ويڪر) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    بدل (قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_32:
        اكسر;
    حالة UCPTRIE_قدر_بٽ_16:
        نقاب_قدرون(0xffff);
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        نقاب_قدرون(0xff);
        اكسر;
    افتراضي:
        اكسر;
    }

    UChar32 fast_حد_بندي = type == UCPTRIE_TYPE_FAST ? BMP_حد_بندي : UCPTRIE_SMALL_حد_بندي;
    int32_t اشاريو_ڊيگهائي = compactTrie(fast_حد_بندي >> UCPTRIE_سرڪايو_3, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) {
        clear();
        أرجع خالي_اشارو;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    إذا (قدر_ويڪر == UCPTRIE_قدر_بٽ_32 && (اشاريو_ڊيگهائي & 1) != 0) {
        index16[اشاريو_ڊيگهائي++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = اشاريو_ڊيگهائي * 2;
    إذا (قدر_ويڪر == UCPTRIE_قدر_بٽ_16) {
        إذا (((اشاريو_ڊيگهائي ^ معلومات_ڊيگهائي) & 1) != 0) {
            // padding
            معلومات[معلومات_ڊيگهائي++] = error_قدر;
        }
        إذا (معلومات[معلومات_ڊيگهائي - 1] != error_قدر || معلومات[معلومات_ڊيگهائي - 2] != مٿاهون_قدر) {
            معلومات[معلومات_ڊيگهائي++] = مٿاهون_قدر;
            معلومات[معلومات_ڊيگهائي++] = error_قدر;
        }
        length += معلومات_ڊيگهائي * 2;
    } وإلا إذا (قدر_ويڪر == UCPTRIE_قدر_بٽ_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        إذا (معلومات[معلومات_ڊيگهائي - 1] != error_قدر || معلومات[معلومات_ڊيگهائي - 2] != مٿاهون_قدر) {
            إذا (معلومات[معلومات_ڊيگهائي - 1] != مٿاهون_قدر) {
                معلومات[معلومات_ڊيگهائي++] = مٿاهون_قدر;
            }
            معلومات[معلومات_ڊيگهائي++] = error_قدر;
        }
        length += معلومات_ڊيگهائي * 4;
    } وإلا {
        int32_t and3 = (length + معلومات_ڊيگهائي) & 3;
        إذا (and3 == 0 && معلومات[معلومات_ڊيگهائي - 1] == error_قدر && معلومات[معلومات_ڊيگهائي - 2] == مٿاهون_قدر) {
            // all set
        } وإلا إذا(and3 == 3 && معلومات[معلومات_ڊيگهائي - 1] == مٿاهون_قدر) {
            معلومات[معلومات_ڊيگهائي++] = error_قدر;
        } وإلا {
            طالما (and3 != 2) {
                معلومات[معلومات_ڊيگهائي++] = مٿاهون_قدر;
                and3 = (and3 + 1) & 3;
            }
            معلومات[معلومات_ڊيگهائي++] = مٿاهون_قدر;
            معلومات[معلومات_ڊيگهائي++] = error_قدر;
        }
        length += معلومات_ڊيگهائي;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += حجم(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* بائيٽون = static_cast<uint8_t*>(uprv_malloc(length));
    إذا (بائيٽون == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        clear();
        أرجع خالي_اشارو;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(بائيٽون);
    uprv_memset(trie, 0, حجم(UCPTrie));
    trie->اشاريو_ڊيگهائي = اشاريو_ڊيگهائي;
    trie->معلومات_ڊيگهائي = معلومات_ڊيگهائي;

    trie->مٿاهون_شروعات = مٿاهون_شروعات;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_مٿاهون_شروعات = (مٿاهون_شروعات + 0xfff) >> 12;
    trie->type = type;
    trie->قدر_ويڪر = قدر_ويڪر;

    trie->index3_Null_وٿي = index3_Null_وٿي;
    trie->معلومات_Null_وٿي = معلومات_Null_وٿي;
    trie->null_قدر = initial_قدر;

    بائيٽون += حجم(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(بائيٽون);
    trie->اشاريو = dest16;

    إذا (مٿاهون_شروعات <= fast_حد_بندي) {
        // Condense only the fast index from the mutable-trie index.
        لكل (int32_t i = 0, j = 0; j < اشاريو_ڊيگهائي; i += SMALL_معلومات_بلاڪون_في_BMP_بلاڪ, ++j) {
            *dest16++ = static_cast<uint16_t>(اشاريو[i]); // dest16[j]
        }
    } وإلا {
        uprv_memcpy(dest16, index16, اشاريو_ڊيگهائي * 2);
        dest16 += اشاريو_ڊيگهائي;
    }
    بائيٽون += اشاريو_ڊيگهائي * 2;

    // Write the data array.
    ثابت uint32_t *p = معلومات;
    بدل (قدر_ويڪر) {
    حالة UCPTRIE_قدر_بٽ_16:
        // Write 16-bit data values.
        trie->معلومات.ptr16 = dest16;
        لكل (int32_t i = معلومات_ڊيگهائي; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        اكسر;
    حالة UCPTRIE_قدر_بٽ_32:
        // Write 32-bit data values.
        trie->معلومات.ptr32 = reinterpret_cast<uint32_t*>(بائيٽون);
        uprv_memcpy(بائيٽون, p, (size_t)معلومات_ڊيگهائي * 4);
        اكسر;
    حالة UCPTRIE_قدر_بٽ_8:
        // Write 8-bit data values.
        trie->معلومات.ptr8 = بائيٽون;
        لكل (int32_t i = معلومات_ڊيگهائي; i > 0; --i) {
            *بائيٽون++ = static_cast<uint8_t>(*p++);
        }
        اكسر;
    افتراضي:
        // Will not occur, valueWidth checked at the beginning.
        اكسر;
    }

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#جيڪڏهن_پورو

    clear();
    أرجع trie;
}

}  // namespace

U_NAMESPACE_پڇاڙي

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_قدر, uint32_t error_قدر, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    Local_اشارو<Mutable_ڪوڊ_نقطو_Trie> trie(
        جديد Mutable_ڪوڊ_نقطو_Trie(initial_قدر, error_قدر, *p_Error_ڪوڊ), *p_Error_ڪوڊ);
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ثابت UMutableCPTrie *other, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا (other == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    Local_اشارو<Mutable_ڪوڊ_نقطو_Trie> clone(
        جديد Mutable_ڪوڊ_نقطو_Trie(*reinterpret_cast<ثابت Mutable_ڪوڊ_نقطو_Trie *>(other), *p_Error_ڪوڊ), *p_Error_ڪوڊ);
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    ميٽيو reinterpret_cast<Mutable_ڪوڊ_نقطو_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ثابت UCPMap *map, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا (map == خالي_اشارو) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_ڪوڊ_نقطو_Trie::fromUCPMap(map, *p_Error_ڪوڊ));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ثابت UCPTrie *trie, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا (trie == خالي_اشارو) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_ڪوڊ_نقطو_Trie::fromUCPTrie(trie, *p_Error_ڪوڊ));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ثابت UMutableCPTrie *trie, UChar32 c) {
    أرجع reinterpret_cast<ثابت Mutable_ڪوڊ_نقطو_Trie *>(trie)->get(c);
}

نطاق {

UChar32 getRange(ثابت فراغ *trie, UChar32 شروعات,
                 UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع reinterpret_cast<ثابت Mutable_ڪوڊ_نقطو_Trie *>(trie)->
        getRange(شروعات, filter, context, p_قدر);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ثابت UMutableCPTrie *trie, UChar32 شروعات,
                        UCPMapRangeOption option, uint32_t surrogate_قدر,
                        UCPMap_قدر_Filter *filter, ثابت فراغ *context, uint32_t *p_قدر) {
    أرجع ucptrie_اندروني_Get_Range(getRange, trie, شروعات,
                                    option, surrogate_قدر,
                                    filter, context, p_قدر);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_سيٽ(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    reinterpret_cast<Mutable_ڪوڊ_نقطو_Trie *>(trie)->سيٽ(c, value, *p_Error_ڪوڊ);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_سيٽ_Range(UMutableCPTrie *trie, UChar32 شروعات, UChar32 پڇاڙي,
                   uint32_t value, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    reinterpret_cast<Mutable_ڪوڊ_نقطو_Trie *>(trie)->سيٽ_Range(شروعات, پڇاڙي, value, *p_Error_ڪوڊ);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_قدر_ويڪر قدر_ويڪر,
                              UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<Mutable_ڪوڊ_نقطو_Trie *>(trie)->build(type, قدر_ويڪر, *p_Error_ڪوڊ);
}

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
U_CFUNC فراغ umutablecptrie_سيٽ_Name(UMutableCPTrie *trie, ثابت حرف *name) {
    reinterpret_cast<Mutable_ڪوڊ_نقطو_Trie *>(trie)->name = name;
}
#جيڪڏهن_پورو
