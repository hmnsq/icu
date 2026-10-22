// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#నిర్వచితమైతే UCPTRIE_DEBUG
#   include <stdio.h>
#అయితే_ముగింపు

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/umutablecptrie.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#నిర్వచితమైతే అధికప్రవాహం_2
#నిర్వచనం_రద్దు అధికప్రవాహం_2
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

నామాకాశ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_హద్దు = 0x110000;
constexpr int32_t BMP_హద్దు = 0x10000;
constexpr int32_t ASCII_హద్దు = 0x80;

constexpr int32_t I_హద్దు = UNICODE_హద్దు >> UCPTRIE_జరుపు_3;
constexpr int32_t BMP_I_హద్దు = BMP_హద్దు >> UCPTRIE_జరుపు_3;
constexpr int32_t ASCII_I_హద్దు = ASCII_హద్దు >> UCPTRIE_జరుపు_3;

constexpr int32_t SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం = (1 << (UCPTRIE_FAST_జరుపు - UCPTRIE_జరుపు_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_దత్తాంశం_నిడివి = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_దత్తాంశం_నిడివి = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_దత్తాంశం_నిడివి = UNICODE_హద్దు;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t సూచీ_3_18_బిట్టు_ఖండం_నిడివి = UCPTRIE_సూచీ_3_ఖండం_నిడివి + UCPTRIE_సూచీ_3_ఖండం_నిడివి / 8;

తరగతి All_Same_ఖండాలు;
తరగతి Mixed_ఖండాలు;

తరగతి Mutable_సంకేతం_బిందువు_Trie : బహిరంగ UMemory {
బహిరంగ:
    Mutable_సంకేతం_బిందువు_Trie(uint32_t initial_విలువ, uint32_t error_విలువ, UError_సంకేతం &error_సంకేతం);
    Mutable_సంకేతం_బిందువు_Trie(స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie &other, UError_సంకేతం &error_సంకేతం);
    Mutable_సంకేతం_బిందువు_Trie(స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie &other) = తొలగించు;
    ~Mutable_సంకేతం_బిందువు_Trie();

    Mutable_సంకేతం_బిందువు_Trie &క్రియాకారకం=(స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie &other) = తొలగించు;

    స్థిర Mutable_సంకేతం_బిందువు_Trie *fromUCPMap(స్థిరరాశి UCPMap *map, UError_సంకేతం &error_సంకేతం);
    స్థిర Mutable_సంకేతం_బిందువు_Trie *fromUCPTrie(స్థిరరాశి UCPTrie *trie, UError_సంకేతం &error_సంకేతం);

    uint32_t get(UChar32 c) స్థిరరాశి;
    int32_t getRange(UChar32 ప్రారంభం, UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context,
                     uint32_t *p_విలువ) స్థిరరాశి;

    శూన్య సమితి(UChar32 c, uint32_t value, UError_సంకేతం &error_సంకేతం);
    శూన్య సమితి_Range(UChar32 ప్రారంభం, UChar32 చివర, uint32_t value, UError_సంకేతం &error_సంకేతం);

    UCPTrie *build(UCPTrieType type, UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు, UError_సంకేతం &error_సంకేతం);

అంతరంగ:
    శూన్య clear();

    తార్కిక ensure_ఎగువ_ప్రారంభం(UChar32 c);
    int32_t alloc_దత్తాంశం_ఖండం(int32_t ఖండం_నిడివి);
    int32_t get_దత్తాంశం_ఖండం(int32_t i);

    శూన్య ముసుగు_విలువలు(uint32_t mask);
    UChar32 కనుగొను_ఎగువ_ప్రారంభం() స్థిరరాశి;
    int32_t compact_సంపూర్ణ_దత్తాంశం_ఖండాలు(int32_t fastILimit, All_Same_ఖండాలు &all_Same_ఖండాలు);
    int32_t compact_దత్తాంశం(
            int32_t fastILimit, uint32_t *new_దత్తాంశం, int32_t new_దత్తాంశం_సామర్థ్యం,
            int32_t దత్తాంశం_Null_సూచీ, Mixed_ఖండాలు &mixed_ఖండాలు, UError_సంకేతం &error_సంకేతం);
    int32_t compact_సూచీ(int32_t fastILimit, Mixed_ఖండాలు &mixed_ఖండాలు, UError_సంకేతం &error_సంకేతం);
    int32_t compactTrie(int32_t fastILimit, UError_సంకేతం &error_సంకేతం);

    uint32_t *సూచీ = శూన్యసూచిక;
    int32_t సూచీ_సామర్థ్యం = 0;
    int32_t index3_Null_స్థానభ్రంశం = -1;
    uint32_t *దత్తాంశం = శూన్యసూచిక;
    int32_t దత్తాంశం_సామర్థ్యం = 0;
    int32_t దత్తాంశం_నిడివి = 0;
    int32_t దత్తాంశం_Null_స్థానభ్రంశం = -1;

    uint32_t orig_Initial_విలువ;
    uint32_t initial_విలువ;
    uint32_t error_విలువ;
    UChar32 ఎగువ_ప్రారంభం;
    uint32_t ఎగువ_విలువ;
#నిర్వచితమైతే UCPTRIE_DEBUG
బహిరంగ:
    స్థిరరాశి అక్షరం *name;
#అయితే_ముగింపు
అంతరంగ:
    /** Temporary array while building the final data. */
    uint16_t *index16 = శూన్యసూచిక;
    uint8_t flags[UNICODE_హద్దు >> UCPTRIE_జరుపు_3];
};

Mutable_సంకేతం_బిందువు_Trie::Mutable_సంకేతం_బిందువు_Trie(uint32_t ini_విలువ, uint32_t err_విలువ, UError_సంకేతం &error_సంకేతం) :
        orig_Initial_విలువ(ini_విలువ), initial_విలువ(ini_విలువ), error_విలువ(err_విలువ),
        ఎగువ_ప్రారంభం(0), ఎగువ_విలువ(initial_విలువ)
#నిర్వచితమైతే UCPTRIE_DEBUG
        , name("open")
#అయితే_ముగింపు
        {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    సూచీ = static_cast<uint32_t*>(uprv_malloc(BMP_I_హద్దు * 4));
    దత్తాంశం = static_cast<uint32_t*>(uprv_malloc(INITIAL_దత్తాంశం_నిడివి * 4));
    ఒకవేళ (సూచీ == శూన్యసూచిక || దత్తాంశం == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    సూచీ_సామర్థ్యం = BMP_I_హద్దు;
    దత్తాంశం_సామర్థ్యం = INITIAL_దత్తాంశం_నిడివి;
}

Mutable_సంకేతం_బిందువు_Trie::Mutable_సంకేతం_బిందువు_Trie(స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie &other, UError_సంకేతం &error_సంకేతం) :
        index3_Null_స్థానభ్రంశం(other.index3_Null_స్థానభ్రంశం),
        దత్తాంశం_Null_స్థానభ్రంశం(other.దత్తాంశం_Null_స్థానభ్రంశం),
        orig_Initial_విలువ(other.orig_Initial_విలువ), initial_విలువ(other.initial_విలువ),
        error_విలువ(other.error_విలువ),
        ఎగువ_ప్రారంభం(other.ఎగువ_ప్రారంభం), ఎగువ_విలువ(other.ఎగువ_విలువ)
#నిర్వచితమైతే UCPTRIE_DEBUG
        , name("mutable clone")
#అయితే_ముగింపు
        {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    int32_t i_సామర్థ్యం = ఎగువ_ప్రారంభం <= BMP_హద్దు ? BMP_I_హద్దు : I_హద్దు;
    సూచీ = static_cast<uint32_t*>(uprv_malloc(i_సామర్థ్యం * 4));
    దత్తాంశం = static_cast<uint32_t*>(uprv_malloc(other.దత్తాంశం_సామర్థ్యం * 4));
    ఒకవేళ (సూచీ == శూన్యసూచిక || దత్తాంశం == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    సూచీ_సామర్థ్యం = i_సామర్థ్యం;
    దత్తాంశం_సామర్థ్యం = other.దత్తాంశం_సామర్థ్యం;

    int32_t i_హద్దు = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
    uprv_memcpy(flags, other.flags, i_హద్దు);
    uprv_memcpy(సూచీ, other.సూచీ, i_హద్దు * 4);
    uprv_memcpy(దత్తాంశం, other.దత్తాంశం, (size_t)other.దత్తాంశం_నిడివి * 4);
    దత్తాంశం_నిడివి = other.దత్తాంశం_నిడివి;
    U_ASSERT(other.index16 == శూన్యసూచిక);
}

Mutable_సంకేతం_బిందువు_Trie::~Mutable_సంకేతం_బిందువు_Trie() {
    uprv_free(సూచీ);
    uprv_free(దత్తాంశం);
    uprv_free(index16);
}

Mutable_సంకేతం_బిందువు_Trie *Mutable_సంకేతం_బిందువు_Trie::fromUCPMap(స్థిరరాశి UCPMap *map, UError_సంకేతం &error_సంకేతం) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_విలువ = ucpmap_get(map, -1);
    uint32_t initial_విలువ = ucpmap_get(map, 0x10ffff);
    Local_సూచిక<Mutable_సంకేతం_బిందువు_Trie> mutableTrie(
        కొత్త Mutable_సంకేతం_బిందువు_Trie(initial_విలువ, error_విలువ, error_సంకేతం),
        error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    UChar32 ప్రారంభం = 0, చివర;
    uint32_t value;
    అయితే ((చివర = ucpmap_getRange(map, ప్రారంభం, UCPMAP_RANGE_సాధారణ, 0,
                                  శూన్యసూచిక, శూన్యసూచిక, &value)) >= 0) {
        ఒకవేళ (value != initial_విలువ) {
            ఒకవేళ (ప్రారంభం == చివర) {
                mutableTrie->సమితి(ప్రారంభం, value, error_సంకేతం);
            } లేకపోతే {
                mutableTrie->సమితి_Range(ప్రారంభం, చివర, value, error_సంకేతం);
            }
        }
        ప్రారంభం = చివర + 1;
    }
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        తిరిగి mutableTrie.orphan();
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

Mutable_సంకేతం_బిందువు_Trie *Mutable_సంకేతం_బిందువు_Trie::fromUCPTrie(స్థిరరాశి UCPTrie *trie, UError_సంకేతం &error_సంకేతం) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_విలువ;
    uint32_t initial_విలువ;
    ఎంపిక (trie->విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        error_విలువ = trie->దత్తాంశం.ptr16[trie->దత్తాంశం_నిడివి - UCPTRIE_ERROR_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం];
        initial_విలువ = trie->దత్తాంశం.ptr16[trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం];
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        error_విలువ = trie->దత్తాంశం.ptr32[trie->దత్తాంశం_నిడివి - UCPTRIE_ERROR_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం];
        initial_విలువ = trie->దత్తాంశం.ptr32[trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం];
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        error_విలువ = trie->దత్తాంశం.ptr8[trie->దత్తాంశం_నిడివి - UCPTRIE_ERROR_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం];
        initial_విలువ = trie->దత్తాంశం.ptr8[trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం];
        ఆపు;
    అప్రమేయ:
        // Unreachable if the trie is properly initialized.
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    Local_సూచిక<Mutable_సంకేతం_బిందువు_Trie> mutableTrie(
        కొత్త Mutable_సంకేతం_బిందువు_Trie(initial_విలువ, error_విలువ, error_సంకేతం),
        error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    UChar32 ప్రారంభం = 0, చివర;
    uint32_t value;
    అయితే ((చివర = ucptrie_getRange(trie, ప్రారంభం, UCPMAP_RANGE_సాధారణ, 0,
                                   శూన్యసూచిక, శూన్యసూచిక, &value)) >= 0) {
        ఒకవేళ (value != initial_విలువ) {
            ఒకవేళ (ప్రారంభం == చివర) {
                mutableTrie->సమితి(ప్రారంభం, value, error_సంకేతం);
            } లేకపోతే {
                mutableTrie->సమితి_Range(ప్రారంభం, చివర, value, error_సంకేతం);
            }
        }
        ప్రారంభం = చివర + 1;
    }
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        తిరిగి mutableTrie.orphan();
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

శూన్య Mutable_సంకేతం_బిందువు_Trie::clear() {
    index3_Null_స్థానభ్రంశం = దత్తాంశం_Null_స్థానభ్రంశం = -1;
    దత్తాంశం_నిడివి = 0;
    ఎగువ_విలువ = initial_విలువ = orig_Initial_విలువ;
    ఎగువ_ప్రారంభం = 0;
    uprv_free(index16);
    index16 = శూన్యసూచిక;
}

uint32_t Mutable_సంకేతం_బిందువు_Trie::get(UChar32 c) స్థిరరాశి {
    ఒకవేళ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        తిరిగి error_విలువ;
    }
    ఒకవేళ (c >= ఎగువ_ప్రారంభం) {
        తిరిగి ఎగువ_విలువ;
    }
    int32_t i = c >> UCPTRIE_జరుపు_3;
    ఒకవేళ (flags[i] == ALL_SAME) {
        తిరిగి సూచీ[i];
    } లేకపోతే {
        తిరిగి దత్తాంశం[సూచీ[i] + (c & UCPTRIE_SMALL_దత్తాంశం_ముసుగు)];
    }
}

అంతఃవరుస uint32_t maybe_Filter_విలువ(uint32_t value, uint32_t initial_విలువ, uint32_t null_విలువ,
                                 UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context) {
    ఒకవేళ (value == initial_విలువ) {
        value = null_విలువ;
    } లేకపోతే ఒకవేళ (filter != శూన్యసూచిక) {
        value = filter(context, value);
    }
    తిరిగి value;
}

UChar32 Mutable_సంకేతం_బిందువు_Trie::getRange(
        UChar32 ప్రారంభం, UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context,
        uint32_t *p_విలువ) స్థిరరాశి {
    ఒకవేళ (static_cast<uint32_t>(ప్రారంభం) > MAX_UNICODE) {
        తిరిగి U_SENTINEL;
    }
    ఒకవేళ (ప్రారంభం >= ఎగువ_ప్రారంభం) {
        ఒకవేళ (p_విలువ != శూన్యసూచిక) {
            uint32_t value = ఎగువ_విలువ;
            ఒకవేళ (filter != శూన్యసూచిక) { value = filter(context, value); }
            *p_విలువ = value;
        }
        తిరిగి MAX_UNICODE;
    }
    uint32_t null_విలువ = initial_విలువ;
    ఒకవేళ (filter != శూన్యసూచిక) { null_విలువ = filter(context, null_విలువ); }
    UChar32 c = ప్రారంభం;
    uint32_t trie_విలువ, value;
    తార్కిక have_విలువ = అబద్ధం;
    int32_t i = c >> UCPTRIE_జరుపు_3;
    చేయి {
        ఒకవేళ (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = సూచీ[i];
            ఒకవేళ (have_విలువ) {
                ఒకవేళ (trieValue2 != trie_విలువ) {
                    ఒకవేళ (filter == శూన్యసూచిక ||
                            maybe_Filter_విలువ(trieValue2, initial_విలువ, null_విలువ,
                                             filter, context) != value) {
                        తిరిగి c - 1;
                    }
                    trie_విలువ = trieValue2;  // may or may not help
                }
            } లేకపోతే {
                trie_విలువ = trieValue2;
                value = maybe_Filter_విలువ(trieValue2, initial_విలువ, null_విలువ, filter, context);
                ఒకవేళ (p_విలువ != శూన్యసూచిక) { *p_విలువ = value; }
                have_విలువ = నిజం;
            }
            c = (c + UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి) & ~UCPTRIE_SMALL_దత్తాంశం_ముసుగు;
        } లేకపోతే /* MIXED */ {
            int32_t di = సూచీ[i] + (c & UCPTRIE_SMALL_దత్తాంశం_ముసుగు);
            uint32_t trieValue2 = దత్తాంశం[di];
            ఒకవేళ (have_విలువ) {
                ఒకవేళ (trieValue2 != trie_విలువ) {
                    ఒకవేళ (filter == శూన్యసూచిక ||
                            maybe_Filter_విలువ(trieValue2, initial_విలువ, null_విలువ,
                                             filter, context) != value) {
                        తిరిగి c - 1;
                    }
                    trie_విలువ = trieValue2;  // may or may not help
                }
            } లేకపోతే {
                trie_విలువ = trieValue2;
                value = maybe_Filter_విలువ(trieValue2, initial_విలువ, null_విలువ, filter, context);
                ఒకవేళ (p_విలువ != శూన్యసూచిక) { *p_విలువ = value; }
                have_విలువ = నిజం;
            }
            అయితే ((++c & UCPTRIE_SMALL_దత్తాంశం_ముసుగు) != 0) {
                trieValue2 = దత్తాంశం[++di];
                ఒకవేళ (trieValue2 != trie_విలువ) {
                    ఒకవేళ (filter == శూన్యసూచిక ||
                            maybe_Filter_విలువ(trieValue2, initial_విలువ, null_విలువ,
                                             filter, context) != value) {
                        తిరిగి c - 1;
                    }
                }
                trie_విలువ = trieValue2;  // may or may not help
            }
        }
        ++i;
    } అయితే (c < ఎగువ_ప్రారంభం);
    U_ASSERT(have_విలువ);
    ఒకవేళ (maybe_Filter_విలువ(ఎగువ_విలువ, initial_విలువ, null_విలువ,
                         filter, context) != value) {
        తిరిగి c - 1;
    } లేకపోతే {
        తిరిగి MAX_UNICODE;
    }
}

శూన్య
write_ఖండం(uint32_t *ఖండం, uint32_t value) {
    uint32_t *హద్దు = ఖండం + UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
    అయితే (ఖండం < హద్దు) {
        *ఖండం++ = value;
    }
}

తార్కిక Mutable_సంకేతం_బిందువు_Trie::ensure_ఎగువ_ప్రారంభం(UChar32 c) {
    ఒకవేళ (c >= ఎగువ_ప్రారంభం) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు) & ~(UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు - 1);
        int32_t i = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
        int32_t i_హద్దు = c >> UCPTRIE_జరుపు_3;
        ఒకవేళ (i_హద్దు > సూచీ_సామర్థ్యం) {
            uint32_t* new_సూచీ = static_cast<uint32_t*>(uprv_malloc(I_హద్దు * 4));
            ఒకవేళ (new_సూచీ == శూన్యసూచిక) { తిరిగి అబద్ధం; }
            uprv_memcpy(new_సూచీ, సూచీ, i * 4);
            uprv_free(సూచీ);
            సూచీ = new_సూచీ;
            సూచీ_సామర్థ్యం = I_హద్దు;
        }
        చేయి {
            flags[i] = ALL_SAME;
            సూచీ[i] = initial_విలువ;
        } అయితే(++i < i_హద్దు);
        ఎగువ_ప్రారంభం = c;
    }
    తిరిగి నిజం;
}

int32_t Mutable_సంకేతం_బిందువు_Trie::alloc_దత్తాంశం_ఖండం(int32_t ఖండం_నిడివి) {
    int32_t new_ఖండం = దత్తాంశం_నిడివి;
    int32_t new_శిఖరం = new_ఖండం + ఖండం_నిడివి;
    ఒకవేళ (new_శిఖరం > దత్తాంశం_సామర్థ్యం) {
        int32_t capacity;
        ఒకవేళ (దత్తాంశం_సామర్థ్యం < MEDIUM_దత్తాంశం_నిడివి) {
            capacity = MEDIUM_దత్తాంశం_నిడివి;
        } లేకపోతే ఒకవేళ (దత్తాంశం_సామర్థ్యం < MAX_దత్తాంశం_నిడివి) {
            capacity = MAX_దత్తాంశం_నిడివి;
        } లేకపోతే {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            తిరిగి -1;
        }
        uint32_t* new_దత్తాంశం = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        ఒకవేళ (new_దత్తాంశం == శూన్యసూచిక) {
            తిరిగి -1;
        }
        uprv_memcpy(new_దత్తాంశం, దత్తాంశం, (size_t)దత్తాంశం_నిడివి * 4);
        uprv_free(దత్తాంశం);
        దత్తాంశం = new_దత్తాంశం;
        దత్తాంశం_సామర్థ్యం = capacity;
    }
    దత్తాంశం_నిడివి = new_శిఖరం;
    తిరిగి new_ఖండం;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_సంకేతం_బిందువు_Trie::get_దత్తాంశం_ఖండం(int32_t i) {
    ఒకవేళ (flags[i] == MIXED) {
        తిరిగి సూచీ[i];
    }
    ఒకవేళ (i < BMP_I_హద్దు) {
        int32_t new_ఖండం = alloc_దత్తాంశం_ఖండం(UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి);
        ఒకవేళ (new_ఖండం < 0) { తిరిగి new_ఖండం; }
        int32_t i_ప్రారంభం = i & ~(SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం -1);
        int32_t i_హద్దు = i_ప్రారంభం + SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం;
        చేయి {
            U_ASSERT(flags[i_ప్రారంభం] == ALL_SAME);
            write_ఖండం(దత్తాంశం + new_ఖండం, సూచీ[i_ప్రారంభం]);
            flags[i_ప్రారంభం] = MIXED;
            సూచీ[i_ప్రారంభం++] = new_ఖండం;
            new_ఖండం += UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
        } అయితే (i_ప్రారంభం < i_హద్దు);
        తిరిగి సూచీ[i];
    } లేకపోతే {
        int32_t new_ఖండం = alloc_దత్తాంశం_ఖండం(UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి);
        ఒకవేళ (new_ఖండం < 0) { తిరిగి new_ఖండం; }
        write_ఖండం(దత్తాంశం + new_ఖండం, సూచీ[i]);
        flags[i] = MIXED;
        సూచీ[i] = new_ఖండం;
        తిరిగి new_ఖండం;
    }
}

శూన్య Mutable_సంకేతం_బిందువు_Trie::సమితి(UChar32 c, uint32_t value, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    int32_t ఖండం;
    ఒకవేళ (!ensure_ఎగువ_ప్రారంభం(c) || (ఖండం = get_దత్తాంశం_ఖండం(c >> UCPTRIE_జరుపు_3)) < 0) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    దత్తాంశం[ఖండం + (c & UCPTRIE_SMALL_దత్తాంశం_ముసుగు)] = value;
}

శూన్య
fill_ఖండం(uint32_t *ఖండం, UChar32 ప్రారంభం, UChar32 హద్దు, uint32_t value) {
    uint32_t *p_హద్దు = ఖండం + హద్దు;
    ఖండం += ప్రారంభం;
    అయితే (ఖండం < p_హద్దు) {
        *ఖండం++ = value;
    }
}

శూన్య Mutable_సంకేతం_బిందువు_Trie::సమితి_Range(UChar32 ప్రారంభం, UChar32 చివర, uint32_t value, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ (static_cast<uint32_t>(ప్రారంభం) > MAX_UNICODE || static_cast<uint32_t>(చివర) > MAX_UNICODE || ప్రారంభం > చివర) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ (!ensure_ఎగువ_ప్రారంభం(చివర)) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    UChar32 హద్దు = చివర + 1;
    ఒకవేళ (ప్రారంభం & UCPTRIE_SMALL_దత్తాంశం_ముసుగు) {
        // Set partial block at [start..following block boundary[.
        int32_t ఖండం = get_దత్తాంశం_ఖండం(ప్రారంభం >> UCPTRIE_జరుపు_3);
        ఒకవేళ (ఖండం < 0) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }

        UChar32 తదుపరి_ప్రారంభం = (ప్రారంభం + UCPTRIE_SMALL_దత్తాంశం_ముసుగు) & ~UCPTRIE_SMALL_దత్తాంశం_ముసుగు;
        ఒకవేళ (తదుపరి_ప్రారంభం <= హద్దు) {
            fill_ఖండం(దత్తాంశం + ఖండం, ప్రారంభం & UCPTRIE_SMALL_దత్తాంశం_ముసుగు, UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి,
                      value);
            ప్రారంభం = తదుపరి_ప్రారంభం;
        } లేకపోతే {
            fill_ఖండం(దత్తాంశం + ఖండం, ప్రారంభం & UCPTRIE_SMALL_దత్తాంశం_ముసుగు, హద్దు & UCPTRIE_SMALL_దత్తాంశం_ముసుగు,
                      value);
            తిరిగి;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = హద్దు & UCPTRIE_SMALL_దత్తాంశం_ముసుగు;

    // Round down limit to a block boundary.
    హద్దు &= ~UCPTRIE_SMALL_దత్తాంశం_ముసుగు;

    // Iterate over all-value blocks.
    అయితే (ప్రారంభం < హద్దు) {
        int32_t i = ప్రారంభం >> UCPTRIE_జరుపు_3;
        ఒకవేళ (flags[i] == ALL_SAME) {
            సూచీ[i] = value;
        } లేకపోతే /* MIXED */ {
            fill_ఖండం(దత్తాంశం + సూచీ[i], 0, UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి, value);
        }
        ప్రారంభం += UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
    }

    ఒకవేళ (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t ఖండం = get_దత్తాంశం_ఖండం(ప్రారంభం >> UCPTRIE_జరుపు_3);
        ఒకవేళ (ఖండం < 0) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }

        fill_ఖండం(దత్తాంశం + ఖండం, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

శూన్య Mutable_సంకేతం_బిందువు_Trie::ముసుగు_విలువలు(uint32_t mask) {
    initial_విలువ &= mask;
    error_విలువ &= mask;
    ఎగువ_విలువ &= mask;
    int32_t i_హద్దు = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
    కొరకు (int32_t i = 0; i < i_హద్దు; ++i) {
        ఒకవేళ (flags[i] == ALL_SAME) {
            సూచీ[i] &= mask;
        }
    }
    కొరకు (int32_t i = 0; i < దత్తాంశం_నిడివి; ++i) {
        దత్తాంశం[i] &= mask;
    }
}

మూస<రకం_పేరు UIntA, రకం_పేరు UIntB>
తార్కిక equal_ఖండాలు(స్థిరరాశి UIntA *s, స్థిరరాశి UIntB *t, int32_t length) {
    అయితే (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    తిరిగి length == 0;
}

తార్కిక all_విలువలు_Same_As(స్థిరరాశి uint32_t *p, int32_t length, uint32_t value) {
    స్థిరరాశి uint32_t *p_హద్దు = p + length;
    అయితే (p < p_హద్దు && *p == value) { ++p; }
    తిరిగి p == p_హద్దు;
}

/** Search for an identical block. */
int32_t కనుగొను_Same_ఖండం(స్థిరరాశి uint16_t *p, int32_t p_ప్రారంభం, int32_t length,
                      స్థిరరాశి uint16_t *q, int32_t q_ప్రారంభం, int32_t ఖండం_నిడివి) {
    // Ensure that we do not even partially get past length.
    length -= ఖండం_నిడివి;

    q += q_ప్రారంభం;
    అయితే (p_ప్రారంభం <= length) {
        ఒకవేళ (equal_ఖండాలు(p + p_ప్రారంభం, q, ఖండం_నిడివి)) {
            తిరిగి p_ప్రారంభం;
        }
        ++p_ప్రారంభం;
    }
    తిరిగి -1;
}

int32_t కనుగొను_All_Same_ఖండం(స్థిరరాశి uint32_t *p, int32_t ప్రారంభం, int32_t హద్దు,
                         uint32_t value, int32_t ఖండం_నిడివి) {
    // Ensure that we do not even partially get past limit.
    హద్దు -= ఖండం_నిడివి;

    కొరకు (int32_t ఖండం = ప్రారంభం; ఖండం <= హద్దు; ++ఖండం) {
        ఒకవేళ (p[ఖండం] == value) {
            కొరకు (int32_t i = 1;; ++i) {
                ఒకవేళ (i == ఖండం_నిడివి) {
                    తిరిగి ఖండం;
                }
                ఒకవేళ (p[ఖండం + i] != value) {
                    ఖండం += i;
                    ఆపు;
                }
            }
        }
    }
    తిరిగి -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
మూస<రకం_పేరు UIntA, రకం_పేరు UIntB>
int32_t getOverlap(స్థిరరాశి UIntA *p, int32_t length,
                   స్థిరరాశి UIntB *q, int32_t q_ప్రారంభం, int32_t ఖండం_నిడివి) {
    int32_t overlap = ఖండం_నిడివి - 1;
    U_ASSERT(overlap <= length);
    q += q_ప్రారంభం;
    అయితే (overlap > 0 && !equal_ఖండాలు(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    తిరిగి overlap;
}

int32_t getAllSameOverlap(స్థిరరాశి uint32_t *p, int32_t length, uint32_t value,
                          int32_t ఖండం_నిడివి) {
    int32_t min = length - (ఖండం_నిడివి - 1);
    int32_t i = length;
    అయితే (min < i && p[i - 1] == value) { --i; }
    తిరిగి length - i;
}

తార్కిక is_ప్రారంభం_యొక్క_Some_Fast_ఖండం(uint32_t దత్తాంశం_స్థానభ్రంశం, స్థిరరాశి uint32_t సూచీ[], int32_t fastILimit) {
    కొరకు (int32_t i = 0; i < fastILimit; i += SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం) {
        ఒకవేళ (సూచీ[i] == దత్తాంశం_స్థానభ్రంశం) {
            తిరిగి నిజం;
        }
    }
    తిరిగి అబద్ధం;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_సంకేతం_బిందువు_Trie::కనుగొను_ఎగువ_ప్రారంభం() స్థిరరాశి {
    int32_t i = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
    అయితే (i > 0) {
        తార్కిక సరిపోలిక;
        ఒకవేళ (flags[--i] == ALL_SAME) {
            సరిపోలిక = సూచీ[i] == ఎగువ_విలువ;
        } లేకపోతే /* MIXED */ {
            స్థిరరాశి uint32_t *p = దత్తాంశం + సూచీ[i];
            కొరకు (int32_t j = 0;; ++j) {
                ఒకవేళ (j == UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి) {
                    సరిపోలిక = నిజం;
                    ఆపు;
                }
                ఒకవేళ (p[j] != ఎగువ_విలువ) {
                    సరిపోలిక = అబద్ధం;
                    ఆపు;
                }
            }
        }
        ఒకవేళ (!సరిపోలిక) {
            తిరిగి (i + 1) << UCPTRIE_జరుపు_3;
        }
    }
    తిరిగి 0;
}

తరగతి All_Same_ఖండాలు {
బహిరంగ:
    స్థిర constexpr int32_t NEW_UNIQUE = -1;
    స్థిర constexpr int32_t అధికప్రవాహం_2 = -2;

    All_Same_ఖండాలు() : length(0), mostRecent(-1) {}

    int32_t కనుగొను_Or_కూడు(int32_t సూచీ, int32_t count, uint32_t value) {
        ఒకవేళ (mostRecent >= 0 && విలువలు[mostRecent] == value) {
            refCounts[mostRecent] += count;
            తిరిగి indexes[mostRecent];
        }
        కొరకు (int32_t i = 0; i < length; ++i) {
            ఒకవేళ (విలువలు[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                తిరిగి indexes[i];
            }
        }
        ఒకవేళ (length == సామర్థ్యం_3) {
            తిరిగి అధికప్రవాహం_2;
        }
        mostRecent = length;
        indexes[length] = సూచీ;
        విలువలు[length] = value;
        refCounts[length++] = count;
        తిరిగి NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    శూన్య కూడు(int32_t సూచీ, int32_t count, uint32_t value) {
        U_ASSERT(length == సామర్థ్యం_3);
        int32_t least = -1;
        int32_t కనిష్ఠ_లెక్క = I_హద్దు;
        కొరకు (int32_t i = 0; i < length; ++i) {
            U_ASSERT(విలువలు[i] != value);
            ఒకవేళ (refCounts[i] < కనిష్ఠ_లెక్క) {
                least = i;
                కనిష్ఠ_లెక్క = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = సూచీ;
        విలువలు[least] = value;
        refCounts[least] = count;
    }

    int32_t కనుగొను_Most_వాడిన() స్థిరరాశి {
        ఒకవేళ (length == 0) { తిరిగి -1; }
        int32_t max = -1;
        int32_t max_లెక్క = 0;
        కొరకు (int32_t i = 0; i < length; ++i) {
            ఒకవేళ (refCounts[i] > max_లెక్క) {
                max = i;
                max_లెక్క = refCounts[i];
            }
        }
        తిరిగి indexes[max];
    }

అంతరంగ:
    స్థిర constexpr int32_t సామర్థ్యం_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[సామర్థ్యం_3];
    uint32_t విలువలు[సామర్థ్యం_3];
    int32_t refCounts[సామర్థ్యం_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
తరగతి Mixed_ఖండాలు {
బహిరంగ:
    Mixed_ఖండాలు() {}
    ~Mixed_ఖండాలు() {
        uprv_free(పట్టిక);
    }

    తార్కిక init(int32_t max_నిడివి_2, int32_t new_ఖండం_నిడివి) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_దత్తాంశం_సూచీ = max_నిడివి_2 - new_ఖండం_నిడివి + 1;
        int32_t new_నిడివి;
        ఒకవేళ (max_దత్తాంశం_సూచీ <= 0xfff) {  // 4k
            new_నిడివి = 6007;
            జరుపు = 12;
            mask = 0xfff;
        } లేకపోతే ఒకవేళ (max_దత్తాంశం_సూచీ <= 0x7fff) {  // 32k
            new_నిడివి = 50021;
            జరుపు = 15;
            mask = 0x7fff;
        } లేకపోతే ఒకవేళ (max_దత్తాంశం_సూచీ <= 0x1ffff) {  // 128k
            new_నిడివి = 200003;
            జరుపు = 17;
            mask = 0x1ffff;
        } లేకపోతే {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_నిడివి = 1500007;
            జరుపు = 21;
            mask = 0x1fffff;
        }
        ఒకవేళ (new_నిడివి > capacity) {
            uprv_free(పట్టిక);
            పట్టిక = static_cast<uint32_t*>(uprv_malloc(new_నిడివి * 4));
            ఒకవేళ (పట్టిక == శూన్యసూచిక) {
                తిరిగి అబద్ధం;
            }
            capacity = new_నిడివి;
        }
        length = new_నిడివి;
        uprv_memset(పట్టిక, 0, length * 4);

        ఖండం_నిడివి = new_ఖండం_నిడివి;
        తిరిగి నిజం;
    }

    మూస<రకం_పేరు UInt>
    శూన్య extend(స్థిరరాశి UInt *దత్తాంశం, int32_t min_ప్రారంభం, int32_t prev_దత్తాంశం_నిడివి, int32_t new_దత్తాంశం_నిడివి) {
        int32_t ప్రారంభం = prev_దత్తాంశం_నిడివి - ఖండం_నిడివి;
        ఒకవేళ (ప్రారంభం >= min_ప్రారంభం) {
            ++ప్రారంభం;  // Skip the last block that we added last time.
        } లేకపోతే {
            ప్రారంభం = min_ప్రారంభం;  // Begin with the first full block.
        }
        కొరకు (int32_t చివర = new_దత్తాంశం_నిడివి - ఖండం_నిడివి; ప్రారంభం <= చివర; ++ప్రారంభం) {
            uint32_t hash_సంకేతం = make_Hash_సంకేతం(దత్తాంశం, ప్రారంభం);
            కూడు_నమోదు(దత్తాంశం, ప్రారంభం, hash_సంకేతం, ప్రారంభం);
        }
    }

    మూస<రకం_పేరు UIntA, రకం_పేరు UIntB>
    int32_t కనుగొను_ఖండం(స్థిరరాశి UIntA *దత్తాంశం, స్థిరరాశి UIntB *ఖండం_దత్తాంశం, int32_t ఖండం_ప్రారంభం) స్థిరరాశి {
        uint32_t hash_సంకేతం = make_Hash_సంకేతం(ఖండం_దత్తాంశం, ఖండం_ప్రారంభం);
        int32_t నమోదు_సూచీ = కనుగొను_నమోదు(దత్తాంశం, ఖండం_దత్తాంశం, ఖండం_ప్రారంభం, hash_సంకేతం);
        ఒకవేళ (నమోదు_సూచీ >= 0) {
            తిరిగి (పట్టిక[నమోదు_సూచీ] & mask) - 1;
        } లేకపోతే {
            తిరిగి -1;
        }
    }

    int32_t కనుగొను_All_Same_ఖండం(స్థిరరాశి uint32_t *దత్తాంశం, uint32_t ఖండం_విలువ) స్థిరరాశి {
        uint32_t hash_సంకేతం = make_Hash_సంకేతం(ఖండం_విలువ);
        int32_t నమోదు_సూచీ = కనుగొను_నమోదు(దత్తాంశం, ఖండం_విలువ, hash_సంకేతం);
        ఒకవేళ (నమోదు_సూచీ >= 0) {
            తిరిగి (పట్టిక[నమోదు_సూచీ] & mask) - 1;
        } లేకపోతే {
            తిరిగి -1;
        }
    }

అంతరంగ:
    మూస<రకం_పేరు UInt>
    uint32_t make_Hash_సంకేతం(స్థిరరాశి UInt *ఖండం_దత్తాంశం, int32_t ఖండం_ప్రారంభం) స్థిరరాశి {
        int32_t ఖండం_హద్దు = ఖండం_ప్రారంభం + ఖండం_నిడివి;
        uint32_t hash_సంకేతం = ఖండం_దత్తాంశం[ఖండం_ప్రారంభం++];
        చేయి {
            hash_సంకేతం = 37 * hash_సంకేతం + ఖండం_దత్తాంశం[ఖండం_ప్రారంభం++];
        } అయితే (ఖండం_ప్రారంభం < ఖండం_హద్దు);
        తిరిగి hash_సంకేతం;
    }

    uint32_t make_Hash_సంకేతం(uint32_t ఖండం_విలువ) స్థిరరాశి {
        uint32_t hash_సంకేతం = ఖండం_విలువ;
        కొరకు (int32_t i = 1; i < ఖండం_నిడివి; ++i) {
            hash_సంకేతం = 37 * hash_సంకేతం + ఖండం_విలువ;
        }
        తిరిగి hash_సంకేతం;
    }

    మూస<రకం_పేరు UInt>
    శూన్య కూడు_నమోదు(స్థిరరాశి UInt *దత్తాంశం, int32_t ఖండం_ప్రారంభం, uint32_t hash_సంకేతం, int32_t దత్తాంశం_సూచీ) {
        U_ASSERT(0 <= దత్తాంశం_సూచీ && దత్తాంశం_సూచీ < (int32_t)mask);
        int32_t నమోదు_సూచీ = కనుగొను_నమోదు(దత్తాంశం, దత్తాంశం, ఖండం_ప్రారంభం, hash_సంకేతం);
        ఒకవేళ (నమోదు_సూచీ < 0) {
            పట్టిక[~నమోదు_సూచీ] = (hash_సంకేతం << జరుపు) | (దత్తాంశం_సూచీ + 1);
        }
    }

    మూస<రకం_పేరు UIntA, రకం_పేరు UIntB>
    int32_t కనుగొను_నమోదు(స్థిరరాశి UIntA *దత్తాంశం, స్థిరరాశి UIntB *ఖండం_దత్తాంశం, int32_t ఖండం_ప్రారంభం,
                      uint32_t hash_సంకేతం) స్థిరరాశి {
        uint32_t shifted_Hash_సంకేతం = hash_సంకేతం << జరుపు;
        int32_t initial_నమోదు_సూచీ = (hash_సంకేతం % (length - 1)) + 1;  // 1..length-1
        కొరకు (int32_t నమోదు_సూచీ = initial_నమోదు_సూచీ;;) {
            uint32_t నమోదు_2 = పట్టిక[నమోదు_సూచీ];
            ఒకవేళ (నమోదు_2 == 0) {
                తిరిగి ~నమోదు_సూచీ;
            }
            ఒకవేళ ((నమోదు_2 & ~mask) == shifted_Hash_సంకేతం) {
                int32_t దత్తాంశం_సూచీ = (నమోదు_2 & mask) - 1;
                ఒకవేళ (equal_ఖండాలు(దత్తాంశం + దత్తాంశం_సూచీ, ఖండం_దత్తాంశం + ఖండం_ప్రారంభం, ఖండం_నిడివి)) {
                    తిరిగి నమోదు_సూచీ;
                }
            }
            నమోదు_సూచీ = తదుపరి_సూచీ(initial_నమోదు_సూచీ, నమోదు_సూచీ);
        }
    }

    int32_t కనుగొను_నమోదు(స్థిరరాశి uint32_t *దత్తాంశం, uint32_t ఖండం_విలువ, uint32_t hash_సంకేతం) స్థిరరాశి {
        uint32_t shifted_Hash_సంకేతం = hash_సంకేతం << జరుపు;
        int32_t initial_నమోదు_సూచీ = (hash_సంకేతం % (length - 1)) + 1;  // 1..length-1
        కొరకు (int32_t నమోదు_సూచీ = initial_నమోదు_సూచీ;;) {
            uint32_t నమోదు_2 = పట్టిక[నమోదు_సూచీ];
            ఒకవేళ (నమోదు_2 == 0) {
                తిరిగి ~నమోదు_సూచీ;
            }
            ఒకవేళ ((నమోదు_2 & ~mask) == shifted_Hash_సంకేతం) {
                int32_t దత్తాంశం_సూచీ = (నమోదు_2 & mask) - 1;
                ఒకవేళ (all_విలువలు_Same_As(దత్తాంశం + దత్తాంశం_సూచీ, ఖండం_నిడివి, ఖండం_విలువ)) {
                    తిరిగి నమోదు_సూచీ;
                }
            }
            నమోదు_సూచీ = తదుపరి_సూచీ(initial_నమోదు_సూచీ, నమోదు_సూచీ);
        }
    }

    అంతఃవరుస int32_t తదుపరి_సూచీ(int32_t initial_నమోదు_సూచీ, int32_t నమోదు_సూచీ) స్థిరరాశి {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        తిరిగి (నమోదు_సూచీ + initial_నమోదు_సూచీ) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *పట్టిక = శూన్యసూచిక;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t జరుపు = 0;
    uint32_t mask = 0;

    int32_t ఖండం_నిడివి = 0;
};

int32_t Mutable_సంకేతం_బిందువు_Trie::compact_సంపూర్ణ_దత్తాంశం_ఖండాలు(int32_t fastILimit, All_Same_ఖండాలు &all_Same_ఖండాలు) {
#నిర్వచితమైతే UCPTRIE_DEBUG
    తార్కిక అధికప్రవాహం = అబద్ధం;
#అయితే_ముగింపు

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_దత్తాంశం_సామర్థ్యం = ASCII_హద్దు;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_దత్తాంశం_సామర్థ్యం += UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
    // Add room for special values (errorValue, highValue) and padding.
    new_దత్తాంశం_సామర్థ్యం += 4;
    int32_t i_హద్దు = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
    int32_t ఖండం_నిడివి = UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి;
    int32_t inc = SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం;
    కొరకు (int32_t i = 0; i < i_హద్దు; i += inc) {
        ఒకవేళ (i == fastILimit) {
            ఖండం_నిడివి = UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
            inc = 1;
        }
        uint32_t value = సూచీ[i];
        ఒకవేళ (flags[i] == MIXED) {
            // Really mixed?
            స్థిరరాశి uint32_t *p = దత్తాంశం + value;
            value = *p;
            ఒకవేళ (all_విలువలు_Same_As(p + 1, ఖండం_నిడివి - 1, value)) {
                flags[i] = ALL_SAME;
                సూచీ[i] = value;
                // Fall through to ALL_SAME handling.
            } లేకపోతే {
                new_దత్తాంశం_సామర్థ్యం += ఖండం_నిడివి;
                కొనసాగు;
            }
        } లేకపోతే {
            U_ASSERT(flags[i] == ALL_SAME);
            ఒకవేళ (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                తార్కిక allSame = నిజం;
                int32_t తదుపరి_i = i + inc;
                కొరకు (int32_t j = i + 1; j < తదుపరి_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    ఒకవేళ (సూచీ[j] != value) {
                        allSame = అబద్ధం;
                        ఆపు;
                    }
                }
                ఒకవేళ (!allSame) {
                    // Turn it into a MIXED block.
                    ఒకవేళ (get_దత్తాంశం_ఖండం(i) < 0) {
                        తిరిగి -1;
                    }
                    new_దత్తాంశం_సామర్థ్యం += ఖండం_నిడివి;
                    కొనసాగు;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_ఖండాలు.కనుగొను_Or_కూడు(i, inc, value);
        ఒకవేళ (other == All_Same_ఖండాలు::అధికప్రవాహం_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#నిర్వచితమైతే UCPTRIE_DEBUG
            ఒకవేళ (!అధికప్రవాహం) {
                puts("UCPTrie AllSameBlocks overflow");
                అధికప్రవాహం = నిజం;
            }
#అయితే_ముగింపు
            int32_t jInc = SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం;
            కొరకు (int32_t j = 0;; j += jInc) {
                ఒకవేళ (j == i) {
                    all_Same_ఖండాలు.కూడు(i, inc, value);
                    ఆపు;
                }
                ఒకవేళ (j == fastILimit) {
                    jInc = 1;
                }
                ఒకవేళ (flags[j] == ALL_SAME && సూచీ[j] == value) {
                    all_Same_ఖండాలు.కూడు(j, jInc + inc, value);
                    other = j;
                    ఆపు;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        ఒకవేళ (other >= 0) {
            flags[i] = SAME_AS;
            సూచీ[i] = other;
        } లేకపోతే {
            // New unique same-value block.
            new_దత్తాంశం_సామర్థ్యం += ఖండం_నిడివి;
        }
    }
    తిరిగి new_దత్తాంశం_సామర్థ్యం;
}

#నిర్వచితమైతే UCPTRIE_DEBUG
#   define DEBUG_DO(సమాసం) సమాసం
#లేకపోతే
#   define DEBUG_DO(సమాసం)
#అయితే_ముగింపు

#నిర్వచితమైతే UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_విలువ(అక్షరం s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (అక్షరం)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (అక్షరం)(0x80 + (value & 0x3F));
    తిరిగి length + 3;
}

శూన్య print_ఖండం(స్థిరరాశి uint32_t *ఖండం, int32_t ఖండం_నిడివి, uint32_t value,
                UChar32 ప్రారంభం, int32_t overlap, uint32_t initial_విలువ) {
    అక్షరం s[UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి * 3 + 3];
    int32_t length = 0;
    int32_t i;
    కొరకు (i = 0; i < overlap; ++i) {
        length = append_విలువ(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    కొరకు (; i < ఖండం_నిడివి; ++i) {
        ఒకవేళ (ఖండం != శూన్యసూచిక) {
            value = ఖండం[i];
        }
        ఒకవేళ (value == initial_విలువ) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_విలువ(s, length, value);
    }
    s[length] = 0;
    ప్రారంభం += overlap;
    ఒకవేళ (ప్రారంభం <= 0xffff) {
        రాయి("    %04lX  %s|\n", (దీర్ఘ)ప్రారంభం, s);
    } లేకపోతే ఒకవేళ (ప్రారంభం <= 0xfffff) {
        రాయి("   %5lX  %s|\n", (దీర్ఘ)ప్రారంభం, s);
    } లేకపోతే {
        రాయి("  %6lX  %s|\n", (దీర్ఘ)ప్రారంభం, s);
    }
}
#అయితే_ముగింపు

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
int32_t Mutable_సంకేతం_బిందువు_Trie::compact_దత్తాంశం(
        int32_t fastILimit, uint32_t *new_దత్తాంశం, int32_t new_దత్తాంశం_సామర్థ్యం,
        int32_t దత్తాంశం_Null_సూచీ, Mixed_ఖండాలు &mixed_ఖండాలు, UError_సంకేతం &error_సంకేతం) {
#నిర్వచితమైతే UCPTRIE_DEBUG
    int32_t లెక్క_Same=0, కూడిక_Overlaps=0;
    తార్కిక print_దత్తాంశం = దత్తాంశం_నిడివి == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        దత్తాంశం_నిడివి == 50400 /* zh.txt~stroke */;
#అయితే_ముగింపు

    // The linear ASCII data has been copied into newData already.
    int32_t new_దత్తాంశం_నిడివి = 0;
    కొరకు (int32_t i = 0; new_దత్తాంశం_నిడివి < ASCII_హద్దు;
            new_దత్తాంశం_నిడివి += UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి, i += SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం) {
        సూచీ[i] = new_దత్తాంశం_నిడివి;
#నిర్వచితమైతే UCPTRIE_DEBUG
        ఒకవేళ (print_దత్తాంశం) {
            print_ఖండం(new_దత్తాంశం + new_దత్తాంశం_నిడివి, UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి, 0, new_దత్తాంశం_నిడివి, 0, initial_విలువ);
        }
#అయితే_ముగింపు
    }

    int32_t ఖండం_నిడివి = UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి;
    ఒకవేళ (!mixed_ఖండాలు.init(new_దత్తాంశం_సామర్థ్యం, ఖండం_నిడివి)) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    mixed_ఖండాలు.extend(new_దత్తాంశం, 0, 0, new_దత్తాంశం_నిడివి);

    int32_t i_హద్దు = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
    int32_t inc = SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం;
    int32_t fast_నిడివి = 0;
    కొరకు (int32_t i = ASCII_I_హద్దు; i < i_హద్దు; i += inc) {
        ఒకవేళ (i == fastILimit) {
            ఖండం_నిడివి = UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
            inc = 1;
            fast_నిడివి = new_దత్తాంశం_నిడివి;
            ఒకవేళ (!mixed_ఖండాలు.init(new_దత్తాంశం_సామర్థ్యం, ఖండం_నిడివి)) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి 0;
            }
            mixed_ఖండాలు.extend(new_దత్తాంశం, 0, 0, new_దత్తాంశం_నిడివి);
        }
        ఒకవేళ (flags[i] == ALL_SAME) {
            uint32_t value = సూచీ[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_ఖండాలు.కనుగొను_All_Same_ఖండం(new_దత్తాంశం, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            అయితే (n >= 0 && i == దత్తాంశం_Null_సూచీ && i >= fastILimit && n < fast_నిడివి &&
                    is_ప్రారంభం_యొక్క_Some_Fast_ఖండం(n, సూచీ, fastILimit)) {
                n = కనుగొను_All_Same_ఖండం(new_దత్తాంశం, n + 1, new_దత్తాంశం_నిడివి, value, ఖండం_నిడివి);
            }
            ఒకవేళ (n >= 0) {
                DEBUG_DO(++లెక్క_Same);
                సూచీ[i] = n;
            } లేకపోతే {
                n = getAllSameOverlap(new_దత్తాంశం, new_దత్తాంశం_నిడివి, value, ఖండం_నిడివి);
                DEBUG_DO(కూడిక_Overlaps += n);
#నిర్వచితమైతే UCPTRIE_DEBUG
                ఒకవేళ (print_దత్తాంశం) {
                    print_ఖండం(శూన్యసూచిక, ఖండం_నిడివి, value, i << UCPTRIE_జరుపు_3, n, initial_విలువ);
                }
#అయితే_ముగింపు
                సూచీ[i] = new_దత్తాంశం_నిడివి - n;
                int32_t prev_దత్తాంశం_నిడివి = new_దత్తాంశం_నిడివి;
                అయితే (n < ఖండం_నిడివి) {
                    new_దత్తాంశం[new_దత్తాంశం_నిడివి++] = value;
                    ++n;
                }
                mixed_ఖండాలు.extend(new_దత్తాంశం, 0, prev_దత్తాంశం_నిడివి, new_దత్తాంశం_నిడివి);
            }
        } లేకపోతే ఒకవేళ (flags[i] == MIXED) {
            స్థిరరాశి uint32_t *ఖండం = దత్తాంశం + సూచీ[i];
            int32_t n = mixed_ఖండాలు.కనుగొను_ఖండం(new_దత్తాంశం, ఖండం, 0);
            ఒకవేళ (n >= 0) {
                DEBUG_DO(++లెక్క_Same);
                సూచీ[i] = n;
            } లేకపోతే {
                n = getOverlap(new_దత్తాంశం, new_దత్తాంశం_నిడివి, ఖండం, 0, ఖండం_నిడివి);
                DEBUG_DO(కూడిక_Overlaps += n);
#నిర్వచితమైతే UCPTRIE_DEBUG
                ఒకవేళ (print_దత్తాంశం) {
                    print_ఖండం(ఖండం, ఖండం_నిడివి, 0, i << UCPTRIE_జరుపు_3, n, initial_విలువ);
                }
#అయితే_ముగింపు
                సూచీ[i] = new_దత్తాంశం_నిడివి - n;
                int32_t prev_దత్తాంశం_నిడివి = new_దత్తాంశం_నిడివి;
                అయితే (n < ఖండం_నిడివి) {
                    new_దత్తాంశం[new_దత్తాంశం_నిడివి++] = ఖండం[n++];
                }
                mixed_ఖండాలు.extend(new_దత్తాంశం, 0, prev_దత్తాంశం_నిడివి, new_దత్తాంశం_నిడివి);
            }
        } లేకపోతే /* SAME_AS */ {
            uint32_t j = సూచీ[i];
            సూచీ[i] = సూచీ[j];
        }
    }

#నిర్వచితమైతే UCPTRIE_DEBUG
    /* we saved some space */
    రాయి("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (దీర్ఘ)దత్తాంశం_నిడివి, (దీర్ఘ)new_దత్తాంశం_నిడివి, (దీర్ఘ)లెక్క_Same, (దీర్ఘ)కూడిక_Overlaps);
#అయితే_ముగింపు
    తిరిగి new_దత్తాంశం_నిడివి;
}

int32_t Mutable_సంకేతం_బిందువు_Trie::compact_సూచీ(int32_t fastILimit, Mixed_ఖండాలు &mixed_ఖండాలు,
                                           UError_సంకేతం &error_సంకేతం) {
    int32_t fast_సూచీ_నిడివి = fastILimit >> (UCPTRIE_FAST_జరుపు - UCPTRIE_జరుపు_3);
    ఒకవేళ ((ఎగువ_ప్రారంభం >> UCPTRIE_FAST_జరుపు) <= fast_సూచీ_నిడివి) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_స్థానభ్రంశం = UCPTRIE_NO_INDEX3_NULL_స్థానభ్రంశం;
        తిరిగి fast_సూచీ_నిడివి;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_సూచీ[UCPTRIE_BMP_సూచీ_నిడివి];  // fastIndexLength
    int32_t i3_మొదటి_Null = -1;
    కొరకు (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = సూచీ[i];
        fast_సూచీ[j] = static_cast<uint16_t>(i3);
        ఒకవేళ (i3 == static_cast<uint32_t>(దత్తాంశం_Null_స్థానభ్రంశం)) {
            ఒకవేళ (i3_మొదటి_Null < 0) {
                i3_మొదటి_Null = j;
            } లేకపోతే ఒకవేళ (index3_Null_స్థానభ్రంశం < 0 &&
                    (j - i3_మొదటి_Null + 1) == UCPTRIE_సూచీ_3_ఖండం_నిడివి) {
                index3_Null_స్థానభ్రంశం = i3_మొదటి_Null;
            }
        } లేకపోతే {
            i3_మొదటి_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_తదుపరి = i + SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం;
        అయితే (++i < i_తదుపరి) {
            i3 += UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
            సూచీ[i] = i3;
        }
    }

    ఒకవేళ (!mixed_ఖండాలు.init(fast_సూచీ_నిడివి, UCPTRIE_సూచీ_3_ఖండం_నిడివి)) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    mixed_ఖండాలు.extend(fast_సూచీ, 0, 0, fast_సూచీ_నిడివి);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_సామర్థ్యం = 0;
    i3_మొదటి_Null = index3_Null_స్థానభ్రంశం;
    తార్కిక has_Long_I3_ఖండాలు = అబద్ధం;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_ప్రారంభం = fastILimit < BMP_I_హద్దు ? 0 : BMP_I_హద్దు;
    int32_t i_హద్దు = ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3;
    కొరకు (int32_t i = i_ప్రారంభం; i < i_హద్దు;) {
        int32_t j = i;
        int32_t j_హద్దు = i + UCPTRIE_సూచీ_3_ఖండం_నిడివి;
        uint32_t oredI3 = 0;
        తార్కిక isNull = నిజం;
        చేయి {
            uint32_t i3 = సూచీ[j];
            oredI3 |= i3;
            ఒకవేళ (i3 != static_cast<uint32_t>(దత్తాంశం_Null_స్థానభ్రంశం)) {
                isNull = అబద్ధం;
            }
        } అయితే (++j < j_హద్దు);
        ఒకవేళ (isNull) {
            flags[i] = I3_NULL;
            ఒకవేళ (i3_మొదటి_Null < 0) {
                ఒకవేళ (oredI3 <= 0xffff) {
                    index3_సామర్థ్యం += UCPTRIE_సూచీ_3_ఖండం_నిడివి;
                } లేకపోతే {
                    index3_సామర్థ్యం += సూచీ_3_18_బిట్టు_ఖండం_నిడివి;
                    has_Long_I3_ఖండాలు = నిజం;
                }
                i3_మొదటి_Null = 0;
            }
        } లేకపోతే {
            ఒకవేళ (oredI3 <= 0xffff) {
                int32_t n = mixed_ఖండాలు.కనుగొను_ఖండం(fast_సూచీ, సూచీ, i);
                ఒకవేళ (n >= 0) {
                    flags[i] = I3_BMP;
                    సూచీ[i] = n;
                } లేకపోతే {
                    flags[i] = I3_16;
                    index3_సామర్థ్యం += UCPTRIE_సూచీ_3_ఖండం_నిడివి;
                }
            } లేకపోతే {
                flags[i] = I3_18;
                index3_సామర్థ్యం += సూచీ_3_18_బిట్టు_ఖండం_నిడివి;
                has_Long_I3_ఖండాలు = నిజం;
            }
        }
        i = j;
    }

    int32_t index2_సామర్థ్యం = (i_హద్దు - i_ప్రారంభం) >> UCPTRIE_జరుపు_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_నిడివి = (index2_సామర్థ్యం + UCPTRIE_సూచీ_2_ముసుగు) >> UCPTRIE_జరుపు_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_సామర్థ్యం = fast_సూచీ_నిడివి + index1_నిడివి + index3_సామర్థ్యం + index2_సామర్థ్యం + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_సామర్థ్యం * 2));
    ఒకవేళ (index16 == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    uprv_memcpy(index16, fast_సూచీ, fast_సూచీ_నిడివి * 2);

    ఒకవేళ (!mixed_ఖండాలు.init(index16_సామర్థ్యం, UCPTRIE_సూచీ_3_ఖండం_నిడివి)) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    Mixed_ఖండాలు long_I3_ఖండాలు;
    ఒకవేళ (has_Long_I3_ఖండాలు) {
        ఒకవేళ (!long_I3_ఖండాలు.init(index16_సామర్థ్యం, సూచీ_3_18_బిట్టు_ఖండం_నిడివి)) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_హద్దు >> UCPTRIE_జరుపు_2];  // index2Capacity
    int32_t i2_నిడివి = 0;
    i3_మొదటి_Null = index3_Null_స్థానభ్రంశం;
    int32_t index3_ప్రారంభం = fast_సూచీ_నిడివి + index1_నిడివి;
    int32_t సూచీ_నిడివి = index3_ప్రారంభం;
    కొరకు (int32_t i = i_ప్రారంభం; i < i_హద్దు; i += UCPTRIE_సూచీ_3_ఖండం_నిడివి) {
        int32_t i3;
        uint8_t f = flags[i];
        ఒకవేళ (f == I3_NULL && i3_మొదటి_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = దత్తాంశం_Null_స్థానభ్రంశం <= 0xffff ? I3_16 : I3_18;
            i3_మొదటి_Null = 0;
        }
        ఒకవేళ (f == I3_NULL) {
            i3 = index3_Null_స్థానభ్రంశం;
        } లేకపోతే ఒకవేళ (f == I3_BMP) {
            i3 = సూచీ[i];
        } లేకపోతే ఒకవేళ (f == I3_16) {
            int32_t n = mixed_ఖండాలు.కనుగొను_ఖండం(index16, సూచీ, i);
            ఒకవేళ (n >= 0) {
                i3 = n;
            } లేకపోతే {
                ఒకవేళ (సూచీ_నిడివి == index3_ప్రారంభం) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } లేకపోతే {
                    n = getOverlap(index16, సూచీ_నిడివి,
                                   సూచీ, i, UCPTRIE_సూచీ_3_ఖండం_నిడివి);
                }
                i3 = సూచీ_నిడివి - n;
                int32_t prev_సూచీ_నిడివి = సూచీ_నిడివి;
                అయితే (n < UCPTRIE_సూచీ_3_ఖండం_నిడివి) {
                    index16[సూచీ_నిడివి++] = సూచీ[i + n++];
                }
                mixed_ఖండాలు.extend(index16, index3_ప్రారంభం, prev_సూచీ_నిడివి, సూచీ_నిడివి);
                ఒకవేళ (has_Long_I3_ఖండాలు) {
                    long_I3_ఖండాలు.extend(index16, index3_ప్రారంభం, prev_సూచీ_నిడివి, సూచీ_నిడివి);
                }
            }
        } లేకపోతే {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_ఖండాలు);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_హద్దు = i + UCPTRIE_సూచీ_3_ఖండం_నిడివి;
            int32_t k = సూచీ_నిడివి;
            చేయి {
                ++k;
                uint32_t v = సూచీ[j++];
                uint32_t పై_బిట్లు = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = సూచీ[j++];
                పై_బిట్లు |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = పై_బిట్లు;
            } అయితే (j < j_హద్దు);
            int32_t n = long_I3_ఖండాలు.కనుగొను_ఖండం(index16, index16, సూచీ_నిడివి);
            ఒకవేళ (n >= 0) {
                i3 = n | 0x8000;
            } లేకపోతే {
                ఒకవేళ (సూచీ_నిడివి == index3_ప్రారంభం) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } లేకపోతే {
                    n = getOverlap(index16, సూచీ_నిడివి,
                                   index16, సూచీ_నిడివి, సూచీ_3_18_బిట్టు_ఖండం_నిడివి);
                }
                i3 = (సూచీ_నిడివి - n) | 0x8000;
                int32_t prev_సూచీ_నిడివి = సూచీ_నిడివి;
                ఒకవేళ (n > 0) {
                    int32_t ప్రారంభం = సూచీ_నిడివి;
                    అయితే (n < సూచీ_3_18_బిట్టు_ఖండం_నిడివి) {
                        index16[సూచీ_నిడివి++] = index16[ప్రారంభం + n++];
                    }
                } లేకపోతే {
                    సూచీ_నిడివి += సూచీ_3_18_బిట్టు_ఖండం_నిడివి;
                }
                mixed_ఖండాలు.extend(index16, index3_ప్రారంభం, prev_సూచీ_నిడివి, సూచీ_నిడివి);
                ఒకవేళ (has_Long_I3_ఖండాలు) {
                    long_I3_ఖండాలు.extend(index16, index3_ప్రారంభం, prev_సూచీ_నిడివి, సూచీ_నిడివి);
                }
            }
        }
        ఒకవేళ (index3_Null_స్థానభ్రంశం < 0 && i3_మొదటి_Null >= 0) {
            index3_Null_స్థానభ్రంశం = i3;
        }
        // Set the index-2 table entry.
        index2[i2_నిడివి++] = i3;
    }
    U_ASSERT(i2_నిడివి == index2_సామర్థ్యం);
    U_ASSERT(సూచీ_నిడివి <= index3_ప్రారంభం + index3_సామర్థ్యం);

    ఒకవేళ (index3_Null_స్థానభ్రంశం < 0) {
        index3_Null_స్థానభ్రంశం = UCPTRIE_NO_INDEX3_NULL_స్థానభ్రంశం;
    }
    ఒకవేళ (సూచీ_నిడివి >= (UCPTRIE_NO_INDEX3_NULL_స్థానభ్రంశం + UCPTRIE_సూచీ_3_ఖండం_నిడివి)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_సూచీ_2_ఖండం_నిడివి == UCPTRIE_సూచీ_3_ఖండం_నిడివి,
                  "must re-init mixedBlocks");
    int32_t ఖండం_నిడివి = UCPTRIE_సూచీ_2_ఖండం_నిడివి;
    int32_t i1 = fast_సూచీ_నిడివి;
    కొరకు (int32_t i = 0; i < i2_నిడివి; i += ఖండం_నిడివి) {
        int32_t n;
        ఒకవేళ ((i2_నిడివి - i) >= ఖండం_నిడివి) {
            // normal block
            U_ASSERT(ఖండం_నిడివి == UCPTRIE_సూచీ_2_ఖండం_నిడివి);
            n = mixed_ఖండాలు.కనుగొను_ఖండం(index16, index2, i);
        } లేకపోతే {
            // highStart is inside the last index-2 block. Shorten it.
            ఖండం_నిడివి = i2_నిడివి - i;
            n = కనుగొను_Same_ఖండం(index16, index3_ప్రారంభం, సూచీ_నిడివి,
                              index2, i, ఖండం_నిడివి);
        }
        int32_t i2;
        ఒకవేళ (n >= 0) {
            i2 = n;
        } లేకపోతే {
            ఒకవేళ (సూచీ_నిడివి == index3_ప్రారంభం) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } లేకపోతే {
                n = getOverlap(index16, సూచీ_నిడివి, index2, i, ఖండం_నిడివి);
            }
            i2 = సూచీ_నిడివి - n;
            int32_t prev_సూచీ_నిడివి = సూచీ_నిడివి;
            అయితే (n < ఖండం_నిడివి) {
                index16[సూచీ_నిడివి++] = index2[i + n++];
            }
            mixed_ఖండాలు.extend(index16, index3_ప్రారంభం, prev_సూచీ_నిడివి, సూచీ_నిడివి);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_ప్రారంభం);
    U_ASSERT(సూచీ_నిడివి <= index16_సామర్థ్యం);

#నిర్వచితమైతే UCPTRIE_DEBUG
    /* we saved some space */
    రాయి("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (దీర్ఘ)i_హద్దు, (దీర్ఘ)సూచీ_నిడివి);
#అయితే_ముగింపు

    తిరిగి సూచీ_నిడివి;
}

int32_t Mutable_సంకేతం_బిందువు_Trie::compactTrie(int32_t fastILimit, UError_సంకేతం &error_సంకేతం) {
    // Find the real highStart and round it up.
    U_ASSERT((ఎగువ_ప్రారంభం & (UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు - 1)) == 0);
    ఎగువ_విలువ = get(MAX_UNICODE);
    int32_t real_ఎగువ_ప్రారంభం = కనుగొను_ఎగువ_ప్రారంభం();
    real_ఎగువ_ప్రారంభం = (real_ఎగువ_ప్రారంభం + (UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు - 1)) &
        ~(UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు - 1);
    ఒకవేళ (real_ఎగువ_ప్రారంభం == UNICODE_హద్దు) {
        ఎగువ_విలువ = initial_విలువ;
    }

#నిర్వచితమైతే UCPTRIE_DEBUG
    రాయి("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (దీర్ఘ)real_ఎగువ_ప్రారంభం, (దీర్ఘ)ఎగువ_విలువ, (దీర్ఘ)initial_విలువ);
#అయితే_ముగింపు

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_హద్దు = fastILimit << UCPTRIE_జరుపు_3;
    ఒకవేళ (real_ఎగువ_ప్రారంభం < fast_హద్దు) {
        కొరకు (int32_t i = (real_ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            సూచీ[i] = ఎగువ_విలువ;
        }
        ఎగువ_ప్రారంభం = fast_హద్దు;
    } లేకపోతే {
        ఎగువ_ప్రారంభం = real_ఎగువ_ప్రారంభం;
    }

    uint32_t ascii_దత్తాంశం[ASCII_హద్దు];
    కొరకు (int32_t i = 0; i < ASCII_హద్దు; ++i) {
        ascii_దత్తాంశం[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_ఖండాలు all_Same_ఖండాలు;
    int32_t new_దత్తాంశం_సామర్థ్యం = compact_సంపూర్ణ_దత్తాంశం_ఖండాలు(fastILimit, all_Same_ఖండాలు);
    ఒకవేళ (new_దత్తాంశం_సామర్థ్యం < 0) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    uint32_t* new_దత్తాంశం = static_cast<uint32_t*>(uprv_malloc(new_దత్తాంశం_సామర్థ్యం * 4));
    ఒకవేళ (new_దత్తాంశం == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    uprv_memcpy(new_దత్తాంశం, ascii_దత్తాంశం, పరిమాణం(ascii_దత్తాంశం));

    int32_t దత్తాంశం_Null_సూచీ = all_Same_ఖండాలు.కనుగొను_Most_వాడిన();

    Mixed_ఖండాలు mixed_ఖండాలు;
    int32_t new_దత్తాంశం_నిడివి = compact_దత్తాంశం(fastILimit, new_దత్తాంశం, new_దత్తాంశం_సామర్థ్యం,
                                        దత్తాంశం_Null_సూచీ, mixed_ఖండాలు, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0; }
    U_ASSERT(new_దత్తాంశం_నిడివి <= new_దత్తాంశం_సామర్థ్యం);
    uprv_free(దత్తాంశం);
    దత్తాంశం = new_దత్తాంశం;
    దత్తాంశం_సామర్థ్యం = new_దత్తాంశం_సామర్థ్యం;
    దత్తాంశం_నిడివి = new_దత్తాంశం_నిడివి;
    ఒకవేళ (దత్తాంశం_నిడివి > (0x3ffff + UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ (దత్తాంశం_Null_సూచీ >= 0) {
        దత్తాంశం_Null_స్థానభ్రంశం = సూచీ[దత్తాంశం_Null_సూచీ];
#నిర్వచితమైతే UCPTRIE_DEBUG
        ఒకవేళ (దత్తాంశం[దత్తాంశం_Null_స్థానభ్రంశం] != initial_విలువ) {
            రాయి("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (దీర్ఘ)initial_విలువ, (దీర్ఘ)దత్తాంశం[దత్తాంశం_Null_స్థానభ్రంశం]);
        }
#అయితే_ముగింపు
        initial_విలువ = దత్తాంశం[దత్తాంశం_Null_స్థానభ్రంశం];
    } లేకపోతే {
        దత్తాంశం_Null_స్థానభ్రంశం = UCPTRIE_NO_దత్తాంశం_NULL_స్థానభ్రంశం;
    }

    int32_t సూచీ_నిడివి = compact_సూచీ(fastILimit, mixed_ఖండాలు, error_సంకేతం);
    ఎగువ_ప్రారంభం = real_ఎగువ_ప్రారంభం;
    తిరిగి సూచీ_నిడివి;
}

UCPTrie *Mutable_సంకేతం_బిందువు_Trie::build(UCPTrieType type, UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            విలువ_వెడల్పు < UCPTRIE_విలువ_బిట్లు_16 || UCPTRIE_విలువ_బిట్లు_8 < విలువ_వెడల్పు) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    ఎంపిక (విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        ముసుగు_విలువలు(0xffff);
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        ముసుగు_విలువలు(0xff);
        ఆపు;
    అప్రమేయ:
        ఆపు;
    }

    UChar32 fast_హద్దు = type == UCPTRIE_TYPE_FAST ? BMP_హద్దు : UCPTRIE_SMALL_హద్దు;
    int32_t సూచీ_నిడివి = compactTrie(fast_హద్దు >> UCPTRIE_జరుపు_3, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        clear();
        తిరిగి శూన్యసూచిక;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    ఒకవేళ (విలువ_వెడల్పు == UCPTRIE_విలువ_బిట్లు_32 && (సూచీ_నిడివి & 1) != 0) {
        index16[సూచీ_నిడివి++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = సూచీ_నిడివి * 2;
    ఒకవేళ (విలువ_వెడల్పు == UCPTRIE_విలువ_బిట్లు_16) {
        ఒకవేళ (((సూచీ_నిడివి ^ దత్తాంశం_నిడివి) & 1) != 0) {
            // padding
            దత్తాంశం[దత్తాంశం_నిడివి++] = error_విలువ;
        }
        ఒకవేళ (దత్తాంశం[దత్తాంశం_నిడివి - 1] != error_విలువ || దత్తాంశం[దత్తాంశం_నిడివి - 2] != ఎగువ_విలువ) {
            దత్తాంశం[దత్తాంశం_నిడివి++] = ఎగువ_విలువ;
            దత్తాంశం[దత్తాంశం_నిడివి++] = error_విలువ;
        }
        length += దత్తాంశం_నిడివి * 2;
    } లేకపోతే ఒకవేళ (విలువ_వెడల్పు == UCPTRIE_విలువ_బిట్లు_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        ఒకవేళ (దత్తాంశం[దత్తాంశం_నిడివి - 1] != error_విలువ || దత్తాంశం[దత్తాంశం_నిడివి - 2] != ఎగువ_విలువ) {
            ఒకవేళ (దత్తాంశం[దత్తాంశం_నిడివి - 1] != ఎగువ_విలువ) {
                దత్తాంశం[దత్తాంశం_నిడివి++] = ఎగువ_విలువ;
            }
            దత్తాంశం[దత్తాంశం_నిడివి++] = error_విలువ;
        }
        length += దత్తాంశం_నిడివి * 4;
    } లేకపోతే {
        int32_t and3 = (length + దత్తాంశం_నిడివి) & 3;
        ఒకవేళ (and3 == 0 && దత్తాంశం[దత్తాంశం_నిడివి - 1] == error_విలువ && దత్తాంశం[దత్తాంశం_నిడివి - 2] == ఎగువ_విలువ) {
            // all set
        } లేకపోతే ఒకవేళ(and3 == 3 && దత్తాంశం[దత్తాంశం_నిడివి - 1] == ఎగువ_విలువ) {
            దత్తాంశం[దత్తాంశం_నిడివి++] = error_విలువ;
        } లేకపోతే {
            అయితే (and3 != 2) {
                దత్తాంశం[దత్తాంశం_నిడివి++] = ఎగువ_విలువ;
                and3 = (and3 + 1) & 3;
            }
            దత్తాంశం[దత్తాంశం_నిడివి++] = ఎగువ_విలువ;
            దత్తాంశం[దత్తాంశం_నిడివి++] = error_విలువ;
        }
        length += దత్తాంశం_నిడివి;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += పరిమాణం(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* బైట్లు = static_cast<uint8_t*>(uprv_malloc(length));
    ఒకవేళ (బైట్లు == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        clear();
        తిరిగి శూన్యసూచిక;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(బైట్లు);
    uprv_memset(trie, 0, పరిమాణం(UCPTrie));
    trie->సూచీ_నిడివి = సూచీ_నిడివి;
    trie->దత్తాంశం_నిడివి = దత్తాంశం_నిడివి;

    trie->ఎగువ_ప్రారంభం = ఎగువ_ప్రారంభం;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ఎగువ_ప్రారంభం = (ఎగువ_ప్రారంభం + 0xfff) >> 12;
    trie->type = type;
    trie->విలువ_వెడల్పు = విలువ_వెడల్పు;

    trie->index3_Null_స్థానభ్రంశం = index3_Null_స్థానభ్రంశం;
    trie->దత్తాంశం_Null_స్థానభ్రంశం = దత్తాంశం_Null_స్థానభ్రంశం;
    trie->null_విలువ = initial_విలువ;

    బైట్లు += పరిమాణం(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(బైట్లు);
    trie->సూచీ = dest16;

    ఒకవేళ (ఎగువ_ప్రారంభం <= fast_హద్దు) {
        // Condense only the fast index from the mutable-trie index.
        కొరకు (int32_t i = 0, j = 0; j < సూచీ_నిడివి; i += SMALL_దత్తాంశం_ఖండాలు_చొప్పున_BMP_ఖండం, ++j) {
            *dest16++ = static_cast<uint16_t>(సూచీ[i]); // dest16[j]
        }
    } లేకపోతే {
        uprv_memcpy(dest16, index16, సూచీ_నిడివి * 2);
        dest16 += సూచీ_నిడివి;
    }
    బైట్లు += సూచీ_నిడివి * 2;

    // Write the data array.
    స్థిరరాశి uint32_t *p = దత్తాంశం;
    ఎంపిక (విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        // Write 16-bit data values.
        trie->దత్తాంశం.ptr16 = dest16;
        కొరకు (int32_t i = దత్తాంశం_నిడివి; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        // Write 32-bit data values.
        trie->దత్తాంశం.ptr32 = reinterpret_cast<uint32_t*>(బైట్లు);
        uprv_memcpy(బైట్లు, p, (size_t)దత్తాంశం_నిడివి * 4);
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        // Write 8-bit data values.
        trie->దత్తాంశం.ptr8 = బైట్లు;
        కొరకు (int32_t i = దత్తాంశం_నిడివి; i > 0; --i) {
            *బైట్లు++ = static_cast<uint8_t>(*p++);
        }
        ఆపు;
    అప్రమేయ:
        // Will not occur, valueWidth checked at the beginning.
        ఆపు;
    }

#నిర్వచితమైతే UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#అయితే_ముగింపు

    clear();
    తిరిగి trie;
}

}  // namespace

U_NAMESPACE_చివర

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_విలువ, uint32_t error_విలువ, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    Local_సూచిక<Mutable_సంకేతం_బిందువు_Trie> trie(
        కొత్త Mutable_సంకేతం_బిందువు_Trie(initial_విలువ, error_విలువ, *p_Error_సంకేతం), *p_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(స్థిరరాశి UMutableCPTrie *other, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (other == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    Local_సూచిక<Mutable_సంకేతం_బిందువు_Trie> clone(
        కొత్త Mutable_సంకేతం_బిందువు_Trie(*reinterpret_cast<స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie *>(other), *p_Error_సంకేతం), *p_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI శూన్య U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    తొలగించు reinterpret_cast<Mutable_సంకేతం_బిందువు_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(స్థిరరాశి UCPMap *map, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (map == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<UMutableCPTrie *>(Mutable_సంకేతం_బిందువు_Trie::fromUCPMap(map, *p_Error_సంకేతం));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(స్థిరరాశి UCPTrie *trie, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (trie == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<UMutableCPTrie *>(Mutable_సంకేతం_బిందువు_Trie::fromUCPTrie(trie, *p_Error_సంకేతం));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(స్థిరరాశి UMutableCPTrie *trie, UChar32 c) {
    తిరిగి reinterpret_cast<స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie *>(trie)->get(c);
}

నామాకాశ {

UChar32 getRange(స్థిరరాశి శూన్య *trie, UChar32 ప్రారంభం,
                 UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context, uint32_t *p_విలువ) {
    తిరిగి reinterpret_cast<స్థిరరాశి Mutable_సంకేతం_బిందువు_Trie *>(trie)->
        getRange(ప్రారంభం, filter, context, p_విలువ);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(స్థిరరాశి UMutableCPTrie *trie, UChar32 ప్రారంభం,
                        UCPMapRangeOption option, uint32_t surrogate_విలువ,
                        UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context, uint32_t *p_విలువ) {
    తిరిగి ucptrie_అంతర్గత_Get_Range(getRange, trie, ప్రారంభం,
                                    option, surrogate_విలువ,
                                    filter, context, p_విలువ);
}

U_CAPI శూన్య U_EXPORT2
umutablecptrie_సమితి(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    reinterpret_cast<Mutable_సంకేతం_బిందువు_Trie *>(trie)->సమితి(c, value, *p_Error_సంకేతం);
}

U_CAPI శూన్య U_EXPORT2
umutablecptrie_సమితి_Range(UMutableCPTrie *trie, UChar32 ప్రారంభం, UChar32 చివర,
                   uint32_t value, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    reinterpret_cast<Mutable_సంకేతం_బిందువు_Trie *>(trie)->సమితి_Range(ప్రారంభం, చివర, value, *p_Error_సంకేతం);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు,
                              UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<Mutable_సంకేతం_బిందువు_Trie *>(trie)->build(type, విలువ_వెడల్పు, *p_Error_సంకేతం);
}

#నిర్వచితమైతే UCPTRIE_DEBUG
U_CFUNC శూన్య umutablecptrie_సమితి_Name(UMutableCPTrie *trie, స్థిరరాశి అక్షరం *name) {
    reinterpret_cast<Mutable_సంకేతం_బిందువు_Trie *>(trie)->name = name;
}
#అయితే_ముగింపు
