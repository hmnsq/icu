// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/umutablecptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ifdef OVERFLOW
#undef OVERFLOW
#endif

U_NAMESPACE_BEGIN

ନାମାକାଶ {

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

ଶ୍ରେଣୀ AllSameBlocks;
ଶ୍ରେଣୀ MixedBlocks;

ଶ୍ରେଣୀ MutableCodePointTrie : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    MutableCodePointTrie(uint32_t initial_ମୂଲ୍ୟ, uint32_t error_ମୂଲ୍ୟ, UErrorCode &errorCode);
    MutableCodePointTrie(ଅଚଳ MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(ଅଚଳ MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(ଅଚଳ MutableCodePointTrie &other) = delete;

    ସ୍ଥିର MutableCodePointTrie *fromUCPMap(ଅଚଳ UCPMap *map, UErrorCode &errorCode);
    ସ୍ଥିର MutableCodePointTrie *fromUCPTrie(ଅଚଳ UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) ଅଚଳ;
    int32_t getRange(UChar32 start, UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context,
                     uint32_t *p_ମୂଲ୍ୟ) ଅଚଳ;

    ଶୂନ୍ୟ set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    ଶୂନ୍ୟ setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width, UErrorCode &errorCode);

ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ clear();

    ତାର୍କିକ ensure_ଉପର_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    ଶୂନ୍ୟ mask_ମୂଲ୍ୟଗୁଡ଼ିକ(uint32_t mask);
    UChar32 find_ଉପର_Start() ଅଚଳ;
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

    uint32_t orig_Initial_ମୂଲ୍ୟ;
    uint32_t initial_ମୂଲ୍ୟ;
    uint32_t error_ମୂଲ୍ୟ;
    UChar32 ଉପର_Start;
    uint32_t ଉପର_ମୂଲ୍ୟ;
#ifdef UCPTRIE_DEBUG
ସର୍ବସାଧାରଣ:
    ଅଚଳ ଅକ୍ଷର *name;
#endif
ବ୍ୟକ୍ତିଗତ:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_ମୂଲ୍ୟ, uint32_t err_ମୂଲ୍ୟ, UErrorCode &errorCode) :
        orig_Initial_ମୂଲ୍ୟ(ini_ମୂଲ୍ୟ), initial_ମୂଲ୍ୟ(ini_ମୂଲ୍ୟ), error_ମୂଲ୍ୟ(err_ମୂଲ୍ୟ),
        ଉପର_Start(0), ଉପର_ମୂଲ୍ୟ(initial_ମୂଲ୍ୟ)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    ଯଦି (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(ଅଚଳ MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_ମୂଲ୍ୟ(other.orig_Initial_ମୂଲ୍ୟ), initial_ମୂଲ୍ୟ(other.initial_ମୂଲ୍ୟ),
        error_ମୂଲ୍ୟ(other.error_ମୂଲ୍ୟ),
        ଉପର_Start(other.ଉପର_Start), ଉପର_ମୂଲ୍ୟ(other.ଉପର_ମୂଲ୍ୟ)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    int32_t iCapacity = ଉପର_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    ଯଦି (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = ଉପର_Start >> UCPTRIE_SHIFT_3;
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

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(ଅଚଳ UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ମୂଲ୍ୟ = ucpmap_get(map, -1);
    uint32_t initial_ମୂଲ୍ୟ = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        ନୂତନ MutableCodePointTrie(initial_ମୂଲ୍ୟ, error_ମୂଲ୍ୟ, errorCode),
        errorCode);
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    ଯେତେବେଳେ ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        ଯଦି (value != initial_ମୂଲ୍ୟ) {
            ଯଦି (start == end) {
                mutableTrie->set(start, value, errorCode);
            } ଅନ୍ୟଥା {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    ଯଦି (U_SUCCESS(errorCode)) {
        ଫେରନ୍ତୁ mutableTrie.orphan();
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(ଅଚଳ UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_ମୂଲ୍ୟ;
    uint32_t initial_ମୂଲ୍ୟ;
    ବାଛ (trie->ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        error_ମୂଲ୍ୟ = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_ମୂଲ୍ୟ_NEG_DATA_OFFSET];
        initial_ମୂଲ୍ୟ = trie->data.ptr16[trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        error_ମୂଲ୍ୟ = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_ମୂଲ୍ୟ_NEG_DATA_OFFSET];
        initial_ମୂଲ୍ୟ = trie->data.ptr32[trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        error_ମୂଲ୍ୟ = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_ମୂଲ୍ୟ_NEG_DATA_OFFSET];
        initial_ମୂଲ୍ୟ = trie->data.ptr8[trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET];
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        ନୂତନ MutableCodePointTrie(initial_ମୂଲ୍ୟ, error_ମୂଲ୍ୟ, errorCode),
        errorCode);
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    ଯେତେବେଳେ ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        ଯଦି (value != initial_ମୂଲ୍ୟ) {
            ଯଦି (start == end) {
                mutableTrie->set(start, value, errorCode);
            } ଅନ୍ୟଥା {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    ଯଦି (U_SUCCESS(errorCode)) {
        ଫେରନ୍ତୁ mutableTrie.orphan();
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

ଶୂନ୍ୟ MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    ଉପର_ମୂଲ୍ୟ = initial_ମୂଲ୍ୟ = orig_Initial_ମୂଲ୍ୟ;
    ଉପର_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) ଅଚଳ {
    ଯଦି (static_cast<uint32_t>(c) > MAX_UNICODE) {
        ଫେରନ୍ତୁ error_ମୂଲ୍ୟ;
    }
    ଯଦି (c >= ଉପର_Start) {
        ଫେରନ୍ତୁ ଉପର_ମୂଲ୍ୟ;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    ଯଦି (flags[i] == ALL_SAME) {
        ଫେରନ୍ତୁ index[i];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

ଧାଡ଼ିଭିତରେ uint32_t maybe_Filter_ମୂଲ୍ୟ(uint32_t value, uint32_t initial_ମୂଲ୍ୟ, uint32_t null_ମୂଲ୍ୟ,
                                 UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context) {
    ଯଦି (value == initial_ମୂଲ୍ୟ) {
        value = null_ମୂଲ୍ୟ;
    } ଅନ୍ୟଥା ଯଦି (filter != nullptr) {
        value = filter(context, value);
    }
    ଫେରନ୍ତୁ value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context,
        uint32_t *p_ମୂଲ୍ୟ) ଅଚଳ {
    ଯଦି (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
    ଯଦି (start >= ଉପର_Start) {
        ଯଦି (p_ମୂଲ୍ୟ != nullptr) {
            uint32_t value = ଉପର_ମୂଲ୍ୟ;
            ଯଦି (filter != nullptr) { value = filter(context, value); }
            *p_ମୂଲ୍ୟ = value;
        }
        ଫେରନ୍ତୁ MAX_UNICODE;
    }
    uint32_t null_ମୂଲ୍ୟ = initial_ମୂଲ୍ୟ;
    ଯଦି (filter != nullptr) { null_ମୂଲ୍ୟ = filter(context, null_ମୂଲ୍ୟ); }
    UChar32 c = start;
    uint32_t trie_ମୂଲ୍ୟ, value;
    ତାର୍କିକ have_ମୂଲ୍ୟ = ମିଥ୍ୟା;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    କର {
        ଯଦି (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            ଯଦି (have_ମୂଲ୍ୟ) {
                ଯଦି (trieValue2 != trie_ମୂଲ୍ୟ) {
                    ଯଦି (filter == nullptr ||
                            maybe_Filter_ମୂଲ୍ୟ(trieValue2, initial_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                                             filter, context) != value) {
                        ଫେରନ୍ତୁ c - 1;
                    }
                    trie_ମୂଲ୍ୟ = trieValue2;  // may or may not help
                }
            } ଅନ୍ୟଥା {
                trie_ମୂଲ୍ୟ = trieValue2;
                value = maybe_Filter_ମୂଲ୍ୟ(trieValue2, initial_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ, filter, context);
                ଯଦି (p_ମୂଲ୍ୟ != nullptr) { *p_ମୂଲ୍ୟ = value; }
                have_ମୂଲ୍ୟ = ସତ୍ୟ;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } ଅନ୍ୟଥା /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            ଯଦି (have_ମୂଲ୍ୟ) {
                ଯଦି (trieValue2 != trie_ମୂଲ୍ୟ) {
                    ଯଦି (filter == nullptr ||
                            maybe_Filter_ମୂଲ୍ୟ(trieValue2, initial_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                                             filter, context) != value) {
                        ଫେରନ୍ତୁ c - 1;
                    }
                    trie_ମୂଲ୍ୟ = trieValue2;  // may or may not help
                }
            } ଅନ୍ୟଥା {
                trie_ମୂଲ୍ୟ = trieValue2;
                value = maybe_Filter_ମୂଲ୍ୟ(trieValue2, initial_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ, filter, context);
                ଯଦି (p_ମୂଲ୍ୟ != nullptr) { *p_ମୂଲ୍ୟ = value; }
                have_ମୂଲ୍ୟ = ସତ୍ୟ;
            }
            ଯେତେବେଳେ ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                ଯଦି (trieValue2 != trie_ମୂଲ୍ୟ) {
                    ଯଦି (filter == nullptr ||
                            maybe_Filter_ମୂଲ୍ୟ(trieValue2, initial_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                                             filter, context) != value) {
                        ଫେରନ୍ତୁ c - 1;
                    }
                }
                trie_ମୂଲ୍ୟ = trieValue2;  // may or may not help
            }
        }
        ++i;
    } ଯେତେବେଳେ (c < ଉପର_Start);
    U_ASSERT(have_ମୂଲ୍ୟ);
    ଯଦି (maybe_Filter_ମୂଲ୍ୟ(ଉପର_ମୂଲ୍ୟ, initial_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                         filter, context) != value) {
        ଫେରନ୍ତୁ c - 1;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ MAX_UNICODE;
    }
}

ଶୂନ୍ୟ
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    ଯେତେବେଳେ (block < limit) {
        *block++ = value;
    }
}

ତାର୍କିକ MutableCodePointTrie::ensure_ଉପର_Start(UChar32 c) {
    ଯଦି (c >= ଉପର_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = ଉପର_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        ଯଦି (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            ଯଦି (newIndex == nullptr) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        କର {
            flags[i] = ALL_SAME;
            index[i] = initial_ମୂଲ୍ୟ;
        } ଯେତେବେଳେ(++i < iLimit);
        ଉପର_Start = c;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

int32_t MutableCodePointTrie::allocDataBlock(int32_t blockLength) {
    int32_t newBlock = dataLength;
    int32_t new_ଶୀର୍ଷ = newBlock + blockLength;
    ଯଦି (new_ଶୀର୍ଷ > dataCapacity) {
        int32_t capacity;
        ଯଦି (dataCapacity < MEDIUM_DATA_LENGTH) {
            capacity = MEDIUM_DATA_LENGTH;
        } ଅନ୍ୟଥା ଯଦି (dataCapacity < MAX_DATA_LENGTH) {
            capacity = MAX_DATA_LENGTH;
        } ଅନ୍ୟଥା {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            ଫେରନ୍ତୁ -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        ଯଦି (newData == nullptr) {
            ଫେରନ୍ତୁ -1;
        }
        uprv_memcpy(newData, data, (size_t)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_ଶୀର୍ଷ;
    ଫେରନ୍ତୁ newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    ଯଦି (flags[i] == MIXED) {
        ଫେରନ୍ତୁ index[i];
    }
    ଯଦି (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        ଯଦି (newBlock < 0) { ଫେରନ୍ତୁ newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        କର {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } ଯେତେବେଳେ (iStart < iLimit);
        ଫେରନ୍ତୁ index[i];
    } ଅନ୍ୟଥା {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        ଯଦି (newBlock < 0) { ଫେରନ୍ତୁ newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        ଫେରନ୍ତୁ newBlock;
    }
}

ଶୂନ୍ୟ MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    int32_t block;
    ଯଦି (!ensure_ଉପର_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

ଶୂନ୍ୟ
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    ଯେତେବେଳେ (block < pLimit) {
        *block++ = value;
    }
}

ଶୂନ୍ୟ MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଯଦି (!ensure_ଉପର_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    UChar32 limit = end + 1;
    ଯଦି (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        ଯଦି (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        ଯଦି (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } ଅନ୍ୟଥା {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            ଫେରନ୍ତୁ;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    ଯେତେବେଳେ (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        ଯଦି (flags[i] == ALL_SAME) {
            index[i] = value;
        } ଅନ୍ୟଥା /* MIXED */ {
            fillBlock(data + index[i], 0, UCPTRIE_SMALL_DATA_BLOCK_LENGTH, value);
        }
        start += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    }

    ଯଦି (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        ଯଦି (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

ଶୂନ୍ୟ MutableCodePointTrie::mask_ମୂଲ୍ୟଗୁଡ଼ିକ(uint32_t mask) {
    initial_ମୂଲ୍ୟ &= mask;
    error_ମୂଲ୍ୟ &= mask;
    ଉପର_ମୂଲ୍ୟ &= mask;
    int32_t iLimit = ଉପର_Start >> UCPTRIE_SHIFT_3;
    ପାଇଁ (int32_t i = 0; i < iLimit; ++i) {
        ଯଦି (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    ପାଇଁ (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
ତାର୍କିକ equalBlocks(ଅଚଳ UIntA *s, ଅଚଳ UIntB *t, int32_t length) {
    ଯେତେବେଳେ (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    ଫେରନ୍ତୁ length == 0;
}

ତାର୍କିକ all_ମୂଲ୍ୟଗୁଡ଼ିକ_Same_As(ଅଚଳ uint32_t *p, int32_t length, uint32_t value) {
    ଅଚଳ uint32_t *pLimit = p + length;
    ଯେତେବେଳେ (p < pLimit && *p == value) { ++p; }
    ଫେରନ୍ତୁ p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(ଅଚଳ uint16_t *p, int32_t pStart, int32_t length,
                      ଅଚଳ uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    ଯେତେବେଳେ (pStart <= length) {
        ଯଦି (equalBlocks(p + pStart, q, blockLength)) {
            ଫେରନ୍ତୁ pStart;
        }
        ++pStart;
    }
    ଫେରନ୍ତୁ -1;
}

int32_t findAllSameBlock(ଅଚଳ uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    ପାଇଁ (int32_t block = start; block <= limit; ++block) {
        ଯଦି (p[block] == value) {
            ପାଇଁ (int32_t i = 1;; ++i) {
                ଯଦି (i == blockLength) {
                    ଫେରନ୍ତୁ block;
                }
                ଯଦି (p[block + i] != value) {
                    block += i;
                    ଭାଙ୍ଗ;
                }
            }
        }
    }
    ଫେରନ୍ତୁ -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
template<typename UIntA, typename UIntB>
int32_t getOverlap(ଅଚଳ UIntA *p, int32_t length,
                   ଅଚଳ UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    ଯେତେବେଳେ (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    ଫେରନ୍ତୁ overlap;
}

int32_t getAllSameOverlap(ଅଚଳ uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    ଯେତେବେଳେ (min < i && p[i - 1] == value) { --i; }
    ଫେରନ୍ତୁ length - i;
}

ତାର୍କିକ isStartOfSomeFastBlock(uint32_t dataOffset, ଅଚଳ uint32_t index[], int32_t fastILimit) {
    ପାଇଁ (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        ଯଦି (index[i] == dataOffset) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_ଉପର_Start() ଅଚଳ {
    int32_t i = ଉପର_Start >> UCPTRIE_SHIFT_3;
    ଯେତେବେଳେ (i > 0) {
        ତାର୍କିକ match;
        ଯଦି (flags[--i] == ALL_SAME) {
            match = index[i] == ଉପର_ମୂଲ୍ୟ;
        } ଅନ୍ୟଥା /* MIXED */ {
            ଅଚଳ uint32_t *p = data + index[i];
            ପାଇଁ (int32_t j = 0;; ++j) {
                ଯଦି (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = ସତ୍ୟ;
                    ଭାଙ୍ଗ;
                }
                ଯଦି (p[j] != ଉପର_ମୂଲ୍ୟ) {
                    match = ମିଥ୍ୟା;
                    ଭାଙ୍ଗ;
                }
            }
        }
        ଯଦି (!match) {
            ଫେରନ୍ତୁ (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    ଫେରନ୍ତୁ 0;
}

ଶ୍ରେଣୀ AllSameBlocks {
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର constexpr int32_t NEW_UNIQUE = -1;
    ସ୍ଥିର constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        ଯଦି (mostRecent >= 0 && ମୂଲ୍ୟଗୁଡ଼ିକ[mostRecent] == value) {
            refCounts[mostRecent] += count;
            ଫେରନ୍ତୁ indexes[mostRecent];
        }
        ପାଇଁ (int32_t i = 0; i < length; ++i) {
            ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                ଫେରନ୍ତୁ indexes[i];
            }
        }
        ଯଦି (length == CAPACITY) {
            ଫେରନ୍ତୁ OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        ମୂଲ୍ୟଗୁଡ଼ିକ[length] = value;
        refCounts[length++] = count;
        ଫେରନ୍ତୁ NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    ଶୂନ୍ୟ add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_ଗଣନା = I_LIMIT;
        ପାଇଁ (int32_t i = 0; i < length; ++i) {
            U_ASSERT(ମୂଲ୍ୟଗୁଡ଼ିକ[i] != value);
            ଯଦି (refCounts[i] < least_ଗଣନା) {
                least = i;
                least_ଗଣନା = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        ମୂଲ୍ୟଗୁଡ଼ିକ[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() ଅଚଳ {
        ଯଦି (length == 0) { ଫେରନ୍ତୁ -1; }
        int32_t max = -1;
        int32_t max_ଗଣନା = 0;
        ପାଇଁ (int32_t i = 0; i < length; ++i) {
            ଯଦି (refCounts[i] > max_ଗଣନା) {
                max = i;
                max_ଗଣନା = refCounts[i];
            }
        }
        ଫେରନ୍ତୁ indexes[max];
    }

ବ୍ୟକ୍ତିଗତ:
    ସ୍ଥିର constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t ମୂଲ୍ୟଗୁଡ଼ିକ[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
ଶ୍ରେଣୀ MixedBlocks {
ସର୍ବସାଧାରଣ:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    ତାର୍କିକ init(int32_t maxLength, int32_t newBlockLength) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t maxDataIndex = maxLength - newBlockLength + 1;
        int32_t newLength;
        ଯଦି (maxDataIndex <= 0xfff) {  // 4k
            newLength = 6007;
            shift = 12;
            mask = 0xfff;
        } ଅନ୍ୟଥା ଯଦି (maxDataIndex <= 0x7fff) {  // 32k
            newLength = 50021;
            shift = 15;
            mask = 0x7fff;
        } ଅନ୍ୟଥା ଯଦି (maxDataIndex <= 0x1ffff) {  // 128k
            newLength = 200003;
            shift = 17;
            mask = 0x1ffff;
        } ଅନ୍ୟଥା {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            newLength = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        ଯଦି (newLength > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(newLength * 4));
            ଯଦି (table == nullptr) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    template<typename UInt>
    ଶୂନ୍ୟ extend(ଅଚଳ UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        ଯଦି (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } ଅନ୍ୟଥା {
            start = minStart;  // Begin with the first full block.
        }
        ପାଇଁ (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(ଅଚଳ UIntA *data, ଅଚଳ UIntB *blockData, int32_t blockStart) ଅଚଳ {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        ଯଦି (entryIndex >= 0) {
            ଫେରନ୍ତୁ (table[entryIndex] & mask) - 1;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ -1;
        }
    }

    int32_t findAllSameBlock(ଅଚଳ uint32_t *data, uint32_t block_ମୂଲ୍ୟ) ଅଚଳ {
        uint32_t hashCode = makeHashCode(block_ମୂଲ୍ୟ);
        int32_t entryIndex = findEntry(data, block_ମୂଲ୍ୟ, hashCode);
        ଯଦି (entryIndex >= 0) {
            ଫେରନ୍ତୁ (table[entryIndex] & mask) - 1;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ -1;
        }
    }

ବ୍ୟକ୍ତିଗତ:
    template<typename UInt>
    uint32_t makeHashCode(ଅଚଳ UInt *blockData, int32_t blockStart) ଅଚଳ {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        କର {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } ଯେତେବେଳେ (blockStart < blockLimit);
        ଫେରନ୍ତୁ hashCode;
    }

    uint32_t makeHashCode(uint32_t block_ମୂଲ୍ୟ) ଅଚଳ {
        uint32_t hashCode = block_ମୂଲ୍ୟ;
        ପାଇଁ (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_ମୂଲ୍ୟ;
        }
        ଫେରନ୍ତୁ hashCode;
    }

    template<typename UInt>
    ଶୂନ୍ୟ addEntry(ଅଚଳ UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        ଯଦି (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(ଅଚଳ UIntA *data, ଅଚଳ UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) ଅଚଳ {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        ପାଇଁ (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            ଯଦି (entry == 0) {
                ଫେରନ୍ତୁ ~entryIndex;
            }
            ଯଦି ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                ଯଦି (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    ଫେରନ୍ତୁ entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(ଅଚଳ uint32_t *data, uint32_t block_ମୂଲ୍ୟ, uint32_t hashCode) ଅଚଳ {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        ପାଇଁ (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            ଯଦି (entry == 0) {
                ଫେରନ୍ତୁ ~entryIndex;
            }
            ଯଦି ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                ଯଦି (all_ମୂଲ୍ୟଗୁଡ଼ିକ_Same_As(data + dataIndex, blockLength, block_ମୂଲ୍ୟ)) {
                    ଫେରନ୍ତୁ entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    ଧାଡ଼ିଭିତରେ int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) ଅଚଳ {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        ଫେରନ୍ତୁ (entryIndex + initialEntryIndex) % length;
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
    ତାର୍କିକ overflow = ମିଥ୍ୟା;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t newDataCapacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    newDataCapacity += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    // Add room for special values (errorValue, highValue) and padding.
    newDataCapacity += 4;
    int32_t iLimit = ଉପର_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    ପାଇଁ (int32_t i = 0; i < iLimit; i += inc) {
        ଯଦି (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        ଯଦି (flags[i] == MIXED) {
            // Really mixed?
            ଅଚଳ uint32_t *p = data + value;
            value = *p;
            ଯଦି (all_ମୂଲ୍ୟଗୁଡ଼ିକ_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } ଅନ୍ୟଥା {
                newDataCapacity += blockLength;
                ଜାରି;
            }
        } ଅନ୍ୟଥା {
            U_ASSERT(flags[i] == ALL_SAME);
            ଯଦି (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                ତାର୍କିକ allSame = ସତ୍ୟ;
                int32_t next_i = i + inc;
                ପାଇଁ (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    ଯଦି (index[j] != value) {
                        allSame = ମିଥ୍ୟା;
                        ଭାଙ୍ଗ;
                    }
                }
                ଯଦି (!allSame) {
                    // Turn it into a MIXED block.
                    ଯଦି (getDataBlock(i) < 0) {
                        ଫେରନ୍ତୁ -1;
                    }
                    newDataCapacity += blockLength;
                    ଜାରି;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        ଯଦି (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            ଯଦି (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = ସତ୍ୟ;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            ପାଇଁ (int32_t j = 0;; j += jInc) {
                ଯଦି (j == i) {
                    allSameBlocks.add(i, inc, value);
                    ଭାଙ୍ଗ;
                }
                ଯଦି (j == fastILimit) {
                    jInc = 1;
                }
                ଯଦି (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    ଭାଙ୍ଗ;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        ଯଦି (other >= 0) {
            flags[i] = SAME_AS;
            index[i] = other;
        } ଅନ୍ୟଥା {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    ଫେରନ୍ତୁ newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(ପଦସମଷ୍ଟି) ପଦସମଷ୍ଟି
#ଅନ୍ୟଥା
#   define DEBUG_DO(ପଦସମଷ୍ଟି)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_ମୂଲ୍ୟ(ଅକ୍ଷର s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (ଅକ୍ଷର)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (ଅକ୍ଷର)(0x80 + (value & 0x3F));
    ଫେରନ୍ତୁ length + 3;
}

ଶୂନ୍ୟ printBlock(ଅଚଳ uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_ମୂଲ୍ୟ) {
    ଅକ୍ଷର s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    ପାଇଁ (i = 0; i < overlap; ++i) {
        length = append_ମୂଲ୍ୟ(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    ପାଇଁ (; i < blockLength; ++i) {
        ଯଦି (block != nullptr) {
            value = block[i];
        }
        ଯଦି (value == initial_ମୂଲ୍ୟ) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_ମୂଲ୍ୟ(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    ଯଦି (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } ଅନ୍ୟଥା ଯଦି (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } ଅନ୍ୟଥା {
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
    int32_t ଗଣନା_Same=0, sumOverlaps=0;
    ତାର୍କିକ printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    ପାଇଁ (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        ଯଦି (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_ମୂଲ୍ୟ);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    ଯଦି (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = ଉପର_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    ପାଇଁ (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        ଯଦି (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            ଯଦି (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            mixedBlocks.extend(newData, 0, 0, newDataLength);
        }
        ଯଦି (flags[i] == ALL_SAME) {
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
            ଯେତେବେଳେ (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            ଯଦି (n >= 0) {
                DEBUG_DO(++ଗଣନା_Same);
                index[i] = n;
            } ଅନ୍ୟଥା {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                ଯଦି (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_ମୂଲ୍ୟ);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                ଯେତେବେଳେ (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } ଅନ୍ୟଥା ଯଦି (flags[i] == MIXED) {
            ଅଚଳ uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            ଯଦି (n >= 0) {
                DEBUG_DO(++ଗଣନା_Same);
                index[i] = n;
            } ଅନ୍ୟଥା {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                ଯଦି (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_ମୂଲ୍ୟ);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                ଯେତେବେଳେ (n < blockLength) {
                    newData[newDataLength++] = block[n++];
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } ଅନ୍ୟଥା /* SAME_AS */ {
            uint32_t j = index[i];
            index[i] = index[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)dataLength, (long)newDataLength, (long)ଗଣନା_Same, (long)sumOverlaps);
#endif
    ଫେରନ୍ତୁ newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    ଯଦି ((ଉପର_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        ଫେରନ୍ତୁ fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    ପାଇଁ (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = index[i];
        fastIndex[j] = static_cast<uint16_t>(i3);
        ଯଦି (i3 == static_cast<uint32_t>(dataNullOffset)) {
            ଯଦି (i3FirstNull < 0) {
                i3FirstNull = j;
            } ଅନ୍ୟଥା ଯଦି (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                index3NullOffset = i3FirstNull;
            }
        } ଅନ୍ୟଥା {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t iNext = i + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        ଯେତେବେଳେ (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    ଯଦି (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
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
    ତାର୍କିକ hasLongI3Blocks = ମିଥ୍ୟା;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = ଉପର_Start >> UCPTRIE_SHIFT_3;
    ପାଇଁ (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        ତାର୍କିକ isNull = ସତ୍ୟ;
        କର {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            ଯଦି (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = ମିଥ୍ୟା;
            }
        } ଯେତେବେଳେ (++j < jLimit);
        ଯଦି (isNull) {
            flags[i] = I3_NULL;
            ଯଦି (i3FirstNull < 0) {
                ଯଦି (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                } ଅନ୍ୟଥା {
                    index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                    hasLongI3Blocks = ସତ୍ୟ;
                }
                i3FirstNull = 0;
            }
        } ଅନ୍ୟଥା {
            ଯଦି (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fastIndex, index, i);
                ଯଦି (n >= 0) {
                    flags[i] = I3_BMP;
                    index[i] = n;
                } ଅନ୍ୟଥା {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                }
            } ଅନ୍ୟଥା {
                flags[i] = I3_18;
                index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                hasLongI3Blocks = ସତ୍ୟ;
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
    ଯଦି (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    ଯଦି (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    MixedBlocks longI3Blocks;
    ଯଦି (hasLongI3Blocks) {
        ଯଦି (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    ପାଇଁ (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
        int32_t i3;
        uint8_t f = flags[i];
        ଯଦି (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = dataNullOffset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        ଯଦି (f == I3_NULL) {
            i3 = index3NullOffset;
        } ଅନ୍ୟଥା ଯଦି (f == I3_BMP) {
            i3 = index[i];
        } ଅନ୍ୟଥା ଯଦି (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, index, i);
            ଯଦି (n >= 0) {
                i3 = n;
            } ଅନ୍ୟଥା {
                ଯଦି (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ଅନ୍ୟଥା {
                    n = getOverlap(index16, indexLength,
                                   index, i, UCPTRIE_INDEX_3_BLOCK_LENGTH);
                }
                i3 = indexLength - n;
                int32_t prevIndexLength = indexLength;
                ଯେତେବେଳେ (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                    index16[indexLength++] = index[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                ଯଦି (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        } ଅନ୍ୟଥା {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
            int32_t k = indexLength;
            କର {
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
            } ଯେତେବେଳେ (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, indexLength);
            ଯଦି (n >= 0) {
                i3 = n | 0x8000;
            } ଅନ୍ୟଥା {
                ଯଦି (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } ଅନ୍ୟଥା {
                    n = getOverlap(index16, indexLength,
                                   index16, indexLength, INDEX_3_18BIT_BLOCK_LENGTH);
                }
                i3 = (indexLength - n) | 0x8000;
                int32_t prevIndexLength = indexLength;
                ଯଦି (n > 0) {
                    int32_t start = indexLength;
                    ଯେତେବେଳେ (n < INDEX_3_18BIT_BLOCK_LENGTH) {
                        index16[indexLength++] = index16[start + n++];
                    }
                } ଅନ୍ୟଥା {
                    indexLength += INDEX_3_18BIT_BLOCK_LENGTH;
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                ଯଦି (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        }
        ଯଦି (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2Length++] = i3;
    }
    U_ASSERT(i2Length == index2Capacity);
    U_ASSERT(indexLength <= index3Start + index3Capacity);

    ଯଦି (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    ଯଦି (indexLength >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    ପାଇଁ (int32_t i = 0; i < i2Length; i += blockLength) {
        int32_t n;
        ଯଦି ((i2Length - i) >= blockLength) {
            // normal block
            U_ASSERT(blockLength == UCPTRIE_INDEX_2_BLOCK_LENGTH);
            n = mixedBlocks.findBlock(index16, index2, i);
        } ଅନ୍ୟଥା {
            // highStart is inside the last index-2 block. Shorten it.
            blockLength = i2Length - i;
            n = findSameBlock(index16, index3Start, indexLength,
                              index2, i, blockLength);
        }
        int32_t i2;
        ଯଦି (n >= 0) {
            i2 = n;
        } ଅନ୍ୟଥା {
            ଯଦି (indexLength == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } ଅନ୍ୟଥା {
                n = getOverlap(index16, indexLength, index2, i, blockLength);
            }
            i2 = indexLength - n;
            int32_t prevIndexLength = indexLength;
            ଯେତେବେଳେ (n < blockLength) {
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

    ଫେରନ୍ତୁ indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((ଉପର_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    ଉପର_ମୂଲ୍ୟ = get(MAX_UNICODE);
    int32_t real_ଉପର_Start = find_ଉପର_Start();
    real_ଉପର_Start = (real_ଉପର_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    ଯଦି (real_ଉପର_Start == UNICODE_LIMIT) {
        ଉପର_ମୂଲ୍ୟ = initial_ମୂଲ୍ୟ;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_ଉପର_Start, (long)ଉପର_ମୂଲ୍ୟ, (long)initial_ମୂଲ୍ୟ);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    ଯଦି (real_ଉପର_Start < fastLimit) {
        ପାଇଁ (int32_t i = (real_ଉପର_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = ଉପର_ମୂଲ୍ୟ;
        }
        ଉପର_Start = fastLimit;
    } ଅନ୍ୟଥା {
        ଉପର_Start = real_ଉପର_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    ପାଇଁ (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    ଯଦି (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    ଯଦି (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    uprv_memcpy(newData, asciiData, ଆକାର(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    ଯଦି (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        ଯଦି (data[dataNullOffset] != initial_ମୂଲ୍ୟ) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_ମୂଲ୍ୟ, (long)data[dataNullOffset]);
        }
#endif
        initial_ମୂଲ୍ୟ = data[dataNullOffset];
    } ଅନ୍ୟଥା {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    ଉପର_Start = real_ଉପର_Start;
    ଫେରନ୍ତୁ indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ମୂଲ୍ୟ_Width < UCPTRIE_ମୂଲ୍ୟ_BITS_16 || UCPTRIE_ମୂଲ୍ୟ_BITS_8 < ମୂଲ୍ୟ_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    ବାଛ (ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        mask_ମୂଲ୍ୟଗୁଡ଼ିକ(0xffff);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        mask_ମୂଲ୍ୟଗୁଡ଼ିକ(0xff);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    ଯଦି (U_FAILURE(errorCode)) {
        clear();
        ଫେରନ୍ତୁ nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    ଯଦି (ମୂଲ୍ୟ_Width == UCPTRIE_ମୂଲ୍ୟ_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    ଯଦି (ମୂଲ୍ୟ_Width == UCPTRIE_ମୂଲ୍ୟ_BITS_16) {
        ଯଦି (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_ମୂଲ୍ୟ;
        }
        ଯଦି (data[dataLength - 1] != error_ମୂଲ୍ୟ || data[dataLength - 2] != ଉପର_ମୂଲ୍ୟ) {
            data[dataLength++] = ଉପର_ମୂଲ୍ୟ;
            data[dataLength++] = error_ମୂଲ୍ୟ;
        }
        length += dataLength * 2;
    } ଅନ୍ୟଥା ଯଦି (ମୂଲ୍ୟ_Width == UCPTRIE_ମୂଲ୍ୟ_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        ଯଦି (data[dataLength - 1] != error_ମୂଲ୍ୟ || data[dataLength - 2] != ଉପର_ମୂଲ୍ୟ) {
            ଯଦି (data[dataLength - 1] != ଉପର_ମୂଲ୍ୟ) {
                data[dataLength++] = ଉପର_ମୂଲ୍ୟ;
            }
            data[dataLength++] = error_ମୂଲ୍ୟ;
        }
        length += dataLength * 4;
    } ଅନ୍ୟଥା {
        int32_t and3 = (length + dataLength) & 3;
        ଯଦି (and3 == 0 && data[dataLength - 1] == error_ମୂଲ୍ୟ && data[dataLength - 2] == ଉପର_ମୂଲ୍ୟ) {
            // all set
        } ଅନ୍ୟଥା ଯଦି(and3 == 3 && data[dataLength - 1] == ଉପର_ମୂଲ୍ୟ) {
            data[dataLength++] = error_ମୂଲ୍ୟ;
        } ଅନ୍ୟଥା {
            ଯେତେବେଳେ (and3 != 2) {
                data[dataLength++] = ଉପର_ମୂଲ୍ୟ;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = ଉପର_ମୂଲ୍ୟ;
            data[dataLength++] = error_ମୂଲ୍ୟ;
        }
        length += dataLength;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += ଆକାର(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    ଯଦି (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        ଫେରନ୍ତୁ nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, ଆକାର(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->ଉପର_Start = ଉପର_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ଉପର_Start = (ଉପର_Start + 0xfff) >> 12;
    trie->type = type;
    trie->ମୂଲ୍ୟ_Width = ମୂଲ୍ୟ_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_ମୂଲ୍ୟ = initial_ମୂଲ୍ୟ;

    bytes += ଆକାର(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    ଯଦି (ଉପର_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        ପାଇଁ (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } ଅନ୍ୟଥା {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    ଅଚଳ uint32_t *p = data;
    ବାଛ (ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        ପାଇଁ (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)dataLength * 4);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        ପାଇଁ (int32_t i = dataLength; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // Will not occur, valueWidth checked at the beginning.
        ଭାଙ୍ଗ;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    ଫେରନ୍ତୁ trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_ମୂଲ୍ୟ, uint32_t error_ମୂଲ୍ୟ, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        ନୂତନ MutableCodePointTrie(initial_ମୂଲ୍ୟ, error_ମୂଲ୍ୟ, *pErrorCode), *pErrorCode);
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ଅଚଳ UMutableCPTrie *other, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (other == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        ନୂତନ MutableCodePointTrie(*reinterpret_cast<ଅଚଳ MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ଅଚଳ UCPMap *map, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ଅଚଳ UCPTrie *trie, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ଅଚଳ UMutableCPTrie *trie, UChar32 c) {
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ MutableCodePointTrie *>(trie)->get(c);
}

ନାମାକାଶ {

UChar32 getRange(ଅଚଳ ଶୂନ୍ୟ *trie, UChar32 start,
                 UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context, uint32_t *p_ମୂଲ୍ୟ) {
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_ମୂଲ୍ୟ);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ଅଚଳ UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_ମୂଲ୍ୟ,
                        UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context, uint32_t *p_ମୂଲ୍ୟ) {
    ଫେରନ୍ତୁ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_ମୂଲ୍ୟ,
                                    filter, context, p_ମୂଲ୍ୟ);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width,
                              UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, ମୂଲ୍ୟ_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC ଶୂନ୍ୟ umutablecptrie_setName(UMutableCPTrie *trie, ଅଚଳ ଅକ୍ଷର *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
