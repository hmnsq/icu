// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ifdef OVERFLOW
#undef OVERFLOW
#endif

U_NAMESPACE_BEGIN

नामाकाश {

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

वर्ग AllSameBlocks;
वर्ग MixedBlocks;

वर्ग MutableCodePointTrie : सार्वजनिक UMemory {
सार्वजनिक:
    MutableCodePointTrie(uint32_t initial_मान, uint32_t error_मान, UErrorCode &errorCode);
    MutableCodePointTrie(अचर MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(अचर MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &चालक=(अचर MutableCodePointTrie &other) = delete;

    स्थिर MutableCodePointTrie *fromUCPMap(अचर UCPMap *map, UErrorCode &errorCode);
    स्थिर MutableCodePointTrie *fromUCPTrie(अचर UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) अचर;
    int32_t getRange(UChar32 start, UCPMap_मान_Filter *filter, अचर शून्य *context,
                     uint32_t *p_मान) अचर;

    शून्य set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    शून्य setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_मान_Width मान_Width, UErrorCode &errorCode);

गुप्त:
    शून्य clear();

    तार्किक ensure_माथिल्लो_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    शून्य mask_मानहरू(uint32_t mask);
    UChar32 find_माथिल्लो_Start() अचर;
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

    uint32_t orig_Initial_मान;
    uint32_t initial_मान;
    uint32_t error_मान;
    UChar32 माथिल्लो_Start;
    uint32_t माथिल्लो_मान;
#ifdef UCPTRIE_DEBUG
सार्वजनिक:
    अचर वर्ण *name;
#endif
गुप्त:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_मान, uint32_t err_मान, UErrorCode &errorCode) :
        orig_Initial_मान(ini_मान), initial_मान(ini_मान), error_मान(err_मान),
        माथिल्लो_Start(0), माथिल्लो_मान(initial_मान)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    यदि (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(अचर MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_मान(other.orig_Initial_मान), initial_मान(other.initial_मान),
        error_मान(other.error_मान),
        माथिल्लो_Start(other.माथिल्लो_Start), माथिल्लो_मान(other.माथिल्लो_मान)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    int32_t iCapacity = माथिल्लो_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    यदि (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
    uprv_memcpy(flags, other.flags, iLimit);
    uprv_memcpy(index, other.index, iLimit * 4);
    uprv_memcpy(data, other.data, (माप_प्रकार)other.dataLength * 4);
    dataLength = other.dataLength;
    U_ASSERT(other.index16 == nullptr);
}

MutableCodePointTrie::~MutableCodePointTrie() {
    uprv_free(index);
    uprv_free(data);
    uprv_free(index16);
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(अचर UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मान = ucpmap_get(map, -1);
    uint32_t initial_मान = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नूतन MutableCodePointTrie(initial_मान, error_मान, errorCode),
        errorCode);
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    जबसम्म ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        यदि (value != initial_मान) {
            यदि (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अन्यथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    यदि (U_SUCCESS(errorCode)) {
        फर्काऊ mutableTrie.orphan();
    } अन्यथा {
        फर्काऊ nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(अचर UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मान;
    uint32_t initial_मान;
    विकल्प (trie->मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        error_मान = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_मान_NEG_DATA_OFFSET];
        initial_मान = trie->data.ptr16[trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET];
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        error_मान = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_मान_NEG_DATA_OFFSET];
        initial_मान = trie->data.ptr32[trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET];
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        error_मान = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_मान_NEG_DATA_OFFSET];
        initial_मान = trie->data.ptr8[trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET];
        रोक;
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नूतन MutableCodePointTrie(initial_मान, error_मान, errorCode),
        errorCode);
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    जबसम्म ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        यदि (value != initial_मान) {
            यदि (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अन्यथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    यदि (U_SUCCESS(errorCode)) {
        फर्काऊ mutableTrie.orphan();
    } अन्यथा {
        फर्काऊ nullptr;
    }
}

शून्य MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    माथिल्लो_मान = initial_मान = orig_Initial_मान;
    माथिल्लो_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) अचर {
    यदि (static_cast<uint32_t>(c) > MAX_UNICODE) {
        फर्काऊ error_मान;
    }
    यदि (c >= माथिल्लो_Start) {
        फर्काऊ माथिल्लो_मान;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    यदि (flags[i] == ALL_SAME) {
        फर्काऊ index[i];
    } अन्यथा {
        फर्काऊ data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

अन्तःस्थ uint32_t maybe_Filter_मान(uint32_t value, uint32_t initial_मान, uint32_t null_मान,
                                 UCPMap_मान_Filter *filter, अचर शून्य *context) {
    यदि (value == initial_मान) {
        value = null_मान;
    } अन्यथा यदि (filter != nullptr) {
        value = filter(context, value);
    }
    फर्काऊ value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_मान_Filter *filter, अचर शून्य *context,
        uint32_t *p_मान) अचर {
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE) {
        फर्काऊ U_SENTINEL;
    }
    यदि (start >= माथिल्लो_Start) {
        यदि (p_मान != nullptr) {
            uint32_t value = माथिल्लो_मान;
            यदि (filter != nullptr) { value = filter(context, value); }
            *p_मान = value;
        }
        फर्काऊ MAX_UNICODE;
    }
    uint32_t null_मान = initial_मान;
    यदि (filter != nullptr) { null_मान = filter(context, null_मान); }
    UChar32 c = start;
    uint32_t trie_मान, value;
    तार्किक have_मान = असत्य;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    गर {
        यदि (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            यदि (have_मान) {
                यदि (trieValue2 != trie_मान) {
                    यदि (filter == nullptr ||
                            maybe_Filter_मान(trieValue2, initial_मान, null_मान,
                                             filter, context) != value) {
                        फर्काऊ c - 1;
                    }
                    trie_मान = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मान = trieValue2;
                value = maybe_Filter_मान(trieValue2, initial_मान, null_मान, filter, context);
                यदि (p_मान != nullptr) { *p_मान = value; }
                have_मान = सत्य;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } अन्यथा /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            यदि (have_मान) {
                यदि (trieValue2 != trie_मान) {
                    यदि (filter == nullptr ||
                            maybe_Filter_मान(trieValue2, initial_मान, null_मान,
                                             filter, context) != value) {
                        फर्काऊ c - 1;
                    }
                    trie_मान = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मान = trieValue2;
                value = maybe_Filter_मान(trieValue2, initial_मान, null_मान, filter, context);
                यदि (p_मान != nullptr) { *p_मान = value; }
                have_मान = सत्य;
            }
            जबसम्म ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                यदि (trieValue2 != trie_मान) {
                    यदि (filter == nullptr ||
                            maybe_Filter_मान(trieValue2, initial_मान, null_मान,
                                             filter, context) != value) {
                        फर्काऊ c - 1;
                    }
                }
                trie_मान = trieValue2;  // may or may not help
            }
        }
        ++i;
    } जबसम्म (c < माथिल्लो_Start);
    U_ASSERT(have_मान);
    यदि (maybe_Filter_मान(माथिल्लो_मान, initial_मान, null_मान,
                         filter, context) != value) {
        फर्काऊ c - 1;
    } अन्यथा {
        फर्काऊ MAX_UNICODE;
    }
}

शून्य
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    जबसम्म (block < limit) {
        *block++ = value;
    }
}

तार्किक MutableCodePointTrie::ensure_माथिल्लो_Start(UChar32 c) {
    यदि (c >= माथिल्लो_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        यदि (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            यदि (newIndex == nullptr) { फर्काऊ असत्य; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        गर {
            flags[i] = ALL_SAME;
            index[i] = initial_मान;
        } जबसम्म(++i < iLimit);
        माथिल्लो_Start = c;
    }
    फर्काऊ सत्य;
}

int32_t MutableCodePointTrie::allocDataBlock(int32_t blockLength) {
    int32_t newBlock = dataLength;
    int32_t new_शीर्ष = newBlock + blockLength;
    यदि (new_शीर्ष > dataCapacity) {
        int32_t capacity;
        यदि (dataCapacity < MEDIUM_DATA_LENGTH) {
            capacity = MEDIUM_DATA_LENGTH;
        } अन्यथा यदि (dataCapacity < MAX_DATA_LENGTH) {
            capacity = MAX_DATA_LENGTH;
        } अन्यथा {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            फर्काऊ -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        यदि (newData == nullptr) {
            फर्काऊ -1;
        }
        uprv_memcpy(newData, data, (माप_प्रकार)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_शीर्ष;
    फर्काऊ newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    यदि (flags[i] == MIXED) {
        फर्काऊ index[i];
    }
    यदि (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        यदि (newBlock < 0) { फर्काऊ newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        गर {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } जबसम्म (iStart < iLimit);
        फर्काऊ index[i];
    } अन्यथा {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        यदि (newBlock < 0) { फर्काऊ newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        फर्काऊ newBlock;
    }
}

शून्य MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    int32_t block;
    यदि (!ensure_माथिल्लो_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

शून्य
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    जबसम्म (block < pLimit) {
        *block++ = value;
    }
}

शून्य MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    यदि (!ensure_माथिल्लो_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }

    UChar32 limit = end + 1;
    यदि (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        यदि (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        यदि (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } अन्यथा {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            फर्काऊ;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    जबसम्म (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        यदि (flags[i] == ALL_SAME) {
            index[i] = value;
        } अन्यथा /* MIXED */ {
            fillBlock(data + index[i], 0, UCPTRIE_SMALL_DATA_BLOCK_LENGTH, value);
        }
        start += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    }

    यदि (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        यदि (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

शून्य MutableCodePointTrie::mask_मानहरू(uint32_t mask) {
    initial_मान &= mask;
    error_मान &= mask;
    माथिल्लो_मान &= mask;
    int32_t iLimit = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
    लागि (int32_t i = 0; i < iLimit; ++i) {
        यदि (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    लागि (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

ढाँचा<typename UIntA, typename UIntB>
तार्किक equalBlocks(अचर UIntA *s, अचर UIntB *t, int32_t length) {
    जबसम्म (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    फर्काऊ length == 0;
}

तार्किक all_मानहरू_Same_As(अचर uint32_t *p, int32_t length, uint32_t value) {
    अचर uint32_t *pLimit = p + length;
    जबसम्म (p < pLimit && *p == value) { ++p; }
    फर्काऊ p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(अचर uint16_t *p, int32_t pStart, int32_t length,
                      अचर uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    जबसम्म (pStart <= length) {
        यदि (equalBlocks(p + pStart, q, blockLength)) {
            फर्काऊ pStart;
        }
        ++pStart;
    }
    फर्काऊ -1;
}

int32_t findAllSameBlock(अचर uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    लागि (int32_t block = start; block <= limit; ++block) {
        यदि (p[block] == value) {
            लागि (int32_t i = 1;; ++i) {
                यदि (i == blockLength) {
                    फर्काऊ block;
                }
                यदि (p[block + i] != value) {
                    block += i;
                    रोक;
                }
            }
        }
    }
    फर्काऊ -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
ढाँचा<typename UIntA, typename UIntB>
int32_t getOverlap(अचर UIntA *p, int32_t length,
                   अचर UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    जबसम्म (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    फर्काऊ overlap;
}

int32_t getAllSameOverlap(अचर uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    जबसम्म (min < i && p[i - 1] == value) { --i; }
    फर्काऊ length - i;
}

तार्किक isStartOfSomeFastBlock(uint32_t dataOffset, अचर uint32_t index[], int32_t fastILimit) {
    लागि (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        यदि (index[i] == dataOffset) {
            फर्काऊ सत्य;
        }
    }
    फर्काऊ असत्य;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_माथिल्लो_Start() अचर {
    int32_t i = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
    जबसम्म (i > 0) {
        तार्किक match;
        यदि (flags[--i] == ALL_SAME) {
            match = index[i] == माथिल्लो_मान;
        } अन्यथा /* MIXED */ {
            अचर uint32_t *p = data + index[i];
            लागि (int32_t j = 0;; ++j) {
                यदि (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = सत्य;
                    रोक;
                }
                यदि (p[j] != माथिल्लो_मान) {
                    match = असत्य;
                    रोक;
                }
            }
        }
        यदि (!match) {
            फर्काऊ (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    फर्काऊ 0;
}

वर्ग AllSameBlocks {
सार्वजनिक:
    स्थिर constexpr int32_t NEW_UNIQUE = -1;
    स्थिर constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        यदि (mostRecent >= 0 && मानहरू[mostRecent] == value) {
            refCounts[mostRecent] += count;
            फर्काऊ indexes[mostRecent];
        }
        लागि (int32_t i = 0; i < length; ++i) {
            यदि (मानहरू[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                फर्काऊ indexes[i];
            }
        }
        यदि (length == CAPACITY) {
            फर्काऊ OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        मानहरू[length] = value;
        refCounts[length++] = count;
        फर्काऊ NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    शून्य add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_गन्ती = I_LIMIT;
        लागि (int32_t i = 0; i < length; ++i) {
            U_ASSERT(मानहरू[i] != value);
            यदि (refCounts[i] < least_गन्ती) {
                least = i;
                least_गन्ती = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        मानहरू[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() अचर {
        यदि (length == 0) { फर्काऊ -1; }
        int32_t max = -1;
        int32_t max_गन्ती = 0;
        लागि (int32_t i = 0; i < length; ++i) {
            यदि (refCounts[i] > max_गन्ती) {
                max = i;
                max_गन्ती = refCounts[i];
            }
        }
        फर्काऊ indexes[max];
    }

गुप्त:
    स्थिर constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t मानहरू[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
वर्ग MixedBlocks {
सार्वजनिक:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    तार्किक init(int32_t maxLength, int32_t newBlockLength) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t maxDataIndex = maxLength - newBlockLength + 1;
        int32_t newLength;
        यदि (maxDataIndex <= 0xfff) {  // 4k
            newLength = 6007;
            shift = 12;
            mask = 0xfff;
        } अन्यथा यदि (maxDataIndex <= 0x7fff) {  // 32k
            newLength = 50021;
            shift = 15;
            mask = 0x7fff;
        } अन्यथा यदि (maxDataIndex <= 0x1ffff) {  // 128k
            newLength = 200003;
            shift = 17;
            mask = 0x1ffff;
        } अन्यथा {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            newLength = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        यदि (newLength > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(newLength * 4));
            यदि (table == nullptr) {
                फर्काऊ असत्य;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        फर्काऊ सत्य;
    }

    ढाँचा<typename UInt>
    शून्य extend(अचर UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        यदि (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } अन्यथा {
            start = minStart;  // Begin with the first full block.
        }
        लागि (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    ढाँचा<typename UIntA, typename UIntB>
    int32_t findBlock(अचर UIntA *data, अचर UIntB *blockData, int32_t blockStart) अचर {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        यदि (entryIndex >= 0) {
            फर्काऊ (table[entryIndex] & mask) - 1;
        } अन्यथा {
            फर्काऊ -1;
        }
    }

    int32_t findAllSameBlock(अचर uint32_t *data, uint32_t block_मान) अचर {
        uint32_t hashCode = makeHashCode(block_मान);
        int32_t entryIndex = findEntry(data, block_मान, hashCode);
        यदि (entryIndex >= 0) {
            फर्काऊ (table[entryIndex] & mask) - 1;
        } अन्यथा {
            फर्काऊ -1;
        }
    }

गुप्त:
    ढाँचा<typename UInt>
    uint32_t makeHashCode(अचर UInt *blockData, int32_t blockStart) अचर {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        गर {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } जबसम्म (blockStart < blockLimit);
        फर्काऊ hashCode;
    }

    uint32_t makeHashCode(uint32_t block_मान) अचर {
        uint32_t hashCode = block_मान;
        लागि (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_मान;
        }
        फर्काऊ hashCode;
    }

    ढाँचा<typename UInt>
    शून्य addEntry(अचर UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        यदि (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    ढाँचा<typename UIntA, typename UIntB>
    int32_t findEntry(अचर UIntA *data, अचर UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) अचर {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        लागि (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            यदि (entry == 0) {
                फर्काऊ ~entryIndex;
            }
            यदि ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                यदि (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    फर्काऊ entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(अचर uint32_t *data, uint32_t block_मान, uint32_t hashCode) अचर {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        लागि (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            यदि (entry == 0) {
                फर्काऊ ~entryIndex;
            }
            यदि ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                यदि (all_मानहरू_Same_As(data + dataIndex, blockLength, block_मान)) {
                    फर्काऊ entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    अन्तःस्थ int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) अचर {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        फर्काऊ (entryIndex + initialEntryIndex) % length;
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
    तार्किक overflow = असत्य;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t newDataCapacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    newDataCapacity += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    // Add room for special values (errorValue, highValue) and padding.
    newDataCapacity += 4;
    int32_t iLimit = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    लागि (int32_t i = 0; i < iLimit; i += inc) {
        यदि (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        यदि (flags[i] == MIXED) {
            // Really mixed?
            अचर uint32_t *p = data + value;
            value = *p;
            यदि (all_मानहरू_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } अन्यथा {
                newDataCapacity += blockLength;
                जारी;
            }
        } अन्यथा {
            U_ASSERT(flags[i] == ALL_SAME);
            यदि (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                तार्किक allSame = सत्य;
                int32_t next_i = i + inc;
                लागि (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    यदि (index[j] != value) {
                        allSame = असत्य;
                        रोक;
                    }
                }
                यदि (!allSame) {
                    // Turn it into a MIXED block.
                    यदि (getDataBlock(i) < 0) {
                        फर्काऊ -1;
                    }
                    newDataCapacity += blockLength;
                    जारी;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        यदि (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            यदि (!overflow) {
                माला_दो("UCPTrie AllSameBlocks overflow");
                overflow = सत्य;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            लागि (int32_t j = 0;; j += jInc) {
                यदि (j == i) {
                    allSameBlocks.add(i, inc, value);
                    रोक;
                }
                यदि (j == fastILimit) {
                    jInc = 1;
                }
                यदि (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    रोक;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        यदि (other >= 0) {
            flags[i] = SAME_AS;
            index[i] = other;
        } अन्यथा {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    फर्काऊ newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(अभिव्यक्ति) अभिव्यक्ति
#अन्यथा
#   define DEBUG_DO(अभिव्यक्ति)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_मान(वर्ण s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (वर्ण)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (वर्ण)(0x80 + (value & 0x3F));
    फर्काऊ length + 3;
}

शून्य printBlock(अचर uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_मान) {
    वर्ण s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    लागि (i = 0; i < overlap; ++i) {
        length = append_मान(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    लागि (; i < blockLength; ++i) {
        यदि (block != nullptr) {
            value = block[i];
        }
        यदि (value == initial_मान) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_मान(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    यदि (start <= 0xffff) {
        म_लिखो("    %04lX  %s|\n", (दीर्घ)start, s);
    } अन्यथा यदि (start <= 0xfffff) {
        म_लिखो("   %5lX  %s|\n", (दीर्घ)start, s);
    } अन्यथा {
        म_लिखो("  %6lX  %s|\n", (दीर्घ)start, s);
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
    int32_t गन्ती_Same=0, sumOverlaps=0;
    तार्किक printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    लागि (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        यदि (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_मान);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    यदि (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    लागि (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        यदि (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            यदि (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ 0;
            }
            mixedBlocks.extend(newData, 0, 0, newDataLength);
        }
        यदि (flags[i] == ALL_SAME) {
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
            जबसम्म (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            यदि (n >= 0) {
                DEBUG_DO(++गन्ती_Same);
                index[i] = n;
            } अन्यथा {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                यदि (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_मान);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                जबसम्म (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } अन्यथा यदि (flags[i] == MIXED) {
            अचर uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            यदि (n >= 0) {
                DEBUG_DO(++गन्ती_Same);
                index[i] = n;
            } अन्यथा {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                यदि (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_मान);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                जबसम्म (n < blockLength) {
                    newData[newDataLength++] = block[n++];
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } अन्यथा /* SAME_AS */ {
            uint32_t j = index[i];
            index[i] = index[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    म_लिखो("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (दीर्घ)dataLength, (दीर्घ)newDataLength, (दीर्घ)गन्ती_Same, (दीर्घ)sumOverlaps);
#endif
    फर्काऊ newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    यदि ((माथिल्लो_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        फर्काऊ fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    लागि (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = index[i];
        fastIndex[j] = static_cast<uint16_t>(i3);
        यदि (i3 == static_cast<uint32_t>(dataNullOffset)) {
            यदि (i3FirstNull < 0) {
                i3FirstNull = j;
            } अन्यथा यदि (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                index3NullOffset = i3FirstNull;
            }
        } अन्यथा {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t iNext = i + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        जबसम्म (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    यदि (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ 0;
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
    तार्किक hasLongI3Blocks = असत्य;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = माथिल्लो_Start >> UCPTRIE_SHIFT_3;
    लागि (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        तार्किक isNull = सत्य;
        गर {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            यदि (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = असत्य;
            }
        } जबसम्म (++j < jLimit);
        यदि (isNull) {
            flags[i] = I3_NULL;
            यदि (i3FirstNull < 0) {
                यदि (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                } अन्यथा {
                    index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                    hasLongI3Blocks = सत्य;
                }
                i3FirstNull = 0;
            }
        } अन्यथा {
            यदि (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fastIndex, index, i);
                यदि (n >= 0) {
                    flags[i] = I3_BMP;
                    index[i] = n;
                } अन्यथा {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                }
            } अन्यथा {
                flags[i] = I3_18;
                index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                hasLongI3Blocks = सत्य;
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
    यदि (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    यदि (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ 0;
    }
    MixedBlocks longI3Blocks;
    यदि (hasLongI3Blocks) {
        यदि (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    लागि (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
        int32_t i3;
        uint8_t f = flags[i];
        यदि (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = dataNullOffset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        यदि (f == I3_NULL) {
            i3 = index3NullOffset;
        } अन्यथा यदि (f == I3_BMP) {
            i3 = index[i];
        } अन्यथा यदि (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, index, i);
            यदि (n >= 0) {
                i3 = n;
            } अन्यथा {
                यदि (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अन्यथा {
                    n = getOverlap(index16, indexLength,
                                   index, i, UCPTRIE_INDEX_3_BLOCK_LENGTH);
                }
                i3 = indexLength - n;
                int32_t prevIndexLength = indexLength;
                जबसम्म (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                    index16[indexLength++] = index[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                यदि (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        } अन्यथा {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
            int32_t k = indexLength;
            गर {
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
            } जबसम्म (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, indexLength);
            यदि (n >= 0) {
                i3 = n | 0x8000;
            } अन्यथा {
                यदि (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अन्यथा {
                    n = getOverlap(index16, indexLength,
                                   index16, indexLength, INDEX_3_18BIT_BLOCK_LENGTH);
                }
                i3 = (indexLength - n) | 0x8000;
                int32_t prevIndexLength = indexLength;
                यदि (n > 0) {
                    int32_t start = indexLength;
                    जबसम्म (n < INDEX_3_18BIT_BLOCK_LENGTH) {
                        index16[indexLength++] = index16[start + n++];
                    }
                } अन्यथा {
                    indexLength += INDEX_3_18BIT_BLOCK_LENGTH;
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                यदि (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        }
        यदि (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2Length++] = i3;
    }
    U_ASSERT(i2Length == index2Capacity);
    U_ASSERT(indexLength <= index3Start + index3Capacity);

    यदि (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    यदि (indexLength >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    लागि (int32_t i = 0; i < i2Length; i += blockLength) {
        int32_t n;
        यदि ((i2Length - i) >= blockLength) {
            // normal block
            U_ASSERT(blockLength == UCPTRIE_INDEX_2_BLOCK_LENGTH);
            n = mixedBlocks.findBlock(index16, index2, i);
        } अन्यथा {
            // highStart is inside the last index-2 block. Shorten it.
            blockLength = i2Length - i;
            n = findSameBlock(index16, index3Start, indexLength,
                              index2, i, blockLength);
        }
        int32_t i2;
        यदि (n >= 0) {
            i2 = n;
        } अन्यथा {
            यदि (indexLength == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } अन्यथा {
                n = getOverlap(index16, indexLength, index2, i, blockLength);
            }
            i2 = indexLength - n;
            int32_t prevIndexLength = indexLength;
            जबसम्म (n < blockLength) {
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
    म_लिखो("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (दीर्घ)iLimit, (दीर्घ)indexLength);
#endif

    फर्काऊ indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((माथिल्लो_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    माथिल्लो_मान = get(MAX_UNICODE);
    int32_t real_माथिल्लो_Start = find_माथिल्लो_Start();
    real_माथिल्लो_Start = (real_माथिल्लो_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    यदि (real_माथिल्लो_Start == UNICODE_LIMIT) {
        माथिल्लो_मान = initial_मान;
    }

#ifdef UCPTRIE_DEBUG
    म_लिखो("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (दीर्घ)real_माथिल्लो_Start, (दीर्घ)माथिल्लो_मान, (दीर्घ)initial_मान);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    यदि (real_माथिल्लो_Start < fastLimit) {
        लागि (int32_t i = (real_माथिल्लो_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = माथिल्लो_मान;
        }
        माथिल्लो_Start = fastLimit;
    } अन्यथा {
        माथिल्लो_Start = real_माथिल्लो_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    लागि (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    यदि (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    यदि (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ 0;
    }
    uprv_memcpy(newData, asciiData, परिमाण(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    यदि (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }

    यदि (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        यदि (data[dataNullOffset] != initial_मान) {
            म_लिखो("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (दीर्घ)initial_मान, (दीर्घ)data[dataNullOffset]);
        }
#endif
        initial_मान = data[dataNullOffset];
    } अन्यथा {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    माथिल्लो_Start = real_माथिल्लो_Start;
    फर्काऊ indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_मान_Width मान_Width, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    यदि (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मान_Width < UCPTRIE_मान_BITS_16 || UCPTRIE_मान_BITS_8 < मान_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    विकल्प (मान_Width) {
    पक्ष UCPTRIE_मान_BITS_32:
        रोक;
    पक्ष UCPTRIE_मान_BITS_16:
        mask_मानहरू(0xffff);
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        mask_मानहरू(0xff);
        रोक;
    स्वभाव:
        रोक;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    यदि (U_FAILURE(errorCode)) {
        clear();
        फर्काऊ nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    यदि (मान_Width == UCPTRIE_मान_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    यदि (मान_Width == UCPTRIE_मान_BITS_16) {
        यदि (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_मान;
        }
        यदि (data[dataLength - 1] != error_मान || data[dataLength - 2] != माथिल्लो_मान) {
            data[dataLength++] = माथिल्लो_मान;
            data[dataLength++] = error_मान;
        }
        length += dataLength * 2;
    } अन्यथा यदि (मान_Width == UCPTRIE_मान_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        यदि (data[dataLength - 1] != error_मान || data[dataLength - 2] != माथिल्लो_मान) {
            यदि (data[dataLength - 1] != माथिल्लो_मान) {
                data[dataLength++] = माथिल्लो_मान;
            }
            data[dataLength++] = error_मान;
        }
        length += dataLength * 4;
    } अन्यथा {
        int32_t and3 = (length + dataLength) & 3;
        यदि (and3 == 0 && data[dataLength - 1] == error_मान && data[dataLength - 2] == माथिल्लो_मान) {
            // all set
        } अन्यथा यदि(and3 == 3 && data[dataLength - 1] == माथिल्लो_मान) {
            data[dataLength++] = error_मान;
        } अन्यथा {
            जबसम्म (and3 != 2) {
                data[dataLength++] = माथिल्लो_मान;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = माथिल्लो_मान;
            data[dataLength++] = error_मान;
        }
        length += dataLength;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += परिमाण(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    यदि (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        फर्काऊ nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, परिमाण(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->माथिल्लो_Start = माथिल्लो_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_माथिल्लो_Start = (माथिल्लो_Start + 0xfff) >> 12;
    trie->type = type;
    trie->मान_Width = मान_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_मान = initial_मान;

    bytes += परिमाण(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    यदि (माथिल्लो_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        लागि (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } अन्यथा {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    अचर uint32_t *p = data;
    विकल्प (मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        लागि (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (माप_प्रकार)dataLength * 4);
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        लागि (int32_t i = dataLength; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        रोक;
    स्वभाव:
        // Will not occur, valueWidth checked at the beginning.
        रोक;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    फर्काऊ trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_मान, uint32_t error_मान, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        नूतन MutableCodePointTrie(initial_मान, error_मान, *pErrorCode), *pErrorCode);
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    फर्काऊ reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(अचर UMutableCPTrie *other, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    यदि (other == nullptr) {
        फर्काऊ nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        नूतन MutableCodePointTrie(*reinterpret_cast<अचर MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    फर्काऊ reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(अचर UCPMap *map, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    यदि (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    फर्काऊ reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(अचर UCPTrie *trie, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    यदि (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    फर्काऊ reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(अचर UMutableCPTrie *trie, UChar32 c) {
    फर्काऊ reinterpret_cast<अचर MutableCodePointTrie *>(trie)->get(c);
}

नामाकाश {

UChar32 getRange(अचर शून्य *trie, UChar32 start,
                 UCPMap_मान_Filter *filter, अचर शून्य *context, uint32_t *p_मान) {
    फर्काऊ reinterpret_cast<अचर MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_मान);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(अचर UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_मान,
                        UCPMap_मान_Filter *filter, अचर शून्य *context, uint32_t *p_मान) {
    फर्काऊ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मान,
                                    filter, context, p_मान);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_मान_Width मान_Width,
                              UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }
    फर्काऊ reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, मान_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC शून्य umutablecptrie_setName(UMutableCPTrie *trie, अचर वर्ण *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
