// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
#   include <stdio.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umutablecptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਉਛਾਲ_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਉਛਾਲ_2
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_ਹੱਦਬੰਦੀ = 0x110000;
constexpr int32_t BMP_ਹੱਦਬੰਦੀ = 0x10000;
constexpr int32_t ASCII_ਹੱਦਬੰਦੀ = 0x80;

constexpr int32_t I_ਹੱਦਬੰਦੀ = UNICODE_ਹੱਦਬੰਦੀ >> UCPTRIE_ਖਿਸਕਾਓ_3;
constexpr int32_t BMP_I_ਹੱਦਬੰਦੀ = BMP_ਹੱਦਬੰਦੀ >> UCPTRIE_ਖਿਸਕਾਓ_3;
constexpr int32_t ASCII_I_ਹੱਦਬੰਦੀ = ASCII_ਹੱਦਬੰਦੀ >> UCPTRIE_ਖਿਸਕਾਓ_3;

constexpr int32_t SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ = (1 << (UCPTRIE_FAST_ਖਿਸਕਾਓ - UCPTRIE_ਖਿਸਕਾਓ_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_ਅੰਕੜੇ_ਲੰਮਾਈ = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_ਅੰਕੜੇ_ਲੰਮਾਈ = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_ਅੰਕੜੇ_ਲੰਮਾਈ = UNICODE_ਹੱਦਬੰਦੀ;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ + UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ / 8;

ਵਰਗ All_Same_ਖੰਡਾਂ;
ਵਰਗ Mixed_ਖੰਡਾਂ;

ਵਰਗ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(uint32_t initial_ਮੁੱਲ, uint32_t error_ਮੁੱਲ, UError_ਕੋਡ &error_ਕੋਡ);
    Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie &other, UError_ਕੋਡ &error_ਕੋਡ);
    Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie &other) = ਮਿਟਾਓ;
    ~Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie();

    Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie &ਸੰਚਾਲਕ=(ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie &other) = ਮਿਟਾਓ;

    ਸਥਿਰ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *fromUCPMap(ਅਚਲ UCPMap *map, UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *fromUCPTrie(ਅਚਲ UCPTrie *trie, UError_ਕੋਡ &error_ਕੋਡ);

    uint32_t get(UChar32 c) ਅਚਲ;
    int32_t getRange(UChar32 ਸ਼ੁਰੂ, UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context,
                     uint32_t *p_ਮੁੱਲ) ਅਚਲ;

    ਖਾਲੀ ਸੈੱਟ(UChar32 c, uint32_t value, UError_ਕੋਡ &error_ਕੋਡ);
    ਖਾਲੀ ਸੈੱਟ_Range(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ, uint32_t value, UError_ਕੋਡ &error_ਕੋਡ);

    UCPTrie *build(UCPTrieType type, UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ, UError_ਕੋਡ &error_ਕੋਡ);

ਨਿਜੀ:
    ਖਾਲੀ clear();

    ਤਾਰਕਿਕ ensure_ਉੱਚਾ_ਸ਼ੁਰੂ(UChar32 c);
    int32_t alloc_ਅੰਕੜੇ_ਖੰਡ(int32_t ਖੰਡ_ਲੰਮਾਈ);
    int32_t get_ਅੰਕੜੇ_ਖੰਡ(int32_t i);

    ਖਾਲੀ ਨਕਾਬ_ਮੁੱਲਾਂ(uint32_t mask);
    UChar32 ਲੱਭੋ_ਉੱਚਾ_ਸ਼ੁਰੂ() ਅਚਲ;
    int32_t compact_ਸਾਰਾ_ਅੰਕੜੇ_ਖੰਡਾਂ(int32_t fastILimit, All_Same_ਖੰਡਾਂ &all_Same_ਖੰਡਾਂ);
    int32_t compact_ਅੰਕੜੇ(
            int32_t fastILimit, uint32_t *new_ਅੰਕੜੇ, int32_t new_ਅੰਕੜੇ_ਸਮਰੱਥਾ,
            int32_t ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ, Mixed_ਖੰਡਾਂ &mixed_ਖੰਡਾਂ, UError_ਕੋਡ &error_ਕੋਡ);
    int32_t compact_ਸੂਚਕਾਂਕ(int32_t fastILimit, Mixed_ਖੰਡਾਂ &mixed_ਖੰਡਾਂ, UError_ਕੋਡ &error_ਕੋਡ);
    int32_t compactTrie(int32_t fastILimit, UError_ਕੋਡ &error_ਕੋਡ);

    uint32_t *ਸੂਚਕਾਂਕ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t ਸੂਚਕਾਂਕ_ਸਮਰੱਥਾ = 0;
    int32_t index3_Null_ਵਿੱਥ = -1;
    uint32_t *ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t ਅੰਕੜੇ_ਸਮਰੱਥਾ = 0;
    int32_t ਅੰਕੜੇ_ਲੰਮਾਈ = 0;
    int32_t ਅੰਕੜੇ_Null_ਵਿੱਥ = -1;

    uint32_t orig_Initial_ਮੁੱਲ;
    uint32_t initial_ਮੁੱਲ;
    uint32_t error_ਮੁੱਲ;
    UChar32 ਉੱਚਾ_ਸ਼ੁਰੂ;
    uint32_t ਉੱਚਾ_ਮੁੱਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
ਜਨਤਕ:
    ਅਚਲ ਅੱਖਰ *name;
#ਜੇ_ਅੰਤ
ਨਿਜੀ:
    /** Temporary array while building the final data. */
    uint16_t *index16 = ਖਾਲੀ_ਸੂਚਕ;
    uint8_t flags[UNICODE_ਹੱਦਬੰਦੀ >> UCPTRIE_ਖਿਸਕਾਓ_3];
};

Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(uint32_t ini_ਮੁੱਲ, uint32_t err_ਮੁੱਲ, UError_ਕੋਡ &error_ਕੋਡ) :
        orig_Initial_ਮੁੱਲ(ini_ਮੁੱਲ), initial_ਮੁੱਲ(ini_ਮੁੱਲ), error_ਮੁੱਲ(err_ਮੁੱਲ),
        ਉੱਚਾ_ਸ਼ੁਰੂ(0), ਉੱਚਾ_ਮੁੱਲ(initial_ਮੁੱਲ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
        , name("open")
#ਜੇ_ਅੰਤ
        {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਸੂਚਕਾਂਕ = static_cast<uint32_t*>(uprv_malloc(BMP_I_ਹੱਦਬੰਦੀ * 4));
    ਅੰਕੜੇ = static_cast<uint32_t*>(uprv_malloc(INITIAL_ਅੰਕੜੇ_ਲੰਮਾਈ * 4));
    ਜੇ (ਸੂਚਕਾਂਕ == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਸੂਚਕਾਂਕ_ਸਮਰੱਥਾ = BMP_I_ਹੱਦਬੰਦੀ;
    ਅੰਕੜੇ_ਸਮਰੱਥਾ = INITIAL_ਅੰਕੜੇ_ਲੰਮਾਈ;
}

Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie &other, UError_ਕੋਡ &error_ਕੋਡ) :
        index3_Null_ਵਿੱਥ(other.index3_Null_ਵਿੱਥ),
        ਅੰਕੜੇ_Null_ਵਿੱਥ(other.ਅੰਕੜੇ_Null_ਵਿੱਥ),
        orig_Initial_ਮੁੱਲ(other.orig_Initial_ਮੁੱਲ), initial_ਮੁੱਲ(other.initial_ਮੁੱਲ),
        error_ਮੁੱਲ(other.error_ਮੁੱਲ),
        ਉੱਚਾ_ਸ਼ੁਰੂ(other.ਉੱਚਾ_ਸ਼ੁਰੂ), ਉੱਚਾ_ਮੁੱਲ(other.ਉੱਚਾ_ਮੁੱਲ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
        , name("mutable clone")
#ਜੇ_ਅੰਤ
        {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    int32_t i_ਸਮਰੱਥਾ = ਉੱਚਾ_ਸ਼ੁਰੂ <= BMP_ਹੱਦਬੰਦੀ ? BMP_I_ਹੱਦਬੰਦੀ : I_ਹੱਦਬੰਦੀ;
    ਸੂਚਕਾਂਕ = static_cast<uint32_t*>(uprv_malloc(i_ਸਮਰੱਥਾ * 4));
    ਅੰਕੜੇ = static_cast<uint32_t*>(uprv_malloc(other.ਅੰਕੜੇ_ਸਮਰੱਥਾ * 4));
    ਜੇ (ਸੂਚਕਾਂਕ == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਸੂਚਕਾਂਕ_ਸਮਰੱਥਾ = i_ਸਮਰੱਥਾ;
    ਅੰਕੜੇ_ਸਮਰੱਥਾ = other.ਅੰਕੜੇ_ਸਮਰੱਥਾ;

    int32_t i_ਹੱਦਬੰਦੀ = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
    uprv_memcpy(flags, other.flags, i_ਹੱਦਬੰਦੀ);
    uprv_memcpy(ਸੂਚਕਾਂਕ, other.ਸੂਚਕਾਂਕ, i_ਹੱਦਬੰਦੀ * 4);
    uprv_memcpy(ਅੰਕੜੇ, other.ਅੰਕੜੇ, (size_t)other.ਅੰਕੜੇ_ਲੰਮਾਈ * 4);
    ਅੰਕੜੇ_ਲੰਮਾਈ = other.ਅੰਕੜੇ_ਲੰਮਾਈ;
    U_ASSERT(other.index16 == ਖਾਲੀ_ਸੂਚਕ);
}

Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::~Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie() {
    uprv_free(ਸੂਚਕਾਂਕ);
    uprv_free(ਅੰਕੜੇ);
    uprv_free(index16);
}

Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::fromUCPMap(ਅਚਲ UCPMap *map, UError_ਕੋਡ &error_ਕੋਡ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ਮੁੱਲ = ucpmap_get(map, -1);
    uint32_t initial_ਮੁੱਲ = ucpmap_get(map, 0x10ffff);
    Local_ਸੂਚਕ<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie> mutableTrie(
        ਨਵਾਂ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(initial_ਮੁੱਲ, error_ਮੁੱਲ, error_ਕੋਡ),
        error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    uint32_t value;
    ਜਦੋਂ ((ਸਿਰਾ = ucpmap_getRange(map, ਸ਼ੁਰੂ, UCPMAP_RANGE_ਸਾਧਾਰਨ, 0,
                                  ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &value)) >= 0) {
        ਜੇ (value != initial_ਮੁੱਲ) {
            ਜੇ (ਸ਼ੁਰੂ == ਸਿਰਾ) {
                mutableTrie->ਸੈੱਟ(ਸ਼ੁਰੂ, value, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                mutableTrie->ਸੈੱਟ_Range(ਸ਼ੁਰੂ, ਸਿਰਾ, value, error_ਕੋਡ);
            }
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        ਵਾਪਸ mutableTrie.orphan();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::fromUCPTrie(ਅਚਲ UCPTrie *trie, UError_ਕੋਡ &error_ਕੋਡ) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ਮੁੱਲ;
    uint32_t initial_ਮੁੱਲ;
    ਚੋਣ (trie->ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        error_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr16[trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ERROR_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ];
        initial_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr16[trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        error_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr32[trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ERROR_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ];
        initial_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr32[trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        error_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr8[trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ERROR_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ];
        initial_ਮੁੱਲ = trie->ਅੰਕੜੇ.ptr8[trie->ਅੰਕੜੇ_ਲੰਮਾਈ - UCPTRIE_ਉੱਚਾ_ਮੁੱਲ_NEG_ਅੰਕੜੇ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਮੂਲ:
        // Unreachable if the trie is properly initialized.
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Local_ਸੂਚਕ<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie> mutableTrie(
        ਨਵਾਂ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(initial_ਮੁੱਲ, error_ਮੁੱਲ, error_ਕੋਡ),
        error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UChar32 ਸ਼ੁਰੂ = 0, ਸਿਰਾ;
    uint32_t value;
    ਜਦੋਂ ((ਸਿਰਾ = ucptrie_getRange(trie, ਸ਼ੁਰੂ, UCPMAP_RANGE_ਸਾਧਾਰਨ, 0,
                                   ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &value)) >= 0) {
        ਜੇ (value != initial_ਮੁੱਲ) {
            ਜੇ (ਸ਼ੁਰੂ == ਸਿਰਾ) {
                mutableTrie->ਸੈੱਟ(ਸ਼ੁਰੂ, value, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                mutableTrie->ਸੈੱਟ_Range(ਸ਼ੁਰੂ, ਸਿਰਾ, value, error_ਕੋਡ);
            }
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
    }
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        ਵਾਪਸ mutableTrie.orphan();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

ਖਾਲੀ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::clear() {
    index3_Null_ਵਿੱਥ = ਅੰਕੜੇ_Null_ਵਿੱਥ = -1;
    ਅੰਕੜੇ_ਲੰਮਾਈ = 0;
    ਉੱਚਾ_ਮੁੱਲ = initial_ਮੁੱਲ = orig_Initial_ਮੁੱਲ;
    ਉੱਚਾ_ਸ਼ੁਰੂ = 0;
    uprv_free(index16);
    index16 = ਖਾਲੀ_ਸੂਚਕ;
}

uint32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::get(UChar32 c) ਅਚਲ {
    ਜੇ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        ਵਾਪਸ error_ਮੁੱਲ;
    }
    ਜੇ (c >= ਉੱਚਾ_ਸ਼ੁਰੂ) {
        ਵਾਪਸ ਉੱਚਾ_ਮੁੱਲ;
    }
    int32_t i = c >> UCPTRIE_ਖਿਸਕਾਓ_3;
    ਜੇ (flags[i] == ALL_SAME) {
        ਵਾਪਸ ਸੂਚਕਾਂਕ[i];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਅੰਕੜੇ[ਸੂਚਕਾਂਕ[i] + (c & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ)];
    }
}

ਸਤਰਅੰਦਰ uint32_t maybe_Filter_ਮੁੱਲ(uint32_t value, uint32_t initial_ਮੁੱਲ, uint32_t null_ਮੁੱਲ,
                                 UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context) {
    ਜੇ (value == initial_ਮੁੱਲ) {
        value = null_ਮੁੱਲ;
    } ਨਹੀਂਤਾਂ ਜੇ (filter != ਖਾਲੀ_ਸੂਚਕ) {
        value = filter(context, value);
    }
    ਵਾਪਸ value;
}

UChar32 Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::getRange(
        UChar32 ਸ਼ੁਰੂ, UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context,
        uint32_t *p_ਮੁੱਲ) ਅਚਲ {
    ਜੇ (static_cast<uint32_t>(ਸ਼ੁਰੂ) > MAX_UNICODE) {
        ਵਾਪਸ U_SENTINEL;
    }
    ਜੇ (ਸ਼ੁਰੂ >= ਉੱਚਾ_ਸ਼ੁਰੂ) {
        ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) {
            uint32_t value = ਉੱਚਾ_ਮੁੱਲ;
            ਜੇ (filter != ਖਾਲੀ_ਸੂਚਕ) { value = filter(context, value); }
            *p_ਮੁੱਲ = value;
        }
        ਵਾਪਸ MAX_UNICODE;
    }
    uint32_t null_ਮੁੱਲ = initial_ਮੁੱਲ;
    ਜੇ (filter != ਖਾਲੀ_ਸੂਚਕ) { null_ਮੁੱਲ = filter(context, null_ਮੁੱਲ); }
    UChar32 c = ਸ਼ੁਰੂ;
    uint32_t trie_ਮੁੱਲ, value;
    ਤਾਰਕਿਕ have_ਮੁੱਲ = ਝੂਠ;
    int32_t i = c >> UCPTRIE_ਖਿਸਕਾਓ_3;
    ਕਰੋ {
        ਜੇ (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = ਸੂਚਕਾਂਕ[i];
            ਜੇ (have_ਮੁੱਲ) {
                ਜੇ (trieValue2 != trie_ਮੁੱਲ) {
                    ਜੇ (filter == ਖਾਲੀ_ਸੂਚਕ ||
                            maybe_Filter_ਮੁੱਲ(trieValue2, initial_ਮੁੱਲ, null_ਮੁੱਲ,
                                             filter, context) != value) {
                        ਵਾਪਸ c - 1;
                    }
                    trie_ਮੁੱਲ = trieValue2;  // may or may not help
                }
            } ਨਹੀਂਤਾਂ {
                trie_ਮੁੱਲ = trieValue2;
                value = maybe_Filter_ਮੁੱਲ(trieValue2, initial_ਮੁੱਲ, null_ਮੁੱਲ, filter, context);
                ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਮੁੱਲ = value; }
                have_ਮੁੱਲ = ਸੱਚ;
            }
            c = (c + UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) & ~UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ;
        } ਨਹੀਂਤਾਂ /* MIXED */ {
            int32_t di = ਸੂਚਕਾਂਕ[i] + (c & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ);
            uint32_t trieValue2 = ਅੰਕੜੇ[di];
            ਜੇ (have_ਮੁੱਲ) {
                ਜੇ (trieValue2 != trie_ਮੁੱਲ) {
                    ਜੇ (filter == ਖਾਲੀ_ਸੂਚਕ ||
                            maybe_Filter_ਮੁੱਲ(trieValue2, initial_ਮੁੱਲ, null_ਮੁੱਲ,
                                             filter, context) != value) {
                        ਵਾਪਸ c - 1;
                    }
                    trie_ਮੁੱਲ = trieValue2;  // may or may not help
                }
            } ਨਹੀਂਤਾਂ {
                trie_ਮੁੱਲ = trieValue2;
                value = maybe_Filter_ਮੁੱਲ(trieValue2, initial_ਮੁੱਲ, null_ਮੁੱਲ, filter, context);
                ਜੇ (p_ਮੁੱਲ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਮੁੱਲ = value; }
                have_ਮੁੱਲ = ਸੱਚ;
            }
            ਜਦੋਂ ((++c & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ) != 0) {
                trieValue2 = ਅੰਕੜੇ[++di];
                ਜੇ (trieValue2 != trie_ਮੁੱਲ) {
                    ਜੇ (filter == ਖਾਲੀ_ਸੂਚਕ ||
                            maybe_Filter_ਮੁੱਲ(trieValue2, initial_ਮੁੱਲ, null_ਮੁੱਲ,
                                             filter, context) != value) {
                        ਵਾਪਸ c - 1;
                    }
                }
                trie_ਮੁੱਲ = trieValue2;  // may or may not help
            }
        }
        ++i;
    } ਜਦੋਂ (c < ਉੱਚਾ_ਸ਼ੁਰੂ);
    U_ASSERT(have_ਮੁੱਲ);
    ਜੇ (maybe_Filter_ਮੁੱਲ(ਉੱਚਾ_ਮੁੱਲ, initial_ਮੁੱਲ, null_ਮੁੱਲ,
                         filter, context) != value) {
        ਵਾਪਸ c - 1;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ MAX_UNICODE;
    }
}

ਖਾਲੀ
write_ਖੰਡ(uint32_t *ਖੰਡ, uint32_t value) {
    uint32_t *ਹੱਦਬੰਦੀ = ਖੰਡ + UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    ਜਦੋਂ (ਖੰਡ < ਹੱਦਬੰਦੀ) {
        *ਖੰਡ++ = value;
    }
}

ਤਾਰਕਿਕ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::ensure_ਉੱਚਾ_ਸ਼ੁਰੂ(UChar32 c) {
    ਜੇ (c >= ਉੱਚਾ_ਸ਼ੁਰੂ) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ) & ~(UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ - 1);
        int32_t i = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
        int32_t i_ਹੱਦਬੰਦੀ = c >> UCPTRIE_ਖਿਸਕਾਓ_3;
        ਜੇ (i_ਹੱਦਬੰਦੀ > ਸੂਚਕਾਂਕ_ਸਮਰੱਥਾ) {
            uint32_t* new_ਸੂਚਕਾਂਕ = static_cast<uint32_t*>(uprv_malloc(I_ਹੱਦਬੰਦੀ * 4));
            ਜੇ (new_ਸੂਚਕਾਂਕ == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ ਝੂਠ; }
            uprv_memcpy(new_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ, i * 4);
            uprv_free(ਸੂਚਕਾਂਕ);
            ਸੂਚਕਾਂਕ = new_ਸੂਚਕਾਂਕ;
            ਸੂਚਕਾਂਕ_ਸਮਰੱਥਾ = I_ਹੱਦਬੰਦੀ;
        }
        ਕਰੋ {
            flags[i] = ALL_SAME;
            ਸੂਚਕਾਂਕ[i] = initial_ਮੁੱਲ;
        } ਜਦੋਂ(++i < i_ਹੱਦਬੰਦੀ);
        ਉੱਚਾ_ਸ਼ੁਰੂ = c;
    }
    ਵਾਪਸ ਸੱਚ;
}

int32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::alloc_ਅੰਕੜੇ_ਖੰਡ(int32_t ਖੰਡ_ਲੰਮਾਈ) {
    int32_t new_ਖੰਡ = ਅੰਕੜੇ_ਲੰਮਾਈ;
    int32_t new_ਸਿਖਰ = new_ਖੰਡ + ਖੰਡ_ਲੰਮਾਈ;
    ਜੇ (new_ਸਿਖਰ > ਅੰਕੜੇ_ਸਮਰੱਥਾ) {
        int32_t capacity;
        ਜੇ (ਅੰਕੜੇ_ਸਮਰੱਥਾ < MEDIUM_ਅੰਕੜੇ_ਲੰਮਾਈ) {
            capacity = MEDIUM_ਅੰਕੜੇ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕੜੇ_ਸਮਰੱਥਾ < MAX_ਅੰਕੜੇ_ਲੰਮਾਈ) {
            capacity = MAX_ਅੰਕੜੇ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            ਵਾਪਸ -1;
        }
        uint32_t* new_ਅੰਕੜੇ = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        ਜੇ (new_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ -1;
        }
        uprv_memcpy(new_ਅੰਕੜੇ, ਅੰਕੜੇ, (size_t)ਅੰਕੜੇ_ਲੰਮਾਈ * 4);
        uprv_free(ਅੰਕੜੇ);
        ਅੰਕੜੇ = new_ਅੰਕੜੇ;
        ਅੰਕੜੇ_ਸਮਰੱਥਾ = capacity;
    }
    ਅੰਕੜੇ_ਲੰਮਾਈ = new_ਸਿਖਰ;
    ਵਾਪਸ new_ਖੰਡ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::get_ਅੰਕੜੇ_ਖੰਡ(int32_t i) {
    ਜੇ (flags[i] == MIXED) {
        ਵਾਪਸ ਸੂਚਕਾਂਕ[i];
    }
    ਜੇ (i < BMP_I_ਹੱਦਬੰਦੀ) {
        int32_t new_ਖੰਡ = alloc_ਅੰਕੜੇ_ਖੰਡ(UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ);
        ਜੇ (new_ਖੰਡ < 0) { ਵਾਪਸ new_ਖੰਡ; }
        int32_t i_ਸ਼ੁਰੂ = i & ~(SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ -1);
        int32_t i_ਹੱਦਬੰਦੀ = i_ਸ਼ੁਰੂ + SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ;
        ਕਰੋ {
            U_ASSERT(flags[i_ਸ਼ੁਰੂ] == ALL_SAME);
            write_ਖੰਡ(ਅੰਕੜੇ + new_ਖੰਡ, ਸੂਚਕਾਂਕ[i_ਸ਼ੁਰੂ]);
            flags[i_ਸ਼ੁਰੂ] = MIXED;
            ਸੂਚਕਾਂਕ[i_ਸ਼ੁਰੂ++] = new_ਖੰਡ;
            new_ਖੰਡ += UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        } ਜਦੋਂ (i_ਸ਼ੁਰੂ < i_ਹੱਦਬੰਦੀ);
        ਵਾਪਸ ਸੂਚਕਾਂਕ[i];
    } ਨਹੀਂਤਾਂ {
        int32_t new_ਖੰਡ = alloc_ਅੰਕੜੇ_ਖੰਡ(UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ);
        ਜੇ (new_ਖੰਡ < 0) { ਵਾਪਸ new_ਖੰਡ; }
        write_ਖੰਡ(ਅੰਕੜੇ + new_ਖੰਡ, ਸੂਚਕਾਂਕ[i]);
        flags[i] = MIXED;
        ਸੂਚਕਾਂਕ[i] = new_ਖੰਡ;
        ਵਾਪਸ new_ਖੰਡ;
    }
}

ਖਾਲੀ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::ਸੈੱਟ(UChar32 c, uint32_t value, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    int32_t ਖੰਡ;
    ਜੇ (!ensure_ਉੱਚਾ_ਸ਼ੁਰੂ(c) || (ਖੰਡ = get_ਅੰਕੜੇ_ਖੰਡ(c >> UCPTRIE_ਖਿਸਕਾਓ_3)) < 0) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    ਅੰਕੜੇ[ਖੰਡ + (c & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ)] = value;
}

ਖਾਲੀ
fill_ਖੰਡ(uint32_t *ਖੰਡ, UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ, uint32_t value) {
    uint32_t *p_ਹੱਦਬੰਦੀ = ਖੰਡ + ਹੱਦਬੰਦੀ;
    ਖੰਡ += ਸ਼ੁਰੂ;
    ਜਦੋਂ (ਖੰਡ < p_ਹੱਦਬੰਦੀ) {
        *ਖੰਡ++ = value;
    }
}

ਖਾਲੀ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::ਸੈੱਟ_Range(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ, uint32_t value, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ (static_cast<uint32_t>(ਸ਼ੁਰੂ) > MAX_UNICODE || static_cast<uint32_t>(ਸਿਰਾ) > MAX_UNICODE || ਸ਼ੁਰੂ > ਸਿਰਾ) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (!ensure_ਉੱਚਾ_ਸ਼ੁਰੂ(ਸਿਰਾ)) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    UChar32 ਹੱਦਬੰਦੀ = ਸਿਰਾ + 1;
    ਜੇ (ਸ਼ੁਰੂ & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ) {
        // Set partial block at [start..following block boundary[.
        int32_t ਖੰਡ = get_ਅੰਕੜੇ_ਖੰਡ(ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3);
        ਜੇ (ਖੰਡ < 0) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }

        UChar32 ਅਗਲਾ_ਸ਼ੁਰੂ = (ਸ਼ੁਰੂ + UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ) & ~UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ;
        ਜੇ (ਅਗਲਾ_ਸ਼ੁਰੂ <= ਹੱਦਬੰਦੀ) {
            fill_ਖੰਡ(ਅੰਕੜੇ + ਖੰਡ, ਸ਼ੁਰੂ & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ, UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ,
                      value);
            ਸ਼ੁਰੂ = ਅਗਲਾ_ਸ਼ੁਰੂ;
        } ਨਹੀਂਤਾਂ {
            fill_ਖੰਡ(ਅੰਕੜੇ + ਖੰਡ, ਸ਼ੁਰੂ & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ, ਹੱਦਬੰਦੀ & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ,
                      value);
            ਵਾਪਸ;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = ਹੱਦਬੰਦੀ & UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ;

    // Round down limit to a block boundary.
    ਹੱਦਬੰਦੀ &= ~UCPTRIE_SMALL_ਅੰਕੜੇ_ਨਕਾਬ;

    // Iterate over all-value blocks.
    ਜਦੋਂ (ਸ਼ੁਰੂ < ਹੱਦਬੰਦੀ) {
        int32_t i = ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
        ਜੇ (flags[i] == ALL_SAME) {
            ਸੂਚਕਾਂਕ[i] = value;
        } ਨਹੀਂਤਾਂ /* MIXED */ {
            fill_ਖੰਡ(ਅੰਕੜੇ + ਸੂਚਕਾਂਕ[i], 0, UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, value);
        }
        ਸ਼ੁਰੂ += UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    }

    ਜੇ (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t ਖੰਡ = get_ਅੰਕੜੇ_ਖੰਡ(ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3);
        ਜੇ (ਖੰਡ < 0) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }

        fill_ਖੰਡ(ਅੰਕੜੇ + ਖੰਡ, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

ਖਾਲੀ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::ਨਕਾਬ_ਮੁੱਲਾਂ(uint32_t mask) {
    initial_ਮੁੱਲ &= mask;
    error_ਮੁੱਲ &= mask;
    ਉੱਚਾ_ਮੁੱਲ &= mask;
    int32_t i_ਹੱਦਬੰਦੀ = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
    ਲਈ (int32_t i = 0; i < i_ਹੱਦਬੰਦੀ; ++i) {
        ਜੇ (flags[i] == ALL_SAME) {
            ਸੂਚਕਾਂਕ[i] &= mask;
        }
    }
    ਲਈ (int32_t i = 0; i < ਅੰਕੜੇ_ਲੰਮਾਈ; ++i) {
        ਅੰਕੜੇ[i] &= mask;
    }
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UIntA, ਕਿਸਮ_ਨਾਂ UIntB>
ਤਾਰਕਿਕ equal_ਖੰਡਾਂ(ਅਚਲ UIntA *s, ਅਚਲ UIntB *t, int32_t length) {
    ਜਦੋਂ (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    ਵਾਪਸ length == 0;
}

ਤਾਰਕਿਕ all_ਮੁੱਲਾਂ_Same_As(ਅਚਲ uint32_t *p, int32_t length, uint32_t value) {
    ਅਚਲ uint32_t *p_ਹੱਦਬੰਦੀ = p + length;
    ਜਦੋਂ (p < p_ਹੱਦਬੰਦੀ && *p == value) { ++p; }
    ਵਾਪਸ p == p_ਹੱਦਬੰਦੀ;
}

/** Search for an identical block. */
int32_t ਲੱਭੋ_Same_ਖੰਡ(ਅਚਲ uint16_t *p, int32_t p_ਸ਼ੁਰੂ, int32_t length,
                      ਅਚਲ uint16_t *q, int32_t q_ਸ਼ੁਰੂ, int32_t ਖੰਡ_ਲੰਮਾਈ) {
    // Ensure that we do not even partially get past length.
    length -= ਖੰਡ_ਲੰਮਾਈ;

    q += q_ਸ਼ੁਰੂ;
    ਜਦੋਂ (p_ਸ਼ੁਰੂ <= length) {
        ਜੇ (equal_ਖੰਡਾਂ(p + p_ਸ਼ੁਰੂ, q, ਖੰਡ_ਲੰਮਾਈ)) {
            ਵਾਪਸ p_ਸ਼ੁਰੂ;
        }
        ++p_ਸ਼ੁਰੂ;
    }
    ਵਾਪਸ -1;
}

int32_t ਲੱਭੋ_All_Same_ਖੰਡ(ਅਚਲ uint32_t *p, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ,
                         uint32_t value, int32_t ਖੰਡ_ਲੰਮਾਈ) {
    // Ensure that we do not even partially get past limit.
    ਹੱਦਬੰਦੀ -= ਖੰਡ_ਲੰਮਾਈ;

    ਲਈ (int32_t ਖੰਡ = ਸ਼ੁਰੂ; ਖੰਡ <= ਹੱਦਬੰਦੀ; ++ਖੰਡ) {
        ਜੇ (p[ਖੰਡ] == value) {
            ਲਈ (int32_t i = 1;; ++i) {
                ਜੇ (i == ਖੰਡ_ਲੰਮਾਈ) {
                    ਵਾਪਸ ਖੰਡ;
                }
                ਜੇ (p[ਖੰਡ + i] != value) {
                    ਖੰਡ += i;
                    ਰੋਕੋ;
                }
            }
        }
    }
    ਵਾਪਸ -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UIntA, ਕਿਸਮ_ਨਾਂ UIntB>
int32_t getOverlap(ਅਚਲ UIntA *p, int32_t length,
                   ਅਚਲ UIntB *q, int32_t q_ਸ਼ੁਰੂ, int32_t ਖੰਡ_ਲੰਮਾਈ) {
    int32_t overlap = ਖੰਡ_ਲੰਮਾਈ - 1;
    U_ASSERT(overlap <= length);
    q += q_ਸ਼ੁਰੂ;
    ਜਦੋਂ (overlap > 0 && !equal_ਖੰਡਾਂ(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    ਵਾਪਸ overlap;
}

int32_t getAllSameOverlap(ਅਚਲ uint32_t *p, int32_t length, uint32_t value,
                          int32_t ਖੰਡ_ਲੰਮਾਈ) {
    int32_t min = length - (ਖੰਡ_ਲੰਮਾਈ - 1);
    int32_t i = length;
    ਜਦੋਂ (min < i && p[i - 1] == value) { --i; }
    ਵਾਪਸ length - i;
}

ਤਾਰਕਿਕ is_ਸ਼ੁਰੂ_ਦਾ_Some_Fast_ਖੰਡ(uint32_t ਅੰਕੜੇ_ਵਿੱਥ, ਅਚਲ uint32_t ਸੂਚਕਾਂਕ[], int32_t fastILimit) {
    ਲਈ (int32_t i = 0; i < fastILimit; i += SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ) {
        ਜੇ (ਸੂਚਕਾਂਕ[i] == ਅੰਕੜੇ_ਵਿੱਥ) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::ਲੱਭੋ_ਉੱਚਾ_ਸ਼ੁਰੂ() ਅਚਲ {
    int32_t i = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
    ਜਦੋਂ (i > 0) {
        ਤਾਰਕਿਕ ਮੇਲ;
        ਜੇ (flags[--i] == ALL_SAME) {
            ਮੇਲ = ਸੂਚਕਾਂਕ[i] == ਉੱਚਾ_ਮੁੱਲ;
        } ਨਹੀਂਤਾਂ /* MIXED */ {
            ਅਚਲ uint32_t *p = ਅੰਕੜੇ + ਸੂਚਕਾਂਕ[i];
            ਲਈ (int32_t j = 0;; ++j) {
                ਜੇ (j == UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
                    ਮੇਲ = ਸੱਚ;
                    ਰੋਕੋ;
                }
                ਜੇ (p[j] != ਉੱਚਾ_ਮੁੱਲ) {
                    ਮੇਲ = ਝੂਠ;
                    ਰੋਕੋ;
                }
            }
        }
        ਜੇ (!ਮੇਲ) {
            ਵਾਪਸ (i + 1) << UCPTRIE_ਖਿਸਕਾਓ_3;
        }
    }
    ਵਾਪਸ 0;
}

ਵਰਗ All_Same_ਖੰਡਾਂ {
ਜਨਤਕ:
    ਸਥਿਰ constexpr int32_t NEW_UNIQUE = -1;
    ਸਥਿਰ constexpr int32_t ਉਛਾਲ_2 = -2;

    All_Same_ਖੰਡਾਂ() : length(0), mostRecent(-1) {}

    int32_t ਲੱਭੋ_Or_ਜੋੜੋ(int32_t ਸੂਚਕਾਂਕ, int32_t count, uint32_t value) {
        ਜੇ (mostRecent >= 0 && ਮੁੱਲਾਂ[mostRecent] == value) {
            refCounts[mostRecent] += count;
            ਵਾਪਸ indexes[mostRecent];
        }
        ਲਈ (int32_t i = 0; i < length; ++i) {
            ਜੇ (ਮੁੱਲਾਂ[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                ਵਾਪਸ indexes[i];
            }
        }
        ਜੇ (length == ਸਮਰੱਥਾ_3) {
            ਵਾਪਸ ਉਛਾਲ_2;
        }
        mostRecent = length;
        indexes[length] = ਸੂਚਕਾਂਕ;
        ਮੁੱਲਾਂ[length] = value;
        refCounts[length++] = count;
        ਵਾਪਸ NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    ਖਾਲੀ ਜੋੜੋ(int32_t ਸੂਚਕਾਂਕ, int32_t count, uint32_t value) {
        U_ASSERT(length == ਸਮਰੱਥਾ_3);
        int32_t least = -1;
        int32_t ਘੱਟ_ਤੋਂ_ਘੱਟ_ਗਿਣਤੀ = I_ਹੱਦਬੰਦੀ;
        ਲਈ (int32_t i = 0; i < length; ++i) {
            U_ASSERT(ਮੁੱਲਾਂ[i] != value);
            ਜੇ (refCounts[i] < ਘੱਟ_ਤੋਂ_ਘੱਟ_ਗਿਣਤੀ) {
                least = i;
                ਘੱਟ_ਤੋਂ_ਘੱਟ_ਗਿਣਤੀ = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = ਸੂਚਕਾਂਕ;
        ਮੁੱਲਾਂ[least] = value;
        refCounts[least] = count;
    }

    int32_t ਲੱਭੋ_Most_ਵਰਤਿਆ() ਅਚਲ {
        ਜੇ (length == 0) { ਵਾਪਸ -1; }
        int32_t max = -1;
        int32_t max_ਗਿਣਤੀ = 0;
        ਲਈ (int32_t i = 0; i < length; ++i) {
            ਜੇ (refCounts[i] > max_ਗਿਣਤੀ) {
                max = i;
                max_ਗਿਣਤੀ = refCounts[i];
            }
        }
        ਵਾਪਸ indexes[max];
    }

ਨਿਜੀ:
    ਸਥਿਰ constexpr int32_t ਸਮਰੱਥਾ_3 = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[ਸਮਰੱਥਾ_3];
    uint32_t ਮੁੱਲਾਂ[ਸਮਰੱਥਾ_3];
    int32_t refCounts[ਸਮਰੱਥਾ_3];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
ਵਰਗ Mixed_ਖੰਡਾਂ {
ਜਨਤਕ:
    Mixed_ਖੰਡਾਂ() {}
    ~Mixed_ਖੰਡਾਂ() {
        uprv_free(ਸਾਰਣੀ);
    }

    ਤਾਰਕਿਕ init(int32_t max_ਲੰਮਾਈ_2, int32_t new_ਖੰਡ_ਲੰਮਾਈ) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ = max_ਲੰਮਾਈ_2 - new_ਖੰਡ_ਲੰਮਾਈ + 1;
        int32_t new_ਲੰਮਾਈ;
        ਜੇ (max_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ <= 0xfff) {  // 4k
            new_ਲੰਮਾਈ = 6007;
            ਖਿਸਕਾਓ = 12;
            mask = 0xfff;
        } ਨਹੀਂਤਾਂ ਜੇ (max_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ <= 0x7fff) {  // 32k
            new_ਲੰਮਾਈ = 50021;
            ਖਿਸਕਾਓ = 15;
            mask = 0x7fff;
        } ਨਹੀਂਤਾਂ ਜੇ (max_ਅੰਕੜੇ_ਸੂਚਕਾਂਕ <= 0x1ffff) {  // 128k
            new_ਲੰਮਾਈ = 200003;
            ਖਿਸਕਾਓ = 17;
            mask = 0x1ffff;
        } ਨਹੀਂਤਾਂ {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_ਲੰਮਾਈ = 1500007;
            ਖਿਸਕਾਓ = 21;
            mask = 0x1fffff;
        }
        ਜੇ (new_ਲੰਮਾਈ > capacity) {
            uprv_free(ਸਾਰਣੀ);
            ਸਾਰਣੀ = static_cast<uint32_t*>(uprv_malloc(new_ਲੰਮਾਈ * 4));
            ਜੇ (ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਝੂਠ;
            }
            capacity = new_ਲੰਮਾਈ;
        }
        length = new_ਲੰਮਾਈ;
        uprv_memset(ਸਾਰਣੀ, 0, length * 4);

        ਖੰਡ_ਲੰਮਾਈ = new_ਖੰਡ_ਲੰਮਾਈ;
        ਵਾਪਸ ਸੱਚ;
    }

    ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UInt>
    ਖਾਲੀ extend(ਅਚਲ UInt *ਅੰਕੜੇ, int32_t min_ਸ਼ੁਰੂ, int32_t prev_ਅੰਕੜੇ_ਲੰਮਾਈ, int32_t new_ਅੰਕੜੇ_ਲੰਮਾਈ) {
        int32_t ਸ਼ੁਰੂ = prev_ਅੰਕੜੇ_ਲੰਮਾਈ - ਖੰਡ_ਲੰਮਾਈ;
        ਜੇ (ਸ਼ੁਰੂ >= min_ਸ਼ੁਰੂ) {
            ++ਸ਼ੁਰੂ;  // Skip the last block that we added last time.
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ = min_ਸ਼ੁਰੂ;  // Begin with the first full block.
        }
        ਲਈ (int32_t ਸਿਰਾ = new_ਅੰਕੜੇ_ਲੰਮਾਈ - ਖੰਡ_ਲੰਮਾਈ; ਸ਼ੁਰੂ <= ਸਿਰਾ; ++ਸ਼ੁਰੂ) {
            uint32_t hash_ਕੋਡ = make_Hash_ਕੋਡ(ਅੰਕੜੇ, ਸ਼ੁਰੂ);
            ਜੋੜੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, ਸ਼ੁਰੂ, hash_ਕੋਡ, ਸ਼ੁਰੂ);
        }
    }

    ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UIntA, ਕਿਸਮ_ਨਾਂ UIntB>
    int32_t ਲੱਭੋ_ਖੰਡ(ਅਚਲ UIntA *ਅੰਕੜੇ, ਅਚਲ UIntB *ਖੰਡ_ਅੰਕੜੇ, int32_t ਖੰਡ_ਸ਼ੁਰੂ) ਅਚਲ {
        uint32_t hash_ਕੋਡ = make_Hash_ਕੋਡ(ਖੰਡ_ਅੰਕੜੇ, ਖੰਡ_ਸ਼ੁਰੂ);
        int32_t ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = ਲੱਭੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, ਖੰਡ_ਅੰਕੜੇ, ਖੰਡ_ਸ਼ੁਰੂ, hash_ਕੋਡ);
        ਜੇ (ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ >= 0) {
            ਵਾਪਸ (ਸਾਰਣੀ[ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ] & mask) - 1;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ -1;
        }
    }

    int32_t ਲੱਭੋ_All_Same_ਖੰਡ(ਅਚਲ uint32_t *ਅੰਕੜੇ, uint32_t ਖੰਡ_ਮੁੱਲ) ਅਚਲ {
        uint32_t hash_ਕੋਡ = make_Hash_ਕੋਡ(ਖੰਡ_ਮੁੱਲ);
        int32_t ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = ਲੱਭੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, ਖੰਡ_ਮੁੱਲ, hash_ਕੋਡ);
        ਜੇ (ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ >= 0) {
            ਵਾਪਸ (ਸਾਰਣੀ[ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ] & mask) - 1;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ -1;
        }
    }

ਨਿਜੀ:
    ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UInt>
    uint32_t make_Hash_ਕੋਡ(ਅਚਲ UInt *ਖੰਡ_ਅੰਕੜੇ, int32_t ਖੰਡ_ਸ਼ੁਰੂ) ਅਚਲ {
        int32_t ਖੰਡ_ਹੱਦਬੰਦੀ = ਖੰਡ_ਸ਼ੁਰੂ + ਖੰਡ_ਲੰਮਾਈ;
        uint32_t hash_ਕੋਡ = ਖੰਡ_ਅੰਕੜੇ[ਖੰਡ_ਸ਼ੁਰੂ++];
        ਕਰੋ {
            hash_ਕੋਡ = 37 * hash_ਕੋਡ + ਖੰਡ_ਅੰਕੜੇ[ਖੰਡ_ਸ਼ੁਰੂ++];
        } ਜਦੋਂ (ਖੰਡ_ਸ਼ੁਰੂ < ਖੰਡ_ਹੱਦਬੰਦੀ);
        ਵਾਪਸ hash_ਕੋਡ;
    }

    uint32_t make_Hash_ਕੋਡ(uint32_t ਖੰਡ_ਮੁੱਲ) ਅਚਲ {
        uint32_t hash_ਕੋਡ = ਖੰਡ_ਮੁੱਲ;
        ਲਈ (int32_t i = 1; i < ਖੰਡ_ਲੰਮਾਈ; ++i) {
            hash_ਕੋਡ = 37 * hash_ਕੋਡ + ਖੰਡ_ਮੁੱਲ;
        }
        ਵਾਪਸ hash_ਕੋਡ;
    }

    ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UInt>
    ਖਾਲੀ ਜੋੜੋ_ਇੰਦਰਾਜ(ਅਚਲ UInt *ਅੰਕੜੇ, int32_t ਖੰਡ_ਸ਼ੁਰੂ, uint32_t hash_ਕੋਡ, int32_t ਅੰਕੜੇ_ਸੂਚਕਾਂਕ) {
        U_ASSERT(0 <= ਅੰਕੜੇ_ਸੂਚਕਾਂਕ && ਅੰਕੜੇ_ਸੂਚਕਾਂਕ < (int32_t)mask);
        int32_t ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = ਲੱਭੋ_ਇੰਦਰਾਜ(ਅੰਕੜੇ, ਅੰਕੜੇ, ਖੰਡ_ਸ਼ੁਰੂ, hash_ਕੋਡ);
        ਜੇ (ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ < 0) {
            ਸਾਰਣੀ[~ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ] = (hash_ਕੋਡ << ਖਿਸਕਾਓ) | (ਅੰਕੜੇ_ਸੂਚਕਾਂਕ + 1);
        }
    }

    ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ UIntA, ਕਿਸਮ_ਨਾਂ UIntB>
    int32_t ਲੱਭੋ_ਇੰਦਰਾਜ(ਅਚਲ UIntA *ਅੰਕੜੇ, ਅਚਲ UIntB *ਖੰਡ_ਅੰਕੜੇ, int32_t ਖੰਡ_ਸ਼ੁਰੂ,
                      uint32_t hash_ਕੋਡ) ਅਚਲ {
        uint32_t shifted_Hash_ਕੋਡ = hash_ਕੋਡ << ਖਿਸਕਾਓ;
        int32_t initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = (hash_ਕੋਡ % (length - 1)) + 1;  // 1..length-1
        ਲਈ (int32_t ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ;;) {
            uint32_t ਇੰਦਰਾਜ = ਸਾਰਣੀ[ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ];
            ਜੇ (ਇੰਦਰਾਜ == 0) {
                ਵਾਪਸ ~ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ;
            }
            ਜੇ ((ਇੰਦਰਾਜ & ~mask) == shifted_Hash_ਕੋਡ) {
                int32_t ਅੰਕੜੇ_ਸੂਚਕਾਂਕ = (ਇੰਦਰਾਜ & mask) - 1;
                ਜੇ (equal_ਖੰਡਾਂ(ਅੰਕੜੇ + ਅੰਕੜੇ_ਸੂਚਕਾਂਕ, ਖੰਡ_ਅੰਕੜੇ + ਖੰਡ_ਸ਼ੁਰੂ, ਖੰਡ_ਲੰਮਾਈ)) {
                    ਵਾਪਸ ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ;
                }
            }
            ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = ਅਗਲਾ_ਸੂਚਕਾਂਕ(initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ, ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ);
        }
    }

    int32_t ਲੱਭੋ_ਇੰਦਰਾਜ(ਅਚਲ uint32_t *ਅੰਕੜੇ, uint32_t ਖੰਡ_ਮੁੱਲ, uint32_t hash_ਕੋਡ) ਅਚਲ {
        uint32_t shifted_Hash_ਕੋਡ = hash_ਕੋਡ << ਖਿਸਕਾਓ;
        int32_t initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = (hash_ਕੋਡ % (length - 1)) + 1;  // 1..length-1
        ਲਈ (int32_t ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ;;) {
            uint32_t ਇੰਦਰਾਜ = ਸਾਰਣੀ[ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ];
            ਜੇ (ਇੰਦਰਾਜ == 0) {
                ਵਾਪਸ ~ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ;
            }
            ਜੇ ((ਇੰਦਰਾਜ & ~mask) == shifted_Hash_ਕੋਡ) {
                int32_t ਅੰਕੜੇ_ਸੂਚਕਾਂਕ = (ਇੰਦਰਾਜ & mask) - 1;
                ਜੇ (all_ਮੁੱਲਾਂ_Same_As(ਅੰਕੜੇ + ਅੰਕੜੇ_ਸੂਚਕਾਂਕ, ਖੰਡ_ਲੰਮਾਈ, ਖੰਡ_ਮੁੱਲ)) {
                    ਵਾਪਸ ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ;
                }
            }
            ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = ਅਗਲਾ_ਸੂਚਕਾਂਕ(initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ, ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ);
        }
    }

    ਸਤਰਅੰਦਰ int32_t ਅਗਲਾ_ਸੂਚਕਾਂਕ(int32_t initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ, int32_t ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ) ਅਚਲ {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        ਵਾਪਸ (ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ + initial_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *ਸਾਰਣੀ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t ਖਿਸਕਾਓ = 0;
    uint32_t mask = 0;

    int32_t ਖੰਡ_ਲੰਮਾਈ = 0;
};

int32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::compact_ਸਾਰਾ_ਅੰਕੜੇ_ਖੰਡਾਂ(int32_t fastILimit, All_Same_ਖੰਡਾਂ &all_Same_ਖੰਡਾਂ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    ਤਾਰਕਿਕ ਉਛਾਲ = ਝੂਠ;
#ਜੇ_ਅੰਤ

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_ਅੰਕੜੇ_ਸਮਰੱਥਾ = ASCII_ਹੱਦਬੰਦੀ;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_ਅੰਕੜੇ_ਸਮਰੱਥਾ += UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    // Add room for special values (errorValue, highValue) and padding.
    new_ਅੰਕੜੇ_ਸਮਰੱਥਾ += 4;
    int32_t i_ਹੱਦਬੰਦੀ = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
    int32_t ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    int32_t inc = SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ;
    ਲਈ (int32_t i = 0; i < i_ਹੱਦਬੰਦੀ; i += inc) {
        ਜੇ (i == fastILimit) {
            ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            inc = 1;
        }
        uint32_t value = ਸੂਚਕਾਂਕ[i];
        ਜੇ (flags[i] == MIXED) {
            // Really mixed?
            ਅਚਲ uint32_t *p = ਅੰਕੜੇ + value;
            value = *p;
            ਜੇ (all_ਮੁੱਲਾਂ_Same_As(p + 1, ਖੰਡ_ਲੰਮਾਈ - 1, value)) {
                flags[i] = ALL_SAME;
                ਸੂਚਕਾਂਕ[i] = value;
                // Fall through to ALL_SAME handling.
            } ਨਹੀਂਤਾਂ {
                new_ਅੰਕੜੇ_ਸਮਰੱਥਾ += ਖੰਡ_ਲੰਮਾਈ;
                ਜਾਰੀ;
            }
        } ਨਹੀਂਤਾਂ {
            U_ASSERT(flags[i] == ALL_SAME);
            ਜੇ (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                ਤਾਰਕਿਕ allSame = ਸੱਚ;
                int32_t ਅਗਲਾ_i = i + inc;
                ਲਈ (int32_t j = i + 1; j < ਅਗਲਾ_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    ਜੇ (ਸੂਚਕਾਂਕ[j] != value) {
                        allSame = ਝੂਠ;
                        ਰੋਕੋ;
                    }
                }
                ਜੇ (!allSame) {
                    // Turn it into a MIXED block.
                    ਜੇ (get_ਅੰਕੜੇ_ਖੰਡ(i) < 0) {
                        ਵਾਪਸ -1;
                    }
                    new_ਅੰਕੜੇ_ਸਮਰੱਥਾ += ਖੰਡ_ਲੰਮਾਈ;
                    ਜਾਰੀ;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = all_Same_ਖੰਡਾਂ.ਲੱਭੋ_Or_ਜੋੜੋ(i, inc, value);
        ਜੇ (other == All_Same_ਖੰਡਾਂ::ਉਛਾਲ_2) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
            ਜੇ (!ਉਛਾਲ) {
                puts("UCPTrie AllSameBlocks overflow");
                ਉਛਾਲ = ਸੱਚ;
            }
#ਜੇ_ਅੰਤ
            int32_t jInc = SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ;
            ਲਈ (int32_t j = 0;; j += jInc) {
                ਜੇ (j == i) {
                    all_Same_ਖੰਡਾਂ.ਜੋੜੋ(i, inc, value);
                    ਰੋਕੋ;
                }
                ਜੇ (j == fastILimit) {
                    jInc = 1;
                }
                ਜੇ (flags[j] == ALL_SAME && ਸੂਚਕਾਂਕ[j] == value) {
                    all_Same_ਖੰਡਾਂ.ਜੋੜੋ(j, jInc + inc, value);
                    other = j;
                    ਰੋਕੋ;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        ਜੇ (other >= 0) {
            flags[i] = SAME_AS;
            ਸੂਚਕਾਂਕ[i] = other;
        } ਨਹੀਂਤਾਂ {
            // New unique same-value block.
            new_ਅੰਕੜੇ_ਸਮਰੱਥਾ += ਖੰਡ_ਲੰਮਾਈ;
        }
    }
    ਵਾਪਸ new_ਅੰਕੜੇ_ਸਮਰੱਥਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
#   define DEBUG_DO(ਵਿਅੰਜਕ) ਵਿਅੰਜਕ
#ਨਹੀਂਤਾਂ
#   define DEBUG_DO(ਵਿਅੰਜਕ)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_ਮੁੱਲ(ਅੱਖਰ s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (ਅੱਖਰ)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (ਅੱਖਰ)(0x80 + (value & 0x3F));
    ਵਾਪਸ length + 3;
}

ਖਾਲੀ print_ਖੰਡ(ਅਚਲ uint32_t *ਖੰਡ, int32_t ਖੰਡ_ਲੰਮਾਈ, uint32_t value,
                UChar32 ਸ਼ੁਰੂ, int32_t overlap, uint32_t initial_ਮੁੱਲ) {
    ਅੱਖਰ s[UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ * 3 + 3];
    int32_t length = 0;
    int32_t i;
    ਲਈ (i = 0; i < overlap; ++i) {
        length = append_ਮੁੱਲ(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    ਲਈ (; i < ਖੰਡ_ਲੰਮਾਈ; ++i) {
        ਜੇ (ਖੰਡ != ਖਾਲੀ_ਸੂਚਕ) {
            value = ਖੰਡ[i];
        }
        ਜੇ (value == initial_ਮੁੱਲ) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_ਮੁੱਲ(s, length, value);
    }
    s[length] = 0;
    ਸ਼ੁਰੂ += overlap;
    ਜੇ (ਸ਼ੁਰੂ <= 0xffff) {
        ਲਿਖੋ("    %04lX  %s|\n", (ਲੰਮਾ)ਸ਼ੁਰੂ, s);
    } ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ <= 0xfffff) {
        ਲਿਖੋ("   %5lX  %s|\n", (ਲੰਮਾ)ਸ਼ੁਰੂ, s);
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("  %6lX  %s|\n", (ਲੰਮਾ)ਸ਼ੁਰੂ, s);
    }
}
#ਜੇ_ਅੰਤ

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
int32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::compact_ਅੰਕੜੇ(
        int32_t fastILimit, uint32_t *new_ਅੰਕੜੇ, int32_t new_ਅੰਕੜੇ_ਸਮਰੱਥਾ,
        int32_t ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ, Mixed_ਖੰਡਾਂ &mixed_ਖੰਡਾਂ, UError_ਕੋਡ &error_ਕੋਡ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    int32_t ਗਿਣਤੀ_Same=0, ਜੋੜ_Overlaps=0;
    ਤਾਰਕਿਕ print_ਅੰਕੜੇ = ਅੰਕੜੇ_ਲੰਮਾਈ == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        ਅੰਕੜੇ_ਲੰਮਾਈ == 50400 /* zh.txt~stroke */;
#ਜੇ_ਅੰਤ

    // The linear ASCII data has been copied into newData already.
    int32_t new_ਅੰਕੜੇ_ਲੰਮਾਈ = 0;
    ਲਈ (int32_t i = 0; new_ਅੰਕੜੇ_ਲੰਮਾਈ < ASCII_ਹੱਦਬੰਦੀ;
            new_ਅੰਕੜੇ_ਲੰਮਾਈ += UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, i += SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ) {
        ਸੂਚਕਾਂਕ[i] = new_ਅੰਕੜੇ_ਲੰਮਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
        ਜੇ (print_ਅੰਕੜੇ) {
            print_ਖੰਡ(new_ਅੰਕੜੇ + new_ਅੰਕੜੇ_ਲੰਮਾਈ, UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, 0, new_ਅੰਕੜੇ_ਲੰਮਾਈ, 0, initial_ਮੁੱਲ);
        }
#ਜੇ_ਅੰਤ
    }

    int32_t ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_FAST_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    ਜੇ (!mixed_ਖੰਡਾਂ.init(new_ਅੰਕੜੇ_ਸਮਰੱਥਾ, ਖੰਡ_ਲੰਮਾਈ)) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    mixed_ਖੰਡਾਂ.extend(new_ਅੰਕੜੇ, 0, 0, new_ਅੰਕੜੇ_ਲੰਮਾਈ);

    int32_t i_ਹੱਦਬੰਦੀ = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
    int32_t inc = SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ;
    int32_t fast_ਲੰਮਾਈ = 0;
    ਲਈ (int32_t i = ASCII_I_ਹੱਦਬੰਦੀ; i < i_ਹੱਦਬੰਦੀ; i += inc) {
        ਜੇ (i == fastILimit) {
            ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            inc = 1;
            fast_ਲੰਮਾਈ = new_ਅੰਕੜੇ_ਲੰਮਾਈ;
            ਜੇ (!mixed_ਖੰਡਾਂ.init(new_ਅੰਕੜੇ_ਸਮਰੱਥਾ, ਖੰਡ_ਲੰਮਾਈ)) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ 0;
            }
            mixed_ਖੰਡਾਂ.extend(new_ਅੰਕੜੇ, 0, 0, new_ਅੰਕੜੇ_ਲੰਮਾਈ);
        }
        ਜੇ (flags[i] == ALL_SAME) {
            uint32_t value = ਸੂਚਕਾਂਕ[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixed_ਖੰਡਾਂ.ਲੱਭੋ_All_Same_ਖੰਡ(new_ਅੰਕੜੇ, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            ਜਦੋਂ (n >= 0 && i == ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ && i >= fastILimit && n < fast_ਲੰਮਾਈ &&
                    is_ਸ਼ੁਰੂ_ਦਾ_Some_Fast_ਖੰਡ(n, ਸੂਚਕਾਂਕ, fastILimit)) {
                n = ਲੱਭੋ_All_Same_ਖੰਡ(new_ਅੰਕੜੇ, n + 1, new_ਅੰਕੜੇ_ਲੰਮਾਈ, value, ਖੰਡ_ਲੰਮਾਈ);
            }
            ਜੇ (n >= 0) {
                DEBUG_DO(++ਗਿਣਤੀ_Same);
                ਸੂਚਕਾਂਕ[i] = n;
            } ਨਹੀਂਤਾਂ {
                n = getAllSameOverlap(new_ਅੰਕੜੇ, new_ਅੰਕੜੇ_ਲੰਮਾਈ, value, ਖੰਡ_ਲੰਮਾਈ);
                DEBUG_DO(ਜੋੜ_Overlaps += n);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
                ਜੇ (print_ਅੰਕੜੇ) {
                    print_ਖੰਡ(ਖਾਲੀ_ਸੂਚਕ, ਖੰਡ_ਲੰਮਾਈ, value, i << UCPTRIE_ਖਿਸਕਾਓ_3, n, initial_ਮੁੱਲ);
                }
#ਜੇ_ਅੰਤ
                ਸੂਚਕਾਂਕ[i] = new_ਅੰਕੜੇ_ਲੰਮਾਈ - n;
                int32_t prev_ਅੰਕੜੇ_ਲੰਮਾਈ = new_ਅੰਕੜੇ_ਲੰਮਾਈ;
                ਜਦੋਂ (n < ਖੰਡ_ਲੰਮਾਈ) {
                    new_ਅੰਕੜੇ[new_ਅੰਕੜੇ_ਲੰਮਾਈ++] = value;
                    ++n;
                }
                mixed_ਖੰਡਾਂ.extend(new_ਅੰਕੜੇ, 0, prev_ਅੰਕੜੇ_ਲੰਮਾਈ, new_ਅੰਕੜੇ_ਲੰਮਾਈ);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (flags[i] == MIXED) {
            ਅਚਲ uint32_t *ਖੰਡ = ਅੰਕੜੇ + ਸੂਚਕਾਂਕ[i];
            int32_t n = mixed_ਖੰਡਾਂ.ਲੱਭੋ_ਖੰਡ(new_ਅੰਕੜੇ, ਖੰਡ, 0);
            ਜੇ (n >= 0) {
                DEBUG_DO(++ਗਿਣਤੀ_Same);
                ਸੂਚਕਾਂਕ[i] = n;
            } ਨਹੀਂਤਾਂ {
                n = getOverlap(new_ਅੰਕੜੇ, new_ਅੰਕੜੇ_ਲੰਮਾਈ, ਖੰਡ, 0, ਖੰਡ_ਲੰਮਾਈ);
                DEBUG_DO(ਜੋੜ_Overlaps += n);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
                ਜੇ (print_ਅੰਕੜੇ) {
                    print_ਖੰਡ(ਖੰਡ, ਖੰਡ_ਲੰਮਾਈ, 0, i << UCPTRIE_ਖਿਸਕਾਓ_3, n, initial_ਮੁੱਲ);
                }
#ਜੇ_ਅੰਤ
                ਸੂਚਕਾਂਕ[i] = new_ਅੰਕੜੇ_ਲੰਮਾਈ - n;
                int32_t prev_ਅੰਕੜੇ_ਲੰਮਾਈ = new_ਅੰਕੜੇ_ਲੰਮਾਈ;
                ਜਦੋਂ (n < ਖੰਡ_ਲੰਮਾਈ) {
                    new_ਅੰਕੜੇ[new_ਅੰਕੜੇ_ਲੰਮਾਈ++] = ਖੰਡ[n++];
                }
                mixed_ਖੰਡਾਂ.extend(new_ਅੰਕੜੇ, 0, prev_ਅੰਕੜੇ_ਲੰਮਾਈ, new_ਅੰਕੜੇ_ਲੰਮਾਈ);
            }
        } ਨਹੀਂਤਾਂ /* SAME_AS */ {
            uint32_t j = ਸੂਚਕਾਂਕ[i];
            ਸੂਚਕਾਂਕ[i] = ਸੂਚਕਾਂਕ[j];
        }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    /* we saved some space */
    ਲਿਖੋ("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (ਲੰਮਾ)ਅੰਕੜੇ_ਲੰਮਾਈ, (ਲੰਮਾ)new_ਅੰਕੜੇ_ਲੰਮਾਈ, (ਲੰਮਾ)ਗਿਣਤੀ_Same, (ਲੰਮਾ)ਜੋੜ_Overlaps);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ new_ਅੰਕੜੇ_ਲੰਮਾਈ;
}

int32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::compact_ਸੂਚਕਾਂਕ(int32_t fastILimit, Mixed_ਖੰਡਾਂ &mixed_ਖੰਡਾਂ,
                                           UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = fastILimit >> (UCPTRIE_FAST_ਖਿਸਕਾਓ - UCPTRIE_ਖਿਸਕਾਓ_3);
    ਜੇ ((ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_FAST_ਖਿਸਕਾਓ) <= fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ) {
        // Only the linear fast index, no multi-stage index tables.
        index3_Null_ਵਿੱਥ = UCPTRIE_NO_INDEX3_NULL_ਵਿੱਥ;
        ਵਾਪਸ fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_ਸੂਚਕਾਂਕ[UCPTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ];  // fastIndexLength
    int32_t i3_ਪਹਿਲਾ_Null = -1;
    ਲਈ (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = ਸੂਚਕਾਂਕ[i];
        fast_ਸੂਚਕਾਂਕ[j] = static_cast<uint16_t>(i3);
        ਜੇ (i3 == static_cast<uint32_t>(ਅੰਕੜੇ_Null_ਵਿੱਥ)) {
            ਜੇ (i3_ਪਹਿਲਾ_Null < 0) {
                i3_ਪਹਿਲਾ_Null = j;
            } ਨਹੀਂਤਾਂ ਜੇ (index3_Null_ਵਿੱਥ < 0 &&
                    (j - i3_ਪਹਿਲਾ_Null + 1) == UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ) {
                index3_Null_ਵਿੱਥ = i3_ਪਹਿਲਾ_Null;
            }
        } ਨਹੀਂਤਾਂ {
            i3_ਪਹਿਲਾ_Null = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_ਅਗਲਾ = i + SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ;
        ਜਦੋਂ (++i < i_ਅਗਲਾ) {
            i3 += UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            ਸੂਚਕਾਂਕ[i] = i3;
        }
    }

    ਜੇ (!mixed_ਖੰਡਾਂ.init(fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ)) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    mixed_ਖੰਡਾਂ.extend(fast_ਸੂਚਕਾਂਕ, 0, 0, fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3_ਸਮਰੱਥਾ = 0;
    i3_ਪਹਿਲਾ_Null = index3_Null_ਵਿੱਥ;
    ਤਾਰਕਿਕ has_Long_I3_ਖੰਡਾਂ = ਝੂਠ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t i_ਸ਼ੁਰੂ = fastILimit < BMP_I_ਹੱਦਬੰਦੀ ? 0 : BMP_I_ਹੱਦਬੰਦੀ;
    int32_t i_ਹੱਦਬੰਦੀ = ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3;
    ਲਈ (int32_t i = i_ਸ਼ੁਰੂ; i < i_ਹੱਦਬੰਦੀ;) {
        int32_t j = i;
        int32_t j_ਹੱਦਬੰਦੀ = i + UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ;
        uint32_t oredI3 = 0;
        ਤਾਰਕਿਕ isNull = ਸੱਚ;
        ਕਰੋ {
            uint32_t i3 = ਸੂਚਕਾਂਕ[j];
            oredI3 |= i3;
            ਜੇ (i3 != static_cast<uint32_t>(ਅੰਕੜੇ_Null_ਵਿੱਥ)) {
                isNull = ਝੂਠ;
            }
        } ਜਦੋਂ (++j < j_ਹੱਦਬੰਦੀ);
        ਜੇ (isNull) {
            flags[i] = I3_NULL;
            ਜੇ (i3_ਪਹਿਲਾ_Null < 0) {
                ਜੇ (oredI3 <= 0xffff) {
                    index3_ਸਮਰੱਥਾ += UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ;
                } ਨਹੀਂਤਾਂ {
                    index3_ਸਮਰੱਥਾ += ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ;
                    has_Long_I3_ਖੰਡਾਂ = ਸੱਚ;
                }
                i3_ਪਹਿਲਾ_Null = 0;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ (oredI3 <= 0xffff) {
                int32_t n = mixed_ਖੰਡਾਂ.ਲੱਭੋ_ਖੰਡ(fast_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ, i);
                ਜੇ (n >= 0) {
                    flags[i] = I3_BMP;
                    ਸੂਚਕਾਂਕ[i] = n;
                } ਨਹੀਂਤਾਂ {
                    flags[i] = I3_16;
                    index3_ਸਮਰੱਥਾ += UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ;
                }
            } ਨਹੀਂਤਾਂ {
                flags[i] = I3_18;
                index3_ਸਮਰੱਥਾ += ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ;
                has_Long_I3_ਖੰਡਾਂ = ਸੱਚ;
            }
        }
        i = j;
    }

    int32_t index2_ਸਮਰੱਥਾ = (i_ਹੱਦਬੰਦੀ - i_ਸ਼ੁਰੂ) >> UCPTRIE_ਖਿਸਕਾਓ_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_ਲੰਮਾਈ = (index2_ਸਮਰੱਥਾ + UCPTRIE_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ) >> UCPTRIE_ਖਿਸਕਾਓ_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16_ਸਮਰੱਥਾ = fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ + index1_ਲੰਮਾਈ + index3_ਸਮਰੱਥਾ + index2_ਸਮਰੱਥਾ + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16_ਸਮਰੱਥਾ * 2));
    ਜੇ (index16 == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    uprv_memcpy(index16, fast_ਸੂਚਕਾਂਕ, fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2);

    ਜੇ (!mixed_ਖੰਡਾਂ.init(index16_ਸਮਰੱਥਾ, UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ)) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    Mixed_ਖੰਡਾਂ long_I3_ਖੰਡਾਂ;
    ਜੇ (has_Long_I3_ਖੰਡਾਂ) {
        ਜੇ (!long_I3_ਖੰਡਾਂ.init(index16_ਸਮਰੱਥਾ, ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ)) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_ਹੱਦਬੰਦੀ >> UCPTRIE_ਖਿਸਕਾਓ_2];  // index2Capacity
    int32_t i2_ਲੰਮਾਈ = 0;
    i3_ਪਹਿਲਾ_Null = index3_Null_ਵਿੱਥ;
    int32_t index3_ਸ਼ੁਰੂ = fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ + index1_ਲੰਮਾਈ;
    int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = index3_ਸ਼ੁਰੂ;
    ਲਈ (int32_t i = i_ਸ਼ੁਰੂ; i < i_ਹੱਦਬੰਦੀ; i += UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ) {
        int32_t i3;
        uint8_t f = flags[i];
        ਜੇ (f == I3_NULL && i3_ਪਹਿਲਾ_Null < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = ਅੰਕੜੇ_Null_ਵਿੱਥ <= 0xffff ? I3_16 : I3_18;
            i3_ਪਹਿਲਾ_Null = 0;
        }
        ਜੇ (f == I3_NULL) {
            i3 = index3_Null_ਵਿੱਥ;
        } ਨਹੀਂਤਾਂ ਜੇ (f == I3_BMP) {
            i3 = ਸੂਚਕਾਂਕ[i];
        } ਨਹੀਂਤਾਂ ਜੇ (f == I3_16) {
            int32_t n = mixed_ਖੰਡਾਂ.ਲੱਭੋ_ਖੰਡ(index16, ਸੂਚਕਾਂਕ, i);
            ਜੇ (n >= 0) {
                i3 = n;
            } ਨਹੀਂਤਾਂ {
                ਜੇ (ਸੂਚਕਾਂਕ_ਲੰਮਾਈ == index3_ਸ਼ੁਰੂ) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ਨਹੀਂਤਾਂ {
                    n = getOverlap(index16, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                                   ਸੂਚਕਾਂਕ, i, UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ);
                }
                i3 = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ - n;
                int32_t prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
                ਜਦੋਂ (n < UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ) {
                    index16[ਸੂਚਕਾਂਕ_ਲੰਮਾਈ++] = ਸੂਚਕਾਂਕ[i + n++];
                }
                mixed_ਖੰਡਾਂ.extend(index16, index3_ਸ਼ੁਰੂ, prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
                ਜੇ (has_Long_I3_ਖੰਡਾਂ) {
                    long_I3_ਖੰਡਾਂ.extend(index16, index3_ਸ਼ੁਰੂ, prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
                }
            }
        } ਨਹੀਂਤਾਂ {
            U_ASSERT(f == I3_18);
            U_ASSERT(has_Long_I3_ਖੰਡਾਂ);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t j_ਹੱਦਬੰਦੀ = i + UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ;
            int32_t k = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
            ਕਰੋ {
                ++k;
                uint32_t v = ਸੂਚਕਾਂਕ[j++];
                uint32_t ਉੱਤਲਾ_ਬਿੱਟ = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = ਸੂਚਕਾਂਕ[j++];
                ਉੱਤਲਾ_ਬਿੱਟ |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = ਉੱਤਲਾ_ਬਿੱਟ;
            } ਜਦੋਂ (j < j_ਹੱਦਬੰਦੀ);
            int32_t n = long_I3_ਖੰਡਾਂ.ਲੱਭੋ_ਖੰਡ(index16, index16, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
            ਜੇ (n >= 0) {
                i3 = n | 0x8000;
            } ਨਹੀਂਤਾਂ {
                ਜੇ (ਸੂਚਕਾਂਕ_ਲੰਮਾਈ == index3_ਸ਼ੁਰੂ) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ਨਹੀਂਤਾਂ {
                    n = getOverlap(index16, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                                   index16, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ);
                }
                i3 = (ਸੂਚਕਾਂਕ_ਲੰਮਾਈ - n) | 0x8000;
                int32_t prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
                ਜੇ (n > 0) {
                    int32_t ਸ਼ੁਰੂ = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
                    ਜਦੋਂ (n < ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ) {
                        index16[ਸੂਚਕਾਂਕ_ਲੰਮਾਈ++] = index16[ਸ਼ੁਰੂ + n++];
                    }
                } ਨਹੀਂਤਾਂ {
                    ਸੂਚਕਾਂਕ_ਲੰਮਾਈ += ਸੂਚਕਾਂਕ_3_18_ਦੋਆਧਾਰੀ_ਅੰਕ_ਖੰਡ_ਲੰਮਾਈ;
                }
                mixed_ਖੰਡਾਂ.extend(index16, index3_ਸ਼ੁਰੂ, prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
                ਜੇ (has_Long_I3_ਖੰਡਾਂ) {
                    long_I3_ਖੰਡਾਂ.extend(index16, index3_ਸ਼ੁਰੂ, prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
                }
            }
        }
        ਜੇ (index3_Null_ਵਿੱਥ < 0 && i3_ਪਹਿਲਾ_Null >= 0) {
            index3_Null_ਵਿੱਥ = i3;
        }
        // Set the index-2 table entry.
        index2[i2_ਲੰਮਾਈ++] = i3;
    }
    U_ASSERT(i2_ਲੰਮਾਈ == index2_ਸਮਰੱਥਾ);
    U_ASSERT(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ <= index3_ਸ਼ੁਰੂ + index3_ਸਮਰੱਥਾ);

    ਜੇ (index3_Null_ਵਿੱਥ < 0) {
        index3_Null_ਵਿੱਥ = UCPTRIE_NO_INDEX3_NULL_ਵਿੱਥ;
    }
    ਜੇ (ਸੂਚਕਾਂਕ_ਲੰਮਾਈ >= (UCPTRIE_NO_INDEX3_NULL_ਵਿੱਥ + UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ == UCPTRIE_ਸੂਚਕਾਂਕ_3_ਖੰਡ_ਲੰਮਾਈ,
                  "must re-init mixedBlocks");
    int32_t ਖੰਡ_ਲੰਮਾਈ = UCPTRIE_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ;
    int32_t i1 = fast_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਲਈ (int32_t i = 0; i < i2_ਲੰਮਾਈ; i += ਖੰਡ_ਲੰਮਾਈ) {
        int32_t n;
        ਜੇ ((i2_ਲੰਮਾਈ - i) >= ਖੰਡ_ਲੰਮਾਈ) {
            // normal block
            U_ASSERT(ਖੰਡ_ਲੰਮਾਈ == UCPTRIE_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ);
            n = mixed_ਖੰਡਾਂ.ਲੱਭੋ_ਖੰਡ(index16, index2, i);
        } ਨਹੀਂਤਾਂ {
            // highStart is inside the last index-2 block. Shorten it.
            ਖੰਡ_ਲੰਮਾਈ = i2_ਲੰਮਾਈ - i;
            n = ਲੱਭੋ_Same_ਖੰਡ(index16, index3_ਸ਼ੁਰੂ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                              index2, i, ਖੰਡ_ਲੰਮਾਈ);
        }
        int32_t i2;
        ਜੇ (n >= 0) {
            i2 = n;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਸੂਚਕਾਂਕ_ਲੰਮਾਈ == index3_ਸ਼ੁਰੂ) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } ਨਹੀਂਤਾਂ {
                n = getOverlap(index16, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, index2, i, ਖੰਡ_ਲੰਮਾਈ);
            }
            i2 = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ - n;
            int32_t prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
            ਜਦੋਂ (n < ਖੰਡ_ਲੰਮਾਈ) {
                index16[ਸੂਚਕਾਂਕ_ਲੰਮਾਈ++] = index2[i + n++];
            }
            mixed_ਖੰਡਾਂ.extend(index16, index3_ਸ਼ੁਰੂ, prev_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3_ਸ਼ੁਰੂ);
    U_ASSERT(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ <= index16_ਸਮਰੱਥਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    /* we saved some space */
    ਲਿਖੋ("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (ਲੰਮਾ)i_ਹੱਦਬੰਦੀ, (ਲੰਮਾ)ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
}

int32_t Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::compactTrie(int32_t fastILimit, UError_ਕੋਡ &error_ਕੋਡ) {
    // Find the real highStart and round it up.
    U_ASSERT((ਉੱਚਾ_ਸ਼ੁਰੂ & (UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ - 1)) == 0);
    ਉੱਚਾ_ਮੁੱਲ = get(MAX_UNICODE);
    int32_t real_ਉੱਚਾ_ਸ਼ੁਰੂ = ਲੱਭੋ_ਉੱਚਾ_ਸ਼ੁਰੂ();
    real_ਉੱਚਾ_ਸ਼ੁਰੂ = (real_ਉੱਚਾ_ਸ਼ੁਰੂ + (UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ - 1)) &
        ~(UCPTRIE_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_2_ਇੰਦਰਾਜ - 1);
    ਜੇ (real_ਉੱਚਾ_ਸ਼ੁਰੂ == UNICODE_ਹੱਦਬੰਦੀ) {
        ਉੱਚਾ_ਮੁੱਲ = initial_ਮੁੱਲ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    ਲਿਖੋ("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (ਲੰਮਾ)real_ਉੱਚਾ_ਸ਼ੁਰੂ, (ਲੰਮਾ)ਉੱਚਾ_ਮੁੱਲ, (ਲੰਮਾ)initial_ਮੁੱਲ);
#ਜੇ_ਅੰਤ

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fast_ਹੱਦਬੰਦੀ = fastILimit << UCPTRIE_ਖਿਸਕਾਓ_3;
    ਜੇ (real_ਉੱਚਾ_ਸ਼ੁਰੂ < fast_ਹੱਦਬੰਦੀ) {
        ਲਈ (int32_t i = (real_ਉੱਚਾ_ਸ਼ੁਰੂ >> UCPTRIE_ਖਿਸਕਾਓ_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            ਸੂਚਕਾਂਕ[i] = ਉੱਚਾ_ਮੁੱਲ;
        }
        ਉੱਚਾ_ਸ਼ੁਰੂ = fast_ਹੱਦਬੰਦੀ;
    } ਨਹੀਂਤਾਂ {
        ਉੱਚਾ_ਸ਼ੁਰੂ = real_ਉੱਚਾ_ਸ਼ੁਰੂ;
    }

    uint32_t ascii_ਅੰਕੜੇ[ASCII_ਹੱਦਬੰਦੀ];
    ਲਈ (int32_t i = 0; i < ASCII_ਹੱਦਬੰਦੀ; ++i) {
        ascii_ਅੰਕੜੇ[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    All_Same_ਖੰਡਾਂ all_Same_ਖੰਡਾਂ;
    int32_t new_ਅੰਕੜੇ_ਸਮਰੱਥਾ = compact_ਸਾਰਾ_ਅੰਕੜੇ_ਖੰਡਾਂ(fastILimit, all_Same_ਖੰਡਾਂ);
    ਜੇ (new_ਅੰਕੜੇ_ਸਮਰੱਥਾ < 0) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    uint32_t* new_ਅੰਕੜੇ = static_cast<uint32_t*>(uprv_malloc(new_ਅੰਕੜੇ_ਸਮਰੱਥਾ * 4));
    ਜੇ (new_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    uprv_memcpy(new_ਅੰਕੜੇ, ascii_ਅੰਕੜੇ, ਆਕਾਰ(ascii_ਅੰਕੜੇ));

    int32_t ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ = all_Same_ਖੰਡਾਂ.ਲੱਭੋ_Most_ਵਰਤਿਆ();

    Mixed_ਖੰਡਾਂ mixed_ਖੰਡਾਂ;
    int32_t new_ਅੰਕੜੇ_ਲੰਮਾਈ = compact_ਅੰਕੜੇ(fastILimit, new_ਅੰਕੜੇ, new_ਅੰਕੜੇ_ਸਮਰੱਥਾ,
                                        ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ, mixed_ਖੰਡਾਂ, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0; }
    U_ASSERT(new_ਅੰਕੜੇ_ਲੰਮਾਈ <= new_ਅੰਕੜੇ_ਸਮਰੱਥਾ);
    uprv_free(ਅੰਕੜੇ);
    ਅੰਕੜੇ = new_ਅੰਕੜੇ;
    ਅੰਕੜੇ_ਸਮਰੱਥਾ = new_ਅੰਕੜੇ_ਸਮਰੱਥਾ;
    ਅੰਕੜੇ_ਲੰਮਾਈ = new_ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਜੇ (ਅੰਕੜੇ_ਲੰਮਾਈ > (0x3ffff + UCPTRIE_SMALL_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ)) {
        // The offset of the last data block is too high to be stored in the index table.
        error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ (ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ >= 0) {
        ਅੰਕੜੇ_Null_ਵਿੱਥ = ਸੂਚਕਾਂਕ[ਅੰਕੜੇ_Null_ਸੂਚਕਾਂਕ];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
        ਜੇ (ਅੰਕੜੇ[ਅੰਕੜੇ_Null_ਵਿੱਥ] != initial_ਮੁੱਲ) {
            ਲਿਖੋ("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (ਲੰਮਾ)initial_ਮੁੱਲ, (ਲੰਮਾ)ਅੰਕੜੇ[ਅੰਕੜੇ_Null_ਵਿੱਥ]);
        }
#ਜੇ_ਅੰਤ
        initial_ਮੁੱਲ = ਅੰਕੜੇ[ਅੰਕੜੇ_Null_ਵਿੱਥ];
    } ਨਹੀਂਤਾਂ {
        ਅੰਕੜੇ_Null_ਵਿੱਥ = UCPTRIE_NO_ਅੰਕੜੇ_NULL_ਵਿੱਥ;
    }

    int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = compact_ਸੂਚਕਾਂਕ(fastILimit, mixed_ਖੰਡਾਂ, error_ਕੋਡ);
    ਉੱਚਾ_ਸ਼ੁਰੂ = real_ਉੱਚਾ_ਸ਼ੁਰੂ;
    ਵਾਪਸ ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
}

UCPTrie *Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::build(UCPTrieType type, UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ਮੁੱਲ_ਚੌੜਾਈ < UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16 || UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 < ਮੁੱਲ_ਚੌੜਾਈ) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    ਚੋਣ (ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        ਨਕਾਬ_ਮੁੱਲਾਂ(0xffff);
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        ਨਕਾਬ_ਮੁੱਲਾਂ(0xff);
        ਰੋਕੋ;
    ਮੂਲ:
        ਰੋਕੋ;
    }

    UChar32 fast_ਹੱਦਬੰਦੀ = type == UCPTRIE_TYPE_FAST ? BMP_ਹੱਦਬੰਦੀ : UCPTRIE_SMALL_ਹੱਦਬੰਦੀ;
    int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = compactTrie(fast_ਹੱਦਬੰਦੀ >> UCPTRIE_ਖਿਸਕਾਓ_3, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        clear();
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    ਜੇ (ਮੁੱਲ_ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32 && (ਸੂਚਕਾਂਕ_ਲੰਮਾਈ & 1) != 0) {
        index16[ਸੂਚਕਾਂਕ_ਲੰਮਾਈ++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2;
    ਜੇ (ਮੁੱਲ_ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16) {
        ਜੇ (((ਸੂਚਕਾਂਕ_ਲੰਮਾਈ ^ ਅੰਕੜੇ_ਲੰਮਾਈ) & 1) != 0) {
            // padding
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = error_ਮੁੱਲ;
        }
        ਜੇ (ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 1] != error_ਮੁੱਲ || ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 2] != ਉੱਚਾ_ਮੁੱਲ) {
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = ਉੱਚਾ_ਮੁੱਲ;
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = error_ਮੁੱਲ;
        }
        length += ਅੰਕੜੇ_ਲੰਮਾਈ * 2;
    } ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ_ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        ਜੇ (ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 1] != error_ਮੁੱਲ || ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 2] != ਉੱਚਾ_ਮੁੱਲ) {
            ਜੇ (ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 1] != ਉੱਚਾ_ਮੁੱਲ) {
                ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = ਉੱਚਾ_ਮੁੱਲ;
            }
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = error_ਮੁੱਲ;
        }
        length += ਅੰਕੜੇ_ਲੰਮਾਈ * 4;
    } ਨਹੀਂਤਾਂ {
        int32_t and3 = (length + ਅੰਕੜੇ_ਲੰਮਾਈ) & 3;
        ਜੇ (and3 == 0 && ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 1] == error_ਮੁੱਲ && ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 2] == ਉੱਚਾ_ਮੁੱਲ) {
            // all set
        } ਨਹੀਂਤਾਂ ਜੇ(and3 == 3 && ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ - 1] == ਉੱਚਾ_ਮੁੱਲ) {
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = error_ਮੁੱਲ;
        } ਨਹੀਂਤਾਂ {
            ਜਦੋਂ (and3 != 2) {
                ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = ਉੱਚਾ_ਮੁੱਲ;
                and3 = (and3 + 1) & 3;
            }
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = ਉੱਚਾ_ਮੁੱਲ;
            ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ++] = error_ਮੁੱਲ;
        }
        length += ਅੰਕੜੇ_ਲੰਮਾਈ;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += ਆਕਾਰ(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* ਬਾਈਟਾਂ = static_cast<uint8_t*>(uprv_malloc(length));
    ਜੇ (ਬਾਈਟਾਂ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        clear();
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(ਬਾਈਟਾਂ);
    uprv_memset(trie, 0, ਆਕਾਰ(UCPTrie));
    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ = ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ = ਅੰਕੜੇ_ਲੰਮਾਈ;

    trie->ਉੱਚਾ_ਸ਼ੁਰੂ = ਉੱਚਾ_ਸ਼ੁਰੂ;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ਉੱਚਾ_ਸ਼ੁਰੂ = (ਉੱਚਾ_ਸ਼ੁਰੂ + 0xfff) >> 12;
    trie->type = type;
    trie->ਮੁੱਲ_ਚੌੜਾਈ = ਮੁੱਲ_ਚੌੜਾਈ;

    trie->index3_Null_ਵਿੱਥ = index3_Null_ਵਿੱਥ;
    trie->ਅੰਕੜੇ_Null_ਵਿੱਥ = ਅੰਕੜੇ_Null_ਵਿੱਥ;
    trie->null_ਮੁੱਲ = initial_ਮੁੱਲ;

    ਬਾਈਟਾਂ += ਆਕਾਰ(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(ਬਾਈਟਾਂ);
    trie->ਸੂਚਕਾਂਕ = dest16;

    ਜੇ (ਉੱਚਾ_ਸ਼ੁਰੂ <= fast_ਹੱਦਬੰਦੀ) {
        // Condense only the fast index from the mutable-trie index.
        ਲਈ (int32_t i = 0, j = 0; j < ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; i += SMALL_ਅੰਕੜੇ_ਖੰਡਾਂ_ਪ੍ਰਤੀ_BMP_ਖੰਡ, ++j) {
            *dest16++ = static_cast<uint16_t>(ਸੂਚਕਾਂਕ[i]); // dest16[j]
        }
    } ਨਹੀਂਤਾਂ {
        uprv_memcpy(dest16, index16, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2);
        dest16 += ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    }
    ਬਾਈਟਾਂ += ਸੂਚਕਾਂਕ_ਲੰਮਾਈ * 2;

    // Write the data array.
    ਅਚਲ uint32_t *p = ਅੰਕੜੇ;
    ਚੋਣ (ਮੁੱਲ_ਚੌੜਾਈ) {
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16:
        // Write 16-bit data values.
        trie->ਅੰਕੜੇ.ptr16 = dest16;
        ਲਈ (int32_t i = ਅੰਕੜੇ_ਲੰਮਾਈ; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32:
        // Write 32-bit data values.
        trie->ਅੰਕੜੇ.ptr32 = reinterpret_cast<uint32_t*>(ਬਾਈਟਾਂ);
        uprv_memcpy(ਬਾਈਟਾਂ, p, (size_t)ਅੰਕੜੇ_ਲੰਮਾਈ * 4);
        ਰੋਕੋ;
    ਹਾਲਤ UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8:
        // Write 8-bit data values.
        trie->ਅੰਕੜੇ.ptr8 = ਬਾਈਟਾਂ;
        ਲਈ (int32_t i = ਅੰਕੜੇ_ਲੰਮਾਈ; i > 0; --i) {
            *ਬਾਈਟਾਂ++ = static_cast<uint8_t>(*p++);
        }
        ਰੋਕੋ;
    ਮੂਲ:
        // Will not occur, valueWidth checked at the beginning.
        ਰੋਕੋ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#ਜੇ_ਅੰਤ

    clear();
    ਵਾਪਸ trie;
}

}  // namespace

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_ਮੁੱਲ, uint32_t error_ਮੁੱਲ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Local_ਸੂਚਕ<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie> trie(
        ਨਵਾਂ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(initial_ਮੁੱਲ, error_ਮੁੱਲ, *p_Error_ਕੋਡ), *p_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ਅਚਲ UMutableCPTrie *other, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (other == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Local_ਸੂਚਕ<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie> clone(
        ਨਵਾਂ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie(*reinterpret_cast<ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(other), *p_Error_ਕੋਡ), *p_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI ਖਾਲੀ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    ਮਿਟਾਓ reinterpret_cast<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ਅਚਲ UCPMap *map, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (map == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<UMutableCPTrie *>(Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::fromUCPMap(map, *p_Error_ਕੋਡ));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ਅਚਲ UCPTrie *trie, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (trie == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<UMutableCPTrie *>(Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie::fromUCPTrie(trie, *p_Error_ਕੋਡ));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ਅਚਲ UMutableCPTrie *trie, UChar32 c) {
    ਵਾਪਸ reinterpret_cast<ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie)->get(c);
}

ਨਾਮਾਕਾਸ਼ {

UChar32 getRange(ਅਚਲ ਖਾਲੀ *trie, UChar32 ਸ਼ੁਰੂ,
                 UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context, uint32_t *p_ਮੁੱਲ) {
    ਵਾਪਸ reinterpret_cast<ਅਚਲ Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie)->
        getRange(ਸ਼ੁਰੂ, filter, context, p_ਮੁੱਲ);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ਅਚਲ UMutableCPTrie *trie, UChar32 ਸ਼ੁਰੂ,
                        UCPMapRangeOption option, uint32_t surrogate_ਮੁੱਲ,
                        UCPMap_ਮੁੱਲ_Filter *filter, ਅਚਲ ਖਾਲੀ *context, uint32_t *p_ਮੁੱਲ) {
    ਵਾਪਸ ucptrie_ਅੰਦਰੂਨੀ_Get_Range(getRange, trie, ਸ਼ੁਰੂ,
                                    option, surrogate_ਮੁੱਲ,
                                    filter, context, p_ਮੁੱਲ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
umutablecptrie_ਸੈੱਟ(UMutableCPTrie *trie, UChar32 c, uint32_t value, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    reinterpret_cast<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie)->ਸੈੱਟ(c, value, *p_Error_ਕੋਡ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
umutablecptrie_ਸੈੱਟ_Range(UMutableCPTrie *trie, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
                   uint32_t value, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    reinterpret_cast<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie)->ਸੈੱਟ_Range(ਸ਼ੁਰੂ, ਸਿਰਾ, value, *p_Error_ਕੋਡ);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ,
                              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie)->build(type, ਮੁੱਲ_ਚੌੜਾਈ, *p_Error_ਕੋਡ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
U_CFUNC ਖਾਲੀ umutablecptrie_ਸੈੱਟ_Name(UMutableCPTrie *trie, ਅਚਲ ਅੱਖਰ *name) {
    reinterpret_cast<Mutable_ਕੋਡ_ਬਿੰਦੂ_Trie *>(trie)->name = name;
}
#ਜੇ_ਅੰਤ
