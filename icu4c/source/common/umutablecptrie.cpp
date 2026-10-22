// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#که_تعریف_شوی UCPTRIE_DEBUG
#   include <stdio.h>
#د_که_پای

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#که_تعریف_شوی ډکېدنه_2
#تعریف_لغوه ډکېدنه_2
#د_که_پای

U_NAMESPACE_BEGIN

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_حد = 0x110000;
constexpr int32_t BMP_حد = 0x10000;
constexpr int32_t ASCII_حد = 0x80;

constexpr int32_t I_حد = UNICODE_حد >> UCPTRIE_ښويدنه_3;
constexpr int32_t BMP_I_حد = BMP_حد >> UCPTRIE_ښويدنه_3;
constexpr int32_t ASCII_I_حد = ASCII_حد >> UCPTRIE_ښويدنه_3;

constexpr int32_t SMALL_معلومات_بلاکونه_فی_BMP_بلاک = (1 << (UCPTRIE_FAST_ښويدنه - UCPTRIE_ښويدنه_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_معلومات_اوږدتیا = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_معلومات_اوږدتیا = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_معلومات_اوږدتیا = UNICODE_حد;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t شاخص_3_18_بټ_بلاک_اوږدتیا = UCPTRIE_شاخص_3_بلاک_اوږدتیا + UCPTRIE_شاخص_3_بلاک_اوږدتیا / 8;

صنف All_Same_بلاکونه;
صنف Mixed_بلاکونه;

صنف Mutable_کوډ_ټکی_Trie : عام UMemory {
عام:
    Mutable_کوډ_ټکی_Trie(uint32_t initial_ارزښت, uint32_t error_ارزښت, UError_کوډ &error_کوډ);
    Mutable_کوډ_ټکی_Trie(ثابت Mutable_کوډ_ټکی_Trie &other, UError_کوډ &error_کوډ);
    Mutable_کوډ_ټکی_Trie(ثابت Mutable_کوډ_ټکی_Trie &other) = ړنګول;
    ~Mutable_کوډ_ټکی_Trie();

    Mutable_کوډ_ټکی_Trie &چلوونکی=(ثابت Mutable_کوډ_ټکی_Trie &other) = ړنګول;

    ساكن Mutable_کوډ_ټکی_Trie *fromUCPMap(ثابت UCPMap *map, UError_کوډ &error_کوډ);
    ساكن Mutable_کوډ_ټکی_Trie *fromUCPTrie(ثابت UCPTrie *trie, UError_کوډ &error_کوډ);

    uint32_t get(UChar32 c) ثابت;
    int32_t getRange(UChar32 پیل, UCPMap_ارزښت_Filter *filter, ثابت فراغ *context,
                     uint32_t *p_ارزښت) ثابت;

    فراغ سېټ(UChar32 c, uint32_t value, UError_کوډ &error_کوډ);
    فراغ سېټ_Range(UChar32 پیل, UChar32 پای_ټکی, uint32_t value, UError_کوډ &error_کوډ);

    UCPTrie *build(UCPTrieType type, UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی, UError_کوډ &error_کوډ);

خاص:
    فراغ clear();

    منطقي ensure_لوړ_پیل(UChar32 c);
    int32_t alloc_معلومات_بلاک(int32_t بلاک_اوږدتیا);
    int32_t get_معلومات_بلاک(int32_t i);

    فراغ ماسک_ارزښتونه(uint32_t mask);
    UChar32 پیدا_کړه_لوړ_پیل() ثابت;
    int32_t compact_ټولیز_معلومات_بلاکونه(int32_t fastILimit, All_Same_بلاکونه &all_Same_بلاکونه);
    int32_t compact_معلومات(
            int32_t fastILimit, uint32_t *new_معلومات, int32_t new_معلومات_ظرفیت,
            int32_t معلومات_Null_شاخص, Mixed_بلاکونه &mixed_بلاکونه, UError_کوډ &error_کوډ);
    int32_t compact_شاخص(int32_t fastILimit, Mixed_بلاکونه &mixed_بلاکونه, UError_کوډ &error_کوډ);
    int32_t compactTrie(int32_t fastILimit, UError_کوډ &error_کوډ);

    uint32_t *شاخص = تش_اشاره;
    int32_t شاخص_ظرفیت = 0;
    int32_t index3_Null_دننه_واټن = -1;
    uint32_t *معلومات = تش_اشاره;
    int32_t معلومات_ظرفیت = 0;
    int32_t معلومات_اوږدتیا = 0;
    int32_t معلومات_Null_دننه_واټن = -1;

    uint32_t orig_Initial_ارزښت;
    uint32_t initial_ارزښت;
    uint32_t error_ارزښت;
    UChar32 لوړ_پیل;
    uint32_t لوړ_ارزښت;
#که_تعریف_شوی UCPTRIE_DEBUG
عام:
    ثابت حرف *name;
#د_که_پای
خاص:
    /** Temporary array while building the final data. */
    uint16_t *index16 = تش_اشاره;
    uint8_t flags[UNICODE_حد >> UCPTRIE_ښويدنه_3];
};

Mutable_کوډ_ټکی_Trie::Mutable_کوډ_ټکی_Trie(uint32_t ini_ارزښت, uint32_t err_ارزښت, UError_کوډ &error_کوډ) :
        orig_Initial_ارزښت(ini_ارزښت), initial_ارزښت(ini_ارزښت), error_ارزښت(err_ارزښت),
        لوړ_پیل(0), لوړ_ارزښت(initial_ارزښت)
#که_تعریف_شوی UCPTRIE_DEBUG
        , name("open")
#د_که_پای
        {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    شاخص = static_cast<uint32_t*>(uprv_malloc(BMP_I_حد * 4));
    معلومات = static_cast<uint32_t*>(uprv_malloc(INITIAL_معلومات_اوږدتیا * 4));
    إذا (شاخص == تش_اشاره || معلومات == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    شاخص_ظرفیت = BMP_I_حد;
    معلومات_ظرفیت = INITIAL_معلومات_اوږدتیا;
}

Mutable_کوډ_ټکی_Trie::Mutable_کوډ_ټکی_Trie(ثابت Mutable_کوډ_ټکی_Trie &other, UError_کوډ &error_کوډ) :
        index3_Null_دننه_واټن(other.index3_Null_دننه_واټن),
        معلومات_Null_دننه_واټن(other.معلومات_Null_دننه_واټن),
        orig_Initial_ارزښت(other.orig_Initial_ارزښت), initial_ارزښت(other.initial_ارزښت),
        error_ارزښت(other.error_ارزښت),
        لوړ_پیل(other.لوړ_پیل), لوړ_ارزښت(other.لوړ_ارزښت)
#که_تعریف_شوی UCPTRIE_DEBUG
        , name("mutable clone")
#د_که_پای
        {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    int32_t i_ظرفیت = لوړ_پیل <= BMP_حد ? BMP_I_حد : I_حد;
    شاخص = static_cast<uint32_t*>(uprv_malloc(i_ظرفیت * 4));
    معلومات = static_cast<uint32_t*>(uprv_malloc(other.معلومات_ظرفیت * 4));
    إذا (شاخص == تش_اشاره || معلومات == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    شاخص_ظرفیت = i_ظرفیت;
    معلومات_ظرفیت = other.معلومات_ظرفیت;

    int32_t i_حد = لوړ_پیل >> UCPTRIE_ښويدنه_3;
    uprv_memcpy(flags, other.flags, i_حد);
    uprv_memcpy(شاخص, other.شاخص, i_حد * 4);
    uprv_memcpy(معلومات, other.معلومات, (size_t)other.معلومات_اوږدتیا * 4);
    معلومات_اوږدتیا = other.معلومات_اوږدتیا;
    U_ASSERT(other.index16 == تش_اشاره);
}

Mutable_کوډ_ټکی_Trie::~Mutable_کوډ_ټکی_Trie() {
    uprv_free(شاخص);
    uprv_free(معلومات);
    uprv_free(index16);
}

Mutable_کوډ_ټکی_Trie *Mutable_کوډ_ټکی_Trie::fromUCPMap(ثابت UCPMap *map, UError_کوډ &error_کوډ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ارزښت = ucpmap_get(map, -1);
    uint32_t initial_ارزښت = ucpmap_get(map, 0x10ffff);
    Local_اشاره_ګر<Mutable_کوډ_ټکی_Trie> mutableTrie(
        جديد Mutable_کوډ_ټکی_Trie(initial_ارزښت, error_ارزښت, error_کوډ),
        error_کوډ);
    إذا (U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    UChar32 پیل = 0, پای_ټکی;
    uint32_t value;
    طالما ((پای_ټکی = ucpmap_getRange(map, پیل, UCPMAP_RANGE_نورمال, 0,
                                  تش_اشاره, تش_اشاره, &value)) >= 0) {
        إذا (value != initial_ارزښت) {
            إذا (پیل == پای_ټکی) {
                mutableTrie->سېټ(پیل, value, error_کوډ);
            } وإلا {
                mutableTrie->سېټ_Range(پیل, پای_ټکی, value, error_کوډ);
            }
        }
        پیل = پای_ټکی + 1;
    }
    إذا (U_SUCCESS(error_کوډ)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع تش_اشاره;
    }
}

Mutable_کوډ_ټکی_Trie *Mutable_کوډ_ټکی_Trie::fromUCPTrie(ثابت UCPTrie *trie, UError_کوډ &error_کوډ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ارزښت;
    uint32_t initial_ارزښت;
    بدل (trie->ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        error_ارزښت = trie->معلومات.ptr16[trie->معلومات_اوږدتیا - UCPTRIE_ERROR_ارزښت_NEG_معلومات_دننه_واټن];
        initial_ارزښت = trie->معلومات.ptr16[trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن];
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        error_ارزښت = trie->معلومات.ptr32[trie->معلومات_اوږدتیا - UCPTRIE_ERROR_ارزښت_NEG_معلومات_دننه_واټن];
        initial_ارزښت = trie->معلومات.ptr32[trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن];
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        error_ارزښت = trie->معلومات.ptr8[trie->معلومات_اوږدتیا - UCPTRIE_ERROR_ارزښت_NEG_معلومات_دننه_واټن];
        initial_ارزښت = trie->معلومات.ptr8[trie->معلومات_اوږدتیا - UCPTRIE_لوړ_ارزښت_NEG_معلومات_دننه_واټن];
        اكسر;
    افتراضي:
        // Unreachable if the trie is properly initialized.
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    Local_اشاره_ګر<Mutable_کوډ_ټکی_Trie> mutableTrie(
        جديد Mutable_کوډ_ټکی_Trie(initial_ارزښت, error_ارزښت, error_کوډ),
        error_کوډ);
    إذا (U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    UChar32 پیل = 0, پای_ټکی;
    uint32_t value;
    طالما ((پای_ټکی = ucptrie_getRange(trie, پیل, UCPMAP_RANGE_نورمال, 0,
                                   تش_اشاره, تش_اشاره, &value)) >= 0) {
        إذا (value != initial_ارزښت) {
            إذا (پیل == پای_ټکی) {
                mutableTrie->سېټ(پیل, value, error_کوډ);
            } وإلا {
                mutableTrie->سېټ_Range(پیل, پای_ټکی, value, error_کوډ);
            }
        }
        پیل = پای_ټکی + 1;
    }
    إذا (U_SUCCESS(error_کوډ)) {
        أرجع mutableTrie.orphan();
    } وإلا {
        أرجع تش_اشاره;
    }
}

فراغ Mutable_کوډ_ټکی_Trie::clear() {
    index3_Null_دننه_واټن = معلومات_Null_دننه_واټن = -1;
    معلومات_اوږدتیا = 0;
    لوړ_ارزښت = initial_ارزښت = orig_Initial_ارزښت;
    لوړ_پیل = 0;
    uprv_free(index16);
    index16 = تش_اشاره;
}

uint32_t Mutable_کوډ_ټکی_Trie::get(UChar32 c) ثابت {
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        أرجع error_ارزښت;
    }
    إذا (c >= لوړ_پیل) {
        أرجع لوړ_ارزښت;
    }
    int32_t i = c >> UCPTRIE_ښويدنه_3;
    إذا (flags[i] == ALL_SAME) {
        أرجع شاخص[i];
    } وإلا {
        أرجع معلومات[شاخص[i] + (c & UCPTRIE_SMALL_معلومات_ماسک)];
    }
}

مضمن uint32_t maybe_Filter_ارزښت(uint32_t value, uint32_t initial_ارزښت, uint32_t null_ارزښت,
                                 UCPMap_ارزښت_Filter *filter, ثابت فراغ *context) {
    إذا (value == initial_ارزښت) {
        value = null_ارزښت;
    } وإلا إذا (filter != تش_اشاره) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 Mutable_کوډ_ټکی_Trie::getRange(
        UChar32 پیل, UCPMap_ارزښت_Filter *filter, ثابت فراغ *context,
        uint32_t *p_ارزښت) ثابت {
    إذا (static_cast<uint32_t>(پیل) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    إذا (پیل >= لوړ_پیل) {
        إذا (p_ارزښت != تش_اشاره) {
            uint32_t value = لوړ_ارزښت;
            إذا (filter != تش_اشاره) { value = filter(context, value); }
            *p_ارزښت = value;
        }
        أرجع MAX_UNICODE;
    }
    uint32_t null_ارزښت = initial_ارزښت;
    إذا (filter != تش_اشاره) { null_ارزښت = filter(context, null_ارزښت); }
    UChar32 c = پیل;
    uint32_t trie_ارزښت, value;
    منطقي have_ارزښت = خطأ;
    int32_t i = c >> UCPTRIE_ښويدنه_3;
    افعل {
        إذا (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = شاخص[i];
            إذا (have_ارزښت) {
                إذا (trieValue2 != trie_ارزښت) {
                    إذا (filter == تش_اشاره ||
                            maybe_Filter_ارزښت(trieValue2, initial_ارزښت, null_ارزښت,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_ارزښت = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_ارزښت = trieValue2;
                value = maybe_Filter_ارزښت(trieValue2, initial_ارزښت, null_ارزښت, filter, context);
                إذا (p_ارزښت != تش_اشاره) { *p_ارزښت = value; }
                have_ارزښت = ریښتیا;
            }
            c = (c + UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا) & ~UCPTRIE_SMALL_معلومات_ماسک;
        } وإلا /* MIXED */ {
            int32_t di = شاخص[i] + (c & UCPTRIE_SMALL_معلومات_ماسک);
            uint32_t trieValue2 = معلومات[di];
            إذا (have_ارزښت) {
                إذا (trieValue2 != trie_ارزښت) {
                    إذا (filter == تش_اشاره ||
                            maybe_Filter_ارزښت(trieValue2, initial_ارزښت, null_ارزښت,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                    trie_ارزښت = trieValue2;  // may or may not help
                }
            } وإلا {
                trie_ارزښت = trieValue2;
                value = maybe_Filter_ارزښت(trieValue2, initial_ارزښت, null_ارزښت, filter, context);
                إذا (p_ارزښت != تش_اشاره) { *p_ارزښت = value; }
                have_ارزښت = ریښتیا;
            }
            طالما ((++c & UCPTRIE_SMALL_معلومات_ماسک) != 0) {
                trieValue2 = معلومات[++di];
                إذا (trieValue2 != trie_ارزښت) {
                    إذا (filter == تش_اشاره ||
                            maybe_Filter_ارزښت(trieValue2, initial_ارزښت, null_ارزښت,
                                             filter, context) != value) {
                        أرجع c - 1;
                    }
                }
                trie_ارزښت = trieValue2;  // may or may not help
            }
        }
        ++i;
    } طالما (c < لوړ_پیل);
    U_ASSERT(have_ارزښت);
    إذا (maybe_Filter_ارزښت(لوړ_ارزښت, initial_ارزښت, null_ارزښت,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

فراغ
write_بلاک(uint32_t *بلاک, uint32_t value) {
    uint32_t *حد = بلاک + UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
    طالما (بلاک < حد) {
        *بلاک++ = value;
    }
}

منطقي Mutable_کوډ_ټکی_Trie::ensure_لوړ_پیل(UChar32 c) {
    إذا (c >= لوړ_پیل) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_فی_شاخص_2_ثبت) & ~(UCPTRIE_CP_فی_شاخص_2_ثبت - 1);
        int32_t i = لوړ_پیل >> UCPTRIE_ښويدنه_3;
        int32_t i_حد = c >> UCPTRIE_ښويدنه_3;
        إذا (i_حد > شاخص_ظرفیت) {
            uint32_t* new_شاخص = static_cast<uint32_t*>(uprv_malloc(I_حد * 4));
            إذا (new_شاخص == تش_اشاره) { أرجع خطأ; }
            uprv_memcpy(new_شاخص, شاخص, i * 4);
            uprv_free(شاخص);
            شاخص = new_شاخص;
            شاخص_ظرفیت = I_حد;
        }
        افعل {
            flags[i] = ALL_SAME;
            شاخص[i] = initial_ارزښت;
        } طالما(++i < i_حد);
        لوړ_پیل = c;
    }
    أرجع ریښتیا;
}

int32_t Mutable_کوډ_ټکی_Trie::alloc_معلومات_بلاک(int32_t بلاک_اوږدتیا) {
    int32_t new_بلاک = معلومات_اوږدتیا;
    int32_t new_سر = new_بلاک + بلاک_اوږدتیا;
    إذا (new_سر > معلومات_ظرفیت) {
        int32_t capacity;
        إذا (معلومات_ظرفیت < MEDIUM_معلومات_اوږدتیا) {
            capacity = MEDIUM_معلومات_اوږدتیا;
        } وإلا إذا (معلومات_ظرفیت < MAX_معلومات_اوږدتیا) {
            capacity = MAX_معلومات_اوږدتیا;
        } وإلا {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            أرجع -1;
        }
        uint32_t* new_معلومات = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        إذا (new_معلومات == تش_اشاره) {
            أرجع -1;
        }
        uprv_memcpy(new_معلومات, معلومات, (size_t)معلومات_اوږدتیا * 4);
        uprv_free(معلومات);
        معلومات = new_معلومات;
        معلومات_ظرفیت = capacity;
    }
    معلومات_اوږدتیا = new_سر;
    أرجع new_بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_کوډ_ټکی_Trie::get_معلومات_بلاک(int32_t i) {
    إذا (flags[i] == MIXED) {
        أرجع شاخص[i];
    }
    إذا (i < BMP_I_حد) {
        int32_t new_بلاک = alloc_معلومات_بلاک(UCPTRIE_FAST_معلومات_بلاک_اوږدتیا);
        إذا (new_بلاک < 0) { أرجع new_بلاک; }
        int32_t i_پیل = i & ~(SMALL_معلومات_بلاکونه_فی_BMP_بلاک -1);
        int32_t i_حد = i_پیل + SMALL_معلومات_بلاکونه_فی_BMP_بلاک;
        افعل {
            U_ASSERT(flags[i_پیل] == ALL_SAME);
            write_بلاک(معلومات + new_بلاک, شاخص[i_پیل]);
            flags[i_پیل] = MIXED;
            شاخص[i_پیل++] = new_بلاک;
            new_بلاک += UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
        } طالما (i_پیل < i_حد);
        أرجع شاخص[i];
    } وإلا {
        int32_t new_بلاک = alloc_معلومات_بلاک(UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا);
        إذا (new_بلاک < 0) { أرجع new_بلاک; }
        write_بلاک(معلومات + new_بلاک, شاخص[i]);
        flags[i] = MIXED;
        شاخص[i] = new_بلاک;
        أرجع new_بلاک;
    }
}

فراغ Mutable_کوډ_ټکی_Trie::سېټ(UChar32 c, uint32_t value, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    int32_t بلاک;
    إذا (!ensure_لوړ_پیل(c) || (بلاک = get_معلومات_بلاک(c >> UCPTRIE_ښويدنه_3)) < 0) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }

    معلومات[بلاک + (c & UCPTRIE_SMALL_معلومات_ماسک)] = value;
}

فراغ
fill_بلاک(uint32_t *بلاک, UChar32 پیل, UChar32 حد, uint32_t value) {
    uint32_t *p_حد = بلاک + حد;
    بلاک += پیل;
    طالما (بلاک < p_حد) {
        *بلاک++ = value;
    }
}

فراغ Mutable_کوډ_ټکی_Trie::سېټ_Range(UChar32 پیل, UChar32 پای_ټکی, uint32_t value, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا (static_cast<uint32_t>(پیل) > MAX_UNICODE || static_cast<uint32_t>(پای_ټکی) > MAX_UNICODE || پیل > پای_ټکی) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (!ensure_لوړ_پیل(پای_ټکی)) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }

    UChar32 حد = پای_ټکی + 1;
    إذا (پیل & UCPTRIE_SMALL_معلومات_ماسک) {
        // Set partial block at [start..following block boundary[.
        int32_t بلاک = get_معلومات_بلاک(پیل >> UCPTRIE_ښويدنه_3);
        إذا (بلاک < 0) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع;
        }

        UChar32 بل_پیل = (پیل + UCPTRIE_SMALL_معلومات_ماسک) & ~UCPTRIE_SMALL_معلومات_ماسک;
        إذا (بل_پیل <= حد) {
            fill_بلاک(معلومات + بلاک, پیل & UCPTRIE_SMALL_معلومات_ماسک, UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا,
                      value);
            پیل = بل_پیل;
        } وإلا {
            fill_بلاک(معلومات + بلاک, پیل & UCPTRIE_SMALL_معلومات_ماسک, حد & UCPTRIE_SMALL_معلومات_ماسک,
                      value);
            أرجع;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = حد & UCPTRIE_SMALL_معلومات_ماسک;

    // Round down limit to a block boundary.
    حد &= ~UCPTRIE_SMALL_معلومات_ماسک;

    // Iterate over all-value blocks.
    طالما (پیل < حد) {
        int32_t i = پیل >> UCPTRIE_ښويدنه_3;
        إذا (flags[i] == ALL_SAME) {
            شاخص[i] = value;
        } وإلا /* MIXED */ {
            fill_بلاک(معلومات + شاخص[i], 0, UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا, value);
        }
        پیل += UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
    }

    إذا (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t بلاک = get_معلومات_بلاک(پیل >> UCPTRIE_ښويدنه_3);
        إذا (بلاک < 0) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع;
        }

        fill_بلاک(معلومات + بلاک, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

فراغ Mutable_کوډ_ټکی_Trie::ماسک_ارزښتونه(uint32_t mask) {
    initial_ارزښت &= mask;
    error_ارزښت &= mask;
    لوړ_ارزښت &= mask;
    int32_t i_حد = لوړ_پیل >> UCPTRIE_ښويدنه_3;
    لكل (int32_t i = 0; i < i_حد; ++i) {
        إذا (flags[i] == ALL_SAME) {
            شاخص[i] &= mask;
        }
    }
    لكل (int32_t i = 0; i < معلومات_اوږدتیا; ++i) {
        معلومات[i] &= mask;
    }
}

کينډۍ<ډول_نوم UIntA, ډول_نوم UIntB>
منطقي equal_بلاکونه(ثابت UIntA *s, ثابت UIntB *t, int32_t length) {
    طالما (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

منطقي all_ارزښتونه_Same_As(ثابت uint32_t *p, int32_t length, uint32_t value) {
    ثابت uint32_t *p_حد = p + length;
    طالما (p < p_حد && *p == value) { ++p; }
    أرجع p == p_حد;
}

/** Search for an identical block. */
int32_t پیدا_کړه_Same_بلاک(ثابت uint16_t *p, int32_t p_پیل, int32_t length,
                      ثابت uint16_t *q, int32_t q_پیل, int32_t بلاک_اوږدتیا) {
    // Ensure that we do not even partially get past length.
    length -= بلاک_اوږدتیا;

    q += q_پیل;
    طالما (p_پیل <= length) {
        إذا (equal_بلاکونه(p + p_پیل, q, بلاک_اوږدتیا)) {
            أرجع p_پیل;
        }
        ++p_پیل;
    }
    أرجع -1;
}

int32_t پیدا_کړه_All_Same_بلاک(ثابت uint32_t *p, int32_t پیل, int32_t حد,
                         uint32_t value, int32_t بلاک_اوږدتیا) {
    // Ensure that we do not even partially get past limit.
    حد -= بلاک_اوږدتیا;

    لكل (int32_t بلاک = پیل; بلاک <= حد; ++بلاک) {
        إذا (p[بلاک] == value) {
            لكل (int32_t i = 1;; ++i) {
                إذا (i == بلاک_اوږدتیا) {
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
کينډۍ<ډول_نوم UIntA, ډول_نوم UIntB>
int32_t getOverlap(ثابت UIntA *p, int32_t length,
                   ثابت UIntB *q, int32_t q_پیل, int32_t بلاک_اوږدتیا) {
    int32_t overlap = بلاک_اوږدتیا - 1;
    U_ASSERT(overlap <= length);
    q += q_پیل;
    طالما (overlap > 0 && !equal_بلاکونه(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    أرجع overlap;
}

int32_t getAllSameOverlap(ثابت uint32_t *p, int32_t length, uint32_t value,
                          int32_t بلاک_اوږدتیا) {
    int32_t min = length - (بلاک_اوږدتیا - 1);
    int32_t i = length;
    طالما (min < i && p[i - 1] == value) { --i; }
    أرجع length - i;
}

منطقي is_پیل_د_Some_Fast_بلاک(uint32_t معلومات_دننه_واټن, ثابت uint32_t شاخص[], int32_t fastILimit) {
    لكل (int32_t i = 0; i < fastILimit; i += SMALL_معلومات_بلاکونه_فی_BMP_بلاک) {
        إذا (شاخص[i] == معلومات_دننه_واټن) {
            أرجع ریښتیا;
        }
    }
    أرجع خطأ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_کوډ_ټکی_Trie::پیدا_کړه_لوړ_پیل() ثابت {
    int32_t i = لوړ_پیل >> UCPTRIE_ښويدنه_3;
    طالما (i > 0) {
        منطقي سمون;
        إذا (flags[--i] == ALL_SAME) {
            سمون = شاخص[i] == لوړ_ارزښت;
        } وإلا /* MIXED */ {
            ثابت uint32_t *p = معلومات + شاخص[i];
            لكل (int32_t j = 0;; ++j) {
                إذا (j == UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا) {
                    سمون = ریښتیا;
                    اكسر;
                }
                إذا (p[j] != لوړ_ارزښت) {
                    سمون = خطأ;
                    اكسر;
                }
            }
        }
        إذا (!سمون) {
            أرجع (i + 1) << UCPTRIE_ښويدنه_3;
        }
    }
    أرجع 0;
}

صنف All_Same_بلاکونه {
عام:
    ساكن constexpr int32_t NEW_UNIQUE = -1;
    ساكن constexpr int32_t ډکېدنه_2 = -2;

    All_Same_بلاکونه() : length(0), mostRecent(-1) {}

    int32_t پیدا_کړه_Or_جمع_کړه(int32_t شاخص, int32_t count, uint32_t value) {
        إذا (mostRecent >= 0 && ارزښتونه[mostRecent] == value) {
            refCounts[mostRecent] += count;
            أرجع indexes[mostRecent];
        }
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (ارزښتونه[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                أرجع indexes[i];
            }
        }
        إذا (length == ظرفیت_3) {
            أرجع ډکېدنه_2;
        }
        mostRecent = length;
        indexes[length] = شاخص;
        ارزښتونه[length] = value;
        refCounts[length++] = count;
        أرجع NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    فراغ جمع_کړه(int32_t شاخص, int32_t count, uint32_t value) {
        U_ASSERT(length == ظرفیت_3);
        int32_t least = -1;
        int32_t لږترلږه_شمېر = I_حد;
        لكل (int32_t i = 0; i < length; ++i) {
            U_ASSERT(ارزښتونه[i] != value);
            إذا (refCounts[i] < لږترلږه_شمېر) {
                least = i;
                لږترلږه_شمېر = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = شاخص;
        ارزښتونه[least] = value;
        refCounts[least] = count;
    }

    int32_t پیدا_کړه_Most_کارول_شوی() ثابت {
        إذا (length == 0) { أرجع -1; }
        int32_t max = -1;
        int32_t max_شمېر = 0;
        لكل (int32_t i = 0; i < length; ++i) {
            إذا (refCounts[i] > max_شمېر) {
                max = i;
                max_شمېر = refCounts[i];
            }
        }
        أرجع indexes[max];
    }

خاص:
    ساكن constexpr int32_t ظرفیت_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[ظرفیت_3];
    uint32_t ارزښتونه[ظرفیت_3];
    int32_t refCounts[ظرفیت_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
صنف Mixed_بلاکونه {
عام:
    Mixed_بلاکونه() {}
    ~Mixed_بلاکونه() {
        uprv_free(جدول);
    }

    منطقي init(int32_t max_اوږدتیا_2, int32_t new_بلاک_اوږدتیا) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_معلومات_شاخص = max_اوږدتیا_2 - new_بلاک_اوږدتیا + 1;
        int32_t new_اوږدتیا;
        إذا (max_معلومات_شاخص <= 0xfff) {  // 4k
            new_اوږدتیا = 6007;
            ښويدنه = 12;
            mask = 0xfff;
        } وإلا إذا (max_معلومات_شاخص <= 0x7fff) {  // 32k
            new_اوږدتیا = 50021;
            ښويدنه = 15;
            mask = 0x7fff;
        } وإلا إذا (max_معلومات_شاخص <= 0x1ffff) {  // 128k
            new_اوږدتیا = 200003;
            ښويدنه = 17;
            mask = 0x1ffff;
        } وإلا {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_اوږدتیا = 1500007;
            ښويدنه = 21;
            mask = 0x1fffff;
        }
        إذا (new_اوږدتیا > capacity) {
            uprv_free(جدول);
            جدول = static_cast<uint32_t*>(uprv_malloc(new_اوږدتیا * 4));
            إذا (جدول == تش_اشاره) {
                أرجع خطأ;
            }
            capacity = new_اوږدتیا;
        }
        length = new_اوږدتیا;
        uprv_memset(جدول, 0, length * 4);

        بلاک_اوږدتیا = new_بلاک_اوږدتیا;
        أرجع ریښتیا;
    }

    کينډۍ<ډول_نوم UInt>
    فراغ extend(ثابت UInt *معلومات, int32_t min_پیل, int32_t prev_معلومات_اوږدتیا, int32_t new_معلومات_اوږدتیا) {
        int32_t پیل = prev_معلومات_اوږدتیا - بلاک_اوږدتیا;
        إذا (پیل >= min_پیل) {
            ++پیل;  // Skip the last block that we added last time.
        } وإلا {
            پیل = min_پیل;  // Begin with the first full block.
        }
        لكل (int32_t پای_ټکی = new_معلومات_اوږدتیا - بلاک_اوږدتیا; پیل <= پای_ټکی; ++پیل) {
            uint32_t hash_کوډ = make_Hash_کوډ(معلومات, پیل);
            جمع_کړه_ثبت(معلومات, پیل, hash_کوډ, پیل);
        }
    }

    کينډۍ<ډول_نوم UIntA, ډول_نوم UIntB>
    int32_t پیدا_کړه_بلاک(ثابت UIntA *معلومات, ثابت UIntB *بلاک_معلومات, int32_t بلاک_پیل) ثابت {
        uint32_t hash_کوډ = make_Hash_کوډ(بلاک_معلومات, بلاک_پیل);
        int32_t ثبت_شاخص = پیدا_کړه_ثبت(معلومات, بلاک_معلومات, بلاک_پیل, hash_کوډ);
        إذا (ثبت_شاخص >= 0) {
            أرجع (جدول[ثبت_شاخص] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

    int32_t پیدا_کړه_All_Same_بلاک(ثابت uint32_t *معلومات, uint32_t بلاک_ارزښت) ثابت {
        uint32_t hash_کوډ = make_Hash_کوډ(بلاک_ارزښت);
        int32_t ثبت_شاخص = پیدا_کړه_ثبت(معلومات, بلاک_ارزښت, hash_کوډ);
        إذا (ثبت_شاخص >= 0) {
            أرجع (جدول[ثبت_شاخص] & mask) - 1;
        } وإلا {
            أرجع -1;
        }
    }

خاص:
    کينډۍ<ډول_نوم UInt>
    uint32_t make_Hash_کوډ(ثابت UInt *بلاک_معلومات, int32_t بلاک_پیل) ثابت {
        int32_t بلاک_حد = بلاک_پیل + بلاک_اوږدتیا;
        uint32_t hash_کوډ = بلاک_معلومات[بلاک_پیل++];
        افعل {
            hash_کوډ = 37 * hash_کوډ + بلاک_معلومات[بلاک_پیل++];
        } طالما (بلاک_پیل < بلاک_حد);
        أرجع hash_کوډ;
    }

    uint32_t make_Hash_کوډ(uint32_t بلاک_ارزښت) ثابت {
        uint32_t hash_کوډ = بلاک_ارزښت;
        لكل (int32_t i = 1; i < بلاک_اوږدتیا; ++i) {
            hash_کوډ = 37 * hash_کوډ + بلاک_ارزښت;
        }
        أرجع hash_کوډ;
    }

    کينډۍ<ډول_نوم UInt>
    فراغ جمع_کړه_ثبت(ثابت UInt *معلومات, int32_t بلاک_پیل, uint32_t hash_کوډ, int32_t معلومات_شاخص) {
        U_ASSERT(0 <= معلومات_شاخص && معلومات_شاخص < (int32_t)mask);
        int32_t ثبت_شاخص = پیدا_کړه_ثبت(معلومات, معلومات, بلاک_پیل, hash_کوډ);
        إذا (ثبت_شاخص < 0) {
            جدول[~ثبت_شاخص] = (hash_کوډ << ښويدنه) | (معلومات_شاخص + 1);
        }
    }

    کينډۍ<ډول_نوم UIntA, ډول_نوم UIntB>
    int32_t پیدا_کړه_ثبت(ثابت UIntA *معلومات, ثابت UIntB *بلاک_معلومات, int32_t بلاک_پیل,
                      uint32_t hash_کوډ) ثابت {
        uint32_t shifted_Hash_کوډ = hash_کوډ << ښويدنه;
        int32_t initial_ثبت_شاخص = (hash_کوډ % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t ثبت_شاخص = initial_ثبت_شاخص;;) {
            uint32_t ثبت = جدول[ثبت_شاخص];
            إذا (ثبت == 0) {
                أرجع ~ثبت_شاخص;
            }
            إذا ((ثبت & ~mask) == shifted_Hash_کوډ) {
                int32_t معلومات_شاخص = (ثبت & mask) - 1;
                إذا (equal_بلاکونه(معلومات + معلومات_شاخص, بلاک_معلومات + بلاک_پیل, بلاک_اوږدتیا)) {
                    أرجع ثبت_شاخص;
                }
            }
            ثبت_شاخص = بل_شاخص(initial_ثبت_شاخص, ثبت_شاخص);
        }
    }

    int32_t پیدا_کړه_ثبت(ثابت uint32_t *معلومات, uint32_t بلاک_ارزښت, uint32_t hash_کوډ) ثابت {
        uint32_t shifted_Hash_کوډ = hash_کوډ << ښويدنه;
        int32_t initial_ثبت_شاخص = (hash_کوډ % (length - 1)) + 1;  // 1..length-1
        لكل (int32_t ثبت_شاخص = initial_ثبت_شاخص;;) {
            uint32_t ثبت = جدول[ثبت_شاخص];
            إذا (ثبت == 0) {
                أرجع ~ثبت_شاخص;
            }
            إذا ((ثبت & ~mask) == shifted_Hash_کوډ) {
                int32_t معلومات_شاخص = (ثبت & mask) - 1;
                إذا (all_ارزښتونه_Same_As(معلومات + معلومات_شاخص, بلاک_اوږدتیا, بلاک_ارزښت)) {
                    أرجع ثبت_شاخص;
                }
            }
            ثبت_شاخص = بل_شاخص(initial_ثبت_شاخص, ثبت_شاخص);
        }
    }

    مضمن int32_t بل_شاخص(int32_t initial_ثبت_شاخص, int32_t ثبت_شاخص) ثابت {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        أرجع (ثبت_شاخص + initial_ثبت_شاخص) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *جدول = تش_اشاره;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t ښويدنه = 0;
    uint32_t mask = 0;

    int32_t بلاک_اوږدتیا = 0;
};

int32_t Mutable_کوډ_ټکی_Trie::compact_ټولیز_معلومات_بلاکونه(int32_t fastILimit, All_Same_بلاکونه &all_Same_بلاکونه) {
#که_تعریف_شوی UCPTRIE_DEBUG
    منطقي ډکېدنه = خطأ;
#د_که_پای

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_معلومات_ظرفیت = ASCII_حد;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_معلومات_ظرفیت += UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
    // Add room for special values (errorValue, highValue) and padding.
    new_معلومات_ظرفیت += 4;
    int32_t i_حد = لوړ_پیل >> UCPTRIE_ښويدنه_3;
    int32_t بلاک_اوږدتیا = UCPTRIE_FAST_معلومات_بلاک_اوږدتیا;
    int32_t inc = SMALL_معلومات_بلاکونه_فی_BMP_بلاک;
    لكل (int32_t i = 0; i < i_حد; i += inc) {
        إذا (i == fastILimit) {
            بلاک_اوږدتیا = UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
            inc = 1;
        }
        uint32_t value = شاخص[i];
        إذا (flags[i] == MIXED) {
            // Really mixed?
            ثابت uint32_t *p = معلومات + value;
            value = *p;
            إذا (all_ارزښتونه_Same_As(p + 1, بلاک_اوږدتیا - 1, value)) {
                flags[i] = ALL_SAME;
                شاخص[i] = value;
                // Fall through to ALL_SAME handling.
            } وإلا {
                new_معلومات_ظرفیت += بلاک_اوږدتیا;
                استمر;
            }
        } وإلا {
            U_ASSERT(flags[i] == ALL_SAME);
            إذا (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                منطقي allSame = ریښتیا;
                int32_t بل_i = i + inc;
                لكل (int32_t j = i + 1; j < بل_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    إذا (شاخص[j] != value) {
                        allSame = خطأ;
                        اكسر;
                    }
                }
                إذا (!allSame) {
                    // Turn it into a MIXED block.
                    إذا (get_معلومات_بلاک(i) < 0) {
                        أرجع -1;
                    }
                    new_معلومات_ظرفیت += بلاک_اوږدتیا;
                    استمر;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_بلاکونه.پیدا_کړه_Or_جمع_کړه(i, inc, value);
        إذا (other == All_Same_بلاکونه::ډکېدنه_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#که_تعریف_شوی UCPTRIE_DEBUG
            إذا (!ډکېدنه) {
                puts("UCPTrie AllSameBlocks overflow");
                ډکېدنه = ریښتیا;
            }
#د_که_پای
            int32_t jInc = SMALL_معلومات_بلاکونه_فی_BMP_بلاک;
            لكل (int32_t j = 0;; j += jInc) {
                إذا (j == i) {
                    all_Same_بلاکونه.جمع_کړه(i, inc, value);
                    اكسر;
                }
                إذا (j == fastILimit) {
                    jInc = 1;
                }
                إذا (flags[j] == ALL_SAME && شاخص[j] == value) {
                    all_Same_بلاکونه.جمع_کړه(j, jInc + inc, value);
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
            new_معلومات_ظرفیت += بلاک_اوږدتیا;
        }
    }
    أرجع new_معلومات_ظرفیت;
}

#که_تعریف_شوی UCPTRIE_DEBUG
#   define DEBUG_DO(بیان) بیان
#وإلا
#   define DEBUG_DO(بیان)
#د_که_پای

#که_تعریف_شوی UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_ارزښت(حرف s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (حرف)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (حرف)(0x80 + (value & 0x3F));
    أرجع length + 3;
}

فراغ print_بلاک(ثابت uint32_t *بلاک, int32_t بلاک_اوږدتیا, uint32_t value,
                UChar32 پیل, int32_t overlap, uint32_t initial_ارزښت) {
    حرف s[UCPTRIE_FAST_معلومات_بلاک_اوږدتیا * 3 + 3];
    int32_t length = 0;
    int32_t i;
    لكل (i = 0; i < overlap; ++i) {
        length = append_ارزښت(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    لكل (; i < بلاک_اوږدتیا; ++i) {
        إذا (بلاک != تش_اشاره) {
            value = بلاک[i];
        }
        إذا (value == initial_ارزښت) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_ارزښت(s, length, value);
    }
    s[length] = 0;
    پیل += overlap;
    إذا (پیل <= 0xffff) {
        چاپ_کړه("    %04lX  %s|\n", (اوږد)پیل, s);
    } وإلا إذا (پیل <= 0xfffff) {
        چاپ_کړه("   %5lX  %s|\n", (اوږد)پیل, s);
    } وإلا {
        چاپ_کړه("  %6lX  %s|\n", (اوږد)پیل, s);
    }
}
#د_که_پای

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
int32_t Mutable_کوډ_ټکی_Trie::compact_معلومات(
        int32_t fastILimit, uint32_t *new_معلومات, int32_t new_معلومات_ظرفیت,
        int32_t معلومات_Null_شاخص, Mixed_بلاکونه &mixed_بلاکونه, UError_کوډ &error_کوډ) {
#که_تعریف_شوی UCPTRIE_DEBUG
    int32_t شمېر_Same=0, مجموعه_Overlaps=0;
    منطقي print_معلومات = معلومات_اوږدتیا == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        معلومات_اوږدتیا == 50400 /* zh.txt~stroke */;
#د_که_پای

    // The linear ASCII data has been copied into newData already.
    int32_t new_معلومات_اوږدتیا = 0;
    لكل (int32_t i = 0; new_معلومات_اوږدتیا < ASCII_حد;
            new_معلومات_اوږدتیا += UCPTRIE_FAST_معلومات_بلاک_اوږدتیا, i += SMALL_معلومات_بلاکونه_فی_BMP_بلاک) {
        شاخص[i] = new_معلومات_اوږدتیا;
#که_تعریف_شوی UCPTRIE_DEBUG
        إذا (print_معلومات) {
            print_بلاک(new_معلومات + new_معلومات_اوږدتیا, UCPTRIE_FAST_معلومات_بلاک_اوږدتیا, 0, new_معلومات_اوږدتیا, 0, initial_ارزښت);
        }
#د_که_پای
    }

    int32_t بلاک_اوږدتیا = UCPTRIE_FAST_معلومات_بلاک_اوږدتیا;
    إذا (!mixed_بلاکونه.init(new_معلومات_ظرفیت, بلاک_اوږدتیا)) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    mixed_بلاکونه.extend(new_معلومات, 0, 0, new_معلومات_اوږدتیا);

    int32_t i_حد = لوړ_پیل >> UCPTRIE_ښويدنه_3;
    int32_t inc = SMALL_معلومات_بلاکونه_فی_BMP_بلاک;
    int32_t fast_اوږدتیا = 0;
    لكل (int32_t i = ASCII_I_حد; i < i_حد; i += inc) {
        إذا (i == fastILimit) {
            بلاک_اوږدتیا = UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
            inc = 1;
            fast_اوږدتیا = new_معلومات_اوږدتیا;
            إذا (!mixed_بلاکونه.init(new_معلومات_ظرفیت, بلاک_اوږدتیا)) {
                error_کوډ = U_حافظه_ویش_ERROR;
                أرجع 0;
            }
            mixed_بلاکونه.extend(new_معلومات, 0, 0, new_معلومات_اوږدتیا);
        }
        إذا (flags[i] == ALL_SAME) {
            uint32_t value = شاخص[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_بلاکونه.پیدا_کړه_All_Same_بلاک(new_معلومات, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            طالما (n >= 0 && i == معلومات_Null_شاخص && i >= fastILimit && n < fast_اوږدتیا &&
                    is_پیل_د_Some_Fast_بلاک(n, شاخص, fastILimit)) {
                n = پیدا_کړه_All_Same_بلاک(new_معلومات, n + 1, new_معلومات_اوږدتیا, value, بلاک_اوږدتیا);
            }
            إذا (n >= 0) {
                DEBUG_DO(++شمېر_Same);
                شاخص[i] = n;
            } وإلا {
                n = getAllSameOverlap(new_معلومات, new_معلومات_اوږدتیا, value, بلاک_اوږدتیا);
                DEBUG_DO(مجموعه_Overlaps += n);
#که_تعریف_شوی UCPTRIE_DEBUG
                إذا (print_معلومات) {
                    print_بلاک(تش_اشاره, بلاک_اوږدتیا, value, i << UCPTRIE_ښويدنه_3, n, initial_ارزښت);
                }
#د_که_پای
                شاخص[i] = new_معلومات_اوږدتیا - n;
                int32_t prev_معلومات_اوږدتیا = new_معلومات_اوږدتیا;
                طالما (n < بلاک_اوږدتیا) {
                    new_معلومات[new_معلومات_اوږدتیا++] = value;
                    ++n;
                }
                mixed_بلاکونه.extend(new_معلومات, 0, prev_معلومات_اوږدتیا, new_معلومات_اوږدتیا);
            }
        } وإلا إذا (flags[i] == MIXED) {
            ثابت uint32_t *بلاک = معلومات + شاخص[i];
            int32_t n = mixed_بلاکونه.پیدا_کړه_بلاک(new_معلومات, بلاک, 0);
            إذا (n >= 0) {
                DEBUG_DO(++شمېر_Same);
                شاخص[i] = n;
            } وإلا {
                n = getOverlap(new_معلومات, new_معلومات_اوږدتیا, بلاک, 0, بلاک_اوږدتیا);
                DEBUG_DO(مجموعه_Overlaps += n);
#که_تعریف_شوی UCPTRIE_DEBUG
                إذا (print_معلومات) {
                    print_بلاک(بلاک, بلاک_اوږدتیا, 0, i << UCPTRIE_ښويدنه_3, n, initial_ارزښت);
                }
#د_که_پای
                شاخص[i] = new_معلومات_اوږدتیا - n;
                int32_t prev_معلومات_اوږدتیا = new_معلومات_اوږدتیا;
                طالما (n < بلاک_اوږدتیا) {
                    new_معلومات[new_معلومات_اوږدتیا++] = بلاک[n++];
                }
                mixed_بلاکونه.extend(new_معلومات, 0, prev_معلومات_اوږدتیا, new_معلومات_اوږدتیا);
            }
        } وإلا /* SAME_AS */ {
            uint32_t j = شاخص[i];
            شاخص[i] = شاخص[j];
        }
    }

#که_تعریف_شوی UCPTRIE_DEBUG
    /* we saved some space */
    چاپ_کړه("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (اوږد)معلومات_اوږدتیا, (اوږد)new_معلومات_اوږدتیا, (اوږد)شمېر_Same, (اوږد)مجموعه_Overlaps);
#د_که_پای
    أرجع new_معلومات_اوږدتیا;
}

int32_t Mutable_کوډ_ټکی_Trie::compact_شاخص(int32_t fastILimit, Mixed_بلاکونه &mixed_بلاکونه,
                                           UError_کوډ &error_کوډ) {
    int32_t fast_شاخص_اوږدتیا = fastILimit >> (UCPTRIE_FAST_ښويدنه - UCPTRIE_ښويدنه_3);
    إذا ((لوړ_پیل >> UCPTRIE_FAST_ښويدنه) <= fast_شاخص_اوږدتیا) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_دننه_واټن = UCPTRIE_NO_INDEX3_NULL_دننه_واټن;
        أرجع fast_شاخص_اوږدتیا;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_شاخص[UCPTRIE_BMP_شاخص_اوږدتیا];  // fastIndexLength
    int32_t i3_لومړی_Null = -1;
    لكل (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = شاخص[i];
        fast_شاخص[j] = static_cast<uint16_t>(i3);
        إذا (i3 == static_cast<uint32_t>(معلومات_Null_دننه_واټن)) {
            إذا (i3_لومړی_Null < 0) {
                i3_لومړی_Null = j;
            } وإلا إذا (index3_Null_دننه_واټن < 0 &&
                    (j - i3_لومړی_Null + 1) == UCPTRIE_شاخص_3_بلاک_اوږدتیا) {
                index3_Null_دننه_واټن = i3_لومړی_Null;
            }
        } وإلا {
            i3_لومړی_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_بل = i + SMALL_معلومات_بلاکونه_فی_BMP_بلاک;
        طالما (++i < i_بل) {
            i3 += UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا;
            شاخص[i] = i3;
        }
    }

    إذا (!mixed_بلاکونه.init(fast_شاخص_اوږدتیا, UCPTRIE_شاخص_3_بلاک_اوږدتیا)) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    mixed_بلاکونه.extend(fast_شاخص, 0, 0, fast_شاخص_اوږدتیا);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_ظرفیت = 0;
    i3_لومړی_Null = index3_Null_دننه_واټن;
    منطقي has_Long_I3_بلاکونه = خطأ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_پیل = fastILimit < BMP_I_حد ? 0 : BMP_I_حد;
    int32_t i_حد = لوړ_پیل >> UCPTRIE_ښويدنه_3;
    لكل (int32_t i = i_پیل; i < i_حد;) {
        int32_t j = i;
        int32_t j_حد = i + UCPTRIE_شاخص_3_بلاک_اوږدتیا;
        uint32_t oredI3 = 0;
        منطقي isNull = ریښتیا;
        افعل {
            uint32_t i3 = شاخص[j];
            oredI3 |= i3;
            إذا (i3 != static_cast<uint32_t>(معلومات_Null_دننه_واټن)) {
                isNull = خطأ;
            }
        } طالما (++j < j_حد);
        إذا (isNull) {
            flags[i] = I3_NULL;
            إذا (i3_لومړی_Null < 0) {
                إذا (oredI3 <= 0xffff) {
                    index3_ظرفیت += UCPTRIE_شاخص_3_بلاک_اوږدتیا;
                } وإلا {
                    index3_ظرفیت += شاخص_3_18_بټ_بلاک_اوږدتیا;
                    has_Long_I3_بلاکونه = ریښتیا;
                }
                i3_لومړی_Null = 0;
            }
        } وإلا {
            إذا (oredI3 <= 0xffff) {
                int32_t n = mixed_بلاکونه.پیدا_کړه_بلاک(fast_شاخص, شاخص, i);
                إذا (n >= 0) {
                    flags[i] = I3_BMP;
                    شاخص[i] = n;
                } وإلا {
                    flags[i] = I3_16;
                    index3_ظرفیت += UCPTRIE_شاخص_3_بلاک_اوږدتیا;
                }
            } وإلا {
                flags[i] = I3_18;
                index3_ظرفیت += شاخص_3_18_بټ_بلاک_اوږدتیا;
                has_Long_I3_بلاکونه = ریښتیا;
            }
        }
        i = j;
    }

    int32_t index2_ظرفیت = (i_حد - i_پیل) >> UCPTRIE_ښويدنه_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_اوږدتیا = (index2_ظرفیت + UCPTRIE_شاخص_2_ماسک) >> UCPTRIE_ښويدنه_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_ظرفیت = fast_شاخص_اوږدتیا + index1_اوږدتیا + index3_ظرفیت + index2_ظرفیت + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_ظرفیت * 2));
    إذا (index16 == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    uprv_memcpy(index16, fast_شاخص, fast_شاخص_اوږدتیا * 2);

    إذا (!mixed_بلاکونه.init(index16_ظرفیت, UCPTRIE_شاخص_3_بلاک_اوږدتیا)) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    Mixed_بلاکونه long_I3_بلاکونه;
    إذا (has_Long_I3_بلاکونه) {
        إذا (!long_I3_بلاکونه.init(index16_ظرفیت, شاخص_3_18_بټ_بلاک_اوږدتیا)) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_حد >> UCPTRIE_ښويدنه_2];  // index2Capacity
    int32_t i2_اوږدتیا = 0;
    i3_لومړی_Null = index3_Null_دننه_واټن;
    int32_t index3_پیل = fast_شاخص_اوږدتیا + index1_اوږدتیا;
    int32_t شاخص_اوږدتیا = index3_پیل;
    لكل (int32_t i = i_پیل; i < i_حد; i += UCPTRIE_شاخص_3_بلاک_اوږدتیا) {
        int32_t i3;
        uint8_t f = flags[i];
        إذا (f == I3_NULL && i3_لومړی_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = معلومات_Null_دننه_واټن <= 0xffff ? I3_16 : I3_18;
            i3_لومړی_Null = 0;
        }
        إذا (f == I3_NULL) {
            i3 = index3_Null_دننه_واټن;
        } وإلا إذا (f == I3_BMP) {
            i3 = شاخص[i];
        } وإلا إذا (f == I3_16) {
            int32_t n = mixed_بلاکونه.پیدا_کړه_بلاک(index16, شاخص, i);
            إذا (n >= 0) {
                i3 = n;
            } وإلا {
                إذا (شاخص_اوږدتیا == index3_پیل) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, شاخص_اوږدتیا,
                                   شاخص, i, UCPTRIE_شاخص_3_بلاک_اوږدتیا);
                }
                i3 = شاخص_اوږدتیا - n;
                int32_t prev_شاخص_اوږدتیا = شاخص_اوږدتیا;
                طالما (n < UCPTRIE_شاخص_3_بلاک_اوږدتیا) {
                    index16[شاخص_اوږدتیا++] = شاخص[i + n++];
                }
                mixed_بلاکونه.extend(index16, index3_پیل, prev_شاخص_اوږدتیا, شاخص_اوږدتیا);
                إذا (has_Long_I3_بلاکونه) {
                    long_I3_بلاکونه.extend(index16, index3_پیل, prev_شاخص_اوږدتیا, شاخص_اوږدتیا);
                }
            }
        } وإلا {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_بلاکونه);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_حد = i + UCPTRIE_شاخص_3_بلاک_اوږدتیا;
            int32_t k = شاخص_اوږدتیا;
            افعل {
                ++k;
                uint32_t v = شاخص[j++];
                uint32_t پاسنی_بټونه = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = شاخص[j++];
                پاسنی_بټونه |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = پاسنی_بټونه;
            } طالما (j < j_حد);
            int32_t n = long_I3_بلاکونه.پیدا_کړه_بلاک(index16, index16, شاخص_اوږدتیا);
            إذا (n >= 0) {
                i3 = n | 0x8000;
            } وإلا {
                إذا (شاخص_اوږدتیا == index3_پیل) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } وإلا {
                    n = getOverlap(index16, شاخص_اوږدتیا,
                                   index16, شاخص_اوږدتیا, شاخص_3_18_بټ_بلاک_اوږدتیا);
                }
                i3 = (شاخص_اوږدتیا - n) | 0x8000;
                int32_t prev_شاخص_اوږدتیا = شاخص_اوږدتیا;
                إذا (n > 0) {
                    int32_t پیل = شاخص_اوږدتیا;
                    طالما (n < شاخص_3_18_بټ_بلاک_اوږدتیا) {
                        index16[شاخص_اوږدتیا++] = index16[پیل + n++];
                    }
                } وإلا {
                    شاخص_اوږدتیا += شاخص_3_18_بټ_بلاک_اوږدتیا;
                }
                mixed_بلاکونه.extend(index16, index3_پیل, prev_شاخص_اوږدتیا, شاخص_اوږدتیا);
                إذا (has_Long_I3_بلاکونه) {
                    long_I3_بلاکونه.extend(index16, index3_پیل, prev_شاخص_اوږدتیا, شاخص_اوږدتیا);
                }
            }
        }
        إذا (index3_Null_دننه_واټن < 0 && i3_لومړی_Null >= 0) {
            index3_Null_دننه_واټن = i3;
        }
        // Set the index-2 table entry.
        index2[i2_اوږدتیا++] = i3;
    }
    U_ASSERT(i2_اوږدتیا == index2_ظرفیت);
    U_ASSERT(شاخص_اوږدتیا <= index3_پیل + index3_ظرفیت);

    إذا (index3_Null_دننه_واټن < 0) {
        index3_Null_دننه_واټن = UCPTRIE_NO_INDEX3_NULL_دننه_واټن;
    }
    إذا (شاخص_اوږدتیا >= (UCPTRIE_NO_INDEX3_NULL_دننه_واټن + UCPTRIE_شاخص_3_بلاک_اوږدتیا)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_شاخص_2_بلاک_اوږدتیا == UCPTRIE_شاخص_3_بلاک_اوږدتیا,
                  "must re-init mixedBlocks");
    int32_t بلاک_اوږدتیا = UCPTRIE_شاخص_2_بلاک_اوږدتیا;
    int32_t i1 = fast_شاخص_اوږدتیا;
    لكل (int32_t i = 0; i < i2_اوږدتیا; i += بلاک_اوږدتیا) {
        int32_t n;
        إذا ((i2_اوږدتیا - i) >= بلاک_اوږدتیا) {
            // normal block
            U_ASSERT(بلاک_اوږدتیا == UCPTRIE_شاخص_2_بلاک_اوږدتیا);
            n = mixed_بلاکونه.پیدا_کړه_بلاک(index16, index2, i);
        } وإلا {
            // highStart is inside the last index-2 block. Shorten it.
            بلاک_اوږدتیا = i2_اوږدتیا - i;
            n = پیدا_کړه_Same_بلاک(index16, index3_پیل, شاخص_اوږدتیا,
                              index2, i, بلاک_اوږدتیا);
        }
        int32_t i2;
        إذا (n >= 0) {
            i2 = n;
        } وإلا {
            إذا (شاخص_اوږدتیا == index3_پیل) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } وإلا {
                n = getOverlap(index16, شاخص_اوږدتیا, index2, i, بلاک_اوږدتیا);
            }
            i2 = شاخص_اوږدتیا - n;
            int32_t prev_شاخص_اوږدتیا = شاخص_اوږدتیا;
            طالما (n < بلاک_اوږدتیا) {
                index16[شاخص_اوږدتیا++] = index2[i + n++];
            }
            mixed_بلاکونه.extend(index16, index3_پیل, prev_شاخص_اوږدتیا, شاخص_اوږدتیا);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_پیل);
    U_ASSERT(شاخص_اوږدتیا <= index16_ظرفیت);

#که_تعریف_شوی UCPTRIE_DEBUG
    /* we saved some space */
    چاپ_کړه("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (اوږد)i_حد, (اوږد)شاخص_اوږدتیا);
#د_که_پای

    أرجع شاخص_اوږدتیا;
}

int32_t Mutable_کوډ_ټکی_Trie::compactTrie(int32_t fastILimit, UError_کوډ &error_کوډ) {
    // Find the real highStart and round it up.
    U_ASSERT((لوړ_پیل & (UCPTRIE_CP_فی_شاخص_2_ثبت - 1)) == 0);
    لوړ_ارزښت = get(MAX_UNICODE);
    int32_t real_لوړ_پیل = پیدا_کړه_لوړ_پیل();
    real_لوړ_پیل = (real_لوړ_پیل + (UCPTRIE_CP_فی_شاخص_2_ثبت - 1)) &
        ~(UCPTRIE_CP_فی_شاخص_2_ثبت - 1);
    إذا (real_لوړ_پیل == UNICODE_حد) {
        لوړ_ارزښت = initial_ارزښت;
    }

#که_تعریف_شوی UCPTRIE_DEBUG
    چاپ_کړه("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (اوږد)real_لوړ_پیل, (اوږد)لوړ_ارزښت, (اوږد)initial_ارزښت);
#د_که_پای

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_حد = fastILimit << UCPTRIE_ښويدنه_3;
    إذا (real_لوړ_پیل < fast_حد) {
        لكل (int32_t i = (real_لوړ_پیل >> UCPTRIE_ښويدنه_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            شاخص[i] = لوړ_ارزښت;
        }
        لوړ_پیل = fast_حد;
    } وإلا {
        لوړ_پیل = real_لوړ_پیل;
    }

    uint32_t ascii_معلومات[ASCII_حد];
    لكل (int32_t i = 0; i < ASCII_حد; ++i) {
        ascii_معلومات[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_بلاکونه all_Same_بلاکونه;
    int32_t new_معلومات_ظرفیت = compact_ټولیز_معلومات_بلاکونه(fastILimit, all_Same_بلاکونه);
    إذا (new_معلومات_ظرفیت < 0) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    uint32_t* new_معلومات = static_cast<uint32_t*>(uprv_malloc(new_معلومات_ظرفیت * 4));
    إذا (new_معلومات == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    uprv_memcpy(new_معلومات, ascii_معلومات, حجم(ascii_معلومات));

    int32_t معلومات_Null_شاخص = all_Same_بلاکونه.پیدا_کړه_Most_کارول_شوی();

    Mixed_بلاکونه mixed_بلاکونه;
    int32_t new_معلومات_اوږدتیا = compact_معلومات(fastILimit, new_معلومات, new_معلومات_ظرفیت,
                                        معلومات_Null_شاخص, mixed_بلاکونه, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع 0; }
    U_ASSERT(new_معلومات_اوږدتیا <= new_معلومات_ظرفیت);
    uprv_free(معلومات);
    معلومات = new_معلومات;
    معلومات_ظرفیت = new_معلومات_ظرفیت;
    معلومات_اوږدتیا = new_معلومات_اوږدتیا;
    إذا (معلومات_اوږدتیا > (0x3ffff + UCPTRIE_SMALL_معلومات_بلاک_اوږدتیا)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا (معلومات_Null_شاخص >= 0) {
        معلومات_Null_دننه_واټن = شاخص[معلومات_Null_شاخص];
#که_تعریف_شوی UCPTRIE_DEBUG
        إذا (معلومات[معلومات_Null_دننه_واټن] != initial_ارزښت) {
            چاپ_کړه("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (اوږد)initial_ارزښت, (اوږد)معلومات[معلومات_Null_دننه_واټن]);
        }
#د_که_پای
        initial_ارزښت = معلومات[معلومات_Null_دننه_واټن];
    } وإلا {
        معلومات_Null_دننه_واټن = UCPTRIE_NO_معلومات_NULL_دننه_واټن;
    }

    int32_t شاخص_اوږدتیا = compact_شاخص(fastILimit, mixed_بلاکونه, error_کوډ);
    لوړ_پیل = real_لوړ_پیل;
    أرجع شاخص_اوږدتیا;
}

UCPTrie *Mutable_کوډ_ټکی_Trie::build(UCPTrieType type, UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ارزښت_پلنوالی < UCPTRIE_ارزښت_بټونه_16 || UCPTRIE_ارزښت_بټونه_8 < ارزښت_پلنوالی) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    بدل (ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_32:
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_16:
        ماسک_ارزښتونه(0xffff);
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        ماسک_ارزښتونه(0xff);
        اكسر;
    افتراضي:
        اكسر;
    }

    UChar32 fast_حد = type == UCPTRIE_TYPE_FAST ? BMP_حد : UCPTRIE_SMALL_حد;
    int32_t شاخص_اوږدتیا = compactTrie(fast_حد >> UCPTRIE_ښويدنه_3, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) {
        clear();
        أرجع تش_اشاره;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    إذا (ارزښت_پلنوالی == UCPTRIE_ارزښت_بټونه_32 && (شاخص_اوږدتیا & 1) != 0) {
        index16[شاخص_اوږدتیا++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = شاخص_اوږدتیا * 2;
    إذا (ارزښت_پلنوالی == UCPTRIE_ارزښت_بټونه_16) {
        إذا (((شاخص_اوږدتیا ^ معلومات_اوږدتیا) & 1) != 0) {
            // padding
            معلومات[معلومات_اوږدتیا++] = error_ارزښت;
        }
        إذا (معلومات[معلومات_اوږدتیا - 1] != error_ارزښت || معلومات[معلومات_اوږدتیا - 2] != لوړ_ارزښت) {
            معلومات[معلومات_اوږدتیا++] = لوړ_ارزښت;
            معلومات[معلومات_اوږدتیا++] = error_ارزښت;
        }
        length += معلومات_اوږدتیا * 2;
    } وإلا إذا (ارزښت_پلنوالی == UCPTRIE_ارزښت_بټونه_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        إذا (معلومات[معلومات_اوږدتیا - 1] != error_ارزښت || معلومات[معلومات_اوږدتیا - 2] != لوړ_ارزښت) {
            إذا (معلومات[معلومات_اوږدتیا - 1] != لوړ_ارزښت) {
                معلومات[معلومات_اوږدتیا++] = لوړ_ارزښت;
            }
            معلومات[معلومات_اوږدتیا++] = error_ارزښت;
        }
        length += معلومات_اوږدتیا * 4;
    } وإلا {
        int32_t and3 = (length + معلومات_اوږدتیا) & 3;
        إذا (and3 == 0 && معلومات[معلومات_اوږدتیا - 1] == error_ارزښت && معلومات[معلومات_اوږدتیا - 2] == لوړ_ارزښت) {
            // all set
        } وإلا إذا(and3 == 3 && معلومات[معلومات_اوږدتیا - 1] == لوړ_ارزښت) {
            معلومات[معلومات_اوږدتیا++] = error_ارزښت;
        } وإلا {
            طالما (and3 != 2) {
                معلومات[معلومات_اوږدتیا++] = لوړ_ارزښت;
                and3 = (and3 + 1) & 3;
            }
            معلومات[معلومات_اوږدتیا++] = لوړ_ارزښت;
            معلومات[معلومات_اوږدتیا++] = error_ارزښت;
        }
        length += معلومات_اوږدتیا;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += حجم(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* بایټونه = static_cast<uint8_t*>(uprv_malloc(length));
    إذا (بایټونه == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        clear();
        أرجع تش_اشاره;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(بایټونه);
    uprv_memset(trie, 0, حجم(UCPTrie));
    trie->شاخص_اوږدتیا = شاخص_اوږدتیا;
    trie->معلومات_اوږدتیا = معلومات_اوږدتیا;

    trie->لوړ_پیل = لوړ_پیل;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_لوړ_پیل = (لوړ_پیل + 0xfff) >> 12;
    trie->type = type;
    trie->ارزښت_پلنوالی = ارزښت_پلنوالی;

    trie->index3_Null_دننه_واټن = index3_Null_دننه_واټن;
    trie->معلومات_Null_دننه_واټن = معلومات_Null_دننه_واټن;
    trie->null_ارزښت = initial_ارزښت;

    بایټونه += حجم(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(بایټونه);
    trie->شاخص = dest16;

    إذا (لوړ_پیل <= fast_حد) {
        // Condense only the fast index from the mutable-trie index.
        لكل (int32_t i = 0, j = 0; j < شاخص_اوږدتیا; i += SMALL_معلومات_بلاکونه_فی_BMP_بلاک, ++j) {
            *dest16++ = static_cast<uint16_t>(شاخص[i]); // dest16[j]
        }
    } وإلا {
        uprv_memcpy(dest16, index16, شاخص_اوږدتیا * 2);
        dest16 += شاخص_اوږدتیا;
    }
    بایټونه += شاخص_اوږدتیا * 2;

    // Write the data array.
    ثابت uint32_t *p = معلومات;
    بدل (ارزښت_پلنوالی) {
    حالة UCPTRIE_ارزښت_بټونه_16:
        // Write 16-bit data values.
        trie->معلومات.ptr16 = dest16;
        لكل (int32_t i = معلومات_اوږدتیا; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_32:
        // Write 32-bit data values.
        trie->معلومات.ptr32 = reinterpret_cast<uint32_t*>(بایټونه);
        uprv_memcpy(بایټونه, p, (size_t)معلومات_اوږدتیا * 4);
        اكسر;
    حالة UCPTRIE_ارزښت_بټونه_8:
        // Write 8-bit data values.
        trie->معلومات.ptr8 = بایټونه;
        لكل (int32_t i = معلومات_اوږدتیا; i > 0; --i) {
            *بایټونه++ = static_cast<uint8_t>(*p++);
        }
        اكسر;
    افتراضي:
        // Will not occur, valueWidth checked at the beginning.
        اكسر;
    }

#که_تعریف_شوی UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#د_که_پای

    clear();
    أرجع trie;
}

}  // namespace

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_ارزښت, uint32_t error_ارزښت, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    Local_اشاره_ګر<Mutable_کوډ_ټکی_Trie> trie(
        جديد Mutable_کوډ_ټکی_Trie(initial_ارزښت, error_ارزښت, *p_Error_کوډ), *p_Error_کوډ);
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ثابت UMutableCPTrie *other, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا (other == تش_اشاره) {
        أرجع تش_اشاره;
    }
    Local_اشاره_ګر<Mutable_کوډ_ټکی_Trie> clone(
        جديد Mutable_کوډ_ټکی_Trie(*reinterpret_cast<ثابت Mutable_کوډ_ټکی_Trie *>(other), *p_Error_کوډ), *p_Error_کوډ);
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    ړنګول reinterpret_cast<Mutable_کوډ_ټکی_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ثابت UCPMap *map, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا (map == تش_اشاره) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_کوډ_ټکی_Trie::fromUCPMap(map, *p_Error_کوډ));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ثابت UCPTrie *trie, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا (trie == تش_اشاره) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<UMutableCPTrie *>(Mutable_کوډ_ټکی_Trie::fromUCPTrie(trie, *p_Error_کوډ));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ثابت UMutableCPTrie *trie, UChar32 c) {
    أرجع reinterpret_cast<ثابت Mutable_کوډ_ټکی_Trie *>(trie)->get(c);
}

نطاق {

UChar32 getRange(ثابت فراغ *trie, UChar32 پیل,
                 UCPMap_ارزښت_Filter *filter, ثابت فراغ *context, uint32_t *p_ارزښت) {
    أرجع reinterpret_cast<ثابت Mutable_کوډ_ټکی_Trie *>(trie)->
        getRange(پیل, filter, context, p_ارزښت);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ثابت UMutableCPTrie *trie, UChar32 پیل,
                        UCPMapRangeOption option, uint32_t surrogate_ارزښت,
                        UCPMap_ارزښت_Filter *filter, ثابت فراغ *context, uint32_t *p_ارزښت) {
    أرجع ucptrie_دننی_Get_Range(getRange, trie, پیل,
                                    option, surrogate_ارزښت,
                                    filter, context, p_ارزښت);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_سېټ(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    reinterpret_cast<Mutable_کوډ_ټکی_Trie *>(trie)->سېټ(c, value, *p_Error_کوډ);
}

U_CAPI فراغ U_EXPORT2
umutablecptrie_سېټ_Range(UMutableCPTrie *trie, UChar32 پیل, UChar32 پای_ټکی,
                   uint32_t value, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    reinterpret_cast<Mutable_کوډ_ټکی_Trie *>(trie)->سېټ_Range(پیل, پای_ټکی, value, *p_Error_کوډ);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی,
                              UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<Mutable_کوډ_ټکی_Trie *>(trie)->build(type, ارزښت_پلنوالی, *p_Error_کوډ);
}

#که_تعریف_شوی UCPTRIE_DEBUG
U_CFUNC فراغ umutablecptrie_سېټ_Name(UMutableCPTrie *trie, ثابت حرف *name) {
    reinterpret_cast<Mutable_کوډ_ټکی_Trie *>(trie)->name = name;
}
#د_که_پای
