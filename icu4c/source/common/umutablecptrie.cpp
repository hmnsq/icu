// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
#   include <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಅತಿಪ್ರವಾಹ_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಅತಿಪ್ರವಾಹ_2
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_ಎಲ್ಲೆ = 0x110000;
constexpr int32_t BMP_ಎಲ್ಲೆ = 0x10000;
constexpr int32_t ASCII_ಎಲ್ಲೆ = 0x80;

constexpr int32_t I_ಎಲ್ಲೆ = UNICODE_ಎಲ್ಲೆ >> UCPTRIE_ಸರಿಸು_3;
constexpr int32_t BMP_I_ಎಲ್ಲೆ = BMP_ಎಲ್ಲೆ >> UCPTRIE_ಸರಿಸು_3;
constexpr int32_t ASCII_I_ಎಲ್ಲೆ = ASCII_ಎಲ್ಲೆ >> UCPTRIE_ಸರಿಸು_3;

constexpr int32_t SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ = (1 << (UCPTRIE_FAST_ಸರಿಸು - UCPTRIE_ಸರಿಸು_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = UNICODE_ಎಲ್ಲೆ;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ + UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ / 8;

ವರ್ಗ All_Same_ಖಂಡಗಳು;
ವರ್ಗ Mixed_ಖಂಡಗಳು;

ವರ್ಗ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie &other, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie &other) = ಅಳಿಸು;
    ~Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie();

    Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie &other) = ಅಳಿಸು;

    ಸ್ಥಿರ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *fromUCPMap(ಸ್ಥಿರಾಂಕ UCPMap *map, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *fromUCPTrie(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    uint32_t get(UChar32 c) ಸ್ಥಿರಾಂಕ;
    int32_t getRange(UChar32 ಆರಂಭ, UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                     uint32_t *p_ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ;

    ಶೂನ್ಯ ಗಣ(UChar32 c, uint32_t value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಶೂನ್ಯ ಗಣ_Range(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ, uint32_t value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    UCPTrie *build(UCPTrieType type, UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

ಖಾಸಗಿ:
    ಶೂನ್ಯ clear();

    ತಾರ್ಕಿಕ ensure_ಮೇಲಿನ_ಆರಂಭ(UChar32 c);
    int32_t alloc_ದತ್ತಾಂಶ_ಖಂಡ(int32_t ಖಂಡ_ಉದ್ದಳತೆ);
    int32_t get_ದತ್ತಾಂಶ_ಖಂಡ(int32_t i);

    ಶೂನ್ಯ ಮುಖವಾಡ_ಮೌಲ್ಯಗಳು(uint32_t mask);
    UChar32 ಕಂಡುಹಿಡಿ_ಮೇಲಿನ_ಆರಂಭ() ಸ್ಥಿರಾಂಕ;
    int32_t compact_ಸಂಪೂರ್ಣ_ದತ್ತಾಂಶ_ಖಂಡಗಳು(int32_t fastILimit, All_Same_ಖಂಡಗಳು &all_Same_ಖಂಡಗಳು);
    int32_t compact_ದತ್ತಾಂಶ(
            int32_t fastILimit, uint32_t *new_ದತ್ತಾಂಶ, int32_t new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
            int32_t ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ, Mixed_ಖಂಡಗಳು &mixed_ಖಂಡಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    int32_t compact_ಸೂಚ್ಯಂಕ(int32_t fastILimit, Mixed_ಖಂಡಗಳು &mixed_ಖಂಡಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    int32_t compactTrie(int32_t fastILimit, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    uint32_t *ಸೂಚ್ಯಂಕ = ಶೂನ್ಯಸೂಚಕ;
    int32_t ಸೂಚ್ಯಂಕ_ಸಾಮರ್ಥ್ಯ = 0;
    int32_t index3_Null_ಸ್ಥಾನಾಂತರ = -1;
    uint32_t *ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    int32_t ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = 0;
    int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = 0;
    int32_t ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ = -1;

    uint32_t orig_Initial_ಮೌಲ್ಯ;
    uint32_t initial_ಮೌಲ್ಯ;
    uint32_t error_ಮೌಲ್ಯ;
    UChar32 ಮೇಲಿನ_ಆರಂಭ;
    uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
#ಆದರೆ_ಅಂತ್ಯ
ಖಾಸಗಿ:
    /** Temporary array while building the final data. */
    uint16_t *index16 = ಶೂನ್ಯಸೂಚಕ;
    uint8_t flags[UNICODE_ಎಲ್ಲೆ >> UCPTRIE_ಸರಿಸು_3];
};

Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(uint32_t ini_ಮೌಲ್ಯ, uint32_t err_ಮೌಲ್ಯ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        orig_Initial_ಮೌಲ್ಯ(ini_ಮೌಲ್ಯ), initial_ಮೌಲ್ಯ(ini_ಮೌಲ್ಯ), error_ಮೌಲ್ಯ(err_ಮೌಲ್ಯ),
        ಮೇಲಿನ_ಆರಂಭ(0), ಮೇಲಿನ_ಮೌಲ್ಯ(initial_ಮೌಲ್ಯ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
        , name("open")
#ಆದರೆ_ಅಂತ್ಯ
        {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಸೂಚ್ಯಂಕ = static_cast<uint32_t*>(uprv_malloc(BMP_I_ಎಲ್ಲೆ * 4));
    ದತ್ತಾಂಶ = static_cast<uint32_t*>(uprv_malloc(INITIAL_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4));
    ಆದರೆ (ಸೂಚ್ಯಂಕ == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಸೂಚ್ಯಂಕ_ಸಾಮರ್ಥ್ಯ = BMP_I_ಎಲ್ಲೆ;
    ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = INITIAL_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
}

Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie &other, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        index3_Null_ಸ್ಥಾನಾಂತರ(other.index3_Null_ಸ್ಥಾನಾಂತರ),
        ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ(other.ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ),
        orig_Initial_ಮೌಲ್ಯ(other.orig_Initial_ಮೌಲ್ಯ), initial_ಮೌಲ್ಯ(other.initial_ಮೌಲ್ಯ),
        error_ಮೌಲ್ಯ(other.error_ಮೌಲ್ಯ),
        ಮೇಲಿನ_ಆರಂಭ(other.ಮೇಲಿನ_ಆರಂಭ), ಮೇಲಿನ_ಮೌಲ್ಯ(other.ಮೇಲಿನ_ಮೌಲ್ಯ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
        , name("mutable clone")
#ಆದರೆ_ಅಂತ್ಯ
        {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    int32_t i_ಸಾಮರ್ಥ್ಯ = ಮೇಲಿನ_ಆರಂಭ <= BMP_ಎಲ್ಲೆ ? BMP_I_ಎಲ್ಲೆ : I_ಎಲ್ಲೆ;
    ಸೂಚ್ಯಂಕ = static_cast<uint32_t*>(uprv_malloc(i_ಸಾಮರ್ಥ್ಯ * 4));
    ದತ್ತಾಂಶ = static_cast<uint32_t*>(uprv_malloc(other.ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ * 4));
    ಆದರೆ (ಸೂಚ್ಯಂಕ == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಸೂಚ್ಯಂಕ_ಸಾಮರ್ಥ್ಯ = i_ಸಾಮರ್ಥ್ಯ;
    ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = other.ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ;

    int32_t i_ಎಲ್ಲೆ = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
    uprv_memcpy(flags, other.flags, i_ಎಲ್ಲೆ);
    uprv_memcpy(ಸೂಚ್ಯಂಕ, other.ಸೂಚ್ಯಂಕ, i_ಎಲ್ಲೆ * 4);
    uprv_memcpy(ದತ್ತಾಂಶ, other.ದತ್ತಾಂಶ, (size_t)other.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4);
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = other.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    U_ASSERT(other.index16 == ಶೂನ್ಯಸೂಚಕ);
}

Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::~Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie() {
    uprv_free(ಸೂಚ್ಯಂಕ);
    uprv_free(ದತ್ತಾಂಶ);
    uprv_free(index16);
}

Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::fromUCPMap(ಸ್ಥಿರಾಂಕ UCPMap *map, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ಮೌಲ್ಯ = ucpmap_get(map, -1);
    uint32_t initial_ಮೌಲ್ಯ = ucpmap_get(map, 0x10ffff);
    Local_ಸೂಚಕ<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie> mutableTrie(
        ಹೊಸ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, error_ಸಂಕೇತ),
        error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    uint32_t value;
    ಆಗ ((ಕೊನೆ = ucpmap_getRange(map, ಆರಂಭ, UCPMAP_RANGE_ಸಾಮಾನ್ಯ, 0,
                                  ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &value)) >= 0) {
        ಆದರೆ (value != initial_ಮೌಲ್ಯ) {
            ಆದರೆ (ಆರಂಭ == ಕೊನೆ) {
                mutableTrie->ಗಣ(ಆರಂಭ, value, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                mutableTrie->ಗಣ_Range(ಆರಂಭ, ಕೊನೆ, value, error_ಸಂಕೇತ);
            }
        }
        ಆರಂಭ = ಕೊನೆ + 1;
    }
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ mutableTrie.orphan();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::fromUCPTrie(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ಮೌಲ್ಯ;
    uint32_t initial_ಮೌಲ್ಯ;
    ಆಯ್ಕೆ (trie->ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        error_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr16[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        initial_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr16[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        error_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr32[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        initial_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr32[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        error_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr8[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        initial_ಮೌಲ್ಯ = trie->ದತ್ತಾಂಶ.ptr8[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // Unreachable if the trie is properly initialized.
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Local_ಸೂಚಕ<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie> mutableTrie(
        ಹೊಸ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, error_ಸಂಕೇತ),
        error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UChar32 ಆರಂಭ = 0, ಕೊನೆ;
    uint32_t value;
    ಆಗ ((ಕೊನೆ = ucptrie_getRange(trie, ಆರಂಭ, UCPMAP_RANGE_ಸಾಮಾನ್ಯ, 0,
                                   ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &value)) >= 0) {
        ಆದರೆ (value != initial_ಮೌಲ್ಯ) {
            ಆದರೆ (ಆರಂಭ == ಕೊನೆ) {
                mutableTrie->ಗಣ(ಆರಂಭ, value, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                mutableTrie->ಗಣ_Range(ಆರಂಭ, ಕೊನೆ, value, error_ಸಂಕೇತ);
            }
        }
        ಆರಂಭ = ಕೊನೆ + 1;
    }
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ mutableTrie.orphan();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

ಶೂನ್ಯ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::clear() {
    index3_Null_ಸ್ಥಾನಾಂತರ = ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ = -1;
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = 0;
    ಮೇಲಿನ_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ = orig_Initial_ಮೌಲ್ಯ;
    ಮೇಲಿನ_ಆರಂಭ = 0;
    uprv_free(index16);
    index16 = ಶೂನ್ಯಸೂಚಕ;
}

uint32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::get(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        ಹಿಂತಿರುಗಿ error_ಮೌಲ್ಯ;
    }
    ಆದರೆ (c >= ಮೇಲಿನ_ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ ಮೇಲಿನ_ಮೌಲ್ಯ;
    }
    int32_t i = c >> UCPTRIE_ಸರಿಸು_3;
    ಆದರೆ (flags[i] == ALL_SAME) {
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ[i];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ[ಸೂಚ್ಯಂಕ[i] + (c & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ)];
    }
}

ಸಾಲೊಳಗೆ uint32_t maybe_Filter_ಮೌಲ್ಯ(uint32_t value, uint32_t initial_ಮೌಲ್ಯ, uint32_t null_ಮೌಲ್ಯ,
                                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    ಆದರೆ (value == initial_ಮೌಲ್ಯ) {
        value = null_ಮೌಲ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filter != ಶೂನ್ಯಸೂಚಕ) {
        value = filter(context, value);
    }
    ಹಿಂತಿರುಗಿ value;
}

UChar32 Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::getRange(
        UChar32 ಆರಂಭ, UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
        uint32_t *p_ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (static_cast<uint32_t>(ಆರಂಭ) > MAX_UNICODE) {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
    ಆದರೆ (ಆರಂಭ >= ಮೇಲಿನ_ಆರಂಭ) {
        ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
            uint32_t value = ಮೇಲಿನ_ಮೌಲ್ಯ;
            ಆದರೆ (filter != ಶೂನ್ಯಸೂಚಕ) { value = filter(context, value); }
            *p_ಮೌಲ್ಯ = value;
        }
        ಹಿಂತಿರುಗಿ MAX_UNICODE;
    }
    uint32_t null_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ;
    ಆದರೆ (filter != ಶೂನ್ಯಸೂಚಕ) { null_ಮೌಲ್ಯ = filter(context, null_ಮೌಲ್ಯ); }
    UChar32 c = ಆರಂಭ;
    uint32_t trie_ಮೌಲ್ಯ, value;
    ತಾರ್ಕಿಕ have_ಮೌಲ್ಯ = ಸುಳ್ಳು;
    int32_t i = c >> UCPTRIE_ಸರಿಸು_3;
    ಮಾಡು {
        ಆದರೆ (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = ಸೂಚ್ಯಂಕ[i];
            ಆದರೆ (have_ಮೌಲ್ಯ) {
                ಆದರೆ (trieValue2 != trie_ಮೌಲ್ಯ) {
                    ಆದರೆ (filter == ಶೂನ್ಯಸೂಚಕ ||
                            maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                             filter, context) != value) {
                        ಹಿಂತಿರುಗಿ c - 1;
                    }
                    trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                trie_ಮೌಲ್ಯ = trieValue2;
                value = maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ, filter, context);
                ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) { *p_ಮೌಲ್ಯ = value; }
                have_ಮೌಲ್ಯ = ನಿಜ;
            }
            c = (c + UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) & ~UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ;
        } ಇಲ್ಲದಿದ್ದರೆ /* MIXED */ {
            int32_t di = ಸೂಚ್ಯಂಕ[i] + (c & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ);
            uint32_t trieValue2 = ದತ್ತಾಂಶ[di];
            ಆದರೆ (have_ಮೌಲ್ಯ) {
                ಆದರೆ (trieValue2 != trie_ಮೌಲ್ಯ) {
                    ಆದರೆ (filter == ಶೂನ್ಯಸೂಚಕ ||
                            maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                             filter, context) != value) {
                        ಹಿಂತಿರುಗಿ c - 1;
                    }
                    trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                trie_ಮೌಲ್ಯ = trieValue2;
                value = maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ, filter, context);
                ಆದರೆ (p_ಮೌಲ್ಯ != ಶೂನ್ಯಸೂಚಕ) { *p_ಮೌಲ್ಯ = value; }
                have_ಮೌಲ್ಯ = ನಿಜ;
            }
            ಆಗ ((++c & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ) != 0) {
                trieValue2 = ದತ್ತಾಂಶ[++di];
                ಆದರೆ (trieValue2 != trie_ಮೌಲ್ಯ) {
                    ಆದರೆ (filter == ಶೂನ್ಯಸೂಚಕ ||
                            maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                             filter, context) != value) {
                        ಹಿಂತಿರುಗಿ c - 1;
                    }
                }
                trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
            }
        }
        ++i;
    } ಆಗ (c < ಮೇಲಿನ_ಆರಂಭ);
    U_ASSERT(have_ಮೌಲ್ಯ);
    ಆದರೆ (maybe_Filter_ಮೌಲ್ಯ(ಮೇಲಿನ_ಮೌಲ್ಯ, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                         filter, context) != value) {
        ಹಿಂತಿರುಗಿ c - 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ MAX_UNICODE;
    }
}

ಶೂನ್ಯ
write_ಖಂಡ(uint32_t *ಖಂಡ, uint32_t value) {
    uint32_t *ಎಲ್ಲೆ = ಖಂಡ + UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    ಆಗ (ಖಂಡ < ಎಲ್ಲೆ) {
        *ಖಂಡ++ = value;
    }
}

ತಾರ್ಕಿಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::ensure_ಮೇಲಿನ_ಆರಂಭ(UChar32 c) {
    ಆದರೆ (c >= ಮೇಲಿನ_ಆರಂಭ) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು) & ~(UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು - 1);
        int32_t i = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
        int32_t i_ಎಲ್ಲೆ = c >> UCPTRIE_ಸರಿಸು_3;
        ಆದರೆ (i_ಎಲ್ಲೆ > ಸೂಚ್ಯಂಕ_ಸಾಮರ್ಥ್ಯ) {
            uint32_t* new_ಸೂಚ್ಯಂಕ = static_cast<uint32_t*>(uprv_malloc(I_ಎಲ್ಲೆ * 4));
            ಆದರೆ (new_ಸೂಚ್ಯಂಕ == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
            uprv_memcpy(new_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ, i * 4);
            uprv_free(ಸೂಚ್ಯಂಕ);
            ಸೂಚ್ಯಂಕ = new_ಸೂಚ್ಯಂಕ;
            ಸೂಚ್ಯಂಕ_ಸಾಮರ್ಥ್ಯ = I_ಎಲ್ಲೆ;
        }
        ಮಾಡು {
            flags[i] = ALL_SAME;
            ಸೂಚ್ಯಂಕ[i] = initial_ಮೌಲ್ಯ;
        } ಆಗ(++i < i_ಎಲ್ಲೆ);
        ಮೇಲಿನ_ಆರಂಭ = c;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

int32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::alloc_ದತ್ತಾಂಶ_ಖಂಡ(int32_t ಖಂಡ_ಉದ್ದಳತೆ) {
    int32_t new_ಖಂಡ = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    int32_t new_ತುದಿ = new_ಖಂಡ + ಖಂಡ_ಉದ್ದಳತೆ;
    ಆದರೆ (new_ತುದಿ > ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
        int32_t capacity;
        ಆದರೆ (ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ < MEDIUM_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
            capacity = MEDIUM_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ < MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
            capacity = MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            ಹಿಂತಿರುಗಿ -1;
        }
        uint32_t* new_ದತ್ತಾಂಶ = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        ಆದರೆ (new_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ -1;
        }
        uprv_memcpy(new_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ, (size_t)ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4);
        uprv_free(ದತ್ತಾಂಶ);
        ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
        ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = capacity;
    }
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = new_ತುದಿ;
    ಹಿಂತಿರುಗಿ new_ಖಂಡ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::get_ದತ್ತಾಂಶ_ಖಂಡ(int32_t i) {
    ಆದರೆ (flags[i] == MIXED) {
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ[i];
    }
    ಆದರೆ (i < BMP_I_ಎಲ್ಲೆ) {
        int32_t new_ಖಂಡ = alloc_ದತ್ತಾಂಶ_ಖಂಡ(UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ);
        ಆದರೆ (new_ಖಂಡ < 0) { ಹಿಂತಿರುಗಿ new_ಖಂಡ; }
        int32_t i_ಆರಂಭ = i & ~(SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ -1);
        int32_t i_ಎಲ್ಲೆ = i_ಆರಂಭ + SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ;
        ಮಾಡು {
            U_ASSERT(flags[i_ಆರಂಭ] == ALL_SAME);
            write_ಖಂಡ(ದತ್ತಾಂಶ + new_ಖಂಡ, ಸೂಚ್ಯಂಕ[i_ಆರಂಭ]);
            flags[i_ಆರಂಭ] = MIXED;
            ಸೂಚ್ಯಂಕ[i_ಆರಂಭ++] = new_ಖಂಡ;
            new_ಖಂಡ += UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        } ಆಗ (i_ಆರಂಭ < i_ಎಲ್ಲೆ);
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ[i];
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t new_ಖಂಡ = alloc_ದತ್ತಾಂಶ_ಖಂಡ(UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ);
        ಆದರೆ (new_ಖಂಡ < 0) { ಹಿಂತಿರುಗಿ new_ಖಂಡ; }
        write_ಖಂಡ(ದತ್ತಾಂಶ + new_ಖಂಡ, ಸೂಚ್ಯಂಕ[i]);
        flags[i] = MIXED;
        ಸೂಚ್ಯಂಕ[i] = new_ಖಂಡ;
        ಹಿಂತಿರುಗಿ new_ಖಂಡ;
    }
}

ಶೂನ್ಯ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::ಗಣ(UChar32 c, uint32_t value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    int32_t ಖಂಡ;
    ಆದರೆ (!ensure_ಮೇಲಿನ_ಆರಂಭ(c) || (ಖಂಡ = get_ದತ್ತಾಂಶ_ಖಂಡ(c >> UCPTRIE_ಸರಿಸು_3)) < 0) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ದತ್ತಾಂಶ[ಖಂಡ + (c & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ)] = value;
}

ಶೂನ್ಯ
fill_ಖಂಡ(uint32_t *ಖಂಡ, UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ, uint32_t value) {
    uint32_t *p_ಎಲ್ಲೆ = ಖಂಡ + ಎಲ್ಲೆ;
    ಖಂಡ += ಆರಂಭ;
    ಆಗ (ಖಂಡ < p_ಎಲ್ಲೆ) {
        *ಖಂಡ++ = value;
    }
}

ಶೂನ್ಯ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::ಗಣ_Range(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ, uint32_t value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (static_cast<uint32_t>(ಆರಂಭ) > MAX_UNICODE || static_cast<uint32_t>(ಕೊನೆ) > MAX_UNICODE || ಆರಂಭ > ಕೊನೆ) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!ensure_ಮೇಲಿನ_ಆರಂಭ(ಕೊನೆ)) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    UChar32 ಎಲ್ಲೆ = ಕೊನೆ + 1;
    ಆದರೆ (ಆರಂಭ & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ) {
        // Set partial block at [start..following block boundary[.
        int32_t ಖಂಡ = get_ದತ್ತಾಂಶ_ಖಂಡ(ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3);
        ಆದರೆ (ಖಂಡ < 0) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        UChar32 ಮುಂದಿನ_ಆರಂಭ = (ಆರಂಭ + UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ) & ~UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ;
        ಆದರೆ (ಮುಂದಿನ_ಆರಂಭ <= ಎಲ್ಲೆ) {
            fill_ಖಂಡ(ದತ್ತಾಂಶ + ಖಂಡ, ಆರಂಭ & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ, UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ,
                      value);
            ಆರಂಭ = ಮುಂದಿನ_ಆರಂಭ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            fill_ಖಂಡ(ದತ್ತಾಂಶ + ಖಂಡ, ಆರಂಭ & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ, ಎಲ್ಲೆ & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ,
                      value);
            ಹಿಂತಿರುಗಿ;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = ಎಲ್ಲೆ & UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ;

    // Round down limit to a block boundary.
    ಎಲ್ಲೆ &= ~UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಮುಖವಾಡ;

    // Iterate over all-value blocks.
    ಆಗ (ಆರಂಭ < ಎಲ್ಲೆ) {
        int32_t i = ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
        ಆದರೆ (flags[i] == ALL_SAME) {
            ಸೂಚ್ಯಂಕ[i] = value;
        } ಇಲ್ಲದಿದ್ದರೆ /* MIXED */ {
            fill_ಖಂಡ(ದತ್ತಾಂಶ + ಸೂಚ್ಯಂಕ[i], 0, UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, value);
        }
        ಆರಂಭ += UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    }

    ಆದರೆ (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t ಖಂಡ = get_ದತ್ತಾಂಶ_ಖಂಡ(ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3);
        ಆದರೆ (ಖಂಡ < 0) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        fill_ಖಂಡ(ದತ್ತಾಂಶ + ಖಂಡ, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

ಶೂನ್ಯ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::ಮುಖವಾಡ_ಮೌಲ್ಯಗಳು(uint32_t mask) {
    initial_ಮೌಲ್ಯ &= mask;
    error_ಮೌಲ್ಯ &= mask;
    ಮೇಲಿನ_ಮೌಲ್ಯ &= mask;
    int32_t i_ಎಲ್ಲೆ = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
    ಗಾಗಿ (int32_t i = 0; i < i_ಎಲ್ಲೆ; ++i) {
        ಆದರೆ (flags[i] == ALL_SAME) {
            ಸೂಚ್ಯಂಕ[i] &= mask;
        }
    }
    ಗಾಗಿ (int32_t i = 0; i < ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; ++i) {
        ದತ್ತಾಂಶ[i] &= mask;
    }
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UIntA, ಬಗೆ_ಹೆಸರು UIntB>
ತಾರ್ಕಿಕ equal_ಖಂಡಗಳು(ಸ್ಥಿರಾಂಕ UIntA *s, ಸ್ಥಿರಾಂಕ UIntB *t, int32_t length) {
    ಆಗ (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    ಹಿಂತಿರುಗಿ length == 0;
}

ತಾರ್ಕಿಕ all_ಮೌಲ್ಯಗಳು_Same_As(ಸ್ಥಿರಾಂಕ uint32_t *p, int32_t length, uint32_t value) {
    ಸ್ಥಿರಾಂಕ uint32_t *p_ಎಲ್ಲೆ = p + length;
    ಆಗ (p < p_ಎಲ್ಲೆ && *p == value) { ++p; }
    ಹಿಂತಿರುಗಿ p == p_ಎಲ್ಲೆ;
}

/** Search for an identical block. */
int32_t ಕಂಡುಹಿಡಿ_Same_ಖಂಡ(ಸ್ಥಿರಾಂಕ uint16_t *p, int32_t p_ಆರಂಭ, int32_t length,
                      ಸ್ಥಿರಾಂಕ uint16_t *q, int32_t q_ಆರಂಭ, int32_t ಖಂಡ_ಉದ್ದಳತೆ) {
    // Ensure that we do not even partially get past length.
    length -= ಖಂಡ_ಉದ್ದಳತೆ;

    q += q_ಆರಂಭ;
    ಆಗ (p_ಆರಂಭ <= length) {
        ಆದರೆ (equal_ಖಂಡಗಳು(p + p_ಆರಂಭ, q, ಖಂಡ_ಉದ್ದಳತೆ)) {
            ಹಿಂತಿರುಗಿ p_ಆರಂಭ;
        }
        ++p_ಆರಂಭ;
    }
    ಹಿಂತಿರುಗಿ -1;
}

int32_t ಕಂಡುಹಿಡಿ_All_Same_ಖಂಡ(ಸ್ಥಿರಾಂಕ uint32_t *p, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ,
                         uint32_t value, int32_t ಖಂಡ_ಉದ್ದಳತೆ) {
    // Ensure that we do not even partially get past limit.
    ಎಲ್ಲೆ -= ಖಂಡ_ಉದ್ದಳತೆ;

    ಗಾಗಿ (int32_t ಖಂಡ = ಆರಂಭ; ಖಂಡ <= ಎಲ್ಲೆ; ++ಖಂಡ) {
        ಆದರೆ (p[ಖಂಡ] == value) {
            ಗಾಗಿ (int32_t i = 1;; ++i) {
                ಆದರೆ (i == ಖಂಡ_ಉದ್ದಳತೆ) {
                    ಹಿಂತಿರುಗಿ ಖಂಡ;
                }
                ಆದರೆ (p[ಖಂಡ + i] != value) {
                    ಖಂಡ += i;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UIntA, ಬಗೆ_ಹೆಸರು UIntB>
int32_t getOverlap(ಸ್ಥಿರಾಂಕ UIntA *p, int32_t length,
                   ಸ್ಥಿರಾಂಕ UIntB *q, int32_t q_ಆರಂಭ, int32_t ಖಂಡ_ಉದ್ದಳತೆ) {
    int32_t overlap = ಖಂಡ_ಉದ್ದಳತೆ - 1;
    U_ASSERT(overlap <= length);
    q += q_ಆರಂಭ;
    ಆಗ (overlap > 0 && !equal_ಖಂಡಗಳು(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    ಹಿಂತಿರುಗಿ overlap;
}

int32_t getAllSameOverlap(ಸ್ಥಿರಾಂಕ uint32_t *p, int32_t length, uint32_t value,
                          int32_t ಖಂಡ_ಉದ್ದಳತೆ) {
    int32_t min = length - (ಖಂಡ_ಉದ್ದಳತೆ - 1);
    int32_t i = length;
    ಆಗ (min < i && p[i - 1] == value) { --i; }
    ಹಿಂತಿರುಗಿ length - i;
}

ತಾರ್ಕಿಕ is_ಆರಂಭ_ದ_Some_Fast_ಖಂಡ(uint32_t ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ uint32_t ಸೂಚ್ಯಂಕ[], int32_t fastILimit) {
    ಗಾಗಿ (int32_t i = 0; i < fastILimit; i += SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ[i] == ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::ಕಂಡುಹಿಡಿ_ಮೇಲಿನ_ಆರಂಭ() ಸ್ಥಿರಾಂಕ {
    int32_t i = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
    ಆಗ (i > 0) {
        ತಾರ್ಕಿಕ ಹೊಂದಿಕೆ;
        ಆದರೆ (flags[--i] == ALL_SAME) {
            ಹೊಂದಿಕೆ = ಸೂಚ್ಯಂಕ[i] == ಮೇಲಿನ_ಮೌಲ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ /* MIXED */ {
            ಸ್ಥಿರಾಂಕ uint32_t *p = ದತ್ತಾಂಶ + ಸೂಚ್ಯಂಕ[i];
            ಗಾಗಿ (int32_t j = 0;; ++j) {
                ಆದರೆ (j == UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
                    ಹೊಂದಿಕೆ = ನಿಜ;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (p[j] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
                    ಹೊಂದಿಕೆ = ಸುಳ್ಳು;
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಆದರೆ (!ಹೊಂದಿಕೆ) {
            ಹಿಂತಿರುಗಿ (i + 1) << UCPTRIE_ಸರಿಸು_3;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

ವರ್ಗ All_Same_ಖಂಡಗಳು {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ constexpr int32_t NEW_UNIQUE = -1;
    ಸ್ಥಿರ constexpr int32_t ಅತಿಪ್ರವಾಹ_2 = -2;

    All_Same_ಖಂಡಗಳು() : length(0), mostRecent(-1) {}

    int32_t ಕಂಡುಹಿಡಿ_Or_ಕೂಡಿಸು(int32_t ಸೂಚ್ಯಂಕ, int32_t count, uint32_t value) {
        ಆದರೆ (mostRecent >= 0 && ಮೌಲ್ಯಗಳು[mostRecent] == value) {
            refCounts[mostRecent] += count;
            ಹಿಂತಿರುಗಿ indexes[mostRecent];
        }
        ಗಾಗಿ (int32_t i = 0; i < length; ++i) {
            ಆದರೆ (ಮೌಲ್ಯಗಳು[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                ಹಿಂತಿರುಗಿ indexes[i];
            }
        }
        ಆದರೆ (length == ಸಾಮರ್ಥ್ಯ_3) {
            ಹಿಂತಿರುಗಿ ಅತಿಪ್ರವಾಹ_2;
        }
        mostRecent = length;
        indexes[length] = ಸೂಚ್ಯಂಕ;
        ಮೌಲ್ಯಗಳು[length] = value;
        refCounts[length++] = count;
        ಹಿಂತಿರುಗಿ NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    ಶೂನ್ಯ ಕೂಡಿಸು(int32_t ಸೂಚ್ಯಂಕ, int32_t count, uint32_t value) {
        U_ASSERT(length == ಸಾಮರ್ಥ್ಯ_3);
        int32_t least = -1;
        int32_t ಕನಿಷ್ಠ_ಎಣಿಕೆ = I_ಎಲ್ಲೆ;
        ಗಾಗಿ (int32_t i = 0; i < length; ++i) {
            U_ASSERT(ಮೌಲ್ಯಗಳು[i] != value);
            ಆದರೆ (refCounts[i] < ಕನಿಷ್ಠ_ಎಣಿಕೆ) {
                least = i;
                ಕನಿಷ್ಠ_ಎಣಿಕೆ = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = ಸೂಚ್ಯಂಕ;
        ಮೌಲ್ಯಗಳು[least] = value;
        refCounts[least] = count;
    }

    int32_t ಕಂಡುಹಿಡಿ_Most_ಬಳಸಿದ() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (length == 0) { ಹಿಂತಿರುಗಿ -1; }
        int32_t max = -1;
        int32_t max_ಎಣಿಕೆ = 0;
        ಗಾಗಿ (int32_t i = 0; i < length; ++i) {
            ಆದರೆ (refCounts[i] > max_ಎಣಿಕೆ) {
                max = i;
                max_ಎಣಿಕೆ = refCounts[i];
            }
        }
        ಹಿಂತಿರುಗಿ indexes[max];
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರ constexpr int32_t ಸಾಮರ್ಥ್ಯ_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[ಸಾಮರ್ಥ್ಯ_3];
    uint32_t ಮೌಲ್ಯಗಳು[ಸಾಮರ್ಥ್ಯ_3];
    int32_t refCounts[ಸಾಮರ್ಥ್ಯ_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
ವರ್ಗ Mixed_ಖಂಡಗಳು {
ಸಾರ್ವಜನಿಕ:
    Mixed_ಖಂಡಗಳು() {}
    ~Mixed_ಖಂಡಗಳು() {
        uprv_free(ಕೋಷ್ಟಕ);
    }

    ತಾರ್ಕಿಕ init(int32_t max_ಉದ್ದಳತೆ_2, int32_t new_ಖಂಡ_ಉದ್ದಳತೆ) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ = max_ಉದ್ದಳತೆ_2 - new_ಖಂಡ_ಉದ್ದಳತೆ + 1;
        int32_t new_ಉದ್ದಳತೆ;
        ಆದರೆ (max_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ <= 0xfff) {  // 4k
            new_ಉದ್ದಳತೆ = 6007;
            ಸರಿಸು = 12;
            mask = 0xfff;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ <= 0x7fff) {  // 32k
            new_ಉದ್ದಳತೆ = 50021;
            ಸರಿಸು = 15;
            mask = 0x7fff;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ <= 0x1ffff) {  // 128k
            new_ಉದ್ದಳತೆ = 200003;
            ಸರಿಸು = 17;
            mask = 0x1ffff;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_ಉದ್ದಳತೆ = 1500007;
            ಸರಿಸು = 21;
            mask = 0x1fffff;
        }
        ಆದರೆ (new_ಉದ್ದಳತೆ > capacity) {
            uprv_free(ಕೋಷ್ಟಕ);
            ಕೋಷ್ಟಕ = static_cast<uint32_t*>(uprv_malloc(new_ಉದ್ದಳತೆ * 4));
            ಆದರೆ (ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            capacity = new_ಉದ್ದಳತೆ;
        }
        length = new_ಉದ್ದಳತೆ;
        uprv_memset(ಕೋಷ್ಟಕ, 0, length * 4);

        ಖಂಡ_ಉದ್ದಳತೆ = new_ಖಂಡ_ಉದ್ದಳತೆ;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UInt>
    ಶೂನ್ಯ extend(ಸ್ಥಿರಾಂಕ UInt *ದತ್ತಾಂಶ, int32_t min_ಆರಂಭ, int32_t prev_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, int32_t new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
        int32_t ಆರಂಭ = prev_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - ಖಂಡ_ಉದ್ದಳತೆ;
        ಆದರೆ (ಆರಂಭ >= min_ಆರಂಭ) {
            ++ಆರಂಭ;  // Skip the last block that we added last time.
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ = min_ಆರಂಭ;  // Begin with the first full block.
        }
        ಗಾಗಿ (int32_t ಕೊನೆ = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - ಖಂಡ_ಉದ್ದಳತೆ; ಆರಂಭ <= ಕೊನೆ; ++ಆರಂಭ) {
            uint32_t hash_ಸಂಕೇತ = make_Hash_ಸಂಕೇತ(ದತ್ತಾಂಶ, ಆರಂಭ);
            ಕೂಡಿಸು_ನಮೂದು(ದತ್ತಾಂಶ, ಆರಂಭ, hash_ಸಂಕೇತ, ಆರಂಭ);
        }
    }

    ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UIntA, ಬಗೆ_ಹೆಸರು UIntB>
    int32_t ಕಂಡುಹಿಡಿ_ಖಂಡ(ಸ್ಥಿರಾಂಕ UIntA *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ UIntB *ಖಂಡ_ದತ್ತಾಂಶ, int32_t ಖಂಡ_ಆರಂಭ) ಸ್ಥಿರಾಂಕ {
        uint32_t hash_ಸಂಕೇತ = make_Hash_ಸಂಕೇತ(ಖಂಡ_ದತ್ತಾಂಶ, ಖಂಡ_ಆರಂಭ);
        int32_t ನಮೂದು_ಸೂಚ್ಯಂಕ = ಕಂಡುಹಿಡಿ_ನಮೂದು(ದತ್ತಾಂಶ, ಖಂಡ_ದತ್ತಾಂಶ, ಖಂಡ_ಆರಂಭ, hash_ಸಂಕೇತ);
        ಆದರೆ (ನಮೂದು_ಸೂಚ್ಯಂಕ >= 0) {
            ಹಿಂತಿರುಗಿ (ಕೋಷ್ಟಕ[ನಮೂದು_ಸೂಚ್ಯಂಕ] & mask) - 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ -1;
        }
    }

    int32_t ಕಂಡುಹಿಡಿ_All_Same_ಖಂಡ(ಸ್ಥಿರಾಂಕ uint32_t *ದತ್ತಾಂಶ, uint32_t ಖಂಡ_ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ {
        uint32_t hash_ಸಂಕೇತ = make_Hash_ಸಂಕೇತ(ಖಂಡ_ಮೌಲ್ಯ);
        int32_t ನಮೂದು_ಸೂಚ್ಯಂಕ = ಕಂಡುಹಿಡಿ_ನಮೂದು(ದತ್ತಾಂಶ, ಖಂಡ_ಮೌಲ್ಯ, hash_ಸಂಕೇತ);
        ಆದರೆ (ನಮೂದು_ಸೂಚ್ಯಂಕ >= 0) {
            ಹಿಂತಿರುಗಿ (ಕೋಷ್ಟಕ[ನಮೂದು_ಸೂಚ್ಯಂಕ] & mask) - 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ -1;
        }
    }

ಖಾಸಗಿ:
    ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UInt>
    uint32_t make_Hash_ಸಂಕೇತ(ಸ್ಥಿರಾಂಕ UInt *ಖಂಡ_ದತ್ತಾಂಶ, int32_t ಖಂಡ_ಆರಂಭ) ಸ್ಥಿರಾಂಕ {
        int32_t ಖಂಡ_ಎಲ್ಲೆ = ಖಂಡ_ಆರಂಭ + ಖಂಡ_ಉದ್ದಳತೆ;
        uint32_t hash_ಸಂಕೇತ = ಖಂಡ_ದತ್ತಾಂಶ[ಖಂಡ_ಆರಂಭ++];
        ಮಾಡು {
            hash_ಸಂಕೇತ = 37 * hash_ಸಂಕೇತ + ಖಂಡ_ದತ್ತಾಂಶ[ಖಂಡ_ಆರಂಭ++];
        } ಆಗ (ಖಂಡ_ಆರಂಭ < ಖಂಡ_ಎಲ್ಲೆ);
        ಹಿಂತಿರುಗಿ hash_ಸಂಕೇತ;
    }

    uint32_t make_Hash_ಸಂಕೇತ(uint32_t ಖಂಡ_ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ {
        uint32_t hash_ಸಂಕೇತ = ಖಂಡ_ಮೌಲ್ಯ;
        ಗಾಗಿ (int32_t i = 1; i < ಖಂಡ_ಉದ್ದಳತೆ; ++i) {
            hash_ಸಂಕೇತ = 37 * hash_ಸಂಕೇತ + ಖಂಡ_ಮೌಲ್ಯ;
        }
        ಹಿಂತಿರುಗಿ hash_ಸಂಕೇತ;
    }

    ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UInt>
    ಶೂನ್ಯ ಕೂಡಿಸು_ನಮೂದು(ಸ್ಥಿರಾಂಕ UInt *ದತ್ತಾಂಶ, int32_t ಖಂಡ_ಆರಂಭ, uint32_t hash_ಸಂಕೇತ, int32_t ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ) {
        U_ASSERT(0 <= ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ && ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ < (int32_t)mask);
        int32_t ನಮೂದು_ಸೂಚ್ಯಂಕ = ಕಂಡುಹಿಡಿ_ನಮೂದು(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ, ಖಂಡ_ಆರಂಭ, hash_ಸಂಕೇತ);
        ಆದರೆ (ನಮೂದು_ಸೂಚ್ಯಂಕ < 0) {
            ಕೋಷ್ಟಕ[~ನಮೂದು_ಸೂಚ್ಯಂಕ] = (hash_ಸಂಕೇತ << ಸರಿಸು) | (ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ + 1);
        }
    }

    ಮಾದರಿ<ಬಗೆ_ಹೆಸರು UIntA, ಬಗೆ_ಹೆಸರು UIntB>
    int32_t ಕಂಡುಹಿಡಿ_ನಮೂದು(ಸ್ಥಿರಾಂಕ UIntA *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ UIntB *ಖಂಡ_ದತ್ತಾಂಶ, int32_t ಖಂಡ_ಆರಂಭ,
                      uint32_t hash_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
        uint32_t shifted_Hash_ಸಂಕೇತ = hash_ಸಂಕೇತ << ಸರಿಸು;
        int32_t initial_ನಮೂದು_ಸೂಚ್ಯಂಕ = (hash_ಸಂಕೇತ % (length - 1)) + 1;  // 1..length-1
        ಗಾಗಿ (int32_t ನಮೂದು_ಸೂಚ್ಯಂಕ = initial_ನಮೂದು_ಸೂಚ್ಯಂಕ;;) {
            uint32_t ನಮೂದು = ಕೋಷ್ಟಕ[ನಮೂದು_ಸೂಚ್ಯಂಕ];
            ಆದರೆ (ನಮೂದು == 0) {
                ಹಿಂತಿರುಗಿ ~ನಮೂದು_ಸೂಚ್ಯಂಕ;
            }
            ಆದರೆ ((ನಮೂದು & ~mask) == shifted_Hash_ಸಂಕೇತ) {
                int32_t ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ = (ನಮೂದು & mask) - 1;
                ಆದರೆ (equal_ಖಂಡಗಳು(ದತ್ತಾಂಶ + ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ, ಖಂಡ_ದತ್ತಾಂಶ + ಖಂಡ_ಆರಂಭ, ಖಂಡ_ಉದ್ದಳತೆ)) {
                    ಹಿಂತಿರುಗಿ ನಮೂದು_ಸೂಚ್ಯಂಕ;
                }
            }
            ನಮೂದು_ಸೂಚ್ಯಂಕ = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(initial_ನಮೂದು_ಸೂಚ್ಯಂಕ, ನಮೂದು_ಸೂಚ್ಯಂಕ);
        }
    }

    int32_t ಕಂಡುಹಿಡಿ_ನಮೂದು(ಸ್ಥಿರಾಂಕ uint32_t *ದತ್ತಾಂಶ, uint32_t ಖಂಡ_ಮೌಲ್ಯ, uint32_t hash_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
        uint32_t shifted_Hash_ಸಂಕೇತ = hash_ಸಂಕೇತ << ಸರಿಸು;
        int32_t initial_ನಮೂದು_ಸೂಚ್ಯಂಕ = (hash_ಸಂಕೇತ % (length - 1)) + 1;  // 1..length-1
        ಗಾಗಿ (int32_t ನಮೂದು_ಸೂಚ್ಯಂಕ = initial_ನಮೂದು_ಸೂಚ್ಯಂಕ;;) {
            uint32_t ನಮೂದು = ಕೋಷ್ಟಕ[ನಮೂದು_ಸೂಚ್ಯಂಕ];
            ಆದರೆ (ನಮೂದು == 0) {
                ಹಿಂತಿರುಗಿ ~ನಮೂದು_ಸೂಚ್ಯಂಕ;
            }
            ಆದರೆ ((ನಮೂದು & ~mask) == shifted_Hash_ಸಂಕೇತ) {
                int32_t ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ = (ನಮೂದು & mask) - 1;
                ಆದರೆ (all_ಮೌಲ್ಯಗಳು_Same_As(ದತ್ತಾಂಶ + ದತ್ತಾಂಶ_ಸೂಚ್ಯಂಕ, ಖಂಡ_ಉದ್ದಳತೆ, ಖಂಡ_ಮೌಲ್ಯ)) {
                    ಹಿಂತಿರುಗಿ ನಮೂದು_ಸೂಚ್ಯಂಕ;
                }
            }
            ನಮೂದು_ಸೂಚ್ಯಂಕ = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(initial_ನಮೂದು_ಸೂಚ್ಯಂಕ, ನಮೂದು_ಸೂಚ್ಯಂಕ);
        }
    }

    ಸಾಲೊಳಗೆ int32_t ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(int32_t initial_ನಮೂದು_ಸೂಚ್ಯಂಕ, int32_t ನಮೂದು_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        ಹಿಂತಿರುಗಿ (ನಮೂದು_ಸೂಚ್ಯಂಕ + initial_ನಮೂದು_ಸೂಚ್ಯಂಕ) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *ಕೋಷ್ಟಕ = ಶೂನ್ಯಸೂಚಕ;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t ಸರಿಸು = 0;
    uint32_t mask = 0;

    int32_t ಖಂಡ_ಉದ್ದಳತೆ = 0;
};

int32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::compact_ಸಂಪೂರ್ಣ_ದತ್ತಾಂಶ_ಖಂಡಗಳು(int32_t fastILimit, All_Same_ಖಂಡಗಳು &all_Same_ಖಂಡಗಳು) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    ತಾರ್ಕಿಕ ಅತಿಪ್ರವಾಹ = ಸುಳ್ಳು;
#ಆದರೆ_ಅಂತ್ಯ

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = ASCII_ಎಲ್ಲೆ;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ += UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    // Add room for special values (errorValue, highValue) and padding.
    new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ += 4;
    int32_t i_ಎಲ್ಲೆ = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
    int32_t ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    int32_t inc = SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ;
    ಗಾಗಿ (int32_t i = 0; i < i_ಎಲ್ಲೆ; i += inc) {
        ಆದರೆ (i == fastILimit) {
            ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            inc = 1;
        }
        uint32_t value = ಸೂಚ್ಯಂಕ[i];
        ಆದರೆ (flags[i] == MIXED) {
            // Really mixed?
            ಸ್ಥಿರಾಂಕ uint32_t *p = ದತ್ತಾಂಶ + value;
            value = *p;
            ಆದರೆ (all_ಮೌಲ್ಯಗಳು_Same_As(p + 1, ಖಂಡ_ಉದ್ದಳತೆ - 1, value)) {
                flags[i] = ALL_SAME;
                ಸೂಚ್ಯಂಕ[i] = value;
                // Fall through to ALL_SAME handling.
            } ಇಲ್ಲದಿದ್ದರೆ {
                new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ += ಖಂಡ_ಉದ್ದಳತೆ;
                ಮುಂದುವರಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_ASSERT(flags[i] == ALL_SAME);
            ಆದರೆ (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                ತಾರ್ಕಿಕ allSame = ನಿಜ;
                int32_t ಮುಂದಿನ_i = i + inc;
                ಗಾಗಿ (int32_t j = i + 1; j < ಮುಂದಿನ_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    ಆದರೆ (ಸೂಚ್ಯಂಕ[j] != value) {
                        allSame = ಸುಳ್ಳು;
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಆದರೆ (!allSame) {
                    // Turn it into a MIXED block.
                    ಆದರೆ (get_ದತ್ತಾಂಶ_ಖಂಡ(i) < 0) {
                        ಹಿಂತಿರುಗಿ -1;
                    }
                    new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ += ಖಂಡ_ಉದ್ದಳತೆ;
                    ಮುಂದುವರಿ;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_Or_ಕೂಡಿಸು(i, inc, value);
        ಆದರೆ (other == All_Same_ಖಂಡಗಳು::ಅತಿಪ್ರವಾಹ_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
            ಆದರೆ (!ಅತಿಪ್ರವಾಹ) {
                puts("UCPTrie AllSameBlocks overflow");
                ಅತಿಪ್ರವಾಹ = ನಿಜ;
            }
#ಆದರೆ_ಅಂತ್ಯ
            int32_t jInc = SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ;
            ಗಾಗಿ (int32_t j = 0;; j += jInc) {
                ಆದರೆ (j == i) {
                    all_Same_ಖಂಡಗಳು.ಕೂಡಿಸು(i, inc, value);
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (j == fastILimit) {
                    jInc = 1;
                }
                ಆದರೆ (flags[j] == ALL_SAME && ಸೂಚ್ಯಂಕ[j] == value) {
                    all_Same_ಖಂಡಗಳು.ಕೂಡಿಸು(j, jInc + inc, value);
                    other = j;
                    ನಿಲ್ಲಿಸು;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        ಆದರೆ (other >= 0) {
            flags[i] = SAME_AS;
            ಸೂಚ್ಯಂಕ[i] = other;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // New unique same-value block.
            new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ += ಖಂಡ_ಉದ್ದಳತೆ;
        }
    }
    ಹಿಂತಿರುಗಿ new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
#   define DEBUG_DO(ಅಭಿವ್ಯಕ್ತಿ) ಅಭಿವ್ಯಕ್ತಿ
#ಇಲ್ಲದಿದ್ದರೆ
#   define DEBUG_DO(ಅಭಿವ್ಯಕ್ತಿ)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_ಮೌಲ್ಯ(ಅಕ್ಷರ s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (ಅಕ್ಷರ)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (ಅಕ್ಷರ)(0x80 + (value & 0x3F));
    ಹಿಂತಿರುಗಿ length + 3;
}

ಶೂನ್ಯ print_ಖಂಡ(ಸ್ಥಿರಾಂಕ uint32_t *ಖಂಡ, int32_t ಖಂಡ_ಉದ್ದಳತೆ, uint32_t value,
                UChar32 ಆರಂಭ, int32_t overlap, uint32_t initial_ಮೌಲ್ಯ) {
    ಅಕ್ಷರ s[UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ * 3 + 3];
    int32_t length = 0;
    int32_t i;
    ಗಾಗಿ (i = 0; i < overlap; ++i) {
        length = append_ಮೌಲ್ಯ(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    ಗಾಗಿ (; i < ಖಂಡ_ಉದ್ದಳತೆ; ++i) {
        ಆದರೆ (ಖಂಡ != ಶೂನ್ಯಸೂಚಕ) {
            value = ಖಂಡ[i];
        }
        ಆದರೆ (value == initial_ಮೌಲ್ಯ) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_ಮೌಲ್ಯ(s, length, value);
    }
    s[length] = 0;
    ಆರಂಭ += overlap;
    ಆದರೆ (ಆರಂಭ <= 0xffff) {
        ಬರೆ("    %04lX  %s|\n", (ದೀರ್ಘ)ಆರಂಭ, s);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ <= 0xfffff) {
        ಬರೆ("   %5lX  %s|\n", (ದೀರ್ಘ)ಆರಂಭ, s);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("  %6lX  %s|\n", (ದೀರ್ಘ)ಆರಂಭ, s);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

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
int32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::compact_ದತ್ತಾಂಶ(
        int32_t fastILimit, uint32_t *new_ದತ್ತಾಂಶ, int32_t new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
        int32_t ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ, Mixed_ಖಂಡಗಳು &mixed_ಖಂಡಗಳು, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    int32_t ಎಣಿಕೆ_Same=0, ಮೊತ್ತ_Overlaps=0;
    ತಾರ್ಕಿಕ print_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        ದತ್ತಾಂಶ_ಉದ್ದಳತೆ == 50400 /* zh.txt~stroke */;
#ಆದರೆ_ಅಂತ್ಯ

    // The linear ASCII data has been copied into newData already.
    int32_t new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = 0;
    ಗಾಗಿ (int32_t i = 0; new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ < ASCII_ಎಲ್ಲೆ;
            new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ += UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, i += SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ) {
        ಸೂಚ್ಯಂಕ[i] = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
        ಆದರೆ (print_ದತ್ತಾಂಶ) {
            print_ಖಂಡ(new_ದತ್ತಾಂಶ + new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, 0, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, 0, initial_ಮೌಲ್ಯ);
        }
#ಆದರೆ_ಅಂತ್ಯ
    }

    int32_t ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_FAST_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    ಆದರೆ (!mixed_ಖಂಡಗಳು.init(new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ, ಖಂಡ_ಉದ್ದಳತೆ)) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    mixed_ಖಂಡಗಳು.extend(new_ದತ್ತಾಂಶ, 0, 0, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);

    int32_t i_ಎಲ್ಲೆ = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
    int32_t inc = SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ;
    int32_t fast_ಉದ್ದಳತೆ = 0;
    ಗಾಗಿ (int32_t i = ASCII_I_ಎಲ್ಲೆ; i < i_ಎಲ್ಲೆ; i += inc) {
        ಆದರೆ (i == fastILimit) {
            ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            inc = 1;
            fast_ಉದ್ದಳತೆ = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
            ಆದರೆ (!mixed_ಖಂಡಗಳು.init(new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ, ಖಂಡ_ಉದ್ದಳತೆ)) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            mixed_ಖಂಡಗಳು.extend(new_ದತ್ತಾಂಶ, 0, 0, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
        }
        ಆದರೆ (flags[i] == ALL_SAME) {
            uint32_t value = ಸೂಚ್ಯಂಕ[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_All_Same_ಖಂಡ(new_ದತ್ತಾಂಶ, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            ಆಗ (n >= 0 && i == ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ && i >= fastILimit && n < fast_ಉದ್ದಳತೆ &&
                    is_ಆರಂಭ_ದ_Some_Fast_ಖಂಡ(n, ಸೂಚ್ಯಂಕ, fastILimit)) {
                n = ಕಂಡುಹಿಡಿ_All_Same_ಖಂಡ(new_ದತ್ತಾಂಶ, n + 1, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, value, ಖಂಡ_ಉದ್ದಳತೆ);
            }
            ಆದರೆ (n >= 0) {
                DEBUG_DO(++ಎಣಿಕೆ_Same);
                ಸೂಚ್ಯಂಕ[i] = n;
            } ಇಲ್ಲದಿದ್ದರೆ {
                n = getAllSameOverlap(new_ದತ್ತಾಂಶ, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, value, ಖಂಡ_ಉದ್ದಳತೆ);
                DEBUG_DO(ಮೊತ್ತ_Overlaps += n);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
                ಆದರೆ (print_ದತ್ತಾಂಶ) {
                    print_ಖಂಡ(ಶೂನ್ಯಸೂಚಕ, ಖಂಡ_ಉದ್ದಳತೆ, value, i << UCPTRIE_ಸರಿಸು_3, n, initial_ಮೌಲ್ಯ);
                }
#ಆದರೆ_ಅಂತ್ಯ
                ಸೂಚ್ಯಂಕ[i] = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - n;
                int32_t prev_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
                ಆಗ (n < ಖಂಡ_ಉದ್ದಳತೆ) {
                    new_ದತ್ತಾಂಶ[new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = value;
                    ++n;
                }
                mixed_ಖಂಡಗಳು.extend(new_ದತ್ತಾಂಶ, 0, prev_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags[i] == MIXED) {
            ಸ್ಥಿರಾಂಕ uint32_t *ಖಂಡ = ದತ್ತಾಂಶ + ಸೂಚ್ಯಂಕ[i];
            int32_t n = mixed_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_ಖಂಡ(new_ದತ್ತಾಂಶ, ಖಂಡ, 0);
            ಆದರೆ (n >= 0) {
                DEBUG_DO(++ಎಣಿಕೆ_Same);
                ಸೂಚ್ಯಂಕ[i] = n;
            } ಇಲ್ಲದಿದ್ದರೆ {
                n = getOverlap(new_ದತ್ತಾಂಶ, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, ಖಂಡ, 0, ಖಂಡ_ಉದ್ದಳತೆ);
                DEBUG_DO(ಮೊತ್ತ_Overlaps += n);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
                ಆದರೆ (print_ದತ್ತಾಂಶ) {
                    print_ಖಂಡ(ಖಂಡ, ಖಂಡ_ಉದ್ದಳತೆ, 0, i << UCPTRIE_ಸರಿಸು_3, n, initial_ಮೌಲ್ಯ);
                }
#ಆದರೆ_ಅಂತ್ಯ
                ಸೂಚ್ಯಂಕ[i] = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - n;
                int32_t prev_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
                ಆಗ (n < ಖಂಡ_ಉದ್ದಳತೆ) {
                    new_ದತ್ತಾಂಶ[new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = ಖಂಡ[n++];
                }
                mixed_ಖಂಡಗಳು.extend(new_ದತ್ತಾಂಶ, 0, prev_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* SAME_AS */ {
            uint32_t j = ಸೂಚ್ಯಂಕ[i];
            ಸೂಚ್ಯಂಕ[i] = ಸೂಚ್ಯಂಕ[j];
        }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    /* we saved some space */
    ಬರೆ("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (ದೀರ್ಘ)ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)ಎಣಿಕೆ_Same, (ದೀರ್ಘ)ಮೊತ್ತ_Overlaps);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
}

int32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::compact_ಸೂಚ್ಯಂಕ(int32_t fastILimit, Mixed_ಖಂಡಗಳು &mixed_ಖಂಡಗಳು,
                                           UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = fastILimit >> (UCPTRIE_FAST_ಸರಿಸು - UCPTRIE_ಸರಿಸು_3);
    ಆದರೆ ((ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_FAST_ಸರಿಸು) <= fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_ಸ್ಥಾನಾಂತರ = UCPTRIE_NO_INDEX3_NULL_ಸ್ಥಾನಾಂತರ;
        ಹಿಂತಿರುಗಿ fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_ಸೂಚ್ಯಂಕ[UCPTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ];  // fastIndexLength
    int32_t i3_ಮೊದಲ_Null = -1;
    ಗಾಗಿ (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = ಸೂಚ್ಯಂಕ[i];
        fast_ಸೂಚ್ಯಂಕ[j] = static_cast<uint16_t>(i3);
        ಆದರೆ (i3 == static_cast<uint32_t>(ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ)) {
            ಆದರೆ (i3_ಮೊದಲ_Null < 0) {
                i3_ಮೊದಲ_Null = j;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (index3_Null_ಸ್ಥಾನಾಂತರ < 0 &&
                    (j - i3_ಮೊದಲ_Null + 1) == UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ) {
                index3_Null_ಸ್ಥಾನಾಂತರ = i3_ಮೊದಲ_Null;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            i3_ಮೊದಲ_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_ಮುಂದಿನ = i + SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ;
        ಆಗ (++i < i_ಮುಂದಿನ) {
            i3 += UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            ಸೂಚ್ಯಂಕ[i] = i3;
        }
    }

    ಆದರೆ (!mixed_ಖಂಡಗಳು.init(fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ)) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    mixed_ಖಂಡಗಳು.extend(fast_ಸೂಚ್ಯಂಕ, 0, 0, fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_ಸಾಮರ್ಥ್ಯ = 0;
    i3_ಮೊದಲ_Null = index3_Null_ಸ್ಥಾನಾಂತರ;
    ತಾರ್ಕಿಕ has_Long_I3_ಖಂಡಗಳು = ಸುಳ್ಳು;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_ಆರಂಭ = fastILimit < BMP_I_ಎಲ್ಲೆ ? 0 : BMP_I_ಎಲ್ಲೆ;
    int32_t i_ಎಲ್ಲೆ = ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3;
    ಗಾಗಿ (int32_t i = i_ಆರಂಭ; i < i_ಎಲ್ಲೆ;) {
        int32_t j = i;
        int32_t j_ಎಲ್ಲೆ = i + UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ;
        uint32_t oredI3 = 0;
        ತಾರ್ಕಿಕ isNull = ನಿಜ;
        ಮಾಡು {
            uint32_t i3 = ಸೂಚ್ಯಂಕ[j];
            oredI3 |= i3;
            ಆದರೆ (i3 != static_cast<uint32_t>(ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ)) {
                isNull = ಸುಳ್ಳು;
            }
        } ಆಗ (++j < j_ಎಲ್ಲೆ);
        ಆದರೆ (isNull) {
            flags[i] = I3_NULL;
            ಆದರೆ (i3_ಮೊದಲ_Null < 0) {
                ಆದರೆ (oredI3 <= 0xffff) {
                    index3_ಸಾಮರ್ಥ್ಯ += UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    index3_ಸಾಮರ್ಥ್ಯ += ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ;
                    has_Long_I3_ಖಂಡಗಳು = ನಿಜ;
                }
                i3_ಮೊದಲ_Null = 0;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (oredI3 <= 0xffff) {
                int32_t n = mixed_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_ಖಂಡ(fast_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ, i);
                ಆದರೆ (n >= 0) {
                    flags[i] = I3_BMP;
                    ಸೂಚ್ಯಂಕ[i] = n;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    flags[i] = I3_16;
                    index3_ಸಾಮರ್ಥ್ಯ += UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                flags[i] = I3_18;
                index3_ಸಾಮರ್ಥ್ಯ += ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ;
                has_Long_I3_ಖಂಡಗಳು = ನಿಜ;
            }
        }
        i = j;
    }

    int32_t index2_ಸಾಮರ್ಥ್ಯ = (i_ಎಲ್ಲೆ - i_ಆರಂಭ) >> UCPTRIE_ಸರಿಸು_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_ಉದ್ದಳತೆ = (index2_ಸಾಮರ್ಥ್ಯ + UCPTRIE_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ) >> UCPTRIE_ಸರಿಸು_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_ಸಾಮರ್ಥ್ಯ = fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ + index1_ಉದ್ದಳತೆ + index3_ಸಾಮರ್ಥ್ಯ + index2_ಸಾಮರ್ಥ್ಯ + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_ಸಾಮರ್ಥ್ಯ * 2));
    ಆದರೆ (index16 == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    uprv_memcpy(index16, fast_ಸೂಚ್ಯಂಕ, fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2);

    ಆದರೆ (!mixed_ಖಂಡಗಳು.init(index16_ಸಾಮರ್ಥ್ಯ, UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ)) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    Mixed_ಖಂಡಗಳು long_I3_ಖಂಡಗಳು;
    ಆದರೆ (has_Long_I3_ಖಂಡಗಳು) {
        ಆದರೆ (!long_I3_ಖಂಡಗಳು.init(index16_ಸಾಮರ್ಥ್ಯ, ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ)) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_ಎಲ್ಲೆ >> UCPTRIE_ಸರಿಸು_2];  // index2Capacity
    int32_t i2_ಉದ್ದಳತೆ = 0;
    i3_ಮೊದಲ_Null = index3_Null_ಸ್ಥಾನಾಂತರ;
    int32_t index3_ಆರಂಭ = fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ + index1_ಉದ್ದಳತೆ;
    int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = index3_ಆರಂಭ;
    ಗಾಗಿ (int32_t i = i_ಆರಂಭ; i < i_ಎಲ್ಲೆ; i += UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ) {
        int32_t i3;
        uint8_t f = flags[i];
        ಆದರೆ (f == I3_NULL && i3_ಮೊದಲ_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ <= 0xffff ? I3_16 : I3_18;
            i3_ಮೊದಲ_Null = 0;
        }
        ಆದರೆ (f == I3_NULL) {
            i3 = index3_Null_ಸ್ಥಾನಾಂತರ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f == I3_BMP) {
            i3 = ಸೂಚ್ಯಂಕ[i];
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f == I3_16) {
            int32_t n = mixed_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_ಖಂಡ(index16, ಸೂಚ್ಯಂಕ, i);
            ಆದರೆ (n >= 0) {
                i3 = n;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ == index3_ಆರಂಭ) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    n = getOverlap(index16, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                                   ಸೂಚ್ಯಂಕ, i, UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ);
                }
                i3 = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ - n;
                int32_t prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
                ಆಗ (n < UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ) {
                    index16[ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ++] = ಸೂಚ್ಯಂಕ[i + n++];
                }
                mixed_ಖಂಡಗಳು.extend(index16, index3_ಆರಂಭ, prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
                ಆದರೆ (has_Long_I3_ಖಂಡಗಳು) {
                    long_I3_ಖಂಡಗಳು.extend(index16, index3_ಆರಂಭ, prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_ಖಂಡಗಳು);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_ಎಲ್ಲೆ = i + UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ;
            int32_t k = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
            ಮಾಡು {
                ++k;
                uint32_t v = ಸೂಚ್ಯಂಕ[j++];
                uint32_t ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = ಸೂಚ್ಯಂಕ[j++];
                ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = ಊರ್ಧ್ವ_ಬಿಟ್ಟುಗಳು;
            } ಆಗ (j < j_ಎಲ್ಲೆ);
            int32_t n = long_I3_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_ಖಂಡ(index16, index16, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
            ಆದರೆ (n >= 0) {
                i3 = n | 0x8000;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ == index3_ಆರಂಭ) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    n = getOverlap(index16, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                                   index16, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ);
                }
                i3 = (ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ - n) | 0x8000;
                int32_t prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
                ಆದರೆ (n > 0) {
                    int32_t ಆರಂಭ = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
                    ಆಗ (n < ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ) {
                        index16[ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ++] = index16[ಆರಂಭ + n++];
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ += ಸೂಚ್ಯಂಕ_3_18_ಬಿಟ್_ಖಂಡ_ಉದ್ದಳತೆ;
                }
                mixed_ಖಂಡಗಳು.extend(index16, index3_ಆರಂಭ, prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
                ಆದರೆ (has_Long_I3_ಖಂಡಗಳು) {
                    long_I3_ಖಂಡಗಳು.extend(index16, index3_ಆರಂಭ, prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
                }
            }
        }
        ಆದರೆ (index3_Null_ಸ್ಥಾನಾಂತರ < 0 && i3_ಮೊದಲ_Null >= 0) {
            index3_Null_ಸ್ಥಾನಾಂತರ = i3;
        }
        // Set the index-2 table entry.
        index2[i2_ಉದ್ದಳತೆ++] = i3;
    }
    U_ASSERT(i2_ಉದ್ದಳತೆ == index2_ಸಾಮರ್ಥ್ಯ);
    U_ASSERT(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ <= index3_ಆರಂಭ + index3_ಸಾಮರ್ಥ್ಯ);

    ಆದರೆ (index3_Null_ಸ್ಥಾನಾಂತರ < 0) {
        index3_Null_ಸ್ಥಾನಾಂತರ = UCPTRIE_NO_INDEX3_NULL_ಸ್ಥಾನಾಂತರ;
    }
    ಆದರೆ (ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ >= (UCPTRIE_NO_INDEX3_NULL_ಸ್ಥಾನಾಂತರ + UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ == UCPTRIE_ಸೂಚ್ಯಂಕ_3_ಖಂಡ_ಉದ್ದಳತೆ,
                  "must re-init mixedBlocks");
    int32_t ಖಂಡ_ಉದ್ದಳತೆ = UCPTRIE_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ;
    int32_t i1 = fast_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಗಾಗಿ (int32_t i = 0; i < i2_ಉದ್ದಳತೆ; i += ಖಂಡ_ಉದ್ದಳತೆ) {
        int32_t n;
        ಆದರೆ ((i2_ಉದ್ದಳತೆ - i) >= ಖಂಡ_ಉದ್ದಳತೆ) {
            // normal block
            U_ASSERT(ಖಂಡ_ಉದ್ದಳತೆ == UCPTRIE_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ);
            n = mixed_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_ಖಂಡ(index16, index2, i);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // highStart is inside the last index-2 block. Shorten it.
            ಖಂಡ_ಉದ್ದಳತೆ = i2_ಉದ್ದಳತೆ - i;
            n = ಕಂಡುಹಿಡಿ_Same_ಖಂಡ(index16, index3_ಆರಂಭ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                              index2, i, ಖಂಡ_ಉದ್ದಳತೆ);
        }
        int32_t i2;
        ಆದರೆ (n >= 0) {
            i2 = n;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ == index3_ಆರಂಭ) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                n = getOverlap(index16, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, index2, i, ಖಂಡ_ಉದ್ದಳತೆ);
            }
            i2 = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ - n;
            int32_t prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
            ಆಗ (n < ಖಂಡ_ಉದ್ದಳತೆ) {
                index16[ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ++] = index2[i + n++];
            }
            mixed_ಖಂಡಗಳು.extend(index16, index3_ಆರಂಭ, prev_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_ಆರಂಭ);
    U_ASSERT(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ <= index16_ಸಾಮರ್ಥ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    /* we saved some space */
    ಬರೆ("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (ದೀರ್ಘ)i_ಎಲ್ಲೆ, (ದೀರ್ಘ)ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
}

int32_t Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::compactTrie(int32_t fastILimit, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // Find the real highStart and round it up.
    U_ASSERT((ಮೇಲಿನ_ಆರಂಭ & (UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು - 1)) == 0);
    ಮೇಲಿನ_ಮೌಲ್ಯ = get(MAX_UNICODE);
    int32_t real_ಮೇಲಿನ_ಆರಂಭ = ಕಂಡುಹಿಡಿ_ಮೇಲಿನ_ಆರಂಭ();
    real_ಮೇಲಿನ_ಆರಂಭ = (real_ಮೇಲಿನ_ಆರಂಭ + (UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು - 1)) &
        ~(UCPTRIE_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_2_ನಮೂದು - 1);
    ಆದರೆ (real_ಮೇಲಿನ_ಆರಂಭ == UNICODE_ಎಲ್ಲೆ) {
        ಮೇಲಿನ_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    ಬರೆ("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (ದೀರ್ಘ)real_ಮೇಲಿನ_ಆರಂಭ, (ದೀರ್ಘ)ಮೇಲಿನ_ಮೌಲ್ಯ, (ದೀರ್ಘ)initial_ಮೌಲ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_ಎಲ್ಲೆ = fastILimit << UCPTRIE_ಸರಿಸು_3;
    ಆದರೆ (real_ಮೇಲಿನ_ಆರಂಭ < fast_ಎಲ್ಲೆ) {
        ಗಾಗಿ (int32_t i = (real_ಮೇಲಿನ_ಆರಂಭ >> UCPTRIE_ಸರಿಸು_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            ಸೂಚ್ಯಂಕ[i] = ಮೇಲಿನ_ಮೌಲ್ಯ;
        }
        ಮೇಲಿನ_ಆರಂಭ = fast_ಎಲ್ಲೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮೇಲಿನ_ಆರಂಭ = real_ಮೇಲಿನ_ಆರಂಭ;
    }

    uint32_t ascii_ದತ್ತಾಂಶ[ASCII_ಎಲ್ಲೆ];
    ಗಾಗಿ (int32_t i = 0; i < ASCII_ಎಲ್ಲೆ; ++i) {
        ascii_ದತ್ತಾಂಶ[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_ಖಂಡಗಳು all_Same_ಖಂಡಗಳು;
    int32_t new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = compact_ಸಂಪೂರ್ಣ_ದತ್ತಾಂಶ_ಖಂಡಗಳು(fastILimit, all_Same_ಖಂಡಗಳು);
    ಆದರೆ (new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ < 0) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    uint32_t* new_ದತ್ತಾಂಶ = static_cast<uint32_t*>(uprv_malloc(new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ * 4));
    ಆದರೆ (new_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    uprv_memcpy(new_ದತ್ತಾಂಶ, ascii_ದತ್ತಾಂಶ, ಗಾತ್ರ(ascii_ದತ್ತಾಂಶ));

    int32_t ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ = all_Same_ಖಂಡಗಳು.ಕಂಡುಹಿಡಿ_Most_ಬಳಸಿದ();

    Mixed_ಖಂಡಗಳು mixed_ಖಂಡಗಳು;
    int32_t new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = compact_ದತ್ತಾಂಶ(fastILimit, new_ದತ್ತಾಂಶ, new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
                                        ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ, mixed_ಖಂಡಗಳು, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    U_ASSERT(new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ <= new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ);
    uprv_free(ದತ್ತಾಂಶ);
    ದತ್ತಾಂಶ = new_ದತ್ತಾಂಶ;
    ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ = new_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ;
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = new_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ಆದರೆ (ದತ್ತಾಂಶ_ಉದ್ದಳತೆ > (0x3ffff + UCPTRIE_SMALL_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ >= 0) {
        ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ = ಸೂಚ್ಯಂಕ[ದತ್ತಾಂಶ_Null_ಸೂಚ್ಯಂಕ];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
        ಆದರೆ (ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ] != initial_ಮೌಲ್ಯ) {
            ಬರೆ("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (ದೀರ್ಘ)initial_ಮೌಲ್ಯ, (ದೀರ್ಘ)ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ]);
        }
#ಆದರೆ_ಅಂತ್ಯ
        initial_ಮೌಲ್ಯ = ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ = UCPTRIE_NO_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ;
    }

    int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = compact_ಸೂಚ್ಯಂಕ(fastILimit, mixed_ಖಂಡಗಳು, error_ಸಂಕೇತ);
    ಮೇಲಿನ_ಆರಂಭ = real_ಮೇಲಿನ_ಆರಂಭ;
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
}

UCPTrie *Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::build(UCPTrieType type, UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ಮೌಲ್ಯ_ಅಗಲ < UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16 || UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 < ಮೌಲ್ಯ_ಅಗಲ) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    ಆಯ್ಕೆ (ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        ಮುಖವಾಡ_ಮೌಲ್ಯಗಳು(0xffff);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        ಮುಖವಾಡ_ಮೌಲ್ಯಗಳು(0xff);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    UChar32 fast_ಎಲ್ಲೆ = type == UCPTRIE_TYPE_FAST ? BMP_ಎಲ್ಲೆ : UCPTRIE_SMALL_ಎಲ್ಲೆ;
    int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = compactTrie(fast_ಎಲ್ಲೆ >> UCPTRIE_ಸರಿಸು_3, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        clear();
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    ಆದರೆ (ಮೌಲ್ಯ_ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32 && (ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ & 1) != 0) {
        index16[ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2;
    ಆದರೆ (ಮೌಲ್ಯ_ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16) {
        ಆದರೆ (((ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ ^ ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) & 1) != 0) {
            // padding
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = error_ಮೌಲ್ಯ;
        }
        ಆದರೆ (ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 1] != error_ಮೌಲ್ಯ || ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 2] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = error_ಮೌಲ್ಯ;
        }
        length += ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ_ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        ಆದರೆ (ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 1] != error_ಮೌಲ್ಯ || ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 2] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
            ಆದರೆ (ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 1] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
                ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
            }
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = error_ಮೌಲ್ಯ;
        }
        length += ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t and3 = (length + ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) & 3;
        ಆದರೆ (and3 == 0 && ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 1] == error_ಮೌಲ್ಯ && ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 2] == ಮೇಲಿನ_ಮೌಲ್ಯ) {
            // all set
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(and3 == 3 && ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ - 1] == ಮೇಲಿನ_ಮೌಲ್ಯ) {
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = error_ಮೌಲ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಗ (and3 != 2) {
                ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
                and3 = (and3 + 1) & 3;
            }
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
            ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++] = error_ಮೌಲ್ಯ;
        }
        length += ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += ಗಾತ್ರ(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* ಬೈಟುಗಳು = static_cast<uint8_t*>(uprv_malloc(length));
    ಆದರೆ (ಬೈಟುಗಳು == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        clear();
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(ಬೈಟುಗಳು);
    uprv_memset(trie, 0, ಗಾತ್ರ(UCPTrie));
    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;

    trie->ಮೇಲಿನ_ಆರಂಭ = ಮೇಲಿನ_ಆರಂಭ;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ಮೇಲಿನ_ಆರಂಭ = (ಮೇಲಿನ_ಆರಂಭ + 0xfff) >> 12;
    trie->type = type;
    trie->ಮೌಲ್ಯ_ಅಗಲ = ಮೌಲ್ಯ_ಅಗಲ;

    trie->index3_Null_ಸ್ಥಾನಾಂತರ = index3_Null_ಸ್ಥಾನಾಂತರ;
    trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ = ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    trie->null_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ;

    ಬೈಟುಗಳು += ಗಾತ್ರ(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(ಬೈಟುಗಳು);
    trie->ಸೂಚ್ಯಂಕ = dest16;

    ಆದರೆ (ಮೇಲಿನ_ಆರಂಭ <= fast_ಎಲ್ಲೆ) {
        // Condense only the fast index from the mutable-trie index.
        ಗಾಗಿ (int32_t i = 0, j = 0; j < ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; i += SMALL_ದತ್ತಾಂಶ_ಖಂಡಗಳು_ಪ್ರತಿ_BMP_ಖಂಡ, ++j) {
            *dest16++ = static_cast<uint16_t>(ಸೂಚ್ಯಂಕ[i]); // dest16[j]
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_memcpy(dest16, index16, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2);
        dest16 += ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    }
    ಬೈಟುಗಳು += ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ * 2;

    // Write the data array.
    ಸ್ಥಿರಾಂಕ uint32_t *p = ದತ್ತಾಂಶ;
    ಆಯ್ಕೆ (ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        // Write 16-bit data values.
        trie->ದತ್ತಾಂಶ.ptr16 = dest16;
        ಗಾಗಿ (int32_t i = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        // Write 32-bit data values.
        trie->ದತ್ತಾಂಶ.ptr32 = reinterpret_cast<uint32_t*>(ಬೈಟುಗಳು);
        uprv_memcpy(ಬೈಟುಗಳು, p, (size_t)ದತ್ತಾಂಶ_ಉದ್ದಳತೆ * 4);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        // Write 8-bit data values.
        trie->ದತ್ತಾಂಶ.ptr8 = ಬೈಟುಗಳು;
        ಗಾಗಿ (int32_t i = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; i > 0; --i) {
            *ಬೈಟುಗಳು++ = static_cast<uint8_t>(*p++);
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // Will not occur, valueWidth checked at the beginning.
        ನಿಲ್ಲಿಸು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#ಆದರೆ_ಅಂತ್ಯ

    clear();
    ಹಿಂತಿರುಗಿ trie;
}

}  // namespace

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Local_ಸೂಚಕ<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie> trie(
        ಹೊಸ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, *p_Error_ಸಂಕೇತ), *p_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ಸ್ಥಿರಾಂಕ UMutableCPTrie *other, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (other == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Local_ಸೂಚಕ<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie> clone(
        ಹೊಸ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie(*reinterpret_cast<ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(other), *p_Error_ಸಂಕೇತ), *p_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    ಅಳಿಸು reinterpret_cast<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ಸ್ಥಿರಾಂಕ UCPMap *map, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (map == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<UMutableCPTrie *>(Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::fromUCPMap(map, *p_Error_ಸಂಕೇತ));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ಸ್ಥಿರಾಂಕ UCPTrie *trie, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (trie == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<UMutableCPTrie *>(Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie::fromUCPTrie(trie, *p_Error_ಸಂಕೇತ));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ಸ್ಥಿರಾಂಕ UMutableCPTrie *trie, UChar32 c) {
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie)->get(c);
}

ನಾಮಾಕಾಶ {

UChar32 getRange(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *trie, UChar32 ಆರಂಭ,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie)->
        getRange(ಆರಂಭ, filter, context, p_ಮೌಲ್ಯ);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ಸ್ಥಿರಾಂಕ UMutableCPTrie *trie, UChar32 ಆರಂಭ,
                        UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                        UCPMap_ಮೌಲ್ಯ_Filter *filter, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಹಿಂತಿರುಗಿ ucptrie_ಆಂತರಿಕ_Get_Range(getRange, trie, ಆರಂಭ,
                                    option, surrogate_ಮೌಲ್ಯ,
                                    filter, context, p_ಮೌಲ್ಯ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
umutablecptrie_ಗಣ(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    reinterpret_cast<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie)->ಗಣ(c, value, *p_Error_ಸಂಕೇತ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
umutablecptrie_ಗಣ_Range(UMutableCPTrie *trie, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
                   uint32_t value, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    reinterpret_cast<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie)->ಗಣ_Range(ಆರಂಭ, ಕೊನೆ, value, *p_Error_ಸಂಕೇತ);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ,
                              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie)->build(type, ಮೌಲ್ಯ_ಅಗಲ, *p_Error_ಸಂಕೇತ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
U_CFUNC ಶೂನ್ಯ umutablecptrie_ಗಣ_Name(UMutableCPTrie *trie, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
    reinterpret_cast<Mutable_ಸಂಕೇತ_ಬಿಂದು_Trie *>(trie)->name = name;
}
#ಆದರೆ_ಅಂತ್ಯ
