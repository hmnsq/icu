// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#यदि_परिभाषितम् UCPTRIE_DEBUG
#   include <stdio.h>
#यदि_अन्तः

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#यदि_परिभाषितम् अतिप्रवाहः_2
#अपरिभाषय अतिप्रवाहः_2
#यदि_अन्तः

U_NAMESPACE_BEGIN

नामाकाश {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_सीमा = 0x110000;
constexpr int32_t BMP_सीमा = 0x10000;
constexpr int32_t ASCII_सीमा = 0x80;

constexpr int32_t I_सीमा = UNICODE_सीमा >> UCPTRIE_सरणम्_3;
constexpr int32_t BMP_I_सीमा = BMP_सीमा >> UCPTRIE_सरणम्_3;
constexpr int32_t ASCII_I_सीमा = ASCII_सीमा >> UCPTRIE_सरणम्_3;

constexpr int32_t SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम् = (1 << (UCPTRIE_FAST_सरणम् - UCPTRIE_सरणम्_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_दत्त_दीर्घता = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_दत्त_दीर्घता = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_दत्त_दीर्घता = UNICODE_सीमा;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t सूचकांक_3_18_BIT_खण्डम्_दीर्घता = UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता + UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता / 8;

वर्ग All_Same_खण्डानि;
वर्ग Mixed_खण्डानि;

वर्ग Mutable_संहिता_Point_Trie : सार्वजनिक UMemory {
सार्वजनिक:
    Mutable_संहिता_Point_Trie(uint32_t initial_मानम्, uint32_t error_मानम्, UError_संहिता &error_संहिता);
    Mutable_संहिता_Point_Trie(ध्रुव Mutable_संहिता_Point_Trie &other, UError_संहिता &error_संहिता);
    Mutable_संहिता_Point_Trie(ध्रुव Mutable_संहिता_Point_Trie &other) = विलोपय;
    ~Mutable_संहिता_Point_Trie();

    Mutable_संहिता_Point_Trie &संक्रियकः=(ध्रुव Mutable_संहिता_Point_Trie &other) = विलोपय;

    स्थिर Mutable_संहिता_Point_Trie *fromUCPMap(ध्रुव UCPMap *map, UError_संहिता &error_संहिता);
    स्थिर Mutable_संहिता_Point_Trie *fromUCPTrie(ध्रुव UCPTrie *trie, UError_संहिता &error_संहिता);

    uint32_t get(UChar32 c) ध्रुव;
    int32_t getRange(UChar32 start, UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context,
                     uint32_t *p_मानम्) ध्रुव;

    शून्य समुच्चयः(UChar32 c, uint32_t value, UError_संहिता &error_संहिता);
    शून्य समुच्चयः_Range(UChar32 start, UChar32 अन्तः, uint32_t value, UError_संहिता &error_संहिता);

    UCPTrie *build(UCPTrieType type, UCPTrie_मानम्_विस्तारः मानम्_विस्तारः, UError_संहिता &error_संहिता);

गुप्त:
    शून्य clear();

    तार्किक ensure_उच्चः_Start(UChar32 c);
    int32_t alloc_दत्त_खण्डम्(int32_t खण्डम्_दीर्घता);
    int32_t get_दत्त_खण्डम्(int32_t i);

    शून्य आवरणम्_मानानि(uint32_t mask);
    UChar32 find_उच्चः_Start() ध्रुव;
    int32_t compact_सम्पूर्णम्_दत्त_खण्डानि(int32_t fastILimit, All_Same_खण्डानि &all_Same_खण्डानि);
    int32_t compact_दत्त(
            int32_t fastILimit, uint32_t *new_दत्त, int32_t new_दत्त_क्षमता,
            int32_t दत्त_Null_सूचकांक, Mixed_खण्डानि &mixed_खण्डानि, UError_संहिता &error_संहिता);
    int32_t compact_सूचकांक(int32_t fastILimit, Mixed_खण्डानि &mixed_खण्डानि, UError_संहिता &error_संहिता);
    int32_t compactTrie(int32_t fastILimit, UError_संहिता &error_संहिता);

    uint32_t *सूचकांक = शून्यसूचकः;
    int32_t सूचकांक_क्षमता = 0;
    int32_t index3_Null_विस्थापनम् = -1;
    uint32_t *दत्त = शून्यसूचकः;
    int32_t दत्त_क्षमता = 0;
    int32_t दत्त_दीर्घता = 0;
    int32_t दत्त_Null_विस्थापनम् = -1;

    uint32_t orig_Initial_मानम्;
    uint32_t initial_मानम्;
    uint32_t error_मानम्;
    UChar32 उच्चः_Start;
    uint32_t उच्चः_मानम्;
#यदि_परिभाषितम् UCPTRIE_DEBUG
सार्वजनिक:
    ध्रुव वर्ण *name;
#यदि_अन्तः
गुप्त:
    /** Temporary array while building the final data. */
    uint16_t *index16 = शून्यसूचकः;
    uint8_t flags[UNICODE_सीमा >> UCPTRIE_सरणम्_3];
};

Mutable_संहिता_Point_Trie::Mutable_संहिता_Point_Trie(uint32_t ini_मानम्, uint32_t err_मानम्, UError_संहिता &error_संहिता) :
        orig_Initial_मानम्(ini_मानम्), initial_मानम्(ini_मानम्), error_मानम्(err_मानम्),
        उच्चः_Start(0), उच्चः_मानम्(initial_मानम्)
#यदि_परिभाषितम् UCPTRIE_DEBUG
        , name("open")
#यदि_अन्तः
        {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    सूचकांक = static_cast<uint32_t*>(uprv_malloc(BMP_I_सीमा * 4));
    दत्त = static_cast<uint32_t*>(uprv_malloc(INITIAL_दत्त_दीर्घता * 4));
    यदि (सूचकांक == शून्यसूचकः || दत्त == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    सूचकांक_क्षमता = BMP_I_सीमा;
    दत्त_क्षमता = INITIAL_दत्त_दीर्घता;
}

Mutable_संहिता_Point_Trie::Mutable_संहिता_Point_Trie(ध्रुव Mutable_संहिता_Point_Trie &other, UError_संहिता &error_संहिता) :
        index3_Null_विस्थापनम्(other.index3_Null_विस्थापनम्),
        दत्त_Null_विस्थापनम्(other.दत्त_Null_विस्थापनम्),
        orig_Initial_मानम्(other.orig_Initial_मानम्), initial_मानम्(other.initial_मानम्),
        error_मानम्(other.error_मानम्),
        उच्चः_Start(other.उच्चः_Start), उच्चः_मानम्(other.उच्चः_मानम्)
#यदि_परिभाषितम् UCPTRIE_DEBUG
        , name("mutable clone")
#यदि_अन्तः
        {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    int32_t i_क्षमता = उच्चः_Start <= BMP_सीमा ? BMP_I_सीमा : I_सीमा;
    सूचकांक = static_cast<uint32_t*>(uprv_malloc(i_क्षमता * 4));
    दत्त = static_cast<uint32_t*>(uprv_malloc(other.दत्त_क्षमता * 4));
    यदि (सूचकांक == शून्यसूचकः || दत्त == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    सूचकांक_क्षमता = i_क्षमता;
    दत्त_क्षमता = other.दत्त_क्षमता;

    int32_t i_सीमा = उच्चः_Start >> UCPTRIE_सरणम्_3;
    uprv_memcpy(flags, other.flags, i_सीमा);
    uprv_memcpy(सूचकांक, other.सूचकांक, i_सीमा * 4);
    uprv_memcpy(दत्त, other.दत्त, (size_t)other.दत्त_दीर्घता * 4);
    दत्त_दीर्घता = other.दत्त_दीर्घता;
    U_ASSERT(other.index16 == शून्यसूचकः);
}

Mutable_संहिता_Point_Trie::~Mutable_संहिता_Point_Trie() {
    uprv_free(सूचकांक);
    uprv_free(दत्त);
    uprv_free(index16);
}

Mutable_संहिता_Point_Trie *Mutable_संहिता_Point_Trie::fromUCPMap(ध्रुव UCPMap *map, UError_संहिता &error_संहिता) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मानम् = ucpmap_get(map, -1);
    uint32_t initial_मानम् = ucpmap_get(map, 0x10ffff);
    LocalPointer<Mutable_संहिता_Point_Trie> mutableTrie(
        नूतन Mutable_संहिता_Point_Trie(initial_मानम्, error_मानम्, error_संहिता),
        error_संहिता);
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    UChar32 start = 0, अन्तः;
    uint32_t value;
    यावत् ((अन्तः = ucpmap_getRange(map, start, UCPMAP_RANGE_प्रसामान्यम्, 0,
                                  शून्यसूचकः, शून्यसूचकः, &value)) >= 0) {
        यदि (value != initial_मानम्) {
            यदि (start == अन्तः) {
                mutableTrie->समुच्चयः(start, value, error_संहिता);
            } अन्यथा {
                mutableTrie->समुच्चयः_Range(start, अन्तः, value, error_संहिता);
            }
        }
        start = अन्तः + 1;
    }
    यदि (U_SUCCESS(error_संहिता)) {
        प्रत्यागम mutableTrie.orphan();
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

Mutable_संहिता_Point_Trie *Mutable_संहिता_Point_Trie::fromUCPTrie(ध्रुव UCPTrie *trie, UError_संहिता &error_संहिता) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मानम्;
    uint32_t initial_मानम्;
    विकल्प (trie->मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        error_मानम् = trie->दत्त.ptr16[trie->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्];
        initial_मानम् = trie->दत्त.ptr16[trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        error_मानम् = trie->दत्त.ptr32[trie->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्];
        initial_मानम् = trie->दत्त.ptr32[trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        error_मानम् = trie->दत्त.ptr8[trie->दत्त_दीर्घता - UCPTRIE_ERROR_मानम्_NEG_दत्त_विस्थापनम्];
        initial_मानम् = trie->दत्त.ptr8[trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्];
        भङ्ग;
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    LocalPointer<Mutable_संहिता_Point_Trie> mutableTrie(
        नूतन Mutable_संहिता_Point_Trie(initial_मानम्, error_मानम्, error_संहिता),
        error_संहिता);
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    UChar32 start = 0, अन्तः;
    uint32_t value;
    यावत् ((अन्तः = ucptrie_getRange(trie, start, UCPMAP_RANGE_प्रसामान्यम्, 0,
                                   शून्यसूचकः, शून्यसूचकः, &value)) >= 0) {
        यदि (value != initial_मानम्) {
            यदि (start == अन्तः) {
                mutableTrie->समुच्चयः(start, value, error_संहिता);
            } अन्यथा {
                mutableTrie->समुच्चयः_Range(start, अन्तः, value, error_संहिता);
            }
        }
        start = अन्तः + 1;
    }
    यदि (U_SUCCESS(error_संहिता)) {
        प्रत्यागम mutableTrie.orphan();
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

शून्य Mutable_संहिता_Point_Trie::clear() {
    index3_Null_विस्थापनम् = दत्त_Null_विस्थापनम् = -1;
    दत्त_दीर्घता = 0;
    उच्चः_मानम् = initial_मानम् = orig_Initial_मानम्;
    उच्चः_Start = 0;
    uprv_free(index16);
    index16 = शून्यसूचकः;
}

uint32_t Mutable_संहिता_Point_Trie::get(UChar32 c) ध्रुव {
    यदि (static_cast<uint32_t>(c) > MAX_UNICODE) {
        प्रत्यागम error_मानम्;
    }
    यदि (c >= उच्चः_Start) {
        प्रत्यागम उच्चः_मानम्;
    }
    int32_t i = c >> UCPTRIE_सरणम्_3;
    यदि (flags[i] == ALL_SAME) {
        प्रत्यागम सूचकांक[i];
    } अन्यथा {
        प्रत्यागम दत्त[सूचकांक[i] + (c & UCPTRIE_SMALL_दत्त_आवरणम्)];
    }
}

अन्तःस्थ uint32_t maybe_Filter_मानम्(uint32_t value, uint32_t initial_मानम्, uint32_t null_मानम्,
                                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context) {
    यदि (value == initial_मानम्) {
        value = null_मानम्;
    } अन्यथा यदि (filter != शून्यसूचकः) {
        value = filter(context, value);
    }
    प्रत्यागम value;
}

UChar32 Mutable_संहिता_Point_Trie::getRange(
        UChar32 start, UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context,
        uint32_t *p_मानम्) ध्रुव {
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE) {
        प्रत्यागम U_SENTINEL;
    }
    यदि (start >= उच्चः_Start) {
        यदि (p_मानम् != शून्यसूचकः) {
            uint32_t value = उच्चः_मानम्;
            यदि (filter != शून्यसूचकः) { value = filter(context, value); }
            *p_मानम् = value;
        }
        प्रत्यागम MAX_UNICODE;
    }
    uint32_t null_मानम् = initial_मानम्;
    यदि (filter != शून्यसूचकः) { null_मानम् = filter(context, null_मानम्); }
    UChar32 c = start;
    uint32_t trie_मानम्, value;
    तार्किक have_मानम् = असत्य;
    int32_t i = c >> UCPTRIE_सरणम्_3;
    कुरु {
        यदि (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = सूचकांक[i];
            यदि (have_मानम्) {
                यदि (trieValue2 != trie_मानम्) {
                    यदि (filter == शून्यसूचकः ||
                            maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्,
                                             filter, context) != value) {
                        प्रत्यागम c - 1;
                    }
                    trie_मानम् = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मानम् = trieValue2;
                value = maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्, filter, context);
                यदि (p_मानम् != शून्यसूचकः) { *p_मानम् = value; }
                have_मानम् = सत्य;
            }
            c = (c + UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता) & ~UCPTRIE_SMALL_दत्त_आवरणम्;
        } अन्यथा /* MIXED */ {
            int32_t di = सूचकांक[i] + (c & UCPTRIE_SMALL_दत्त_आवरणम्);
            uint32_t trieValue2 = दत्त[di];
            यदि (have_मानम्) {
                यदि (trieValue2 != trie_मानम्) {
                    यदि (filter == शून्यसूचकः ||
                            maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्,
                                             filter, context) != value) {
                        प्रत्यागम c - 1;
                    }
                    trie_मानम् = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मानम् = trieValue2;
                value = maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्, filter, context);
                यदि (p_मानम् != शून्यसूचकः) { *p_मानम् = value; }
                have_मानम् = सत्य;
            }
            यावत् ((++c & UCPTRIE_SMALL_दत्त_आवरणम्) != 0) {
                trieValue2 = दत्त[++di];
                यदि (trieValue2 != trie_मानम्) {
                    यदि (filter == शून्यसूचकः ||
                            maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्,
                                             filter, context) != value) {
                        प्रत्यागम c - 1;
                    }
                }
                trie_मानम् = trieValue2;  // may or may not help
            }
        }
        ++i;
    } यावत् (c < उच्चः_Start);
    U_ASSERT(have_मानम्);
    यदि (maybe_Filter_मानम्(उच्चः_मानम्, initial_मानम्, null_मानम्,
                         filter, context) != value) {
        प्रत्यागम c - 1;
    } अन्यथा {
        प्रत्यागम MAX_UNICODE;
    }
}

शून्य
write_खण्डम्(uint32_t *खण्डम्, uint32_t value) {
    uint32_t *सीमा = खण्डम् + UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
    यावत् (खण्डम् < सीमा) {
        *खण्डम्++ = value;
    }
}

तार्किक Mutable_संहिता_Point_Trie::ensure_उच्चः_Start(UChar32 c) {
    यदि (c >= उच्चः_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः) & ~(UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः - 1);
        int32_t i = उच्चः_Start >> UCPTRIE_सरणम्_3;
        int32_t i_सीमा = c >> UCPTRIE_सरणम्_3;
        यदि (i_सीमा > सूचकांक_क्षमता) {
            uint32_t* new_सूचकांक = static_cast<uint32_t*>(uprv_malloc(I_सीमा * 4));
            यदि (new_सूचकांक == शून्यसूचकः) { प्रत्यागम असत्य; }
            uprv_memcpy(new_सूचकांक, सूचकांक, i * 4);
            uprv_free(सूचकांक);
            सूचकांक = new_सूचकांक;
            सूचकांक_क्षमता = I_सीमा;
        }
        कुरु {
            flags[i] = ALL_SAME;
            सूचकांक[i] = initial_मानम्;
        } यावत्(++i < i_सीमा);
        उच्चः_Start = c;
    }
    प्रत्यागम सत्य;
}

int32_t Mutable_संहिता_Point_Trie::alloc_दत्त_खण्डम्(int32_t खण्डम्_दीर्घता) {
    int32_t new_खण्डम् = दत्त_दीर्घता;
    int32_t new_शिखरम् = new_खण्डम् + खण्डम्_दीर्घता;
    यदि (new_शिखरम् > दत्त_क्षमता) {
        int32_t capacity;
        यदि (दत्त_क्षमता < MEDIUM_दत्त_दीर्घता) {
            capacity = MEDIUM_दत्त_दीर्घता;
        } अन्यथा यदि (दत्त_क्षमता < MAX_दत्त_दीर्घता) {
            capacity = MAX_दत्त_दीर्घता;
        } अन्यथा {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            प्रत्यागम -1;
        }
        uint32_t* new_दत्त = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        यदि (new_दत्त == शून्यसूचकः) {
            प्रत्यागम -1;
        }
        uprv_memcpy(new_दत्त, दत्त, (size_t)दत्त_दीर्घता * 4);
        uprv_free(दत्त);
        दत्त = new_दत्त;
        दत्त_क्षमता = capacity;
    }
    दत्त_दीर्घता = new_शिखरम्;
    प्रत्यागम new_खण्डम्;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_संहिता_Point_Trie::get_दत्त_खण्डम्(int32_t i) {
    यदि (flags[i] == MIXED) {
        प्रत्यागम सूचकांक[i];
    }
    यदि (i < BMP_I_सीमा) {
        int32_t new_खण्डम् = alloc_दत्त_खण्डम्(UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता);
        यदि (new_खण्डम् < 0) { प्रत्यागम new_खण्डम्; }
        int32_t iStart = i & ~(SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम् -1);
        int32_t i_सीमा = iStart + SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्;
        कुरु {
            U_ASSERT(flags[iStart] == ALL_SAME);
            write_खण्डम्(दत्त + new_खण्डम्, सूचकांक[iStart]);
            flags[iStart] = MIXED;
            सूचकांक[iStart++] = new_खण्डम्;
            new_खण्डम् += UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
        } यावत् (iStart < i_सीमा);
        प्रत्यागम सूचकांक[i];
    } अन्यथा {
        int32_t new_खण्डम् = alloc_दत्त_खण्डम्(UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता);
        यदि (new_खण्डम् < 0) { प्रत्यागम new_खण्डम्; }
        write_खण्डम्(दत्त + new_खण्डम्, सूचकांक[i]);
        flags[i] = MIXED;
        सूचकांक[i] = new_खण्डम्;
        प्रत्यागम new_खण्डम्;
    }
}

शून्य Mutable_संहिता_Point_Trie::समुच्चयः(UChar32 c, uint32_t value, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    int32_t खण्डम्;
    यदि (!ensure_उच्चः_Start(c) || (खण्डम् = get_दत्त_खण्डम्(c >> UCPTRIE_सरणम्_3)) < 0) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    दत्त[खण्डम् + (c & UCPTRIE_SMALL_दत्त_आवरणम्)] = value;
}

शून्य
fill_खण्डम्(uint32_t *खण्डम्, UChar32 start, UChar32 सीमा, uint32_t value) {
    uint32_t *p_सीमा = खण्डम् + सीमा;
    खण्डम् += start;
    यावत् (खण्डम् < p_सीमा) {
        *खण्डम्++ = value;
    }
}

शून्य Mutable_संहिता_Point_Trie::समुच्चयः_Range(UChar32 start, UChar32 अन्तः, uint32_t value, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(अन्तः) > MAX_UNICODE || start > अन्तः) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि (!ensure_उच्चः_Start(अन्तः)) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    UChar32 सीमा = अन्तः + 1;
    यदि (start & UCPTRIE_SMALL_दत्त_आवरणम्) {
        // Set partial block at [start..following block boundary[.
        int32_t खण्डम् = get_दत्त_खण्डम्(start >> UCPTRIE_सरणम्_3);
        यदि (खण्डम् < 0) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }

        UChar32 अग्र_Start = (start + UCPTRIE_SMALL_दत्त_आवरणम्) & ~UCPTRIE_SMALL_दत्त_आवरणम्;
        यदि (अग्र_Start <= सीमा) {
            fill_खण्डम्(दत्त + खण्डम्, start & UCPTRIE_SMALL_दत्त_आवरणम्, UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता,
                      value);
            start = अग्र_Start;
        } अन्यथा {
            fill_खण्डम्(दत्त + खण्डम्, start & UCPTRIE_SMALL_दत्त_आवरणम्, सीमा & UCPTRIE_SMALL_दत्त_आवरणम्,
                      value);
            प्रत्यागम;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = सीमा & UCPTRIE_SMALL_दत्त_आवरणम्;

    // Round down limit to a block boundary.
    सीमा &= ~UCPTRIE_SMALL_दत्त_आवरणम्;

    // Iterate over all-value blocks.
    यावत् (start < सीमा) {
        int32_t i = start >> UCPTRIE_सरणम्_3;
        यदि (flags[i] == ALL_SAME) {
            सूचकांक[i] = value;
        } अन्यथा /* MIXED */ {
            fill_खण्डम्(दत्त + सूचकांक[i], 0, UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता, value);
        }
        start += UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
    }

    यदि (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t खण्डम् = get_दत्त_खण्डम्(start >> UCPTRIE_सरणम्_3);
        यदि (खण्डम् < 0) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }

        fill_खण्डम्(दत्त + खण्डम्, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

शून्य Mutable_संहिता_Point_Trie::आवरणम्_मानानि(uint32_t mask) {
    initial_मानम् &= mask;
    error_मानम् &= mask;
    उच्चः_मानम् &= mask;
    int32_t i_सीमा = उच्चः_Start >> UCPTRIE_सरणम्_3;
    प्रति (int32_t i = 0; i < i_सीमा; ++i) {
        यदि (flags[i] == ALL_SAME) {
            सूचकांक[i] &= mask;
        }
    }
    प्रति (int32_t i = 0; i < दत्त_दीर्घता; ++i) {
        दत्त[i] &= mask;
    }
}

प्रतिरूपम्<प्रकारनाम UIntA, प्रकारनाम UIntB>
तार्किक equal_खण्डानि(ध्रुव UIntA *s, ध्रुव UIntB *t, int32_t length) {
    यावत् (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    प्रत्यागम length == 0;
}

तार्किक all_मानानि_Same_As(ध्रुव uint32_t *p, int32_t length, uint32_t value) {
    ध्रुव uint32_t *p_सीमा = p + length;
    यावत् (p < p_सीमा && *p == value) { ++p; }
    प्रत्यागम p == p_सीमा;
}

/** Search for an identical block. */
int32_t find_Same_खण्डम्(ध्रुव uint16_t *p, int32_t pStart, int32_t length,
                      ध्रुव uint16_t *q, int32_t qStart, int32_t खण्डम्_दीर्घता) {
    // Ensure that we do not even partially get past length.
    length -= खण्डम्_दीर्घता;

    q += qStart;
    यावत् (pStart <= length) {
        यदि (equal_खण्डानि(p + pStart, q, खण्डम्_दीर्घता)) {
            प्रत्यागम pStart;
        }
        ++pStart;
    }
    प्रत्यागम -1;
}

int32_t find_All_Same_खण्डम्(ध्रुव uint32_t *p, int32_t start, int32_t सीमा,
                         uint32_t value, int32_t खण्डम्_दीर्घता) {
    // Ensure that we do not even partially get past limit.
    सीमा -= खण्डम्_दीर्घता;

    प्रति (int32_t खण्डम् = start; खण्डम् <= सीमा; ++खण्डम्) {
        यदि (p[खण्डम्] == value) {
            प्रति (int32_t i = 1;; ++i) {
                यदि (i == खण्डम्_दीर्घता) {
                    प्रत्यागम खण्डम्;
                }
                यदि (p[खण्डम् + i] != value) {
                    खण्डम् += i;
                    भङ्ग;
                }
            }
        }
    }
    प्रत्यागम -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
प्रतिरूपम्<प्रकारनाम UIntA, प्रकारनाम UIntB>
int32_t getOverlap(ध्रुव UIntA *p, int32_t length,
                   ध्रुव UIntB *q, int32_t qStart, int32_t खण्डम्_दीर्घता) {
    int32_t overlap = खण्डम्_दीर्घता - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    यावत् (overlap > 0 && !equal_खण्डानि(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    प्रत्यागम overlap;
}

int32_t getAllSameOverlap(ध्रुव uint32_t *p, int32_t length, uint32_t value,
                          int32_t खण्डम्_दीर्घता) {
    int32_t min = length - (खण्डम्_दीर्घता - 1);
    int32_t i = length;
    यावत् (min < i && p[i - 1] == value) { --i; }
    प्रत्यागम length - i;
}

तार्किक is_Start_सम्बन्धि_Some_Fast_खण्डम्(uint32_t दत्त_विस्थापनम्, ध्रुव uint32_t सूचकांक[], int32_t fastILimit) {
    प्रति (int32_t i = 0; i < fastILimit; i += SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्) {
        यदि (सूचकांक[i] == दत्त_विस्थापनम्) {
            प्रत्यागम सत्य;
        }
    }
    प्रत्यागम असत्य;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_संहिता_Point_Trie::find_उच्चः_Start() ध्रुव {
    int32_t i = उच्चः_Start >> UCPTRIE_सरणम्_3;
    यावत् (i > 0) {
        तार्किक मेलनम्;
        यदि (flags[--i] == ALL_SAME) {
            मेलनम् = सूचकांक[i] == उच्चः_मानम्;
        } अन्यथा /* MIXED */ {
            ध्रुव uint32_t *p = दत्त + सूचकांक[i];
            प्रति (int32_t j = 0;; ++j) {
                यदि (j == UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता) {
                    मेलनम् = सत्य;
                    भङ्ग;
                }
                यदि (p[j] != उच्चः_मानम्) {
                    मेलनम् = असत्य;
                    भङ्ग;
                }
            }
        }
        यदि (!मेलनम्) {
            प्रत्यागम (i + 1) << UCPTRIE_सरणम्_3;
        }
    }
    प्रत्यागम 0;
}

वर्ग All_Same_खण्डानि {
सार्वजनिक:
    स्थिर constexpr int32_t NEW_UNIQUE = -1;
    स्थिर constexpr int32_t अतिप्रवाहः_2 = -2;

    All_Same_खण्डानि() : length(0), mostRecent(-1) {}

    int32_t find_Or_सङ्कलय(int32_t सूचकांक, int32_t count, uint32_t value) {
        यदि (mostRecent >= 0 && मानानि[mostRecent] == value) {
            refCounts[mostRecent] += count;
            प्रत्यागम indexes[mostRecent];
        }
        प्रति (int32_t i = 0; i < length; ++i) {
            यदि (मानानि[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                प्रत्यागम indexes[i];
            }
        }
        यदि (length == क्षमता_3) {
            प्रत्यागम अतिप्रवाहः_2;
        }
        mostRecent = length;
        indexes[length] = सूचकांक;
        मानानि[length] = value;
        refCounts[length++] = count;
        प्रत्यागम NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    शून्य सङ्कलय(int32_t सूचकांक, int32_t count, uint32_t value) {
        U_ASSERT(length == क्षमता_3);
        int32_t least = -1;
        int32_t न्यूनतमम्_गणनम् = I_सीमा;
        प्रति (int32_t i = 0; i < length; ++i) {
            U_ASSERT(मानानि[i] != value);
            यदि (refCounts[i] < न्यूनतमम्_गणनम्) {
                least = i;
                न्यूनतमम्_गणनम् = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = सूचकांक;
        मानानि[least] = value;
        refCounts[least] = count;
    }

    int32_t find_Most_प्रयुक्तम्() ध्रुव {
        यदि (length == 0) { प्रत्यागम -1; }
        int32_t max = -1;
        int32_t max_गणनम् = 0;
        प्रति (int32_t i = 0; i < length; ++i) {
            यदि (refCounts[i] > max_गणनम्) {
                max = i;
                max_गणनम् = refCounts[i];
            }
        }
        प्रत्यागम indexes[max];
    }

गुप्त:
    स्थिर constexpr int32_t क्षमता_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[क्षमता_3];
    uint32_t मानानि[क्षमता_3];
    int32_t refCounts[क्षमता_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
वर्ग Mixed_खण्डानि {
सार्वजनिक:
    Mixed_खण्डानि() {}
    ~Mixed_खण्डानि() {
        uprv_free(सारणी);
    }

    तार्किक init(int32_t max_दीर्घता_2, int32_t new_खण्डम्_दीर्घता) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_दत्त_सूचकांक = max_दीर्घता_2 - new_खण्डम्_दीर्घता + 1;
        int32_t new_दीर्घता;
        यदि (max_दत्त_सूचकांक <= 0xfff) {  // 4k
            new_दीर्घता = 6007;
            सरणम् = 12;
            mask = 0xfff;
        } अन्यथा यदि (max_दत्त_सूचकांक <= 0x7fff) {  // 32k
            new_दीर्घता = 50021;
            सरणम् = 15;
            mask = 0x7fff;
        } अन्यथा यदि (max_दत्त_सूचकांक <= 0x1ffff) {  // 128k
            new_दीर्घता = 200003;
            सरणम् = 17;
            mask = 0x1ffff;
        } अन्यथा {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_दीर्घता = 1500007;
            सरणम् = 21;
            mask = 0x1fffff;
        }
        यदि (new_दीर्घता > capacity) {
            uprv_free(सारणी);
            सारणी = static_cast<uint32_t*>(uprv_malloc(new_दीर्घता * 4));
            यदि (सारणी == शून्यसूचकः) {
                प्रत्यागम असत्य;
            }
            capacity = new_दीर्घता;
        }
        length = new_दीर्घता;
        uprv_memset(सारणी, 0, length * 4);

        खण्डम्_दीर्घता = new_खण्डम्_दीर्घता;
        प्रत्यागम सत्य;
    }

    प्रतिरूपम्<प्रकारनाम UInt>
    शून्य extend(ध्रुव UInt *दत्त, int32_t minStart, int32_t पूर्व_दत्त_दीर्घता, int32_t new_दत्त_दीर्घता) {
        int32_t start = पूर्व_दत्त_दीर्घता - खण्डम्_दीर्घता;
        यदि (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } अन्यथा {
            start = minStart;  // Begin with the first full block.
        }
        प्रति (int32_t अन्तः = new_दत्त_दीर्घता - खण्डम्_दीर्घता; start <= अन्तः; ++start) {
            uint32_t hash_संहिता = make_Hash_संहिता(दत्त, start);
            सङ्कलय_प्रविष्टिः(दत्त, start, hash_संहिता, start);
        }
    }

    प्रतिरूपम्<प्रकारनाम UIntA, प्रकारनाम UIntB>
    int32_t find_खण्डम्(ध्रुव UIntA *दत्त, ध्रुव UIntB *खण्डम्_दत्त, int32_t खण्डम्_Start) ध्रुव {
        uint32_t hash_संहिता = make_Hash_संहिता(खण्डम्_दत्त, खण्डम्_Start);
        int32_t प्रविष्टिः_सूचकांक = find_प्रविष्टिः(दत्त, खण्डम्_दत्त, खण्डम्_Start, hash_संहिता);
        यदि (प्रविष्टिः_सूचकांक >= 0) {
            प्रत्यागम (सारणी[प्रविष्टिः_सूचकांक] & mask) - 1;
        } अन्यथा {
            प्रत्यागम -1;
        }
    }

    int32_t find_All_Same_खण्डम्(ध्रुव uint32_t *दत्त, uint32_t खण्डम्_मानम्) ध्रुव {
        uint32_t hash_संहिता = make_Hash_संहिता(खण्डम्_मानम्);
        int32_t प्रविष्टिः_सूचकांक = find_प्रविष्टिः(दत्त, खण्डम्_मानम्, hash_संहिता);
        यदि (प्रविष्टिः_सूचकांक >= 0) {
            प्रत्यागम (सारणी[प्रविष्टिः_सूचकांक] & mask) - 1;
        } अन्यथा {
            प्रत्यागम -1;
        }
    }

गुप्त:
    प्रतिरूपम्<प्रकारनाम UInt>
    uint32_t make_Hash_संहिता(ध्रुव UInt *खण्डम्_दत्त, int32_t खण्डम्_Start) ध्रुव {
        int32_t खण्डम्_सीमा = खण्डम्_Start + खण्डम्_दीर्घता;
        uint32_t hash_संहिता = खण्डम्_दत्त[खण्डम्_Start++];
        कुरु {
            hash_संहिता = 37 * hash_संहिता + खण्डम्_दत्त[खण्डम्_Start++];
        } यावत् (खण्डम्_Start < खण्डम्_सीमा);
        प्रत्यागम hash_संहिता;
    }

    uint32_t make_Hash_संहिता(uint32_t खण्डम्_मानम्) ध्रुव {
        uint32_t hash_संहिता = खण्डम्_मानम्;
        प्रति (int32_t i = 1; i < खण्डम्_दीर्घता; ++i) {
            hash_संहिता = 37 * hash_संहिता + खण्डम्_मानम्;
        }
        प्रत्यागम hash_संहिता;
    }

    प्रतिरूपम्<प्रकारनाम UInt>
    शून्य सङ्कलय_प्रविष्टिः(ध्रुव UInt *दत्त, int32_t खण्डम्_Start, uint32_t hash_संहिता, int32_t दत्त_सूचकांक) {
        U_ASSERT(0 <= दत्त_सूचकांक && दत्त_सूचकांक < (int32_t)mask);
        int32_t प्रविष्टिः_सूचकांक = find_प्रविष्टिः(दत्त, दत्त, खण्डम्_Start, hash_संहिता);
        यदि (प्रविष्टिः_सूचकांक < 0) {
            सारणी[~प्रविष्टिः_सूचकांक] = (hash_संहिता << सरणम्) | (दत्त_सूचकांक + 1);
        }
    }

    प्रतिरूपम्<प्रकारनाम UIntA, प्रकारनाम UIntB>
    int32_t find_प्रविष्टिः(ध्रुव UIntA *दत्त, ध्रुव UIntB *खण्डम्_दत्त, int32_t खण्डम्_Start,
                      uint32_t hash_संहिता) ध्रुव {
        uint32_t shifted_Hash_संहिता = hash_संहिता << सरणम्;
        int32_t initial_प्रविष्टिः_सूचकांक = (hash_संहिता % (length - 1)) + 1;  // 1..length-1
        प्रति (int32_t प्रविष्टिः_सूचकांक = initial_प्रविष्टिः_सूचकांक;;) {
            uint32_t प्रविष्टिः = सारणी[प्रविष्टिः_सूचकांक];
            यदि (प्रविष्टिः == 0) {
                प्रत्यागम ~प्रविष्टिः_सूचकांक;
            }
            यदि ((प्रविष्टिः & ~mask) == shifted_Hash_संहिता) {
                int32_t दत्त_सूचकांक = (प्रविष्टिः & mask) - 1;
                यदि (equal_खण्डानि(दत्त + दत्त_सूचकांक, खण्डम्_दत्त + खण्डम्_Start, खण्डम्_दीर्घता)) {
                    प्रत्यागम प्रविष्टिः_सूचकांक;
                }
            }
            प्रविष्टिः_सूचकांक = अग्र_सूचकांक(initial_प्रविष्टिः_सूचकांक, प्रविष्टिः_सूचकांक);
        }
    }

    int32_t find_प्रविष्टिः(ध्रुव uint32_t *दत्त, uint32_t खण्डम्_मानम्, uint32_t hash_संहिता) ध्रुव {
        uint32_t shifted_Hash_संहिता = hash_संहिता << सरणम्;
        int32_t initial_प्रविष्टिः_सूचकांक = (hash_संहिता % (length - 1)) + 1;  // 1..length-1
        प्रति (int32_t प्रविष्टिः_सूचकांक = initial_प्रविष्टिः_सूचकांक;;) {
            uint32_t प्रविष्टिः = सारणी[प्रविष्टिः_सूचकांक];
            यदि (प्रविष्टिः == 0) {
                प्रत्यागम ~प्रविष्टिः_सूचकांक;
            }
            यदि ((प्रविष्टिः & ~mask) == shifted_Hash_संहिता) {
                int32_t दत्त_सूचकांक = (प्रविष्टिः & mask) - 1;
                यदि (all_मानानि_Same_As(दत्त + दत्त_सूचकांक, खण्डम्_दीर्घता, खण्डम्_मानम्)) {
                    प्रत्यागम प्रविष्टिः_सूचकांक;
                }
            }
            प्रविष्टिः_सूचकांक = अग्र_सूचकांक(initial_प्रविष्टिः_सूचकांक, प्रविष्टिः_सूचकांक);
        }
    }

    अन्तःस्थ int32_t अग्र_सूचकांक(int32_t initial_प्रविष्टिः_सूचकांक, int32_t प्रविष्टिः_सूचकांक) ध्रुव {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        प्रत्यागम (प्रविष्टिः_सूचकांक + initial_प्रविष्टिः_सूचकांक) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *सारणी = शून्यसूचकः;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t सरणम् = 0;
    uint32_t mask = 0;

    int32_t खण्डम्_दीर्घता = 0;
};

int32_t Mutable_संहिता_Point_Trie::compact_सम्पूर्णम्_दत्त_खण्डानि(int32_t fastILimit, All_Same_खण्डानि &all_Same_खण्डानि) {
#यदि_परिभाषितम् UCPTRIE_DEBUG
    तार्किक अतिप्रवाहः = असत्य;
#यदि_अन्तः

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_दत्त_क्षमता = ASCII_सीमा;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_दत्त_क्षमता += UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
    // Add room for special values (errorValue, highValue) and padding.
    new_दत्त_क्षमता += 4;
    int32_t i_सीमा = उच्चः_Start >> UCPTRIE_सरणम्_3;
    int32_t खण्डम्_दीर्घता = UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता;
    int32_t inc = SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्;
    प्रति (int32_t i = 0; i < i_सीमा; i += inc) {
        यदि (i == fastILimit) {
            खण्डम्_दीर्घता = UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
            inc = 1;
        }
        uint32_t value = सूचकांक[i];
        यदि (flags[i] == MIXED) {
            // Really mixed?
            ध्रुव uint32_t *p = दत्त + value;
            value = *p;
            यदि (all_मानानि_Same_As(p + 1, खण्डम्_दीर्घता - 1, value)) {
                flags[i] = ALL_SAME;
                सूचकांक[i] = value;
                // Fall through to ALL_SAME handling.
            } अन्यथा {
                new_दत्त_क्षमता += खण्डम्_दीर्घता;
                अनुवर्त;
            }
        } अन्यथा {
            U_ASSERT(flags[i] == ALL_SAME);
            यदि (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                तार्किक allSame = सत्य;
                int32_t अग्र_i = i + inc;
                प्रति (int32_t j = i + 1; j < अग्र_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    यदि (सूचकांक[j] != value) {
                        allSame = असत्य;
                        भङ्ग;
                    }
                }
                यदि (!allSame) {
                    // Turn it into a MIXED block.
                    यदि (get_दत्त_खण्डम्(i) < 0) {
                        प्रत्यागम -1;
                    }
                    new_दत्त_क्षमता += खण्डम्_दीर्घता;
                    अनुवर्त;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_खण्डानि.find_Or_सङ्कलय(i, inc, value);
        यदि (other == All_Same_खण्डानि::अतिप्रवाहः_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#यदि_परिभाषितम् UCPTRIE_DEBUG
            यदि (!अतिप्रवाहः) {
                पङ्क्तिं_लिख("UCPTrie AllSameBlocks overflow");
                अतिप्रवाहः = सत्य;
            }
#यदि_अन्तः
            int32_t jInc = SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्;
            प्रति (int32_t j = 0;; j += jInc) {
                यदि (j == i) {
                    all_Same_खण्डानि.सङ्कलय(i, inc, value);
                    भङ्ग;
                }
                यदि (j == fastILimit) {
                    jInc = 1;
                }
                यदि (flags[j] == ALL_SAME && सूचकांक[j] == value) {
                    all_Same_खण्डानि.सङ्कलय(j, jInc + inc, value);
                    other = j;
                    भङ्ग;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        यदि (other >= 0) {
            flags[i] = SAME_AS;
            सूचकांक[i] = other;
        } अन्यथा {
            // New unique same-value block.
            new_दत्त_क्षमता += खण्डम्_दीर्घता;
        }
    }
    प्रत्यागम new_दत्त_क्षमता;
}

#यदि_परिभाषितम् UCPTRIE_DEBUG
#   define DEBUG_DO(व्यञ्जकम्) व्यञ्जकम्
#अन्यथा
#   define DEBUG_DO(व्यञ्जकम्)
#यदि_अन्तः

#यदि_परिभाषितम् UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_मानम्(वर्ण s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (वर्ण)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (वर्ण)(0x80 + (value & 0x3F));
    प्रत्यागम length + 3;
}

शून्य print_खण्डम्(ध्रुव uint32_t *खण्डम्, int32_t खण्डम्_दीर्घता, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_मानम्) {
    वर्ण s[UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता * 3 + 3];
    int32_t length = 0;
    int32_t i;
    प्रति (i = 0; i < overlap; ++i) {
        length = append_मानम्(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    प्रति (; i < खण्डम्_दीर्घता; ++i) {
        यदि (खण्डम् != शून्यसूचकः) {
            value = खण्डम्[i];
        }
        यदि (value == initial_मानम्) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_मानम्(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    यदि (start <= 0xffff) {
        प्रारूप_लिख("    %04lX  %s|\n", (दीर्घ)start, s);
    } अन्यथा यदि (start <= 0xfffff) {
        प्रारूप_लिख("   %5lX  %s|\n", (दीर्घ)start, s);
    } अन्यथा {
        प्रारूप_लिख("  %6lX  %s|\n", (दीर्घ)start, s);
    }
}
#यदि_अन्तः

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
int32_t Mutable_संहिता_Point_Trie::compact_दत्त(
        int32_t fastILimit, uint32_t *new_दत्त, int32_t new_दत्त_क्षमता,
        int32_t दत्त_Null_सूचकांक, Mixed_खण्डानि &mixed_खण्डानि, UError_संहिता &error_संहिता) {
#यदि_परिभाषितम् UCPTRIE_DEBUG
    int32_t गणनम्_Same=0, योग_Overlaps=0;
    तार्किक print_दत्त = दत्त_दीर्घता == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        दत्त_दीर्घता == 50400 /* zh.txt~stroke */;
#यदि_अन्तः

    // The linear ASCII data has been copied into newData already.
    int32_t new_दत्त_दीर्घता = 0;
    प्रति (int32_t i = 0; new_दत्त_दीर्घता < ASCII_सीमा;
            new_दत्त_दीर्घता += UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता, i += SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्) {
        सूचकांक[i] = new_दत्त_दीर्घता;
#यदि_परिभाषितम् UCPTRIE_DEBUG
        यदि (print_दत्त) {
            print_खण्डम्(new_दत्त + new_दत्त_दीर्घता, UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता, 0, new_दत्त_दीर्घता, 0, initial_मानम्);
        }
#यदि_अन्तः
    }

    int32_t खण्डम्_दीर्घता = UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता;
    यदि (!mixed_खण्डानि.init(new_दत्त_क्षमता, खण्डम्_दीर्घता)) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    mixed_खण्डानि.extend(new_दत्त, 0, 0, new_दत्त_दीर्घता);

    int32_t i_सीमा = उच्चः_Start >> UCPTRIE_सरणम्_3;
    int32_t inc = SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्;
    int32_t fast_दीर्घता = 0;
    प्रति (int32_t i = ASCII_I_सीमा; i < i_सीमा; i += inc) {
        यदि (i == fastILimit) {
            खण्डम्_दीर्घता = UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
            inc = 1;
            fast_दीर्घता = new_दत्त_दीर्घता;
            यदि (!mixed_खण्डानि.init(new_दत्त_क्षमता, खण्डम्_दीर्घता)) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम 0;
            }
            mixed_खण्डानि.extend(new_दत्त, 0, 0, new_दत्त_दीर्घता);
        }
        यदि (flags[i] == ALL_SAME) {
            uint32_t value = सूचकांक[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_खण्डानि.find_All_Same_खण्डम्(new_दत्त, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            यावत् (n >= 0 && i == दत्त_Null_सूचकांक && i >= fastILimit && n < fast_दीर्घता &&
                    is_Start_सम्बन्धि_Some_Fast_खण्डम्(n, सूचकांक, fastILimit)) {
                n = find_All_Same_खण्डम्(new_दत्त, n + 1, new_दत्त_दीर्घता, value, खण्डम्_दीर्घता);
            }
            यदि (n >= 0) {
                DEBUG_DO(++गणनम्_Same);
                सूचकांक[i] = n;
            } अन्यथा {
                n = getAllSameOverlap(new_दत्त, new_दत्त_दीर्घता, value, खण्डम्_दीर्घता);
                DEBUG_DO(योग_Overlaps += n);
#यदि_परिभाषितम् UCPTRIE_DEBUG
                यदि (print_दत्त) {
                    print_खण्डम्(शून्यसूचकः, खण्डम्_दीर्घता, value, i << UCPTRIE_सरणम्_3, n, initial_मानम्);
                }
#यदि_अन्तः
                सूचकांक[i] = new_दत्त_दीर्घता - n;
                int32_t पूर्व_दत्त_दीर्घता = new_दत्त_दीर्घता;
                यावत् (n < खण्डम्_दीर्घता) {
                    new_दत्त[new_दत्त_दीर्घता++] = value;
                    ++n;
                }
                mixed_खण्डानि.extend(new_दत्त, 0, पूर्व_दत्त_दीर्घता, new_दत्त_दीर्घता);
            }
        } अन्यथा यदि (flags[i] == MIXED) {
            ध्रुव uint32_t *खण्डम् = दत्त + सूचकांक[i];
            int32_t n = mixed_खण्डानि.find_खण्डम्(new_दत्त, खण्डम्, 0);
            यदि (n >= 0) {
                DEBUG_DO(++गणनम्_Same);
                सूचकांक[i] = n;
            } अन्यथा {
                n = getOverlap(new_दत्त, new_दत्त_दीर्घता, खण्डम्, 0, खण्डम्_दीर्घता);
                DEBUG_DO(योग_Overlaps += n);
#यदि_परिभाषितम् UCPTRIE_DEBUG
                यदि (print_दत्त) {
                    print_खण्डम्(खण्डम्, खण्डम्_दीर्घता, 0, i << UCPTRIE_सरणम्_3, n, initial_मानम्);
                }
#यदि_अन्तः
                सूचकांक[i] = new_दत्त_दीर्घता - n;
                int32_t पूर्व_दत्त_दीर्घता = new_दत्त_दीर्घता;
                यावत् (n < खण्डम्_दीर्घता) {
                    new_दत्त[new_दत्त_दीर्घता++] = खण्डम्[n++];
                }
                mixed_खण्डानि.extend(new_दत्त, 0, पूर्व_दत्त_दीर्घता, new_दत्त_दीर्घता);
            }
        } अन्यथा /* SAME_AS */ {
            uint32_t j = सूचकांक[i];
            सूचकांक[i] = सूचकांक[j];
        }
    }

#यदि_परिभाषितम् UCPTRIE_DEBUG
    /* we saved some space */
    प्रारूप_लिख("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (दीर्घ)दत्त_दीर्घता, (दीर्घ)new_दत्त_दीर्घता, (दीर्घ)गणनम्_Same, (दीर्घ)योग_Overlaps);
#यदि_अन्तः
    प्रत्यागम new_दत्त_दीर्घता;
}

int32_t Mutable_संहिता_Point_Trie::compact_सूचकांक(int32_t fastILimit, Mixed_खण्डानि &mixed_खण्डानि,
                                           UError_संहिता &error_संहिता) {
    int32_t fast_सूचकांक_दीर्घता = fastILimit >> (UCPTRIE_FAST_सरणम् - UCPTRIE_सरणम्_3);
    यदि ((उच्चः_Start >> UCPTRIE_FAST_सरणम्) <= fast_सूचकांक_दीर्घता) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_विस्थापनम् = UCPTRIE_NO_INDEX3_NULL_विस्थापनम्;
        प्रत्यागम fast_सूचकांक_दीर्घता;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_सूचकांक[UCPTRIE_BMP_सूचकांक_दीर्घता];  // fastIndexLength
    int32_t i3FirstNull = -1;
    प्रति (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = सूचकांक[i];
        fast_सूचकांक[j] = static_cast<uint16_t>(i3);
        यदि (i3 == static_cast<uint32_t>(दत्त_Null_विस्थापनम्)) {
            यदि (i3FirstNull < 0) {
                i3FirstNull = j;
            } अन्यथा यदि (index3_Null_विस्थापनम् < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता) {
                index3_Null_विस्थापनम् = i3FirstNull;
            }
        } अन्यथा {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_अग्र = i + SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्;
        यावत् (++i < i_अग्र) {
            i3 += UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
            सूचकांक[i] = i3;
        }
    }

    यदि (!mixed_खण्डानि.init(fast_सूचकांक_दीर्घता, UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता)) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    mixed_खण्डानि.extend(fast_सूचकांक, 0, 0, fast_सूचकांक_दीर्घता);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_क्षमता = 0;
    i3FirstNull = index3_Null_विस्थापनम्;
    तार्किक has_Long_I3_खण्डानि = असत्य;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_सीमा ? 0 : BMP_I_सीमा;
    int32_t i_सीमा = उच्चः_Start >> UCPTRIE_सरणम्_3;
    प्रति (int32_t i = iStart; i < i_सीमा;) {
        int32_t j = i;
        int32_t j_सीमा = i + UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता;
        uint32_t oredI3 = 0;
        तार्किक isNull = सत्य;
        कुरु {
            uint32_t i3 = सूचकांक[j];
            oredI3 |= i3;
            यदि (i3 != static_cast<uint32_t>(दत्त_Null_विस्थापनम्)) {
                isNull = असत्य;
            }
        } यावत् (++j < j_सीमा);
        यदि (isNull) {
            flags[i] = I3_NULL;
            यदि (i3FirstNull < 0) {
                यदि (oredI3 <= 0xffff) {
                    index3_क्षमता += UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता;
                } अन्यथा {
                    index3_क्षमता += सूचकांक_3_18_BIT_खण्डम्_दीर्घता;
                    has_Long_I3_खण्डानि = सत्य;
                }
                i3FirstNull = 0;
            }
        } अन्यथा {
            यदि (oredI3 <= 0xffff) {
                int32_t n = mixed_खण्डानि.find_खण्डम्(fast_सूचकांक, सूचकांक, i);
                यदि (n >= 0) {
                    flags[i] = I3_BMP;
                    सूचकांक[i] = n;
                } अन्यथा {
                    flags[i] = I3_16;
                    index3_क्षमता += UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता;
                }
            } अन्यथा {
                flags[i] = I3_18;
                index3_क्षमता += सूचकांक_3_18_BIT_खण्डम्_दीर्घता;
                has_Long_I3_खण्डानि = सत्य;
            }
        }
        i = j;
    }

    int32_t index2_क्षमता = (i_सीमा - iStart) >> UCPTRIE_सरणम्_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_दीर्घता = (index2_क्षमता + UCPTRIE_सूचकांक_2_आवरणम्) >> UCPTRIE_सरणम्_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_क्षमता = fast_सूचकांक_दीर्घता + index1_दीर्घता + index3_क्षमता + index2_क्षमता + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_क्षमता * 2));
    यदि (index16 == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    uprv_memcpy(index16, fast_सूचकांक, fast_सूचकांक_दीर्घता * 2);

    यदि (!mixed_खण्डानि.init(index16_क्षमता, UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता)) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    Mixed_खण्डानि long_I3_खण्डानि;
    यदि (has_Long_I3_खण्डानि) {
        यदि (!long_I3_खण्डानि.init(index16_क्षमता, सूचकांक_3_18_BIT_खण्डम्_दीर्घता)) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_सीमा >> UCPTRIE_सरणम्_2];  // index2Capacity
    int32_t i2_दीर्घता = 0;
    i3FirstNull = index3_Null_विस्थापनम्;
    int32_t index3Start = fast_सूचकांक_दीर्घता + index1_दीर्घता;
    int32_t सूचकांक_दीर्घता = index3Start;
    प्रति (int32_t i = iStart; i < i_सीमा; i += UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता) {
        int32_t i3;
        uint8_t f = flags[i];
        यदि (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = दत्त_Null_विस्थापनम् <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        यदि (f == I3_NULL) {
            i3 = index3_Null_विस्थापनम्;
        } अन्यथा यदि (f == I3_BMP) {
            i3 = सूचकांक[i];
        } अन्यथा यदि (f == I3_16) {
            int32_t n = mixed_खण्डानि.find_खण्डम्(index16, सूचकांक, i);
            यदि (n >= 0) {
                i3 = n;
            } अन्यथा {
                यदि (सूचकांक_दीर्घता == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अन्यथा {
                    n = getOverlap(index16, सूचकांक_दीर्घता,
                                   सूचकांक, i, UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता);
                }
                i3 = सूचकांक_दीर्घता - n;
                int32_t पूर्व_सूचकांक_दीर्घता = सूचकांक_दीर्घता;
                यावत् (n < UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता) {
                    index16[सूचकांक_दीर्घता++] = सूचकांक[i + n++];
                }
                mixed_खण्डानि.extend(index16, index3Start, पूर्व_सूचकांक_दीर्घता, सूचकांक_दीर्घता);
                यदि (has_Long_I3_खण्डानि) {
                    long_I3_खण्डानि.extend(index16, index3Start, पूर्व_सूचकांक_दीर्घता, सूचकांक_दीर्घता);
                }
            }
        } अन्यथा {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_खण्डानि);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_सीमा = i + UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता;
            int32_t k = सूचकांक_दीर्घता;
            कुरु {
                ++k;
                uint32_t v = सूचकांक[j++];
                uint32_t ऊर्ध्वस्थम्_द्वयङ्काः = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = सूचकांक[j++];
                ऊर्ध्वस्थम्_द्वयङ्काः |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = ऊर्ध्वस्थम्_द्वयङ्काः;
            } यावत् (j < j_सीमा);
            int32_t n = long_I3_खण्डानि.find_खण्डम्(index16, index16, सूचकांक_दीर्घता);
            यदि (n >= 0) {
                i3 = n | 0x8000;
            } अन्यथा {
                यदि (सूचकांक_दीर्घता == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अन्यथा {
                    n = getOverlap(index16, सूचकांक_दीर्घता,
                                   index16, सूचकांक_दीर्घता, सूचकांक_3_18_BIT_खण्डम्_दीर्घता);
                }
                i3 = (सूचकांक_दीर्घता - n) | 0x8000;
                int32_t पूर्व_सूचकांक_दीर्घता = सूचकांक_दीर्घता;
                यदि (n > 0) {
                    int32_t start = सूचकांक_दीर्घता;
                    यावत् (n < सूचकांक_3_18_BIT_खण्डम्_दीर्घता) {
                        index16[सूचकांक_दीर्घता++] = index16[start + n++];
                    }
                } अन्यथा {
                    सूचकांक_दीर्घता += सूचकांक_3_18_BIT_खण्डम्_दीर्घता;
                }
                mixed_खण्डानि.extend(index16, index3Start, पूर्व_सूचकांक_दीर्घता, सूचकांक_दीर्घता);
                यदि (has_Long_I3_खण्डानि) {
                    long_I3_खण्डानि.extend(index16, index3Start, पूर्व_सूचकांक_दीर्घता, सूचकांक_दीर्घता);
                }
            }
        }
        यदि (index3_Null_विस्थापनम् < 0 && i3FirstNull >= 0) {
            index3_Null_विस्थापनम् = i3;
        }
        // Set the index-2 table entry.
        index2[i2_दीर्घता++] = i3;
    }
    U_ASSERT(i2_दीर्घता == index2_क्षमता);
    U_ASSERT(सूचकांक_दीर्घता <= index3Start + index3_क्षमता);

    यदि (index3_Null_विस्थापनम् < 0) {
        index3_Null_विस्थापनम् = UCPTRIE_NO_INDEX3_NULL_विस्थापनम्;
    }
    यदि (सूचकांक_दीर्घता >= (UCPTRIE_NO_INDEX3_NULL_विस्थापनम् + UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_सूचकांक_2_खण्डम्_दीर्घता == UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता,
                  "must re-init mixedBlocks");
    int32_t खण्डम्_दीर्घता = UCPTRIE_सूचकांक_2_खण्डम्_दीर्घता;
    int32_t i1 = fast_सूचकांक_दीर्घता;
    प्रति (int32_t i = 0; i < i2_दीर्घता; i += खण्डम्_दीर्घता) {
        int32_t n;
        यदि ((i2_दीर्घता - i) >= खण्डम्_दीर्घता) {
            // normal block
            U_ASSERT(खण्डम्_दीर्घता == UCPTRIE_सूचकांक_2_खण्डम्_दीर्घता);
            n = mixed_खण्डानि.find_खण्डम्(index16, index2, i);
        } अन्यथा {
            // highStart is inside the last index-2 block. Shorten it.
            खण्डम्_दीर्घता = i2_दीर्घता - i;
            n = find_Same_खण्डम्(index16, index3Start, सूचकांक_दीर्घता,
                              index2, i, खण्डम्_दीर्घता);
        }
        int32_t i2;
        यदि (n >= 0) {
            i2 = n;
        } अन्यथा {
            यदि (सूचकांक_दीर्घता == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } अन्यथा {
                n = getOverlap(index16, सूचकांक_दीर्घता, index2, i, खण्डम्_दीर्घता);
            }
            i2 = सूचकांक_दीर्घता - n;
            int32_t पूर्व_सूचकांक_दीर्घता = सूचकांक_दीर्घता;
            यावत् (n < खण्डम्_दीर्घता) {
                index16[सूचकांक_दीर्घता++] = index2[i + n++];
            }
            mixed_खण्डानि.extend(index16, index3Start, पूर्व_सूचकांक_दीर्घता, सूचकांक_दीर्घता);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3Start);
    U_ASSERT(सूचकांक_दीर्घता <= index16_क्षमता);

#यदि_परिभाषितम् UCPTRIE_DEBUG
    /* we saved some space */
    प्रारूप_लिख("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (दीर्घ)i_सीमा, (दीर्घ)सूचकांक_दीर्घता);
#यदि_अन्तः

    प्रत्यागम सूचकांक_दीर्घता;
}

int32_t Mutable_संहिता_Point_Trie::compactTrie(int32_t fastILimit, UError_संहिता &error_संहिता) {
    // Find the real highStart and round it up.
    U_ASSERT((उच्चः_Start & (UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः - 1)) == 0);
    उच्चः_मानम् = get(MAX_UNICODE);
    int32_t real_उच्चः_Start = find_उच्चः_Start();
    real_उच्चः_Start = (real_उच्चः_Start + (UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः - 1)) &
        ~(UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः - 1);
    यदि (real_उच्चः_Start == UNICODE_सीमा) {
        उच्चः_मानम् = initial_मानम्;
    }

#यदि_परिभाषितम् UCPTRIE_DEBUG
    प्रारूप_लिख("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (दीर्घ)real_उच्चः_Start, (दीर्घ)उच्चः_मानम्, (दीर्घ)initial_मानम्);
#यदि_अन्तः

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_सीमा = fastILimit << UCPTRIE_सरणम्_3;
    यदि (real_उच्चः_Start < fast_सीमा) {
        प्रति (int32_t i = (real_उच्चः_Start >> UCPTRIE_सरणम्_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            सूचकांक[i] = उच्चः_मानम्;
        }
        उच्चः_Start = fast_सीमा;
    } अन्यथा {
        उच्चः_Start = real_उच्चः_Start;
    }

    uint32_t ascii_दत्त[ASCII_सीमा];
    प्रति (int32_t i = 0; i < ASCII_सीमा; ++i) {
        ascii_दत्त[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_खण्डानि all_Same_खण्डानि;
    int32_t new_दत्त_क्षमता = compact_सम्पूर्णम्_दत्त_खण्डानि(fastILimit, all_Same_खण्डानि);
    यदि (new_दत्त_क्षमता < 0) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    uint32_t* new_दत्त = static_cast<uint32_t*>(uprv_malloc(new_दत्त_क्षमता * 4));
    यदि (new_दत्त == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    uprv_memcpy(new_दत्त, ascii_दत्त, परिमाण(ascii_दत्त));

    int32_t दत्त_Null_सूचकांक = all_Same_खण्डानि.find_Most_प्रयुक्तम्();

    Mixed_खण्डानि mixed_खण्डानि;
    int32_t new_दत्त_दीर्घता = compact_दत्त(fastILimit, new_दत्त, new_दत्त_क्षमता,
                                        दत्त_Null_सूचकांक, mixed_खण्डानि, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम 0; }
    U_ASSERT(new_दत्त_दीर्घता <= new_दत्त_क्षमता);
    uprv_free(दत्त);
    दत्त = new_दत्त;
    दत्त_क्षमता = new_दत्त_क्षमता;
    दत्त_दीर्घता = new_दत्त_दीर्घता;
    यदि (दत्त_दीर्घता > (0x3ffff + UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    यदि (दत्त_Null_सूचकांक >= 0) {
        दत्त_Null_विस्थापनम् = सूचकांक[दत्त_Null_सूचकांक];
#यदि_परिभाषितम् UCPTRIE_DEBUG
        यदि (दत्त[दत्त_Null_विस्थापनम्] != initial_मानम्) {
            प्रारूप_लिख("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (दीर्घ)initial_मानम्, (दीर्घ)दत्त[दत्त_Null_विस्थापनम्]);
        }
#यदि_अन्तः
        initial_मानम् = दत्त[दत्त_Null_विस्थापनम्];
    } अन्यथा {
        दत्त_Null_विस्थापनम् = UCPTRIE_NO_दत्त_NULL_विस्थापनम्;
    }

    int32_t सूचकांक_दीर्घता = compact_सूचकांक(fastILimit, mixed_खण्डानि, error_संहिता);
    उच्चः_Start = real_उच्चः_Start;
    प्रत्यागम सूचकांक_दीर्घता;
}

UCPTrie *Mutable_संहिता_Point_Trie::build(UCPTrieType type, UCPTrie_मानम्_विस्तारः मानम्_विस्तारः, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मानम्_विस्तारः < UCPTRIE_मानम्_द्वयङ्काः_16 || UCPTRIE_मानम्_द्वयङ्काः_8 < मानम्_विस्तारः) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    विकल्प (मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        आवरणम्_मानानि(0xffff);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        आवरणम्_मानानि(0xff);
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    UChar32 fast_सीमा = type == UCPTRIE_TYPE_FAST ? BMP_सीमा : UCPTRIE_SMALL_सीमा;
    int32_t सूचकांक_दीर्घता = compactTrie(fast_सीमा >> UCPTRIE_सरणम्_3, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) {
        clear();
        प्रत्यागम शून्यसूचकः;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    यदि (मानम्_विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_32 && (सूचकांक_दीर्घता & 1) != 0) {
        index16[सूचकांक_दीर्घता++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = सूचकांक_दीर्घता * 2;
    यदि (मानम्_विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_16) {
        यदि (((सूचकांक_दीर्घता ^ दत्त_दीर्घता) & 1) != 0) {
            // padding
            दत्त[दत्त_दीर्घता++] = error_मानम्;
        }
        यदि (दत्त[दत्त_दीर्घता - 1] != error_मानम् || दत्त[दत्त_दीर्घता - 2] != उच्चः_मानम्) {
            दत्त[दत्त_दीर्घता++] = उच्चः_मानम्;
            दत्त[दत्त_दीर्घता++] = error_मानम्;
        }
        length += दत्त_दीर्घता * 2;
    } अन्यथा यदि (मानम्_विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        यदि (दत्त[दत्त_दीर्घता - 1] != error_मानम् || दत्त[दत्त_दीर्घता - 2] != उच्चः_मानम्) {
            यदि (दत्त[दत्त_दीर्घता - 1] != उच्चः_मानम्) {
                दत्त[दत्त_दीर्घता++] = उच्चः_मानम्;
            }
            दत्त[दत्त_दीर्घता++] = error_मानम्;
        }
        length += दत्त_दीर्घता * 4;
    } अन्यथा {
        int32_t and3 = (length + दत्त_दीर्घता) & 3;
        यदि (and3 == 0 && दत्त[दत्त_दीर्घता - 1] == error_मानम् && दत्त[दत्त_दीर्घता - 2] == उच्चः_मानम्) {
            // all set
        } अन्यथा यदि(and3 == 3 && दत्त[दत्त_दीर्घता - 1] == उच्चः_मानम्) {
            दत्त[दत्त_दीर्घता++] = error_मानम्;
        } अन्यथा {
            यावत् (and3 != 2) {
                दत्त[दत्त_दीर्घता++] = उच्चः_मानम्;
                and3 = (and3 + 1) & 3;
            }
            दत्त[दत्त_दीर्घता++] = उच्चः_मानम्;
            दत्त[दत्त_दीर्घता++] = error_मानम्;
        }
        length += दत्त_दीर्घता;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += परिमाण(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* अष्टकानि = static_cast<uint8_t*>(uprv_malloc(length));
    यदि (अष्टकानि == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        clear();
        प्रत्यागम शून्यसूचकः;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(अष्टकानि);
    uprv_memset(trie, 0, परिमाण(UCPTrie));
    trie->सूचकांक_दीर्घता = सूचकांक_दीर्घता;
    trie->दत्त_दीर्घता = दत्त_दीर्घता;

    trie->उच्चः_Start = उच्चः_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_उच्चः_Start = (उच्चः_Start + 0xfff) >> 12;
    trie->type = type;
    trie->मानम्_विस्तारः = मानम्_विस्तारः;

    trie->index3_Null_विस्थापनम् = index3_Null_विस्थापनम्;
    trie->दत्त_Null_विस्थापनम् = दत्त_Null_विस्थापनम्;
    trie->null_मानम् = initial_मानम्;

    अष्टकानि += परिमाण(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(अष्टकानि);
    trie->सूचकांक = dest16;

    यदि (उच्चः_Start <= fast_सीमा) {
        // Condense only the fast index from the mutable-trie index.
        प्रति (int32_t i = 0, j = 0; j < सूचकांक_दीर्घता; i += SMALL_दत्त_खण्डानि_प्रत्येकम्_BMP_खण्डम्, ++j) {
            *dest16++ = static_cast<uint16_t>(सूचकांक[i]); // dest16[j]
        }
    } अन्यथा {
        uprv_memcpy(dest16, index16, सूचकांक_दीर्घता * 2);
        dest16 += सूचकांक_दीर्घता;
    }
    अष्टकानि += सूचकांक_दीर्घता * 2;

    // Write the data array.
    ध्रुव uint32_t *p = दत्त;
    विकल्प (मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        // Write 16-bit data values.
        trie->दत्त.ptr16 = dest16;
        प्रति (int32_t i = दत्त_दीर्घता; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        // Write 32-bit data values.
        trie->दत्त.ptr32 = reinterpret_cast<uint32_t*>(अष्टकानि);
        uprv_memcpy(अष्टकानि, p, (size_t)दत्त_दीर्घता * 4);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        // Write 8-bit data values.
        trie->दत्त.ptr8 = अष्टकानि;
        प्रति (int32_t i = दत्त_दीर्घता; i > 0; --i) {
            *अष्टकानि++ = static_cast<uint8_t>(*p++);
        }
        भङ्ग;
    स्वभाव:
        // Will not occur, valueWidth checked at the beginning.
        भङ्ग;
    }

#यदि_परिभाषितम् UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#यदि_अन्तः

    clear();
    प्रत्यागम trie;
}

}  // namespace

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_मानम्, uint32_t error_मानम्, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    LocalPointer<Mutable_संहिता_Point_Trie> trie(
        नूतन Mutable_संहिता_Point_Trie(initial_मानम्, error_मानम्, *p_Error_संहिता), *p_Error_संहिता);
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ध्रुव UMutableCPTrie *other, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (other == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    LocalPointer<Mutable_संहिता_Point_Trie> clone(
        नूतन Mutable_संहिता_Point_Trie(*reinterpret_cast<ध्रुव Mutable_संहिता_Point_Trie *>(other), *p_Error_संहिता), *p_Error_संहिता);
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    विलोपय reinterpret_cast<Mutable_संहिता_Point_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ध्रुव UCPMap *map, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (map == शून्यसूचकः) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(Mutable_संहिता_Point_Trie::fromUCPMap(map, *p_Error_संहिता));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ध्रुव UCPTrie *trie, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (trie == शून्यसूचकः) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(Mutable_संहिता_Point_Trie::fromUCPTrie(trie, *p_Error_संहिता));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ध्रुव UMutableCPTrie *trie, UChar32 c) {
    प्रत्यागम reinterpret_cast<ध्रुव Mutable_संहिता_Point_Trie *>(trie)->get(c);
}

नामाकाश {

UChar32 getRange(ध्रुव शून्य *trie, UChar32 start,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    प्रत्यागम reinterpret_cast<ध्रुव Mutable_संहिता_Point_Trie *>(trie)->
        getRange(start, filter, context, p_मानम्);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ध्रुव UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_मानम्,
                        UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    प्रत्यागम ucptrie_आन्तरिकम्_Get_Range(getRange, trie, start,
                                    option, surrogate_मानम्,
                                    filter, context, p_मानम्);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_समुच्चयः(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    reinterpret_cast<Mutable_संहिता_Point_Trie *>(trie)->समुच्चयः(c, value, *p_Error_संहिता);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_समुच्चयः_Range(UMutableCPTrie *trie, UChar32 start, UChar32 अन्तः,
                   uint32_t value, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    reinterpret_cast<Mutable_संहिता_Point_Trie *>(trie)->समुच्चयः_Range(start, अन्तः, value, *p_Error_संहिता);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_मानम्_विस्तारः मानम्_विस्तारः,
                              UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<Mutable_संहिता_Point_Trie *>(trie)->build(type, मानम्_विस्तारः, *p_Error_संहिता);
}

#यदि_परिभाषितम् UCPTRIE_DEBUG
U_CFUNC शून्य umutablecptrie_समुच्चयः_Name(UMutableCPTrie *trie, ध्रुव वर्ण *name) {
    reinterpret_cast<Mutable_संहिता_Point_Trie *>(trie)->name = name;
}
#यदि_अन्तः
