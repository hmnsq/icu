// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ifdef OVERFLOW
#undef OVERFLOW
#endif

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_LIMIT = 0x110000;
constexpr int32_t BMP_LIMIT = 0x10000;
constexpr int32_t ASCII_LIMIT = 0x80;

constexpr int32_t I_LIMIT = UNICODE_LIMIT >> UCPTRIE_SHIFT_3;
constexpr int32_t BMP_I_LIMIT = BMP_LIMIT >> UCPTRIE_SHIFT_3;
constexpr int32_t ASCII_I_LIMIT = ASCII_LIMIT >> UCPTRIE_SHIFT_3;

constexpr int32_t SMALL_DATA_BLOCKS_PER_BMP_BLOCK = (1 << (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_DATA_LENGTH = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_DATA_LENGTH = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_DATA_LENGTH = UNICODE_LIMIT;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t INDEX_3_18BIT_BLOCK_LENGTH = UCPTRIE_INDEX_3_BLOCK_LENGTH + UCPTRIE_INDEX_3_BLOCK_LENGTH / 8;

ವರ್ಗ AllSameBlocks;
ವರ್ಗ MixedBlocks;

ವರ್ಗ MutableCodePointTrie : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    MutableCodePointTrie(uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ, UErrorCode &errorCode);
    MutableCodePointTrie(ಧ್ರುವ MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(ಧ್ರುವ MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(ಧ್ರುವ MutableCodePointTrie &other) = delete;

    ಸ್ಥಿರ MutableCodePointTrie *fromUCPMap(ಧ್ರುವ UCPMap *map, UErrorCode &errorCode);
    ಸ್ಥಿರ MutableCodePointTrie *fromUCPTrie(ಧ್ರುವ UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) ಧ್ರುವ;
    int32_t getRange(UChar32 start, UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context,
                     uint32_t *p_ಮೌಲ್ಯ) ಧ್ರುವ;

    ಶೂನ್ಯ set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    ಶೂನ್ಯ setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width, UErrorCode &errorCode);

ಗುಪ್ತ:
    ಶೂನ್ಯ clear();

    ತಾರ್ಕಿಕ ensure_ಮೇಲಿನ_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    ಶೂನ್ಯ mask_ಮೌಲ್ಯಗಳು(uint32_t mask);
    UChar32 find_ಮೇಲಿನ_Start() ಧ್ರುವ;
    int32_t compactWholeDataBlocks(int32_t fastILimit, AllSameBlocks &allSameBlocks);
    int32_t compactData(
            int32_t fastILimit, uint32_t *newData, int32_t newDataCapacity,
            int32_t dataNullIndex, MixedBlocks &mixedBlocks, UErrorCode &errorCode);
    int32_t compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks, UErrorCode &errorCode);
    int32_t compactTrie(int32_t fastILimit, UErrorCode &errorCode);

    uint32_t *index = nullptr;
    int32_t indexCapacity = 0;
    int32_t index3NullOffset = -1;
    uint32_t *data = nullptr;
    int32_t dataCapacity = 0;
    int32_t dataLength = 0;
    int32_t dataNullOffset = -1;

    uint32_t orig_Initial_ಮೌಲ್ಯ;
    uint32_t initial_ಮೌಲ್ಯ;
    uint32_t error_ಮೌಲ್ಯ;
    UChar32 ಮೇಲಿನ_Start;
    uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ;
#ifdef UCPTRIE_DEBUG
ಸಾರ್ವಜನಿಕ:
    ಧ್ರುವ ವರ್ಣ *name;
#endif
ಗುಪ್ತ:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_ಮೌಲ್ಯ, uint32_t err_ಮೌಲ್ಯ, UErrorCode &errorCode) :
        orig_Initial_ಮೌಲ್ಯ(ini_ಮೌಲ್ಯ), initial_ಮೌಲ್ಯ(ini_ಮೌಲ್ಯ), error_ಮೌಲ್ಯ(err_ಮೌಲ್ಯ),
        ಮೇಲಿನ_Start(0), ಮೇಲಿನ_ಮೌಲ್ಯ(initial_ಮೌಲ್ಯ)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    ಯದಿ (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(ಧ್ರುವ MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_ಮೌಲ್ಯ(other.orig_Initial_ಮೌಲ್ಯ), initial_ಮೌಲ್ಯ(other.initial_ಮೌಲ್ಯ),
        error_ಮೌಲ್ಯ(other.error_ಮೌಲ್ಯ),
        ಮೇಲಿನ_Start(other.ಮೇಲಿನ_Start), ಮೇಲಿನ_ಮೌಲ್ಯ(other.ಮೇಲಿನ_ಮೌಲ್ಯ)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    int32_t iCapacity = ಮೇಲಿನ_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    ಯದಿ (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
    uprv_memcpy(flags, other.flags, iLimit);
    uprv_memcpy(index, other.index, iLimit * 4);
    uprv_memcpy(data, other.data, (size_t)other.dataLength * 4);
    dataLength = other.dataLength;
    U_ASSERT(other.index16 == nullptr);
}

MutableCodePointTrie::~MutableCodePointTrie() {
    uprv_free(index);
    uprv_free(data);
    uprv_free(index16);
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(ಧ್ರುವ UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ಮೌಲ್ಯ = ucpmap_get(map, -1);
    uint32_t initial_ಮೌಲ್ಯ = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        ನೂತನ MutableCodePointTrie(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, errorCode),
        errorCode);
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    ಯಾವತ್ ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        ಯದಿ (value != initial_ಮೌಲ್ಯ) {
            ಯದಿ (start == end) {
                mutableTrie->set(start, value, errorCode);
            } ಅನ್ಯಥಾ {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    ಯದಿ (U_SUCCESS(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ mutableTrie.orphan();
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(ಧ್ರುವ UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ಮೌಲ್ಯ;
    uint32_t initial_ಮೌಲ್ಯ;
    ವಿಕಲ್ಪ (trie->ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        error_ಮೌಲ್ಯ = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_DATA_OFFSET];
        initial_ಮೌಲ್ಯ = trie->data.ptr16[trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET];
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        error_ಮೌಲ್ಯ = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_DATA_OFFSET];
        initial_ಮೌಲ್ಯ = trie->data.ptr32[trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET];
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        error_ಮೌಲ್ಯ = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_ಮೌಲ್ಯ_NEG_DATA_OFFSET];
        initial_ಮೌಲ್ಯ = trie->data.ptr8[trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET];
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        ನೂತನ MutableCodePointTrie(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, errorCode),
        errorCode);
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    ಯಾವತ್ ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        ಯದಿ (value != initial_ಮೌಲ್ಯ) {
            ಯದಿ (start == end) {
                mutableTrie->set(start, value, errorCode);
            } ಅನ್ಯಥಾ {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    ಯದಿ (U_SUCCESS(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ mutableTrie.orphan();
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

ಶೂನ್ಯ MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    ಮೇಲಿನ_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ = orig_Initial_ಮೌಲ್ಯ;
    ಮೇಲಿನ_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) ಧ್ರುವ {
    ಯದಿ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        ಪ್ರತ್ಯಾಗಮ error_ಮೌಲ್ಯ;
    }
    ಯದಿ (c >= ಮೇಲಿನ_Start) {
        ಪ್ರತ್ಯಾಗಮ ಮೇಲಿನ_ಮೌಲ್ಯ;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    ಯದಿ (flags[i] == ALL_SAME) {
        ಪ್ರತ್ಯಾಗಮ index[i];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

ಅನ್ತಃಸ್ಥ uint32_t maybe_Filter_ಮೌಲ್ಯ(uint32_t value, uint32_t initial_ಮೌಲ್ಯ, uint32_t null_ಮೌಲ್ಯ,
                                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context) {
    ಯದಿ (value == initial_ಮೌಲ್ಯ) {
        value = null_ಮೌಲ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ (filter != nullptr) {
        value = filter(context, value);
    }
    ಪ್ರತ್ಯಾಗಮ value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context,
        uint32_t *p_ಮೌಲ್ಯ) ಧ್ರುವ {
    ಯದಿ (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
    ಯದಿ (start >= ಮೇಲಿನ_Start) {
        ಯದಿ (p_ಮೌಲ್ಯ != nullptr) {
            uint32_t value = ಮೇಲಿನ_ಮೌಲ್ಯ;
            ಯದಿ (filter != nullptr) { value = filter(context, value); }
            *p_ಮೌಲ್ಯ = value;
        }
        ಪ್ರತ್ಯಾಗಮ MAX_UNICODE;
    }
    uint32_t null_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ;
    ಯದಿ (filter != nullptr) { null_ಮೌಲ್ಯ = filter(context, null_ಮೌಲ್ಯ); }
    UChar32 c = start;
    uint32_t trie_ಮೌಲ್ಯ, value;
    ತಾರ್ಕಿಕ have_ಮೌಲ್ಯ = ಅಸತ್ಯ;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    ಕುರು {
        ಯದಿ (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            ಯದಿ (have_ಮೌಲ್ಯ) {
                ಯದಿ (trieValue2 != trie_ಮೌಲ್ಯ) {
                    ಯದಿ (filter == nullptr ||
                            maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                             filter, context) != value) {
                        ಪ್ರತ್ಯಾಗಮ c - 1;
                    }
                    trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                }
            } ಅನ್ಯಥಾ {
                trie_ಮೌಲ್ಯ = trieValue2;
                value = maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ, filter, context);
                ಯದಿ (p_ಮೌಲ್ಯ != nullptr) { *p_ಮೌಲ್ಯ = value; }
                have_ಮೌಲ್ಯ = ಸತ್ಯ;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } ಅನ್ಯಥಾ /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            ಯದಿ (have_ಮೌಲ್ಯ) {
                ಯದಿ (trieValue2 != trie_ಮೌಲ್ಯ) {
                    ಯದಿ (filter == nullptr ||
                            maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                             filter, context) != value) {
                        ಪ್ರತ್ಯಾಗಮ c - 1;
                    }
                    trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                }
            } ಅನ್ಯಥಾ {
                trie_ಮೌಲ್ಯ = trieValue2;
                value = maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ, filter, context);
                ಯದಿ (p_ಮೌಲ್ಯ != nullptr) { *p_ಮೌಲ್ಯ = value; }
                have_ಮೌಲ್ಯ = ಸತ್ಯ;
            }
            ಯಾವತ್ ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                ಯದಿ (trieValue2 != trie_ಮೌಲ್ಯ) {
                    ಯದಿ (filter == nullptr ||
                            maybe_Filter_ಮೌಲ್ಯ(trieValue2, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                             filter, context) != value) {
                        ಪ್ರತ್ಯಾಗಮ c - 1;
                    }
                }
                trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
            }
        }
        ++i;
    } ಯಾವತ್ (c < ಮೇಲಿನ_Start);
    U_ASSERT(have_ಮೌಲ್ಯ);
    ಯದಿ (maybe_Filter_ಮೌಲ್ಯ(ಮೇಲಿನ_ಮೌಲ್ಯ, initial_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                         filter, context) != value) {
        ಪ್ರತ್ಯಾಗಮ c - 1;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ MAX_UNICODE;
    }
}

ಶೂನ್ಯ
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    ಯಾವತ್ (block < limit) {
        *block++ = value;
    }
}

ತಾರ್ಕಿಕ MutableCodePointTrie::ensure_ಮೇಲಿನ_Start(UChar32 c) {
    ಯದಿ (c >= ಮೇಲಿನ_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        ಯದಿ (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            ಯದಿ (newIndex == nullptr) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        ಕುರು {
            flags[i] = ALL_SAME;
            index[i] = initial_ಮೌಲ್ಯ;
        } ಯಾವತ್(++i < iLimit);
        ಮೇಲಿನ_Start = c;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

int32_t MutableCodePointTrie::allocDataBlock(int32_t blockLength) {
    int32_t newBlock = dataLength;
    int32_t new_ತುದಿ = newBlock + blockLength;
    ಯದಿ (new_ತುದಿ > dataCapacity) {
        int32_t capacity;
        ಯದಿ (dataCapacity < MEDIUM_DATA_LENGTH) {
            capacity = MEDIUM_DATA_LENGTH;
        } ಅನ್ಯಥಾ ಯದಿ (dataCapacity < MAX_DATA_LENGTH) {
            capacity = MAX_DATA_LENGTH;
        } ಅನ್ಯಥಾ {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        ಯದಿ (newData == nullptr) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        uprv_memcpy(newData, data, (size_t)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_ತುದಿ;
    ಪ್ರತ್ಯಾಗಮ newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    ಯದಿ (flags[i] == MIXED) {
        ಪ್ರತ್ಯಾಗಮ index[i];
    }
    ಯದಿ (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        ಯದಿ (newBlock < 0) { ಪ್ರತ್ಯಾಗಮ newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        ಕುರು {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } ಯಾವತ್ (iStart < iLimit);
        ಪ್ರತ್ಯಾಗಮ index[i];
    } ಅನ್ಯಥಾ {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        ಯದಿ (newBlock < 0) { ಪ್ರತ್ಯಾಗಮ newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        ಪ್ರತ್ಯಾಗಮ newBlock;
    }
}

ಶೂನ್ಯ MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t block;
    ಯದಿ (!ensure_ಮೇಲಿನ_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

ಶೂನ್ಯ
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    ಯಾವತ್ (block < pLimit) {
        *block++ = value;
    }
}

ಶೂನ್ಯ MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!ensure_ಮೇಲಿನ_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    UChar32 limit = end + 1;
    ಯದಿ (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        ಯದಿ (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        ಯದಿ (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } ಅನ್ಯಥಾ {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            ಪ್ರತ್ಯಾಗಮ;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    ಯಾವತ್ (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        ಯದಿ (flags[i] == ALL_SAME) {
            index[i] = value;
        } ಅನ್ಯಥಾ /* MIXED */ {
            fillBlock(data + index[i], 0, UCPTRIE_SMALL_DATA_BLOCK_LENGTH, value);
        }
        start += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    }

    ಯದಿ (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        ಯದಿ (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

ಶೂನ್ಯ MutableCodePointTrie::mask_ಮೌಲ್ಯಗಳು(uint32_t mask) {
    initial_ಮೌಲ್ಯ &= mask;
    error_ಮೌಲ್ಯ &= mask;
    ಮೇಲಿನ_ಮೌಲ್ಯ &= mask;
    int32_t iLimit = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
    ಪ್ರತಿ (int32_t i = 0; i < iLimit; ++i) {
        ಯದಿ (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    ಪ್ರತಿ (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
ತಾರ್ಕಿಕ equalBlocks(ಧ್ರುವ UIntA *s, ಧ್ರುವ UIntB *t, int32_t length) {
    ಯಾವತ್ (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    ಪ್ರತ್ಯಾಗಮ length == 0;
}

ತಾರ್ಕಿಕ all_ಮೌಲ್ಯಗಳು_Same_As(ಧ್ರುವ uint32_t *p, int32_t length, uint32_t value) {
    ಧ್ರುವ uint32_t *pLimit = p + length;
    ಯಾವತ್ (p < pLimit && *p == value) { ++p; }
    ಪ್ರತ್ಯಾಗಮ p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(ಧ್ರುವ uint16_t *p, int32_t pStart, int32_t length,
                      ಧ್ರುವ uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    ಯಾವತ್ (pStart <= length) {
        ಯದಿ (equalBlocks(p + pStart, q, blockLength)) {
            ಪ್ರತ್ಯಾಗಮ pStart;
        }
        ++pStart;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

int32_t findAllSameBlock(ಧ್ರುವ uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    ಪ್ರತಿ (int32_t block = start; block <= limit; ++block) {
        ಯದಿ (p[block] == value) {
            ಪ್ರತಿ (int32_t i = 1;; ++i) {
                ಯದಿ (i == blockLength) {
                    ಪ್ರತ್ಯಾಗಮ block;
                }
                ಯದಿ (p[block + i] != value) {
                    block += i;
                    ಭಙ್ಗ;
                }
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
template<typename UIntA, typename UIntB>
int32_t getOverlap(ಧ್ರುವ UIntA *p, int32_t length,
                   ಧ್ರುವ UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    ಯಾವತ್ (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    ಪ್ರತ್ಯಾಗಮ overlap;
}

int32_t getAllSameOverlap(ಧ್ರುವ uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    ಯಾವತ್ (min < i && p[i - 1] == value) { --i; }
    ಪ್ರತ್ಯಾಗಮ length - i;
}

ತಾರ್ಕಿಕ isStartOfSomeFastBlock(uint32_t dataOffset, ಧ್ರುವ uint32_t index[], int32_t fastILimit) {
    ಪ್ರತಿ (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        ಯದಿ (index[i] == dataOffset) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_ಮೇಲಿನ_Start() ಧ್ರುವ {
    int32_t i = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
    ಯಾವತ್ (i > 0) {
        ತಾರ್ಕಿಕ match;
        ಯದಿ (flags[--i] == ALL_SAME) {
            match = index[i] == ಮೇಲಿನ_ಮೌಲ್ಯ;
        } ಅನ್ಯಥಾ /* MIXED */ {
            ಧ್ರುವ uint32_t *p = data + index[i];
            ಪ್ರತಿ (int32_t j = 0;; ++j) {
                ಯದಿ (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = ಸತ್ಯ;
                    ಭಙ್ಗ;
                }
                ಯದಿ (p[j] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
                    match = ಅಸತ್ಯ;
                    ಭಙ್ಗ;
                }
            }
        }
        ಯದಿ (!match) {
            ಪ್ರತ್ಯಾಗಮ (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಗ AllSameBlocks {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ constexpr int32_t NEW_UNIQUE = -1;
    ಸ್ಥಿರ constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        ಯದಿ (mostRecent >= 0 && ಮೌಲ್ಯಗಳು[mostRecent] == value) {
            refCounts[mostRecent] += count;
            ಪ್ರತ್ಯಾಗಮ indexes[mostRecent];
        }
        ಪ್ರತಿ (int32_t i = 0; i < length; ++i) {
            ಯದಿ (ಮೌಲ್ಯಗಳು[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                ಪ್ರತ್ಯಾಗಮ indexes[i];
            }
        }
        ಯದಿ (length == CAPACITY) {
            ಪ್ರತ್ಯಾಗಮ OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        ಮೌಲ್ಯಗಳು[length] = value;
        refCounts[length++] = count;
        ಪ್ರತ್ಯಾಗಮ NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    ಶೂನ್ಯ add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_ಎಣಿಕೆ = I_LIMIT;
        ಪ್ರತಿ (int32_t i = 0; i < length; ++i) {
            U_ASSERT(ಮೌಲ್ಯಗಳು[i] != value);
            ಯದಿ (refCounts[i] < least_ಎಣಿಕೆ) {
                least = i;
                least_ಎಣಿಕೆ = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        ಮೌಲ್ಯಗಳು[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() ಧ್ರುವ {
        ಯದಿ (length == 0) { ಪ್ರತ್ಯಾಗಮ -1; }
        int32_t max = -1;
        int32_t max_ಎಣಿಕೆ = 0;
        ಪ್ರತಿ (int32_t i = 0; i < length; ++i) {
            ಯದಿ (refCounts[i] > max_ಎಣಿಕೆ) {
                max = i;
                max_ಎಣಿಕೆ = refCounts[i];
            }
        }
        ಪ್ರತ್ಯಾಗಮ indexes[max];
    }

ಗುಪ್ತ:
    ಸ್ಥಿರ constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t ಮೌಲ್ಯಗಳು[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
ವರ್ಗ MixedBlocks {
ಸಾರ್ವಜನಿಕ:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    ತಾರ್ಕಿಕ init(int32_t maxLength, int32_t newBlockLength) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t maxDataIndex = maxLength - newBlockLength + 1;
        int32_t newLength;
        ಯದಿ (maxDataIndex <= 0xfff) {  // 4k
            newLength = 6007;
            shift = 12;
            mask = 0xfff;
        } ಅನ್ಯಥಾ ಯದಿ (maxDataIndex <= 0x7fff) {  // 32k
            newLength = 50021;
            shift = 15;
            mask = 0x7fff;
        } ಅನ್ಯಥಾ ಯದಿ (maxDataIndex <= 0x1ffff) {  // 128k
            newLength = 200003;
            shift = 17;
            mask = 0x1ffff;
        } ಅನ್ಯಥಾ {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            newLength = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        ಯದಿ (newLength > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(newLength * 4));
            ಯದಿ (table == nullptr) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    template<typename UInt>
    ಶೂನ್ಯ extend(ಧ್ರುವ UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        ಯದಿ (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } ಅನ್ಯಥಾ {
            start = minStart;  // Begin with the first full block.
        }
        ಪ್ರತಿ (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(ಧ್ರುವ UIntA *data, ಧ್ರುವ UIntB *blockData, int32_t blockStart) ಧ್ರುವ {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        ಯದಿ (entryIndex >= 0) {
            ಪ್ರತ್ಯಾಗಮ (table[entryIndex] & mask) - 1;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }

    int32_t findAllSameBlock(ಧ್ರುವ uint32_t *data, uint32_t block_ಮೌಲ್ಯ) ಧ್ರುವ {
        uint32_t hashCode = makeHashCode(block_ಮೌಲ್ಯ);
        int32_t entryIndex = findEntry(data, block_ಮೌಲ್ಯ, hashCode);
        ಯದಿ (entryIndex >= 0) {
            ಪ್ರತ್ಯಾಗಮ (table[entryIndex] & mask) - 1;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
    }

ಗುಪ್ತ:
    template<typename UInt>
    uint32_t makeHashCode(ಧ್ರುವ UInt *blockData, int32_t blockStart) ಧ್ರುವ {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        ಕುರು {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } ಯಾವತ್ (blockStart < blockLimit);
        ಪ್ರತ್ಯಾಗಮ hashCode;
    }

    uint32_t makeHashCode(uint32_t block_ಮೌಲ್ಯ) ಧ್ರುವ {
        uint32_t hashCode = block_ಮೌಲ್ಯ;
        ಪ್ರತಿ (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_ಮೌಲ್ಯ;
        }
        ಪ್ರತ್ಯಾಗಮ hashCode;
    }

    template<typename UInt>
    ಶೂನ್ಯ addEntry(ಧ್ರುವ UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        ಯದಿ (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(ಧ್ರುವ UIntA *data, ಧ್ರುವ UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) ಧ್ರುವ {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        ಪ್ರತಿ (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            ಯದಿ (entry == 0) {
                ಪ್ರತ್ಯಾಗಮ ~entryIndex;
            }
            ಯದಿ ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                ಯದಿ (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    ಪ್ರತ್ಯಾಗಮ entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(ಧ್ರುವ uint32_t *data, uint32_t block_ಮೌಲ್ಯ, uint32_t hashCode) ಧ್ರುವ {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        ಪ್ರತಿ (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            ಯದಿ (entry == 0) {
                ಪ್ರತ್ಯಾಗಮ ~entryIndex;
            }
            ಯದಿ ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                ಯದಿ (all_ಮೌಲ್ಯಗಳು_Same_As(data + dataIndex, blockLength, block_ಮೌಲ್ಯ)) {
                    ಪ್ರತ್ಯಾಗಮ entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    ಅನ್ತಃಸ್ಥ int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) ಧ್ರುವ {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        ಪ್ರತ್ಯಾಗಮ (entryIndex + initialEntryIndex) % length;
    }

    // Hash table.
    // The length is a prime number, larger than the maximum data length.
    // The "shift" lower bits store a data index + 1.
    // The remaining upper bits store a partial hashCode of the block data values.
    uint32_t *table = nullptr;
    int32_t capacity = 0;
    int32_t length = 0;
    int32_t shift = 0;
    uint32_t mask = 0;

    int32_t blockLength = 0;
};

int32_t MutableCodePointTrie::compactWholeDataBlocks(int32_t fastILimit, AllSameBlocks &allSameBlocks) {
#ifdef UCPTRIE_DEBUG
    ತಾರ್ಕಿಕ overflow = ಅಸತ್ಯ;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t newDataCapacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    newDataCapacity += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    // Add room for special values (errorValue, highValue) and padding.
    newDataCapacity += 4;
    int32_t iLimit = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    ಪ್ರತಿ (int32_t i = 0; i < iLimit; i += inc) {
        ಯದಿ (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        ಯದಿ (flags[i] == MIXED) {
            // Really mixed?
            ಧ್ರುವ uint32_t *p = data + value;
            value = *p;
            ಯದಿ (all_ಮೌಲ್ಯಗಳು_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } ಅನ್ಯಥಾ {
                newDataCapacity += blockLength;
                ಅನುವರ್ತ;
            }
        } ಅನ್ಯಥಾ {
            U_ASSERT(flags[i] == ALL_SAME);
            ಯದಿ (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                ತಾರ್ಕಿಕ allSame = ಸತ್ಯ;
                int32_t next_i = i + inc;
                ಪ್ರತಿ (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    ಯದಿ (index[j] != value) {
                        allSame = ಅಸತ್ಯ;
                        ಭಙ್ಗ;
                    }
                }
                ಯದಿ (!allSame) {
                    // Turn it into a MIXED block.
                    ಯದಿ (getDataBlock(i) < 0) {
                        ಪ್ರತ್ಯಾಗಮ -1;
                    }
                    newDataCapacity += blockLength;
                    ಅನುವರ್ತ;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        ಯದಿ (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            ಯದಿ (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = ಸತ್ಯ;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            ಪ್ರತಿ (int32_t j = 0;; j += jInc) {
                ಯದಿ (j == i) {
                    allSameBlocks.add(i, inc, value);
                    ಭಙ್ಗ;
                }
                ಯದಿ (j == fastILimit) {
                    jInc = 1;
                }
                ಯದಿ (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    ಭಙ್ಗ;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        ಯದಿ (other >= 0) {
            flags[i] = SAME_AS;
            index[i] = other;
        } ಅನ್ಯಥಾ {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    ಪ್ರತ್ಯಾಗಮ newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(ಅಭಿವ್ಯಕ್ತಿ) ಅಭಿವ್ಯಕ್ತಿ
#ಅನ್ಯಥಾ
#   define DEBUG_DO(ಅಭಿವ್ಯಕ್ತಿ)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_ಮೌಲ್ಯ(ವರ್ಣ s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (ವರ್ಣ)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (ವರ್ಣ)(0x80 + (value & 0x3F));
    ಪ್ರತ್ಯಾಗಮ length + 3;
}

ಶೂನ್ಯ printBlock(ಧ್ರುವ uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_ಮೌಲ್ಯ) {
    ವರ್ಣ s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    ಪ್ರತಿ (i = 0; i < overlap; ++i) {
        length = append_ಮೌಲ್ಯ(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    ಪ್ರತಿ (; i < blockLength; ++i) {
        ಯದಿ (block != nullptr) {
            value = block[i];
        }
        ಯದಿ (value == initial_ಮೌಲ್ಯ) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_ಮೌಲ್ಯ(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    ಯದಿ (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } ಅನ್ಯಥಾ ಯದಿ (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } ಅನ್ಯಥಾ {
        printf("  %6lX  %s|\n", (long)start, s);
    }
}
#endif

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
int32_t MutableCodePointTrie::compactData(
        int32_t fastILimit, uint32_t *newData, int32_t newDataCapacity,
        int32_t dataNullIndex, MixedBlocks &mixedBlocks, UErrorCode &errorCode) {
#ifdef UCPTRIE_DEBUG
    int32_t ಎಣಿಕೆ_Same=0, sumOverlaps=0;
    ತಾರ್ಕಿಕ printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    ಪ್ರತಿ (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        ಯದಿ (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_ಮೌಲ್ಯ);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    ಯದಿ (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    ಪ್ರತಿ (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        ಯದಿ (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            ಯದಿ (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            mixedBlocks.extend(newData, 0, 0, newDataLength);
        }
        ಯದಿ (flags[i] == ALL_SAME) {
            uint32_t value = index[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixedBlocks.findAllSameBlock(newData, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            ಯಾವತ್ (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            ಯದಿ (n >= 0) {
                DEBUG_DO(++ಎಣಿಕೆ_Same);
                index[i] = n;
            } ಅನ್ಯಥಾ {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                ಯದಿ (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_ಮೌಲ್ಯ);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                ಯಾವತ್ (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } ಅನ್ಯಥಾ ಯದಿ (flags[i] == MIXED) {
            ಧ್ರುವ uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            ಯದಿ (n >= 0) {
                DEBUG_DO(++ಎಣಿಕೆ_Same);
                index[i] = n;
            } ಅನ್ಯಥಾ {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                ಯದಿ (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_ಮೌಲ್ಯ);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                ಯಾವತ್ (n < blockLength) {
                    newData[newDataLength++] = block[n++];
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } ಅನ್ಯಥಾ /* SAME_AS */ {
            uint32_t j = index[i];
            index[i] = index[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)dataLength, (long)newDataLength, (long)ಎಣಿಕೆ_Same, (long)sumOverlaps);
#endif
    ಪ್ರತ್ಯಾಗಮ newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    ಯದಿ ((ಮೇಲಿನ_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        ಪ್ರತ್ಯಾಗಮ fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    ಪ್ರತಿ (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = index[i];
        fastIndex[j] = static_cast<uint16_t>(i3);
        ಯದಿ (i3 == static_cast<uint32_t>(dataNullOffset)) {
            ಯದಿ (i3FirstNull < 0) {
                i3FirstNull = j;
            } ಅನ್ಯಥಾ ಯದಿ (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                index3NullOffset = i3FirstNull;
            }
        } ಅನ್ಯಥಾ {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t iNext = i + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        ಯಾವತ್ (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    ಯದಿ (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    mixedBlocks.extend(fastIndex, 0, 0, fastIndexLength);

    // Examine index-3 blocks. For each determine one of:
    // - same as the index-3 null block
    // - same as a fast-index block
    // - 16-bit indexes
    // - 18-bit indexes
    // We store this in the first flags entry for the index-3 block.
    //
    // Also determine an upper limit for the index-3 table length.
    int32_t index3Capacity = 0;
    i3FirstNull = index3NullOffset;
    ತಾರ್ಕಿಕ hasLongI3Blocks = ಅಸತ್ಯ;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3;
    ಪ್ರತಿ (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        ತಾರ್ಕಿಕ isNull = ಸತ್ಯ;
        ಕುರು {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            ಯದಿ (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = ಅಸತ್ಯ;
            }
        } ಯಾವತ್ (++j < jLimit);
        ಯದಿ (isNull) {
            flags[i] = I3_NULL;
            ಯದಿ (i3FirstNull < 0) {
                ಯದಿ (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                } ಅನ್ಯಥಾ {
                    index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                    hasLongI3Blocks = ಸತ್ಯ;
                }
                i3FirstNull = 0;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fastIndex, index, i);
                ಯದಿ (n >= 0) {
                    flags[i] = I3_BMP;
                    index[i] = n;
                } ಅನ್ಯಥಾ {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                }
            } ಅನ್ಯಥಾ {
                flags[i] = I3_18;
                index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                hasLongI3Blocks = ಸತ್ಯ;
            }
        }
        i = j;
    }

    int32_t index2Capacity = (iLimit - iStart) >> UCPTRIE_SHIFT_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1Length = (index2Capacity + UCPTRIE_INDEX_2_MASK) >> UCPTRIE_SHIFT_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16Capacity = fastIndexLength + index1Length + index3Capacity + index2Capacity + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16Capacity * 2));
    ಯದಿ (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    ಯದಿ (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    MixedBlocks longI3Blocks;
    ಯದಿ (hasLongI3Blocks) {
        ಯದಿ (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    ಪ್ರತಿ (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
        int32_t i3;
        uint8_t f = flags[i];
        ಯದಿ (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = dataNullOffset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        ಯದಿ (f == I3_NULL) {
            i3 = index3NullOffset;
        } ಅನ್ಯಥಾ ಯದಿ (f == I3_BMP) {
            i3 = index[i];
        } ಅನ್ಯಥಾ ಯದಿ (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, index, i);
            ಯದಿ (n >= 0) {
                i3 = n;
            } ಅನ್ಯಥಾ {
                ಯದಿ (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ಅನ್ಯಥಾ {
                    n = getOverlap(index16, indexLength,
                                   index, i, UCPTRIE_INDEX_3_BLOCK_LENGTH);
                }
                i3 = indexLength - n;
                int32_t prevIndexLength = indexLength;
                ಯಾವತ್ (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                    index16[indexLength++] = index[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                ಯದಿ (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        } ಅನ್ಯಥಾ {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
            int32_t k = indexLength;
            ಕುರು {
                ++k;
                uint32_t v = index[j++];
                uint32_t upperBits = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = index[j++];
                upperBits |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = upperBits;
            } ಯಾವತ್ (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, indexLength);
            ಯದಿ (n >= 0) {
                i3 = n | 0x8000;
            } ಅನ್ಯಥಾ {
                ಯದಿ (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ಅನ್ಯಥಾ {
                    n = getOverlap(index16, indexLength,
                                   index16, indexLength, INDEX_3_18BIT_BLOCK_LENGTH);
                }
                i3 = (indexLength - n) | 0x8000;
                int32_t prevIndexLength = indexLength;
                ಯದಿ (n > 0) {
                    int32_t start = indexLength;
                    ಯಾವತ್ (n < INDEX_3_18BIT_BLOCK_LENGTH) {
                        index16[indexLength++] = index16[start + n++];
                    }
                } ಅನ್ಯಥಾ {
                    indexLength += INDEX_3_18BIT_BLOCK_LENGTH;
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                ಯದಿ (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        }
        ಯದಿ (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2Length++] = i3;
    }
    U_ASSERT(i2Length == index2Capacity);
    U_ASSERT(indexLength <= index3Start + index3Capacity);

    ಯದಿ (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    ಯದಿ (indexLength >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    ಪ್ರತಿ (int32_t i = 0; i < i2Length; i += blockLength) {
        int32_t n;
        ಯದಿ ((i2Length - i) >= blockLength) {
            // normal block
            U_ASSERT(blockLength == UCPTRIE_INDEX_2_BLOCK_LENGTH);
            n = mixedBlocks.findBlock(index16, index2, i);
        } ಅನ್ಯಥಾ {
            // highStart is inside the last index-2 block. Shorten it.
            blockLength = i2Length - i;
            n = findSameBlock(index16, index3Start, indexLength,
                              index2, i, blockLength);
        }
        int32_t i2;
        ಯದಿ (n >= 0) {
            i2 = n;
        } ಅನ್ಯಥಾ {
            ಯದಿ (indexLength == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } ಅನ್ಯಥಾ {
                n = getOverlap(index16, indexLength, index2, i, blockLength);
            }
            i2 = indexLength - n;
            int32_t prevIndexLength = indexLength;
            ಯಾವತ್ (n < blockLength) {
                index16[indexLength++] = index2[i + n++];
            }
            mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3Start);
    U_ASSERT(indexLength <= index16Capacity);

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (long)iLimit, (long)indexLength);
#endif

    ಪ್ರತ್ಯಾಗಮ indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((ಮೇಲಿನ_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    ಮೇಲಿನ_ಮೌಲ್ಯ = get(MAX_UNICODE);
    int32_t real_ಮೇಲಿನ_Start = find_ಮೇಲಿನ_Start();
    real_ಮೇಲಿನ_Start = (real_ಮೇಲಿನ_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    ಯದಿ (real_ಮೇಲಿನ_Start == UNICODE_LIMIT) {
        ಮೇಲಿನ_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_ಮೇಲಿನ_Start, (long)ಮೇಲಿನ_ಮೌಲ್ಯ, (long)initial_ಮೌಲ್ಯ);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    ಯದಿ (real_ಮೇಲಿನ_Start < fastLimit) {
        ಪ್ರತಿ (int32_t i = (real_ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = ಮೇಲಿನ_ಮೌಲ್ಯ;
        }
        ಮೇಲಿನ_Start = fastLimit;
    } ಅನ್ಯಥಾ {
        ಮೇಲಿನ_Start = real_ಮೇಲಿನ_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    ಪ್ರತಿ (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    ಯದಿ (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    ಯದಿ (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    uprv_memcpy(newData, asciiData, ಪರಿಮಾಣ(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    ಯದಿ (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        ಯದಿ (data[dataNullOffset] != initial_ಮೌಲ್ಯ) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_ಮೌಲ್ಯ, (long)data[dataNullOffset]);
        }
#endif
        initial_ಮೌಲ್ಯ = data[dataNullOffset];
    } ಅನ್ಯಥಾ {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    ಮೇಲಿನ_Start = real_ಮೇಲಿನ_Start;
    ಪ್ರತ್ಯಾಗಮ indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ಮೌಲ್ಯ_Width < UCPTRIE_ಮೌಲ್ಯ_BITS_16 || UCPTRIE_ಮೌಲ್ಯ_BITS_8 < ಮೌಲ್ಯ_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        mask_ಮೌಲ್ಯಗಳು(0xffff);
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        mask_ಮೌಲ್ಯಗಳು(0xff);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) {
        clear();
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    ಯದಿ (ಮೌಲ್ಯ_Width == UCPTRIE_ಮೌಲ್ಯ_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    ಯದಿ (ಮೌಲ್ಯ_Width == UCPTRIE_ಮೌಲ್ಯ_BITS_16) {
        ಯದಿ (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_ಮೌಲ್ಯ;
        }
        ಯದಿ (data[dataLength - 1] != error_ಮೌಲ್ಯ || data[dataLength - 2] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
            data[dataLength++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
            data[dataLength++] = error_ಮೌಲ್ಯ;
        }
        length += dataLength * 2;
    } ಅನ್ಯಥಾ ಯದಿ (ಮೌಲ್ಯ_Width == UCPTRIE_ಮೌಲ್ಯ_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        ಯದಿ (data[dataLength - 1] != error_ಮೌಲ್ಯ || data[dataLength - 2] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
            ಯದಿ (data[dataLength - 1] != ಮೇಲಿನ_ಮೌಲ್ಯ) {
                data[dataLength++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
            }
            data[dataLength++] = error_ಮೌಲ್ಯ;
        }
        length += dataLength * 4;
    } ಅನ್ಯಥಾ {
        int32_t and3 = (length + dataLength) & 3;
        ಯದಿ (and3 == 0 && data[dataLength - 1] == error_ಮೌಲ್ಯ && data[dataLength - 2] == ಮೇಲಿನ_ಮೌಲ್ಯ) {
            // all set
        } ಅನ್ಯಥಾ ಯದಿ(and3 == 3 && data[dataLength - 1] == ಮೇಲಿನ_ಮೌಲ್ಯ) {
            data[dataLength++] = error_ಮೌಲ್ಯ;
        } ಅನ್ಯಥಾ {
            ಯಾವತ್ (and3 != 2) {
                data[dataLength++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = ಮೇಲಿನ_ಮೌಲ್ಯ;
            data[dataLength++] = error_ಮೌಲ್ಯ;
        }
        length += dataLength;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += ಪರಿಮಾಣ(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    ಯದಿ (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, ಪರಿಮಾಣ(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->ಮೇಲಿನ_Start = ಮೇಲಿನ_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ಮೇಲಿನ_Start = (ಮೇಲಿನ_Start + 0xfff) >> 12;
    trie->type = type;
    trie->ಮೌಲ್ಯ_Width = ಮೌಲ್ಯ_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_ಮೌಲ್ಯ = initial_ಮೌಲ್ಯ;

    bytes += ಪರಿಮಾಣ(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    ಯದಿ (ಮೇಲಿನ_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        ಪ್ರತಿ (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } ಅನ್ಯಥಾ {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    ಧ್ರುವ uint32_t *p = data;
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        ಪ್ರತಿ (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)dataLength * 4);
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        ಪ್ರತಿ (int32_t i = dataLength; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // Will not occur, valueWidth checked at the beginning.
        ಭಙ್ಗ;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    ಪ್ರತ್ಯಾಗಮ trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        ನೂತನ MutableCodePointTrie(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, *pErrorCode), *pErrorCode);
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ಧ್ರುವ UMutableCPTrie *other, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (other == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        ನೂತನ MutableCodePointTrie(*reinterpret_cast<ಧ್ರುವ MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ಧ್ರುವ UCPMap *map, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ಧ್ರುವ UCPTrie *trie, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ಧ್ರುವ UMutableCPTrie *trie, UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ MutableCodePointTrie *>(trie)->get(c);
}

ನಾಮಾಕಾಶ {

UChar32 getRange(ಧ್ರುವ ಶೂನ್ಯ *trie, UChar32 start,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_ಮೌಲ್ಯ);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ಧ್ರುವ UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                        UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಪ್ರತ್ಯಾಗಮ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_ಮೌಲ್ಯ,
                                    filter, context, p_ಮೌಲ್ಯ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width,
                              UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, ಮೌಲ್ಯ_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC ಶೂನ್ಯ umutablecptrie_setName(UMutableCPTrie *trie, ಧ್ರುವ ವರ್ಣ *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
