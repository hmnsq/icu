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

वग्ग AllSameBlocks;
वग्ग MixedBlocks;

वग्ग MutableCodePointTrie : सार्वजनिक UMemory {
सार्वजनिक:
    MutableCodePointTrie(uint32_t initial_मानम्, uint32_t error_मानम्, UErrorCode &errorCode);
    MutableCodePointTrie(ध्रुव MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(ध्रुव MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(ध्रुव MutableCodePointTrie &other) = delete;

    स्थिर MutableCodePointTrie *fromUCPMap(ध्रुव UCPMap *map, UErrorCode &errorCode);
    स्थिर MutableCodePointTrie *fromUCPTrie(ध्रुव UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) ध्रुव;
    int32_t getRange(UChar32 start, UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context,
                     uint32_t *p_मानम्) ध्रुव;

    शून्य set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    शून्य setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_मानम्_Width मानम्_Width, UErrorCode &errorCode);

गुप्त:
    शून्य clear();

    तार्किक ensure_उच्चः_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    शून्य mask_मानानि(uint32_t mask);
    UChar32 find_उच्चः_Start() ध्रुव;
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

    uint32_t orig_Initial_मानम्;
    uint32_t initial_मानम्;
    uint32_t error_मानम्;
    UChar32 उच्चः_Start;
    uint32_t उच्चः_मानम्;
#ifdef UCPTRIE_DEBUG
सार्वजनिक:
    ध्रुव वर्ण *name;
#endif
गुप्त:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_मानम्, uint32_t err_मानम्, UErrorCode &errorCode) :
        orig_Initial_मानम्(ini_मानम्), initial_मानम्(ini_मानम्), error_मानम्(err_मानम्),
        उच्चः_Start(0), उच्चः_मानम्(initial_मानम्)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    सचे (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(ध्रुव MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_मानम्(other.orig_Initial_मानम्), initial_मानम्(other.initial_मानम्),
        error_मानम्(other.error_मानम्),
        उच्चः_Start(other.उच्चः_Start), उच्चः_मानम्(other.उच्चः_मानम्)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
    int32_t iCapacity = उच्चः_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    सचे (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = उच्चः_Start >> UCPTRIE_SHIFT_3;
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

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(ध्रुव UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मानम् = ucpmap_get(map, -1);
    uint32_t initial_मानम् = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नूतन MutableCodePointTrie(initial_मानम्, error_मानम्, errorCode),
        errorCode);
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    याव ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        सचे (value != initial_मानम्) {
            सचे (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अञ्ञथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    सचे (U_SUCCESS(errorCode)) {
        पटिनिवत्त mutableTrie.orphan();
    } अञ्ञथा {
        पटिनिवत्त nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(ध्रुव UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मानम्;
    uint32_t initial_मानम्;
    विकल्प (trie->मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        error_मानम् = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_मानम्_NEG_DATA_OFFSET];
        initial_मानम् = trie->data.ptr16[trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        error_मानम् = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_मानम्_NEG_DATA_OFFSET];
        initial_मानम् = trie->data.ptr32[trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        error_मानम् = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_मानम्_NEG_DATA_OFFSET];
        initial_मानम् = trie->data.ptr8[trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET];
        भङ्ग;
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नूतन MutableCodePointTrie(initial_मानम्, error_मानम्, errorCode),
        errorCode);
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    याव ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        सचे (value != initial_मानम्) {
            सचे (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अञ्ञथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    सचे (U_SUCCESS(errorCode)) {
        पटिनिवत्त mutableTrie.orphan();
    } अञ्ञथा {
        पटिनिवत्त nullptr;
    }
}

शून्य MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    उच्चः_मानम् = initial_मानम् = orig_Initial_मानम्;
    उच्चः_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) ध्रुव {
    सचे (static_cast<uint32_t>(c) > MAX_UNICODE) {
        पटिनिवत्त error_मानम्;
    }
    सचे (c >= उच्चः_Start) {
        पटिनिवत्त उच्चः_मानम्;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    सचे (flags[i] == ALL_SAME) {
        पटिनिवत्त index[i];
    } अञ्ञथा {
        पटिनिवत्त data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

अन्तःस्थ uint32_t maybe_Filter_मानम्(uint32_t value, uint32_t initial_मानम्, uint32_t null_मानम्,
                                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context) {
    सचे (value == initial_मानम्) {
        value = null_मानम्;
    } अञ्ञथा सचे (filter != nullptr) {
        value = filter(context, value);
    }
    पटिनिवत्त value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context,
        uint32_t *p_मानम्) ध्रुव {
    सचे (static_cast<uint32_t>(start) > MAX_UNICODE) {
        पटिनिवत्त U_SENTINEL;
    }
    सचे (start >= उच्चः_Start) {
        सचे (p_मानम् != nullptr) {
            uint32_t value = उच्चः_मानम्;
            सचे (filter != nullptr) { value = filter(context, value); }
            *p_मानम् = value;
        }
        पटिनिवत्त MAX_UNICODE;
    }
    uint32_t null_मानम् = initial_मानम्;
    सचे (filter != nullptr) { null_मानम् = filter(context, null_मानम्); }
    UChar32 c = start;
    uint32_t trie_मानम्, value;
    तार्किक have_मानम् = मिच्छा;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    करोहि {
        सचे (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            सचे (have_मानम्) {
                सचे (trieValue2 != trie_मानम्) {
                    सचे (filter == nullptr ||
                            maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्,
                                             filter, context) != value) {
                        पटिनिवत्त c - 1;
                    }
                    trie_मानम् = trieValue2;  // may or may not help
                }
            } अञ्ञथा {
                trie_मानम् = trieValue2;
                value = maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्, filter, context);
                सचे (p_मानम् != nullptr) { *p_मानम् = value; }
                have_मानम् = सच्च;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } अञ्ञथा /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            सचे (have_मानम्) {
                सचे (trieValue2 != trie_मानम्) {
                    सचे (filter == nullptr ||
                            maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्,
                                             filter, context) != value) {
                        पटिनिवत्त c - 1;
                    }
                    trie_मानम् = trieValue2;  // may or may not help
                }
            } अञ्ञथा {
                trie_मानम् = trieValue2;
                value = maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्, filter, context);
                सचे (p_मानम् != nullptr) { *p_मानम् = value; }
                have_मानम् = सच्च;
            }
            याव ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                सचे (trieValue2 != trie_मानम्) {
                    सचे (filter == nullptr ||
                            maybe_Filter_मानम्(trieValue2, initial_मानम्, null_मानम्,
                                             filter, context) != value) {
                        पटिनिवत्त c - 1;
                    }
                }
                trie_मानम् = trieValue2;  // may or may not help
            }
        }
        ++i;
    } याव (c < उच्चः_Start);
    U_ASSERT(have_मानम्);
    सचे (maybe_Filter_मानम्(उच्चः_मानम्, initial_मानम्, null_मानम्,
                         filter, context) != value) {
        पटिनिवत्त c - 1;
    } अञ्ञथा {
        पटिनिवत्त MAX_UNICODE;
    }
}

शून्य
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    याव (block < limit) {
        *block++ = value;
    }
}

तार्किक MutableCodePointTrie::ensure_उच्चः_Start(UChar32 c) {
    सचे (c >= उच्चः_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = उच्चः_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        सचे (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            सचे (newIndex == nullptr) { पटिनिवत्त मिच्छा; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        करोहि {
            flags[i] = ALL_SAME;
            index[i] = initial_मानम्;
        } याव(++i < iLimit);
        उच्चः_Start = c;
    }
    पटिनिवत्त सच्च;
}

int32_t MutableCodePointTrie::allocDataBlock(int32_t blockLength) {
    int32_t newBlock = dataLength;
    int32_t new_शिखरम् = newBlock + blockLength;
    सचे (new_शिखरम् > dataCapacity) {
        int32_t capacity;
        सचे (dataCapacity < MEDIUM_DATA_LENGTH) {
            capacity = MEDIUM_DATA_LENGTH;
        } अञ्ञथा सचे (dataCapacity < MAX_DATA_LENGTH) {
            capacity = MAX_DATA_LENGTH;
        } अञ्ञथा {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            पटिनिवत्त -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        सचे (newData == nullptr) {
            पटिनिवत्त -1;
        }
        uprv_memcpy(newData, data, (size_t)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_शिखरम्;
    पटिनिवत्त newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    सचे (flags[i] == MIXED) {
        पटिनिवत्त index[i];
    }
    सचे (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        सचे (newBlock < 0) { पटिनिवत्त newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        करोहि {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } याव (iStart < iLimit);
        पटिनिवत्त index[i];
    } अञ्ञथा {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        सचे (newBlock < 0) { पटिनिवत्त newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        पटिनिवत्त newBlock;
    }
}

शून्य MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

    int32_t block;
    सचे (!ensure_उच्चः_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

शून्य
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    याव (block < pLimit) {
        *block++ = value;
    }
}

शून्य MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    सचे (!ensure_उच्चः_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }

    UChar32 limit = end + 1;
    सचे (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        सचे (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        सचे (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } अञ्ञथा {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            पटिनिवत्त;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    याव (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        सचे (flags[i] == ALL_SAME) {
            index[i] = value;
        } अञ्ञथा /* MIXED */ {
            fillBlock(data + index[i], 0, UCPTRIE_SMALL_DATA_BLOCK_LENGTH, value);
        }
        start += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    }

    सचे (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        सचे (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

शून्य MutableCodePointTrie::mask_मानानि(uint32_t mask) {
    initial_मानम् &= mask;
    error_मानम् &= mask;
    उच्चः_मानम् &= mask;
    int32_t iLimit = उच्चः_Start >> UCPTRIE_SHIFT_3;
    अत्थाय (int32_t i = 0; i < iLimit; ++i) {
        सचे (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    अत्थाय (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
तार्किक equalBlocks(ध्रुव UIntA *s, ध्रुव UIntB *t, int32_t length) {
    याव (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    पटिनिवत्त length == 0;
}

तार्किक all_मानानि_Same_As(ध्रुव uint32_t *p, int32_t length, uint32_t value) {
    ध्रुव uint32_t *pLimit = p + length;
    याव (p < pLimit && *p == value) { ++p; }
    पटिनिवत्त p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(ध्रुव uint16_t *p, int32_t pStart, int32_t length,
                      ध्रुव uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    याव (pStart <= length) {
        सचे (equalBlocks(p + pStart, q, blockLength)) {
            पटिनिवत्त pStart;
        }
        ++pStart;
    }
    पटिनिवत्त -1;
}

int32_t findAllSameBlock(ध्रुव uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    अत्थाय (int32_t block = start; block <= limit; ++block) {
        सचे (p[block] == value) {
            अत्थाय (int32_t i = 1;; ++i) {
                सचे (i == blockLength) {
                    पटिनिवत्त block;
                }
                सचे (p[block + i] != value) {
                    block += i;
                    भङ्ग;
                }
            }
        }
    }
    पटिनिवत्त -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
template<typename UIntA, typename UIntB>
int32_t getOverlap(ध्रुव UIntA *p, int32_t length,
                   ध्रुव UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    याव (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    पटिनिवत्त overlap;
}

int32_t getAllSameOverlap(ध्रुव uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    याव (min < i && p[i - 1] == value) { --i; }
    पटिनिवत्त length - i;
}

तार्किक isStartOfSomeFastBlock(uint32_t dataOffset, ध्रुव uint32_t index[], int32_t fastILimit) {
    अत्थाय (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        सचे (index[i] == dataOffset) {
            पटिनिवत्त सच्च;
        }
    }
    पटिनिवत्त मिच्छा;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_उच्चः_Start() ध्रुव {
    int32_t i = उच्चः_Start >> UCPTRIE_SHIFT_3;
    याव (i > 0) {
        तार्किक match;
        सचे (flags[--i] == ALL_SAME) {
            match = index[i] == उच्चः_मानम्;
        } अञ्ञथा /* MIXED */ {
            ध्रुव uint32_t *p = data + index[i];
            अत्थाय (int32_t j = 0;; ++j) {
                सचे (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = सच्च;
                    भङ्ग;
                }
                सचे (p[j] != उच्चः_मानम्) {
                    match = मिच्छा;
                    भङ्ग;
                }
            }
        }
        सचे (!match) {
            पटिनिवत्त (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    पटिनिवत्त 0;
}

वग्ग AllSameBlocks {
सार्वजनिक:
    स्थिर constexpr int32_t NEW_UNIQUE = -1;
    स्थिर constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        सचे (mostRecent >= 0 && मानानि[mostRecent] == value) {
            refCounts[mostRecent] += count;
            पटिनिवत्त indexes[mostRecent];
        }
        अत्थाय (int32_t i = 0; i < length; ++i) {
            सचे (मानानि[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                पटिनिवत्त indexes[i];
            }
        }
        सचे (length == CAPACITY) {
            पटिनिवत्त OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        मानानि[length] = value;
        refCounts[length++] = count;
        पटिनिवत्त NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    शून्य add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_गणनम् = I_LIMIT;
        अत्थाय (int32_t i = 0; i < length; ++i) {
            U_ASSERT(मानानि[i] != value);
            सचे (refCounts[i] < least_गणनम्) {
                least = i;
                least_गणनम् = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        मानानि[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() ध्रुव {
        सचे (length == 0) { पटिनिवत्त -1; }
        int32_t max = -1;
        int32_t max_गणनम् = 0;
        अत्थाय (int32_t i = 0; i < length; ++i) {
            सचे (refCounts[i] > max_गणनम्) {
                max = i;
                max_गणनम् = refCounts[i];
            }
        }
        पटिनिवत्त indexes[max];
    }

गुप्त:
    स्थिर constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t मानानि[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
वग्ग MixedBlocks {
सार्वजनिक:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    तार्किक init(int32_t maxLength, int32_t newBlockLength) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t maxDataIndex = maxLength - newBlockLength + 1;
        int32_t newLength;
        सचे (maxDataIndex <= 0xfff) {  // 4k
            newLength = 6007;
            shift = 12;
            mask = 0xfff;
        } अञ्ञथा सचे (maxDataIndex <= 0x7fff) {  // 32k
            newLength = 50021;
            shift = 15;
            mask = 0x7fff;
        } अञ्ञथा सचे (maxDataIndex <= 0x1ffff) {  // 128k
            newLength = 200003;
            shift = 17;
            mask = 0x1ffff;
        } अञ्ञथा {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            newLength = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        सचे (newLength > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(newLength * 4));
            सचे (table == nullptr) {
                पटिनिवत्त मिच्छा;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        पटिनिवत्त सच्च;
    }

    template<typename UInt>
    शून्य extend(ध्रुव UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        सचे (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } अञ्ञथा {
            start = minStart;  // Begin with the first full block.
        }
        अत्थाय (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(ध्रुव UIntA *data, ध्रुव UIntB *blockData, int32_t blockStart) ध्रुव {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        सचे (entryIndex >= 0) {
            पटिनिवत्त (table[entryIndex] & mask) - 1;
        } अञ्ञथा {
            पटिनिवत्त -1;
        }
    }

    int32_t findAllSameBlock(ध्रुव uint32_t *data, uint32_t block_मानम्) ध्रुव {
        uint32_t hashCode = makeHashCode(block_मानम्);
        int32_t entryIndex = findEntry(data, block_मानम्, hashCode);
        सचे (entryIndex >= 0) {
            पटिनिवत्त (table[entryIndex] & mask) - 1;
        } अञ्ञथा {
            पटिनिवत्त -1;
        }
    }

गुप्त:
    template<typename UInt>
    uint32_t makeHashCode(ध्रुव UInt *blockData, int32_t blockStart) ध्रुव {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        करोहि {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } याव (blockStart < blockLimit);
        पटिनिवत्त hashCode;
    }

    uint32_t makeHashCode(uint32_t block_मानम्) ध्रुव {
        uint32_t hashCode = block_मानम्;
        अत्थाय (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_मानम्;
        }
        पटिनिवत्त hashCode;
    }

    template<typename UInt>
    शून्य addEntry(ध्रुव UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        सचे (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(ध्रुव UIntA *data, ध्रुव UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) ध्रुव {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        अत्थाय (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            सचे (entry == 0) {
                पटिनिवत्त ~entryIndex;
            }
            सचे ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                सचे (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    पटिनिवत्त entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(ध्रुव uint32_t *data, uint32_t block_मानम्, uint32_t hashCode) ध्रुव {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        अत्थाय (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            सचे (entry == 0) {
                पटिनिवत्त ~entryIndex;
            }
            सचे ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                सचे (all_मानानि_Same_As(data + dataIndex, blockLength, block_मानम्)) {
                    पटिनिवत्त entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    अन्तःस्थ int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) ध्रुव {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        पटिनिवत्त (entryIndex + initialEntryIndex) % length;
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
    तार्किक overflow = मिच्छा;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t newDataCapacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    newDataCapacity += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    // Add room for special values (errorValue, highValue) and padding.
    newDataCapacity += 4;
    int32_t iLimit = उच्चः_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    अत्थाय (int32_t i = 0; i < iLimit; i += inc) {
        सचे (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        सचे (flags[i] == MIXED) {
            // Really mixed?
            ध्रुव uint32_t *p = data + value;
            value = *p;
            सचे (all_मानानि_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } अञ्ञथा {
                newDataCapacity += blockLength;
                अनुवर्त;
            }
        } अञ्ञथा {
            U_ASSERT(flags[i] == ALL_SAME);
            सचे (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                तार्किक allSame = सच्च;
                int32_t next_i = i + inc;
                अत्थाय (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    सचे (index[j] != value) {
                        allSame = मिच्छा;
                        भङ्ग;
                    }
                }
                सचे (!allSame) {
                    // Turn it into a MIXED block.
                    सचे (getDataBlock(i) < 0) {
                        पटिनिवत्त -1;
                    }
                    newDataCapacity += blockLength;
                    अनुवर्त;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        सचे (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            सचे (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = सच्च;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            अत्थाय (int32_t j = 0;; j += jInc) {
                सचे (j == i) {
                    allSameBlocks.add(i, inc, value);
                    भङ्ग;
                }
                सचे (j == fastILimit) {
                    jInc = 1;
                }
                सचे (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    भङ्ग;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        सचे (other >= 0) {
            flags[i] = SAME_AS;
            index[i] = other;
        } अञ्ञथा {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    पटिनिवत्त newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(व्यञ्जकम्) व्यञ्जकम्
#अञ्ञथा
#   define DEBUG_DO(व्यञ्जकम्)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_मानम्(वर्ण s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (वर्ण)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (वर्ण)(0x80 + (value & 0x3F));
    पटिनिवत्त length + 3;
}

शून्य printBlock(ध्रुव uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_मानम्) {
    वर्ण s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    अत्थाय (i = 0; i < overlap; ++i) {
        length = append_मानम्(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    अत्थाय (; i < blockLength; ++i) {
        सचे (block != nullptr) {
            value = block[i];
        }
        सचे (value == initial_मानम्) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_मानम्(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    सचे (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } अञ्ञथा सचे (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } अञ्ञथा {
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
    int32_t गणनम्_Same=0, sumOverlaps=0;
    तार्किक printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    अत्थाय (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        सचे (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_मानम्);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    सचे (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = उच्चः_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    अत्थाय (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        सचे (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            सचे (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त 0;
            }
            mixedBlocks.extend(newData, 0, 0, newDataLength);
        }
        सचे (flags[i] == ALL_SAME) {
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
            याव (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            सचे (n >= 0) {
                DEBUG_DO(++गणनम्_Same);
                index[i] = n;
            } अञ्ञथा {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                सचे (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_मानम्);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                याव (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } अञ्ञथा सचे (flags[i] == MIXED) {
            ध्रुव uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            सचे (n >= 0) {
                DEBUG_DO(++गणनम्_Same);
                index[i] = n;
            } अञ्ञथा {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                सचे (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_मानम्);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                याव (n < blockLength) {
                    newData[newDataLength++] = block[n++];
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } अञ्ञथा /* SAME_AS */ {
            uint32_t j = index[i];
            index[i] = index[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)dataLength, (long)newDataLength, (long)गणनम्_Same, (long)sumOverlaps);
#endif
    पटिनिवत्त newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    सचे ((उच्चः_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        पटिनिवत्त fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    अत्थाय (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = index[i];
        fastIndex[j] = static_cast<uint16_t>(i3);
        सचे (i3 == static_cast<uint32_t>(dataNullOffset)) {
            सचे (i3FirstNull < 0) {
                i3FirstNull = j;
            } अञ्ञथा सचे (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                index3NullOffset = i3FirstNull;
            }
        } अञ्ञथा {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t iNext = i + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        याव (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    सचे (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त 0;
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
    तार्किक hasLongI3Blocks = मिच्छा;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = उच्चः_Start >> UCPTRIE_SHIFT_3;
    अत्थाय (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        तार्किक isNull = सच्च;
        करोहि {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            सचे (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = मिच्छा;
            }
        } याव (++j < jLimit);
        सचे (isNull) {
            flags[i] = I3_NULL;
            सचे (i3FirstNull < 0) {
                सचे (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                } अञ्ञथा {
                    index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                    hasLongI3Blocks = सच्च;
                }
                i3FirstNull = 0;
            }
        } अञ्ञथा {
            सचे (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fastIndex, index, i);
                सचे (n >= 0) {
                    flags[i] = I3_BMP;
                    index[i] = n;
                } अञ्ञथा {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                }
            } अञ्ञथा {
                flags[i] = I3_18;
                index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                hasLongI3Blocks = सच्च;
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
    सचे (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    सचे (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त 0;
    }
    MixedBlocks longI3Blocks;
    सचे (hasLongI3Blocks) {
        सचे (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    अत्थाय (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
        int32_t i3;
        uint8_t f = flags[i];
        सचे (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = dataNullOffset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        सचे (f == I3_NULL) {
            i3 = index3NullOffset;
        } अञ्ञथा सचे (f == I3_BMP) {
            i3 = index[i];
        } अञ्ञथा सचे (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, index, i);
            सचे (n >= 0) {
                i3 = n;
            } अञ्ञथा {
                सचे (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अञ्ञथा {
                    n = getOverlap(index16, indexLength,
                                   index, i, UCPTRIE_INDEX_3_BLOCK_LENGTH);
                }
                i3 = indexLength - n;
                int32_t prevIndexLength = indexLength;
                याव (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                    index16[indexLength++] = index[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                सचे (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        } अञ्ञथा {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
            int32_t k = indexLength;
            करोहि {
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
            } याव (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, indexLength);
            सचे (n >= 0) {
                i3 = n | 0x8000;
            } अञ्ञथा {
                सचे (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अञ्ञथा {
                    n = getOverlap(index16, indexLength,
                                   index16, indexLength, INDEX_3_18BIT_BLOCK_LENGTH);
                }
                i3 = (indexLength - n) | 0x8000;
                int32_t prevIndexLength = indexLength;
                सचे (n > 0) {
                    int32_t start = indexLength;
                    याव (n < INDEX_3_18BIT_BLOCK_LENGTH) {
                        index16[indexLength++] = index16[start + n++];
                    }
                } अञ्ञथा {
                    indexLength += INDEX_3_18BIT_BLOCK_LENGTH;
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                सचे (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        }
        सचे (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2Length++] = i3;
    }
    U_ASSERT(i2Length == index2Capacity);
    U_ASSERT(indexLength <= index3Start + index3Capacity);

    सचे (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    सचे (indexLength >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    अत्थाय (int32_t i = 0; i < i2Length; i += blockLength) {
        int32_t n;
        सचे ((i2Length - i) >= blockLength) {
            // normal block
            U_ASSERT(blockLength == UCPTRIE_INDEX_2_BLOCK_LENGTH);
            n = mixedBlocks.findBlock(index16, index2, i);
        } अञ्ञथा {
            // highStart is inside the last index-2 block. Shorten it.
            blockLength = i2Length - i;
            n = findSameBlock(index16, index3Start, indexLength,
                              index2, i, blockLength);
        }
        int32_t i2;
        सचे (n >= 0) {
            i2 = n;
        } अञ्ञथा {
            सचे (indexLength == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } अञ्ञथा {
                n = getOverlap(index16, indexLength, index2, i, blockLength);
            }
            i2 = indexLength - n;
            int32_t prevIndexLength = indexLength;
            याव (n < blockLength) {
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

    पटिनिवत्त indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((उच्चः_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    उच्चः_मानम् = get(MAX_UNICODE);
    int32_t real_उच्चः_Start = find_उच्चः_Start();
    real_उच्चः_Start = (real_उच्चः_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    सचे (real_उच्चः_Start == UNICODE_LIMIT) {
        उच्चः_मानम् = initial_मानम्;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_उच्चः_Start, (long)उच्चः_मानम्, (long)initial_मानम्);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    सचे (real_उच्चः_Start < fastLimit) {
        अत्थाय (int32_t i = (real_उच्चः_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = उच्चः_मानम्;
        }
        उच्चः_Start = fastLimit;
    } अञ्ञथा {
        उच्चः_Start = real_उच्चः_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    अत्थाय (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    सचे (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    सचे (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त 0;
    }
    uprv_memcpy(newData, asciiData, परिमाण(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    सचे (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    सचे (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        सचे (data[dataNullOffset] != initial_मानम्) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_मानम्, (long)data[dataNullOffset]);
        }
#endif
        initial_मानम् = data[dataNullOffset];
    } अञ्ञथा {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    उच्चः_Start = real_उच्चः_Start;
    पटिनिवत्त indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_मानम्_Width मानम्_Width, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मानम्_Width < UCPTRIE_मानम्_BITS_16 || UCPTRIE_मानम्_BITS_8 < मानम्_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    विकल्प (मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_32:
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_16:
        mask_मानानि(0xffff);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        mask_मानानि(0xff);
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    सचे (U_FAILURE(errorCode)) {
        clear();
        पटिनिवत्त nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    सचे (मानम्_Width == UCPTRIE_मानम्_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    सचे (मानम्_Width == UCPTRIE_मानम्_BITS_16) {
        सचे (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_मानम्;
        }
        सचे (data[dataLength - 1] != error_मानम् || data[dataLength - 2] != उच्चः_मानम्) {
            data[dataLength++] = उच्चः_मानम्;
            data[dataLength++] = error_मानम्;
        }
        length += dataLength * 2;
    } अञ्ञथा सचे (मानम्_Width == UCPTRIE_मानम्_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        सचे (data[dataLength - 1] != error_मानम् || data[dataLength - 2] != उच्चः_मानम्) {
            सचे (data[dataLength - 1] != उच्चः_मानम्) {
                data[dataLength++] = उच्चः_मानम्;
            }
            data[dataLength++] = error_मानम्;
        }
        length += dataLength * 4;
    } अञ्ञथा {
        int32_t and3 = (length + dataLength) & 3;
        सचे (and3 == 0 && data[dataLength - 1] == error_मानम् && data[dataLength - 2] == उच्चः_मानम्) {
            // all set
        } अञ्ञथा सचे(and3 == 3 && data[dataLength - 1] == उच्चः_मानम्) {
            data[dataLength++] = error_मानम्;
        } अञ्ञथा {
            याव (and3 != 2) {
                data[dataLength++] = उच्चः_मानम्;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = उच्चः_मानम्;
            data[dataLength++] = error_मानम्;
        }
        length += dataLength;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += परिमाण(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    सचे (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        पटिनिवत्त nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, परिमाण(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->उच्चः_Start = उच्चः_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_उच्चः_Start = (उच्चः_Start + 0xfff) >> 12;
    trie->type = type;
    trie->मानम्_Width = मानम्_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_मानम् = initial_मानम्;

    bytes += परिमाण(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    सचे (उच्चः_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        अत्थाय (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } अञ्ञथा {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    ध्रुव uint32_t *p = data;
    विकल्प (मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        अत्थाय (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)dataLength * 4);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        अत्थाय (int32_t i = dataLength; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        भङ्ग;
    स्वभाव:
        // Will not occur, valueWidth checked at the beginning.
        भङ्ग;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    पटिनिवत्त trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_मानम्, uint32_t error_मानम्, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        नूतन MutableCodePointTrie(initial_मानम्, error_मानम्, *pErrorCode), *pErrorCode);
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ध्रुव UMutableCPTrie *other, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे (other == nullptr) {
        पटिनिवत्त nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        नूतन MutableCodePointTrie(*reinterpret_cast<ध्रुव MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ध्रुव UCPMap *map, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ध्रुव UCPTrie *trie, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ध्रुव UMutableCPTrie *trie, UChar32 c) {
    पटिनिवत्त reinterpret_cast<ध्रुव MutableCodePointTrie *>(trie)->get(c);
}

नामाकाश {

UChar32 getRange(ध्रुव शून्य *trie, UChar32 start,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    पटिनिवत्त reinterpret_cast<ध्रुव MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_मानम्);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ध्रुव UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_मानम्,
                        UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    पटिनिवत्त ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मानम्,
                                    filter, context, p_मानम्);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_मानम्_Width मानम्_Width,
                              UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, मानम्_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC शून्य umutablecptrie_setName(UMutableCPTrie *trie, ध्रुव वर्ण *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
