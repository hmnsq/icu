// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/umutablecptrie.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ifdef OVERFLOW
#undef OVERFLOW
#endif

U_NAMESPACE_BEGIN

নামাকাশ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_LIMIT = 0x110000;
constexpr int32_t BMP_LIMIT = 0x10000;
constexpr int32_t ASCII_LIMIT = 0x80;

constexpr int32_t I_LIMIT = UNICODE_LIMIT >> UCPTRIE_SHIFT_3;
constexpr int32_t BMP_I_LIMIT = BMP_LIMIT >> UCPTRIE_SHIFT_3;
constexpr int32_t ASCII_I_LIMIT = ASCII_LIMIT >> UCPTRIE_SHIFT_3;

constexpr int32_t SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK = (1 << (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_তথ্য_দৈর্ঘ্য = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_তথ্য_দৈর্ঘ্য = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_তথ্য_দৈর্ঘ্য = UNICODE_LIMIT;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য = UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য + UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য / 8;

শ্রেণী AllSameBlocks;
শ্রেণী MixedBlocks;

শ্রেণী MutableCodePointTrie : সর্বজনীন UMemory {
সর্বজনীন:
    MutableCodePointTrie(uint32_t initial_মান, uint32_t error_মান, UErrorCode &errorCode);
    MutableCodePointTrie(ধ্রুবক MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(ধ্রুবক MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(ধ্রুবক MutableCodePointTrie &other) = delete;

    স্থির MutableCodePointTrie *fromUCPMap(ধ্রুবক UCPMap *map, UErrorCode &errorCode);
    স্থির MutableCodePointTrie *fromUCPTrie(ধ্রুবক UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) ধ্রুবক;
    int32_t getRange(UChar32 start, UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context,
                     uint32_t *p_মান) ধ্রুবক;

    শূন্য set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    শূন্য setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_মান_Width মান_Width, UErrorCode &errorCode);

ব্যক্তিগত:
    শূন্য clear();

    যৌক্তিক ensure_উপরের_Start(UChar32 c);
    int32_t alloc_তথ্য_Block(int32_t block_দৈর্ঘ্য);
    int32_t get_তথ্য_Block(int32_t i);

    শূন্য mask_মানসমূহ(uint32_t mask);
    UChar32 find_উপরের_Start() ধ্রুবক;
    int32_t compact_Whole_তথ্য_Blocks(int32_t fastILimit, AllSameBlocks &allSameBlocks);
    int32_t compact_তথ্য(
            int32_t fastILimit, uint32_t *new_তথ্য, int32_t new_তথ্য_Capacity,
            int32_t তথ্য_Null_সূচক, MixedBlocks &mixedBlocks, UErrorCode &errorCode);
    int32_t compact_সূচক(int32_t fastILimit, MixedBlocks &mixedBlocks, UErrorCode &errorCode);
    int32_t compactTrie(int32_t fastILimit, UErrorCode &errorCode);

    uint32_t *সূচক = nullptr;
    int32_t সূচক_Capacity = 0;
    int32_t index3NullOffset = -1;
    uint32_t *তথ্য = nullptr;
    int32_t তথ্য_Capacity = 0;
    int32_t তথ্য_দৈর্ঘ্য = 0;
    int32_t তথ্য_Null_Offset = -1;

    uint32_t orig_Initial_মান;
    uint32_t initial_মান;
    uint32_t error_মান;
    UChar32 উপরের_Start;
    uint32_t উপরের_মান;
#ifdef UCPTRIE_DEBUG
সর্বজনীন:
    ধ্রুবক অক্ষর *name;
#endif
ব্যক্তিগত:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_মান, uint32_t err_মান, UErrorCode &errorCode) :
        orig_Initial_মান(ini_মান), initial_মান(ini_মান), error_মান(err_মান),
        উপরের_Start(0), উপরের_মান(initial_মান)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    সূচক = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    তথ্য = static_cast<uint32_t*>(uprv_malloc(INITIAL_তথ্য_দৈর্ঘ্য * 4));
    যদি (সূচক == nullptr || তথ্য == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    সূচক_Capacity = BMP_I_LIMIT;
    তথ্য_Capacity = INITIAL_তথ্য_দৈর্ঘ্য;
}

MutableCodePointTrie::MutableCodePointTrie(ধ্রুবক MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        তথ্য_Null_Offset(other.তথ্য_Null_Offset),
        orig_Initial_মান(other.orig_Initial_মান), initial_মান(other.initial_মান),
        error_মান(other.error_মান),
        উপরের_Start(other.উপরের_Start), উপরের_মান(other.উপরের_মান)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    int32_t iCapacity = উপরের_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    সূচক = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    তথ্য = static_cast<uint32_t*>(uprv_malloc(other.তথ্য_Capacity * 4));
    যদি (সূচক == nullptr || তথ্য == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    সূচক_Capacity = iCapacity;
    তথ্য_Capacity = other.তথ্য_Capacity;

    int32_t iLimit = উপরের_Start >> UCPTRIE_SHIFT_3;
    uprv_memcpy(flags, other.flags, iLimit);
    uprv_memcpy(সূচক, other.সূচক, iLimit * 4);
    uprv_memcpy(তথ্য, other.তথ্য, (size_t)other.তথ্য_দৈর্ঘ্য * 4);
    তথ্য_দৈর্ঘ্য = other.তথ্য_দৈর্ঘ্য;
    U_ASSERT(other.index16 == nullptr);
}

MutableCodePointTrie::~MutableCodePointTrie() {
    uprv_free(সূচক);
    uprv_free(তথ্য);
    uprv_free(index16);
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(ধ্রুবক UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_মান = ucpmap_get(map, -1);
    uint32_t initial_মান = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        নতুন MutableCodePointTrie(initial_মান, error_মান, errorCode),
        errorCode);
    যদি (U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    যখন ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        যদি (value != initial_মান) {
            যদি (start == end) {
                mutableTrie->set(start, value, errorCode);
            } অন্যথা {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    যদি (U_SUCCESS(errorCode)) {
        ফেরত mutableTrie.orphan();
    } অন্যথা {
        ফেরত nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(ধ্রুবক UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_মান;
    uint32_t initial_মান;
    নির্বাচন (trie->মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        error_মান = trie->তথ্য.ptr16[trie->তথ্য_দৈর্ঘ্য - UCPTRIE_ERROR_মান_NEG_তথ্য_OFFSET];
        initial_মান = trie->তথ্য.ptr16[trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET];
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        error_মান = trie->তথ্য.ptr32[trie->তথ্য_দৈর্ঘ্য - UCPTRIE_ERROR_মান_NEG_তথ্য_OFFSET];
        initial_মান = trie->তথ্য.ptr32[trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET];
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        error_মান = trie->তথ্য.ptr8[trie->তথ্য_দৈর্ঘ্য - UCPTRIE_ERROR_মান_NEG_তথ্য_OFFSET];
        initial_মান = trie->তথ্য.ptr8[trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET];
        বিরতি;
    পূর্বনির্ধারিত:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        নতুন MutableCodePointTrie(initial_মান, error_মান, errorCode),
        errorCode);
    যদি (U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    যখন ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        যদি (value != initial_মান) {
            যদি (start == end) {
                mutableTrie->set(start, value, errorCode);
            } অন্যথা {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    যদি (U_SUCCESS(errorCode)) {
        ফেরত mutableTrie.orphan();
    } অন্যথা {
        ফেরত nullptr;
    }
}

শূন্য MutableCodePointTrie::clear() {
    index3NullOffset = তথ্য_Null_Offset = -1;
    তথ্য_দৈর্ঘ্য = 0;
    উপরের_মান = initial_মান = orig_Initial_মান;
    উপরের_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) ধ্রুবক {
    যদি (static_cast<uint32_t>(c) > MAX_UNICODE) {
        ফেরত error_মান;
    }
    যদি (c >= উপরের_Start) {
        ফেরত উপরের_মান;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    যদি (flags[i] == ALL_SAME) {
        ফেরত সূচক[i];
    } অন্যথা {
        ফেরত তথ্য[সূচক[i] + (c & UCPTRIE_SMALL_তথ্য_MASK)];
    }
}

অন্তঃরেখ uint32_t maybe_Filter_মান(uint32_t value, uint32_t initial_মান, uint32_t null_মান,
                                 UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context) {
    যদি (value == initial_মান) {
        value = null_মান;
    } অন্যথা যদি (filter != nullptr) {
        value = filter(context, value);
    }
    ফেরত value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context,
        uint32_t *p_মান) ধ্রুবক {
    যদি (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ফেরত U_SENTINEL;
    }
    যদি (start >= উপরের_Start) {
        যদি (p_মান != nullptr) {
            uint32_t value = উপরের_মান;
            যদি (filter != nullptr) { value = filter(context, value); }
            *p_মান = value;
        }
        ফেরত MAX_UNICODE;
    }
    uint32_t null_মান = initial_মান;
    যদি (filter != nullptr) { null_মান = filter(context, null_মান); }
    UChar32 c = start;
    uint32_t trie_মান, value;
    যৌক্তিক have_মান = মিথ্যা;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    করো {
        যদি (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = সূচক[i];
            যদি (have_মান) {
                যদি (trieValue2 != trie_মান) {
                    যদি (filter == nullptr ||
                            maybe_Filter_মান(trieValue2, initial_মান, null_মান,
                                             filter, context) != value) {
                        ফেরত c - 1;
                    }
                    trie_মান = trieValue2;  // may or may not help
                }
            } অন্যথা {
                trie_মান = trieValue2;
                value = maybe_Filter_মান(trieValue2, initial_মান, null_মান, filter, context);
                যদি (p_মান != nullptr) { *p_মান = value; }
                have_মান = সত্য;
            }
            c = (c + UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য) & ~UCPTRIE_SMALL_তথ্য_MASK;
        } অন্যথা /* MIXED */ {
            int32_t di = সূচক[i] + (c & UCPTRIE_SMALL_তথ্য_MASK);
            uint32_t trieValue2 = তথ্য[di];
            যদি (have_মান) {
                যদি (trieValue2 != trie_মান) {
                    যদি (filter == nullptr ||
                            maybe_Filter_মান(trieValue2, initial_মান, null_মান,
                                             filter, context) != value) {
                        ফেরত c - 1;
                    }
                    trie_মান = trieValue2;  // may or may not help
                }
            } অন্যথা {
                trie_মান = trieValue2;
                value = maybe_Filter_মান(trieValue2, initial_মান, null_মান, filter, context);
                যদি (p_মান != nullptr) { *p_মান = value; }
                have_মান = সত্য;
            }
            যখন ((++c & UCPTRIE_SMALL_তথ্য_MASK) != 0) {
                trieValue2 = তথ্য[++di];
                যদি (trieValue2 != trie_মান) {
                    যদি (filter == nullptr ||
                            maybe_Filter_মান(trieValue2, initial_মান, null_মান,
                                             filter, context) != value) {
                        ফেরত c - 1;
                    }
                }
                trie_মান = trieValue2;  // may or may not help
            }
        }
        ++i;
    } যখন (c < উপরের_Start);
    U_ASSERT(have_মান);
    যদি (maybe_Filter_মান(উপরের_মান, initial_মান, null_মান,
                         filter, context) != value) {
        ফেরত c - 1;
    } অন্যথা {
        ফেরত MAX_UNICODE;
    }
}

শূন্য
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
    যখন (block < limit) {
        *block++ = value;
    }
}

যৌক্তিক MutableCodePointTrie::ensure_উপরের_Start(UChar32 c) {
    যদি (c >= উপরের_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_সূচক_2_ENTRY) & ~(UCPTRIE_CP_PER_সূচক_2_ENTRY - 1);
        int32_t i = উপরের_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        যদি (iLimit > সূচক_Capacity) {
            uint32_t* new_সূচক = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            যদি (new_সূচক == nullptr) { ফেরত মিথ্যা; }
            uprv_memcpy(new_সূচক, সূচক, i * 4);
            uprv_free(সূচক);
            সূচক = new_সূচক;
            সূচক_Capacity = I_LIMIT;
        }
        করো {
            flags[i] = ALL_SAME;
            সূচক[i] = initial_মান;
        } যখন(++i < iLimit);
        উপরের_Start = c;
    }
    ফেরত সত্য;
}

int32_t MutableCodePointTrie::alloc_তথ্য_Block(int32_t block_দৈর্ঘ্য) {
    int32_t newBlock = তথ্য_দৈর্ঘ্য;
    int32_t new_শীর্ষ = newBlock + block_দৈর্ঘ্য;
    যদি (new_শীর্ষ > তথ্য_Capacity) {
        int32_t capacity;
        যদি (তথ্য_Capacity < MEDIUM_তথ্য_দৈর্ঘ্য) {
            capacity = MEDIUM_তথ্য_দৈর্ঘ্য;
        } অন্যথা যদি (তথ্য_Capacity < MAX_তথ্য_দৈর্ঘ্য) {
            capacity = MAX_তথ্য_দৈর্ঘ্য;
        } অন্যথা {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            ফেরত -1;
        }
        uint32_t* new_তথ্য = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        যদি (new_তথ্য == nullptr) {
            ফেরত -1;
        }
        uprv_memcpy(new_তথ্য, তথ্য, (size_t)তথ্য_দৈর্ঘ্য * 4);
        uprv_free(তথ্য);
        তথ্য = new_তথ্য;
        তথ্য_Capacity = capacity;
    }
    তথ্য_দৈর্ঘ্য = new_শীর্ষ;
    ফেরত newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::get_তথ্য_Block(int32_t i) {
    যদি (flags[i] == MIXED) {
        ফেরত সূচক[i];
    }
    যদি (i < BMP_I_LIMIT) {
        int32_t newBlock = alloc_তথ্য_Block(UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য);
        যদি (newBlock < 0) { ফেরত newBlock; }
        int32_t iStart = i & ~(SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK;
        করো {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(তথ্য + newBlock, সূচক[iStart]);
            flags[iStart] = MIXED;
            সূচক[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
        } যখন (iStart < iLimit);
        ফেরত সূচক[i];
    } অন্যথা {
        int32_t newBlock = alloc_তথ্য_Block(UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য);
        যদি (newBlock < 0) { ফেরত newBlock; }
        writeBlock(তথ্য + newBlock, সূচক[i]);
        flags[i] = MIXED;
        সূচক[i] = newBlock;
        ফেরত newBlock;
    }
}

শূন্য MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    int32_t block;
    যদি (!ensure_উপরের_Start(c) || (block = get_তথ্য_Block(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    তথ্য[block + (c & UCPTRIE_SMALL_তথ্য_MASK)] = value;
}

শূন্য
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    যখন (block < pLimit) {
        *block++ = value;
    }
}

শূন্য MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি (!ensure_উপরের_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    UChar32 limit = end + 1;
    যদি (start & UCPTRIE_SMALL_তথ্য_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = get_তথ্য_Block(start >> UCPTRIE_SHIFT_3);
        যদি (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }

        UChar32 পরবর্তী_Start = (start + UCPTRIE_SMALL_তথ্য_MASK) & ~UCPTRIE_SMALL_তথ্য_MASK;
        যদি (পরবর্তী_Start <= limit) {
            fillBlock(তথ্য + block, start & UCPTRIE_SMALL_তথ্য_MASK, UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য,
                      value);
            start = পরবর্তী_Start;
        } অন্যথা {
            fillBlock(তথ্য + block, start & UCPTRIE_SMALL_তথ্য_MASK, limit & UCPTRIE_SMALL_তথ্য_MASK,
                      value);
            ফেরত;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_তথ্য_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_তথ্য_MASK;

    // Iterate over all-value blocks.
    যখন (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        যদি (flags[i] == ALL_SAME) {
            সূচক[i] = value;
        } অন্যথা /* MIXED */ {
            fillBlock(তথ্য + সূচক[i], 0, UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য, value);
        }
        start += UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
    }

    যদি (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = get_তথ্য_Block(start >> UCPTRIE_SHIFT_3);
        যদি (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }

        fillBlock(তথ্য + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

শূন্য MutableCodePointTrie::mask_মানসমূহ(uint32_t mask) {
    initial_মান &= mask;
    error_মান &= mask;
    উপরের_মান &= mask;
    int32_t iLimit = উপরের_Start >> UCPTRIE_SHIFT_3;
    জন্য (int32_t i = 0; i < iLimit; ++i) {
        যদি (flags[i] == ALL_SAME) {
            সূচক[i] &= mask;
        }
    }
    জন্য (int32_t i = 0; i < তথ্য_দৈর্ঘ্য; ++i) {
        তথ্য[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
যৌক্তিক equalBlocks(ধ্রুবক UIntA *s, ধ্রুবক UIntB *t, int32_t length) {
    যখন (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    ফেরত length == 0;
}

যৌক্তিক all_মানসমূহ_Same_As(ধ্রুবক uint32_t *p, int32_t length, uint32_t value) {
    ধ্রুবক uint32_t *pLimit = p + length;
    যখন (p < pLimit && *p == value) { ++p; }
    ফেরত p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(ধ্রুবক uint16_t *p, int32_t pStart, int32_t length,
                      ধ্রুবক uint16_t *q, int32_t qStart, int32_t block_দৈর্ঘ্য) {
    // Ensure that we do not even partially get past length.
    length -= block_দৈর্ঘ্য;

    q += qStart;
    যখন (pStart <= length) {
        যদি (equalBlocks(p + pStart, q, block_দৈর্ঘ্য)) {
            ফেরত pStart;
        }
        ++pStart;
    }
    ফেরত -1;
}

int32_t findAllSameBlock(ধ্রুবক uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t block_দৈর্ঘ্য) {
    // Ensure that we do not even partially get past limit.
    limit -= block_দৈর্ঘ্য;

    জন্য (int32_t block = start; block <= limit; ++block) {
        যদি (p[block] == value) {
            জন্য (int32_t i = 1;; ++i) {
                যদি (i == block_দৈর্ঘ্য) {
                    ফেরত block;
                }
                যদি (p[block + i] != value) {
                    block += i;
                    বিরতি;
                }
            }
        }
    }
    ফেরত -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
template<typename UIntA, typename UIntB>
int32_t getOverlap(ধ্রুবক UIntA *p, int32_t length,
                   ধ্রুবক UIntB *q, int32_t qStart, int32_t block_দৈর্ঘ্য) {
    int32_t overlap = block_দৈর্ঘ্য - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    যখন (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    ফেরত overlap;
}

int32_t getAllSameOverlap(ধ্রুবক uint32_t *p, int32_t length, uint32_t value,
                          int32_t block_দৈর্ঘ্য) {
    int32_t min = length - (block_দৈর্ঘ্য - 1);
    int32_t i = length;
    যখন (min < i && p[i - 1] == value) { --i; }
    ফেরত length - i;
}

যৌক্তিক isStartOfSomeFastBlock(uint32_t তথ্য_Offset, ধ্রুবক uint32_t সূচক[], int32_t fastILimit) {
    জন্য (int32_t i = 0; i < fastILimit; i += SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK) {
        যদি (সূচক[i] == তথ্য_Offset) {
            ফেরত সত্য;
        }
    }
    ফেরত মিথ্যা;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_উপরের_Start() ধ্রুবক {
    int32_t i = উপরের_Start >> UCPTRIE_SHIFT_3;
    যখন (i > 0) {
        যৌক্তিক match;
        যদি (flags[--i] == ALL_SAME) {
            match = সূচক[i] == উপরের_মান;
        } অন্যথা /* MIXED */ {
            ধ্রুবক uint32_t *p = তথ্য + সূচক[i];
            জন্য (int32_t j = 0;; ++j) {
                যদি (j == UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য) {
                    match = সত্য;
                    বিরতি;
                }
                যদি (p[j] != উপরের_মান) {
                    match = মিথ্যা;
                    বিরতি;
                }
            }
        }
        যদি (!match) {
            ফেরত (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    ফেরত 0;
}

শ্রেণী AllSameBlocks {
সর্বজনীন:
    স্থির constexpr int32_t NEW_UNIQUE = -1;
    স্থির constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t সূচক, int32_t count, uint32_t value) {
        যদি (mostRecent >= 0 && মানসমূহ[mostRecent] == value) {
            refCounts[mostRecent] += count;
            ফেরত indexes[mostRecent];
        }
        জন্য (int32_t i = 0; i < length; ++i) {
            যদি (মানসমূহ[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                ফেরত indexes[i];
            }
        }
        যদি (length == CAPACITY) {
            ফেরত OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = সূচক;
        মানসমূহ[length] = value;
        refCounts[length++] = count;
        ফেরত NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    শূন্য add(int32_t সূচক, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_গুনতি = I_LIMIT;
        জন্য (int32_t i = 0; i < length; ++i) {
            U_ASSERT(মানসমূহ[i] != value);
            যদি (refCounts[i] < least_গুনতি) {
                least = i;
                least_গুনতি = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = সূচক;
        মানসমূহ[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() ধ্রুবক {
        যদি (length == 0) { ফেরত -1; }
        int32_t max = -1;
        int32_t max_গুনতি = 0;
        জন্য (int32_t i = 0; i < length; ++i) {
            যদি (refCounts[i] > max_গুনতি) {
                max = i;
                max_গুনতি = refCounts[i];
            }
        }
        ফেরত indexes[max];
    }

ব্যক্তিগত:
    স্থির constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t মানসমূহ[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
শ্রেণী MixedBlocks {
সর্বজনীন:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    যৌক্তিক init(int32_t max_দৈর্ঘ্য_2, int32_t new_Block_দৈর্ঘ্য) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_তথ্য_সূচক = max_দৈর্ঘ্য_2 - new_Block_দৈর্ঘ্য + 1;
        int32_t new_দৈর্ঘ্য;
        যদি (max_তথ্য_সূচক <= 0xfff) {  // 4k
            new_দৈর্ঘ্য = 6007;
            shift = 12;
            mask = 0xfff;
        } অন্যথা যদি (max_তথ্য_সূচক <= 0x7fff) {  // 32k
            new_দৈর্ঘ্য = 50021;
            shift = 15;
            mask = 0x7fff;
        } অন্যথা যদি (max_তথ্য_সূচক <= 0x1ffff) {  // 128k
            new_দৈর্ঘ্য = 200003;
            shift = 17;
            mask = 0x1ffff;
        } অন্যথা {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_দৈর্ঘ্য = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        যদি (new_দৈর্ঘ্য > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(new_দৈর্ঘ্য * 4));
            যদি (table == nullptr) {
                ফেরত মিথ্যা;
            }
            capacity = new_দৈর্ঘ্য;
        }
        length = new_দৈর্ঘ্য;
        uprv_memset(table, 0, length * 4);

        block_দৈর্ঘ্য = new_Block_দৈর্ঘ্য;
        ফেরত সত্য;
    }

    template<typename UInt>
    শূন্য extend(ধ্রুবক UInt *তথ্য, int32_t minStart, int32_t পূর্ব_তথ্য_দৈর্ঘ্য, int32_t new_তথ্য_দৈর্ঘ্য) {
        int32_t start = পূর্ব_তথ্য_দৈর্ঘ্য - block_দৈর্ঘ্য;
        যদি (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } অন্যথা {
            start = minStart;  // Begin with the first full block.
        }
        জন্য (int32_t end = new_তথ্য_দৈর্ঘ্য - block_দৈর্ঘ্য; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(তথ্য, start);
            addEntry(তথ্য, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(ধ্রুবক UIntA *তথ্য, ধ্রুবক UIntB *block_তথ্য, int32_t blockStart) ধ্রুবক {
        uint32_t hashCode = makeHashCode(block_তথ্য, blockStart);
        int32_t entry_সূচক = findEntry(তথ্য, block_তথ্য, blockStart, hashCode);
        যদি (entry_সূচক >= 0) {
            ফেরত (table[entry_সূচক] & mask) - 1;
        } অন্যথা {
            ফেরত -1;
        }
    }

    int32_t findAllSameBlock(ধ্রুবক uint32_t *তথ্য, uint32_t block_মান) ধ্রুবক {
        uint32_t hashCode = makeHashCode(block_মান);
        int32_t entry_সূচক = findEntry(তথ্য, block_মান, hashCode);
        যদি (entry_সূচক >= 0) {
            ফেরত (table[entry_সূচক] & mask) - 1;
        } অন্যথা {
            ফেরত -1;
        }
    }

ব্যক্তিগত:
    template<typename UInt>
    uint32_t makeHashCode(ধ্রুবক UInt *block_তথ্য, int32_t blockStart) ধ্রুবক {
        int32_t blockLimit = blockStart + block_দৈর্ঘ্য;
        uint32_t hashCode = block_তথ্য[blockStart++];
        করো {
            hashCode = 37 * hashCode + block_তথ্য[blockStart++];
        } যখন (blockStart < blockLimit);
        ফেরত hashCode;
    }

    uint32_t makeHashCode(uint32_t block_মান) ধ্রুবক {
        uint32_t hashCode = block_মান;
        জন্য (int32_t i = 1; i < block_দৈর্ঘ্য; ++i) {
            hashCode = 37 * hashCode + block_মান;
        }
        ফেরত hashCode;
    }

    template<typename UInt>
    শূন্য addEntry(ধ্রুবক UInt *তথ্য, int32_t blockStart, uint32_t hashCode, int32_t তথ্য_সূচক) {
        U_ASSERT(0 <= তথ্য_সূচক && তথ্য_সূচক < (int32_t)mask);
        int32_t entry_সূচক = findEntry(তথ্য, তথ্য, blockStart, hashCode);
        যদি (entry_সূচক < 0) {
            table[~entry_সূচক] = (hashCode << shift) | (তথ্য_সূচক + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(ধ্রুবক UIntA *তথ্য, ধ্রুবক UIntB *block_তথ্য, int32_t blockStart,
                      uint32_t hashCode) ধ্রুবক {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initial_Entry_সূচক = (hashCode % (length - 1)) + 1;  // 1..length-1
        জন্য (int32_t entry_সূচক = initial_Entry_সূচক;;) {
            uint32_t entry = table[entry_সূচক];
            যদি (entry == 0) {
                ফেরত ~entry_সূচক;
            }
            যদি ((entry & ~mask) == shiftedHashCode) {
                int32_t তথ্য_সূচক = (entry & mask) - 1;
                যদি (equalBlocks(তথ্য + তথ্য_সূচক, block_তথ্য + blockStart, block_দৈর্ঘ্য)) {
                    ফেরত entry_সূচক;
                }
            }
            entry_সূচক = পরবর্তী_সূচক(initial_Entry_সূচক, entry_সূচক);
        }
    }

    int32_t findEntry(ধ্রুবক uint32_t *তথ্য, uint32_t block_মান, uint32_t hashCode) ধ্রুবক {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initial_Entry_সূচক = (hashCode % (length - 1)) + 1;  // 1..length-1
        জন্য (int32_t entry_সূচক = initial_Entry_সূচক;;) {
            uint32_t entry = table[entry_সূচক];
            যদি (entry == 0) {
                ফেরত ~entry_সূচক;
            }
            যদি ((entry & ~mask) == shiftedHashCode) {
                int32_t তথ্য_সূচক = (entry & mask) - 1;
                যদি (all_মানসমূহ_Same_As(তথ্য + তথ্য_সূচক, block_দৈর্ঘ্য, block_মান)) {
                    ফেরত entry_সূচক;
                }
            }
            entry_সূচক = পরবর্তী_সূচক(initial_Entry_সূচক, entry_সূচক);
        }
    }

    অন্তঃরেখ int32_t পরবর্তী_সূচক(int32_t initial_Entry_সূচক, int32_t entry_সূচক) ধ্রুবক {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        ফেরত (entry_সূচক + initial_Entry_সূচক) % length;
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

    int32_t block_দৈর্ঘ্য = 0;
};

int32_t MutableCodePointTrie::compact_Whole_তথ্য_Blocks(int32_t fastILimit, AllSameBlocks &allSameBlocks) {
#ifdef UCPTRIE_DEBUG
    যৌক্তিক overflow = মিথ্যা;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_তথ্য_Capacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_তথ্য_Capacity += UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
    // Add room for special values (errorValue, highValue) and padding.
    new_তথ্য_Capacity += 4;
    int32_t iLimit = উপরের_Start >> UCPTRIE_SHIFT_3;
    int32_t block_দৈর্ঘ্য = UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য;
    int32_t inc = SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK;
    জন্য (int32_t i = 0; i < iLimit; i += inc) {
        যদি (i == fastILimit) {
            block_দৈর্ঘ্য = UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
            inc = 1;
        }
        uint32_t value = সূচক[i];
        যদি (flags[i] == MIXED) {
            // Really mixed?
            ধ্রুবক uint32_t *p = তথ্য + value;
            value = *p;
            যদি (all_মানসমূহ_Same_As(p + 1, block_দৈর্ঘ্য - 1, value)) {
                flags[i] = ALL_SAME;
                সূচক[i] = value;
                // Fall through to ALL_SAME handling.
            } অন্যথা {
                new_তথ্য_Capacity += block_দৈর্ঘ্য;
                চালু;
            }
        } অন্যথা {
            U_ASSERT(flags[i] == ALL_SAME);
            যদি (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                যৌক্তিক allSame = সত্য;
                int32_t পরবর্তী_i = i + inc;
                জন্য (int32_t j = i + 1; j < পরবর্তী_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    যদি (সূচক[j] != value) {
                        allSame = মিথ্যা;
                        বিরতি;
                    }
                }
                যদি (!allSame) {
                    // Turn it into a MIXED block.
                    যদি (get_তথ্য_Block(i) < 0) {
                        ফেরত -1;
                    }
                    new_তথ্য_Capacity += block_দৈর্ঘ্য;
                    চালু;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        যদি (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            যদি (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = সত্য;
            }
#endif
            int32_t jInc = SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK;
            জন্য (int32_t j = 0;; j += jInc) {
                যদি (j == i) {
                    allSameBlocks.add(i, inc, value);
                    বিরতি;
                }
                যদি (j == fastILimit) {
                    jInc = 1;
                }
                যদি (flags[j] == ALL_SAME && সূচক[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    বিরতি;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        যদি (other >= 0) {
            flags[i] = SAME_AS;
            সূচক[i] = other;
        } অন্যথা {
            // New unique same-value block.
            new_তথ্য_Capacity += block_দৈর্ঘ্য;
        }
    }
    ফেরত new_তথ্য_Capacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(রাশিমালা) রাশিমালা
#অন্যথা
#   define DEBUG_DO(রাশিমালা)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_মান(অক্ষর s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (অক্ষর)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (অক্ষর)(0x80 + (value & 0x3F));
    ফেরত length + 3;
}

শূন্য printBlock(ধ্রুবক uint32_t *block, int32_t block_দৈর্ঘ্য, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_মান) {
    অক্ষর s[UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য * 3 + 3];
    int32_t length = 0;
    int32_t i;
    জন্য (i = 0; i < overlap; ++i) {
        length = append_মান(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    জন্য (; i < block_দৈর্ঘ্য; ++i) {
        যদি (block != nullptr) {
            value = block[i];
        }
        যদি (value == initial_মান) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_মান(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    যদি (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } অন্যথা যদি (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } অন্যথা {
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
int32_t MutableCodePointTrie::compact_তথ্য(
        int32_t fastILimit, uint32_t *new_তথ্য, int32_t new_তথ্য_Capacity,
        int32_t তথ্য_Null_সূচক, MixedBlocks &mixedBlocks, UErrorCode &errorCode) {
#ifdef UCPTRIE_DEBUG
    int32_t গুনতি_Same=0, যোগ_Overlaps=0;
    যৌক্তিক print_তথ্য = তথ্য_দৈর্ঘ্য == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        তথ্য_দৈর্ঘ্য == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t new_তথ্য_দৈর্ঘ্য = 0;
    জন্য (int32_t i = 0; new_তথ্য_দৈর্ঘ্য < ASCII_LIMIT;
            new_তথ্য_দৈর্ঘ্য += UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য, i += SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK) {
        সূচক[i] = new_তথ্য_দৈর্ঘ্য;
#ifdef UCPTRIE_DEBUG
        যদি (print_তথ্য) {
            printBlock(new_তথ্য + new_তথ্য_দৈর্ঘ্য, UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য, 0, new_তথ্য_দৈর্ঘ্য, 0, initial_মান);
        }
#endif
    }

    int32_t block_দৈর্ঘ্য = UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য;
    যদি (!mixedBlocks.init(new_তথ্য_Capacity, block_দৈর্ঘ্য)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    mixedBlocks.extend(new_তথ্য, 0, 0, new_তথ্য_দৈর্ঘ্য);

    int32_t iLimit = উপরের_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK;
    int32_t fast_দৈর্ঘ্য = 0;
    জন্য (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        যদি (i == fastILimit) {
            block_দৈর্ঘ্য = UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
            inc = 1;
            fast_দৈর্ঘ্য = new_তথ্য_দৈর্ঘ্য;
            যদি (!mixedBlocks.init(new_তথ্য_Capacity, block_দৈর্ঘ্য)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত 0;
            }
            mixedBlocks.extend(new_তথ্য, 0, 0, new_তথ্য_দৈর্ঘ্য);
        }
        যদি (flags[i] == ALL_SAME) {
            uint32_t value = সূচক[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixedBlocks.findAllSameBlock(new_তথ্য, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            যখন (n >= 0 && i == তথ্য_Null_সূচক && i >= fastILimit && n < fast_দৈর্ঘ্য &&
                    isStartOfSomeFastBlock(n, সূচক, fastILimit)) {
                n = findAllSameBlock(new_তথ্য, n + 1, new_তথ্য_দৈর্ঘ্য, value, block_দৈর্ঘ্য);
            }
            যদি (n >= 0) {
                DEBUG_DO(++গুনতি_Same);
                সূচক[i] = n;
            } অন্যথা {
                n = getAllSameOverlap(new_তথ্য, new_তথ্য_দৈর্ঘ্য, value, block_দৈর্ঘ্য);
                DEBUG_DO(যোগ_Overlaps += n);
#ifdef UCPTRIE_DEBUG
                যদি (print_তথ্য) {
                    printBlock(nullptr, block_দৈর্ঘ্য, value, i << UCPTRIE_SHIFT_3, n, initial_মান);
                }
#endif
                সূচক[i] = new_তথ্য_দৈর্ঘ্য - n;
                int32_t পূর্ব_তথ্য_দৈর্ঘ্য = new_তথ্য_দৈর্ঘ্য;
                যখন (n < block_দৈর্ঘ্য) {
                    new_তথ্য[new_তথ্য_দৈর্ঘ্য++] = value;
                    ++n;
                }
                mixedBlocks.extend(new_তথ্য, 0, পূর্ব_তথ্য_দৈর্ঘ্য, new_তথ্য_দৈর্ঘ্য);
            }
        } অন্যথা যদি (flags[i] == MIXED) {
            ধ্রুবক uint32_t *block = তথ্য + সূচক[i];
            int32_t n = mixedBlocks.findBlock(new_তথ্য, block, 0);
            যদি (n >= 0) {
                DEBUG_DO(++গুনতি_Same);
                সূচক[i] = n;
            } অন্যথা {
                n = getOverlap(new_তথ্য, new_তথ্য_দৈর্ঘ্য, block, 0, block_দৈর্ঘ্য);
                DEBUG_DO(যোগ_Overlaps += n);
#ifdef UCPTRIE_DEBUG
                যদি (print_তথ্য) {
                    printBlock(block, block_দৈর্ঘ্য, 0, i << UCPTRIE_SHIFT_3, n, initial_মান);
                }
#endif
                সূচক[i] = new_তথ্য_দৈর্ঘ্য - n;
                int32_t পূর্ব_তথ্য_দৈর্ঘ্য = new_তথ্য_দৈর্ঘ্য;
                যখন (n < block_দৈর্ঘ্য) {
                    new_তথ্য[new_তথ্য_দৈর্ঘ্য++] = block[n++];
                }
                mixedBlocks.extend(new_তথ্য, 0, পূর্ব_তথ্য_দৈর্ঘ্য, new_তথ্য_দৈর্ঘ্য);
            }
        } অন্যথা /* SAME_AS */ {
            uint32_t j = সূচক[i];
            সূচক[i] = সূচক[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)তথ্য_দৈর্ঘ্য, (long)new_তথ্য_দৈর্ঘ্য, (long)গুনতি_Same, (long)যোগ_Overlaps);
#endif
    ফেরত new_তথ্য_দৈর্ঘ্য;
}

int32_t MutableCodePointTrie::compact_সূচক(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fast_সূচক_দৈর্ঘ্য = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    যদি ((উপরের_Start >> UCPTRIE_FAST_SHIFT) <= fast_সূচক_দৈর্ঘ্য) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        ফেরত fast_সূচক_দৈর্ঘ্য;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_সূচক[UCPTRIE_BMP_সূচক_দৈর্ঘ্য];  // fastIndexLength
    int32_t i3FirstNull = -1;
    জন্য (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = সূচক[i];
        fast_সূচক[j] = static_cast<uint16_t>(i3);
        যদি (i3 == static_cast<uint32_t>(তথ্য_Null_Offset)) {
            যদি (i3FirstNull < 0) {
                i3FirstNull = j;
            } অন্যথা যদি (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য) {
                index3NullOffset = i3FirstNull;
            }
        } অন্যথা {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_পরবর্তী = i + SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK;
        যখন (++i < i_পরবর্তী) {
            i3 += UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
            সূচক[i] = i3;
        }
    }

    যদি (!mixedBlocks.init(fast_সূচক_দৈর্ঘ্য, UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    mixedBlocks.extend(fast_সূচক, 0, 0, fast_সূচক_দৈর্ঘ্য);

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
    যৌক্তিক hasLongI3Blocks = মিথ্যা;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = উপরের_Start >> UCPTRIE_SHIFT_3;
    জন্য (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য;
        uint32_t oredI3 = 0;
        যৌক্তিক isNull = সত্য;
        করো {
            uint32_t i3 = সূচক[j];
            oredI3 |= i3;
            যদি (i3 != static_cast<uint32_t>(তথ্য_Null_Offset)) {
                isNull = মিথ্যা;
            }
        } যখন (++j < jLimit);
        যদি (isNull) {
            flags[i] = I3_NULL;
            যদি (i3FirstNull < 0) {
                যদি (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য;
                } অন্যথা {
                    index3Capacity += সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য;
                    hasLongI3Blocks = সত্য;
                }
                i3FirstNull = 0;
            }
        } অন্যথা {
            যদি (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fast_সূচক, সূচক, i);
                যদি (n >= 0) {
                    flags[i] = I3_BMP;
                    সূচক[i] = n;
                } অন্যথা {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য;
                }
            } অন্যথা {
                flags[i] = I3_18;
                index3Capacity += সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য;
                hasLongI3Blocks = সত্য;
            }
        }
        i = j;
    }

    int32_t index2Capacity = (iLimit - iStart) >> UCPTRIE_SHIFT_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_দৈর্ঘ্য = (index2Capacity + UCPTRIE_সূচক_2_MASK) >> UCPTRIE_SHIFT_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16Capacity = fast_সূচক_দৈর্ঘ্য + index1_দৈর্ঘ্য + index3Capacity + index2Capacity + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16Capacity * 2));
    যদি (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    uprv_memcpy(index16, fast_সূচক, fast_সূচক_দৈর্ঘ্য * 2);

    যদি (!mixedBlocks.init(index16Capacity, UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    MixedBlocks longI3Blocks;
    যদি (hasLongI3Blocks) {
        যদি (!longI3Blocks.init(index16Capacity, সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2_দৈর্ঘ্য = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fast_সূচক_দৈর্ঘ্য + index1_দৈর্ঘ্য;
    int32_t সূচক_দৈর্ঘ্য = index3Start;
    জন্য (int32_t i = iStart; i < iLimit; i += UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য) {
        int32_t i3;
        uint8_t f = flags[i];
        যদি (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = তথ্য_Null_Offset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        যদি (f == I3_NULL) {
            i3 = index3NullOffset;
        } অন্যথা যদি (f == I3_BMP) {
            i3 = সূচক[i];
        } অন্যথা যদি (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, সূচক, i);
            যদি (n >= 0) {
                i3 = n;
            } অন্যথা {
                যদি (সূচক_দৈর্ঘ্য == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } অন্যথা {
                    n = getOverlap(index16, সূচক_দৈর্ঘ্য,
                                   সূচক, i, UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য);
                }
                i3 = সূচক_দৈর্ঘ্য - n;
                int32_t পূর্ব_সূচক_দৈর্ঘ্য = সূচক_দৈর্ঘ্য;
                যখন (n < UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য) {
                    index16[সূচক_দৈর্ঘ্য++] = সূচক[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, পূর্ব_সূচক_দৈর্ঘ্য, সূচক_দৈর্ঘ্য);
                যদি (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, পূর্ব_সূচক_দৈর্ঘ্য, সূচক_দৈর্ঘ্য);
                }
            }
        } অন্যথা {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য;
            int32_t k = সূচক_দৈর্ঘ্য;
            করো {
                ++k;
                uint32_t v = সূচক[j++];
                uint32_t upperBits = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = সূচক[j++];
                upperBits |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = upperBits;
            } যখন (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, সূচক_দৈর্ঘ্য);
            যদি (n >= 0) {
                i3 = n | 0x8000;
            } অন্যথা {
                যদি (সূচক_দৈর্ঘ্য == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } অন্যথা {
                    n = getOverlap(index16, সূচক_দৈর্ঘ্য,
                                   index16, সূচক_দৈর্ঘ্য, সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য);
                }
                i3 = (সূচক_দৈর্ঘ্য - n) | 0x8000;
                int32_t পূর্ব_সূচক_দৈর্ঘ্য = সূচক_দৈর্ঘ্য;
                যদি (n > 0) {
                    int32_t start = সূচক_দৈর্ঘ্য;
                    যখন (n < সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য) {
                        index16[সূচক_দৈর্ঘ্য++] = index16[start + n++];
                    }
                } অন্যথা {
                    সূচক_দৈর্ঘ্য += সূচক_3_18_BIT_BLOCK_দৈর্ঘ্য;
                }
                mixedBlocks.extend(index16, index3Start, পূর্ব_সূচক_দৈর্ঘ্য, সূচক_দৈর্ঘ্য);
                যদি (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, পূর্ব_সূচক_দৈর্ঘ্য, সূচক_দৈর্ঘ্য);
                }
            }
        }
        যদি (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2_দৈর্ঘ্য++] = i3;
    }
    U_ASSERT(i2_দৈর্ঘ্য == index2Capacity);
    U_ASSERT(সূচক_দৈর্ঘ্য <= index3Start + index3Capacity);

    যদি (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    যদি (সূচক_দৈর্ঘ্য >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_সূচক_2_BLOCK_দৈর্ঘ্য == UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য,
                  "must re-init mixedBlocks");
    int32_t block_দৈর্ঘ্য = UCPTRIE_সূচক_2_BLOCK_দৈর্ঘ্য;
    int32_t i1 = fast_সূচক_দৈর্ঘ্য;
    জন্য (int32_t i = 0; i < i2_দৈর্ঘ্য; i += block_দৈর্ঘ্য) {
        int32_t n;
        যদি ((i2_দৈর্ঘ্য - i) >= block_দৈর্ঘ্য) {
            // normal block
            U_ASSERT(block_দৈর্ঘ্য == UCPTRIE_সূচক_2_BLOCK_দৈর্ঘ্য);
            n = mixedBlocks.findBlock(index16, index2, i);
        } অন্যথা {
            // highStart is inside the last index-2 block. Shorten it.
            block_দৈর্ঘ্য = i2_দৈর্ঘ্য - i;
            n = findSameBlock(index16, index3Start, সূচক_দৈর্ঘ্য,
                              index2, i, block_দৈর্ঘ্য);
        }
        int32_t i2;
        যদি (n >= 0) {
            i2 = n;
        } অন্যথা {
            যদি (সূচক_দৈর্ঘ্য == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } অন্যথা {
                n = getOverlap(index16, সূচক_দৈর্ঘ্য, index2, i, block_দৈর্ঘ্য);
            }
            i2 = সূচক_দৈর্ঘ্য - n;
            int32_t পূর্ব_সূচক_দৈর্ঘ্য = সূচক_দৈর্ঘ্য;
            যখন (n < block_দৈর্ঘ্য) {
                index16[সূচক_দৈর্ঘ্য++] = index2[i + n++];
            }
            mixedBlocks.extend(index16, index3Start, পূর্ব_সূচক_দৈর্ঘ্য, সূচক_দৈর্ঘ্য);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3Start);
    U_ASSERT(সূচক_দৈর্ঘ্য <= index16Capacity);

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (long)iLimit, (long)সূচক_দৈর্ঘ্য);
#endif

    ফেরত সূচক_দৈর্ঘ্য;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((উপরের_Start & (UCPTRIE_CP_PER_সূচক_2_ENTRY - 1)) == 0);
    উপরের_মান = get(MAX_UNICODE);
    int32_t real_উপরের_Start = find_উপরের_Start();
    real_উপরের_Start = (real_উপরের_Start + (UCPTRIE_CP_PER_সূচক_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_সূচক_2_ENTRY - 1);
    যদি (real_উপরের_Start == UNICODE_LIMIT) {
        উপরের_মান = initial_মান;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_উপরের_Start, (long)উপরের_মান, (long)initial_মান);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    যদি (real_উপরের_Start < fastLimit) {
        জন্য (int32_t i = (real_উপরের_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            সূচক[i] = উপরের_মান;
        }
        উপরের_Start = fastLimit;
    } অন্যথা {
        উপরের_Start = real_উপরের_Start;
    }

    uint32_t ascii_তথ্য[ASCII_LIMIT];
    জন্য (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        ascii_তথ্য[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t new_তথ্য_Capacity = compact_Whole_তথ্য_Blocks(fastILimit, allSameBlocks);
    যদি (new_তথ্য_Capacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    uint32_t* new_তথ্য = static_cast<uint32_t*>(uprv_malloc(new_তথ্য_Capacity * 4));
    যদি (new_তথ্য == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    uprv_memcpy(new_তথ্য, ascii_তথ্য, আকার(ascii_তথ্য));

    int32_t তথ্য_Null_সূচক = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t new_তথ্য_দৈর্ঘ্য = compact_তথ্য(fastILimit, new_তথ্য, new_তথ্য_Capacity,
                                        তথ্য_Null_সূচক, mixedBlocks, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত 0; }
    U_ASSERT(new_তথ্য_দৈর্ঘ্য <= new_তথ্য_Capacity);
    uprv_free(তথ্য);
    তথ্য = new_তথ্য;
    তথ্য_Capacity = new_তথ্য_Capacity;
    তথ্য_দৈর্ঘ্য = new_তথ্য_দৈর্ঘ্য;
    যদি (তথ্য_দৈর্ঘ্য > (0x3ffff + UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    যদি (তথ্য_Null_সূচক >= 0) {
        তথ্য_Null_Offset = সূচক[তথ্য_Null_সূচক];
#ifdef UCPTRIE_DEBUG
        যদি (তথ্য[তথ্য_Null_Offset] != initial_মান) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_মান, (long)তথ্য[তথ্য_Null_Offset]);
        }
#endif
        initial_মান = তথ্য[তথ্য_Null_Offset];
    } অন্যথা {
        তথ্য_Null_Offset = UCPTRIE_NO_তথ্য_NULL_OFFSET;
    }

    int32_t সূচক_দৈর্ঘ্য = compact_সূচক(fastILimit, mixedBlocks, errorCode);
    উপরের_Start = real_উপরের_Start;
    ফেরত সূচক_দৈর্ঘ্য;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_মান_Width মান_Width, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    যদি (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            মান_Width < UCPTRIE_মান_BITS_16 || UCPTRIE_মান_BITS_8 < মান_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    নির্বাচন (মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        mask_মানসমূহ(0xffff);
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        mask_মানসমূহ(0xff);
        বিরতি;
    পূর্বনির্ধারিত:
        বিরতি;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t সূচক_দৈর্ঘ্য = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    যদি (U_FAILURE(errorCode)) {
        clear();
        ফেরত nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    যদি (মান_Width == UCPTRIE_মান_BITS_32 && (সূচক_দৈর্ঘ্য & 1) != 0) {
        index16[সূচক_দৈর্ঘ্য++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = সূচক_দৈর্ঘ্য * 2;
    যদি (মান_Width == UCPTRIE_মান_BITS_16) {
        যদি (((সূচক_দৈর্ঘ্য ^ তথ্য_দৈর্ঘ্য) & 1) != 0) {
            // padding
            তথ্য[তথ্য_দৈর্ঘ্য++] = error_মান;
        }
        যদি (তথ্য[তথ্য_দৈর্ঘ্য - 1] != error_মান || তথ্য[তথ্য_দৈর্ঘ্য - 2] != উপরের_মান) {
            তথ্য[তথ্য_দৈর্ঘ্য++] = উপরের_মান;
            তথ্য[তথ্য_দৈর্ঘ্য++] = error_মান;
        }
        length += তথ্য_দৈর্ঘ্য * 2;
    } অন্যথা যদি (মান_Width == UCPTRIE_মান_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        যদি (তথ্য[তথ্য_দৈর্ঘ্য - 1] != error_মান || তথ্য[তথ্য_দৈর্ঘ্য - 2] != উপরের_মান) {
            যদি (তথ্য[তথ্য_দৈর্ঘ্য - 1] != উপরের_মান) {
                তথ্য[তথ্য_দৈর্ঘ্য++] = উপরের_মান;
            }
            তথ্য[তথ্য_দৈর্ঘ্য++] = error_মান;
        }
        length += তথ্য_দৈর্ঘ্য * 4;
    } অন্যথা {
        int32_t and3 = (length + তথ্য_দৈর্ঘ্য) & 3;
        যদি (and3 == 0 && তথ্য[তথ্য_দৈর্ঘ্য - 1] == error_মান && তথ্য[তথ্য_দৈর্ঘ্য - 2] == উপরের_মান) {
            // all set
        } অন্যথা যদি(and3 == 3 && তথ্য[তথ্য_দৈর্ঘ্য - 1] == উপরের_মান) {
            তথ্য[তথ্য_দৈর্ঘ্য++] = error_মান;
        } অন্যথা {
            যখন (and3 != 2) {
                তথ্য[তথ্য_দৈর্ঘ্য++] = উপরের_মান;
                and3 = (and3 + 1) & 3;
            }
            তথ্য[তথ্য_দৈর্ঘ্য++] = উপরের_মান;
            তথ্য[তথ্য_দৈর্ঘ্য++] = error_মান;
        }
        length += তথ্য_দৈর্ঘ্য;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += আকার(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    যদি (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        ফেরত nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, আকার(UCPTrie));
    trie->সূচক_দৈর্ঘ্য = সূচক_দৈর্ঘ্য;
    trie->তথ্য_দৈর্ঘ্য = তথ্য_দৈর্ঘ্য;

    trie->উপরের_Start = উপরের_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_উপরের_Start = (উপরের_Start + 0xfff) >> 12;
    trie->type = type;
    trie->মান_Width = মান_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->তথ্য_Null_Offset = তথ্য_Null_Offset;
    trie->null_মান = initial_মান;

    bytes += আকার(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->সূচক = dest16;

    যদি (উপরের_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        জন্য (int32_t i = 0, j = 0; j < সূচক_দৈর্ঘ্য; i += SMALL_তথ্য_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(সূচক[i]); // dest16[j]
        }
    } অন্যথা {
        uprv_memcpy(dest16, index16, সূচক_দৈর্ঘ্য * 2);
        dest16 += সূচক_দৈর্ঘ্য;
    }
    bytes += সূচক_দৈর্ঘ্য * 2;

    // Write the data array.
    ধ্রুবক uint32_t *p = তথ্য;
    নির্বাচন (মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        // Write 16-bit data values.
        trie->তথ্য.ptr16 = dest16;
        জন্য (int32_t i = তথ্য_দৈর্ঘ্য; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        // Write 32-bit data values.
        trie->তথ্য.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)তথ্য_দৈর্ঘ্য * 4);
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        // Write 8-bit data values.
        trie->তথ্য.ptr8 = bytes;
        জন্য (int32_t i = তথ্য_দৈর্ঘ্য; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        বিরতি;
    পূর্বনির্ধারিত:
        // Will not occur, valueWidth checked at the beginning.
        বিরতি;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    ফেরত trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_মান, uint32_t error_মান, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        নতুন MutableCodePointTrie(initial_মান, error_মান, *pErrorCode), *pErrorCode);
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ধ্রুবক UMutableCPTrie *other, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি (other == nullptr) {
        ফেরত nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        নতুন MutableCodePointTrie(*reinterpret_cast<ধ্রুবক MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI শূন্য U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ধ্রুবক UCPMap *map, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ধ্রুবক UCPTrie *trie, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ধ্রুবক UMutableCPTrie *trie, UChar32 c) {
    ফেরত reinterpret_cast<ধ্রুবক MutableCodePointTrie *>(trie)->get(c);
}

নামাকাশ {

UChar32 getRange(ধ্রুবক শূন্য *trie, UChar32 start,
                 UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context, uint32_t *p_মান) {
    ফেরত reinterpret_cast<ধ্রুবক MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_মান);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ধ্রুবক UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_মান,
                        UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context, uint32_t *p_মান) {
    ফেরত ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_মান,
                                    filter, context, p_মান);
}

U_CAPI শূন্য U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_মান_Width মান_Width,
                              UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, মান_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC শূন্য umutablecptrie_setName(UMutableCPTrie *trie, ধ্রুবক অক্ষর *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
