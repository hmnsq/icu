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
    MutableCodePointTrie(uint32_t initial_मूल्य, uint32_t error_मूल्य, UErrorCode &errorCode);
    MutableCodePointTrie(ध्रुव MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(ध्रुव MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(ध्रुव MutableCodePointTrie &other) = delete;

    स्थिर MutableCodePointTrie *fromUCPMap(ध्रुव UCPMap *map, UErrorCode &errorCode);
    स्थिर MutableCodePointTrie *fromUCPTrie(ध्रुव UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) ध्रुव;
    int32_t getRange(UChar32 start, UCPMap_मूल्य_Filter *filter, ध्रुव शून्य *context,
                     uint32_t *p_मूल्य) ध्रुव;

    शून्य set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    शून्य setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_मूल्य_Width मूल्य_Width, UErrorCode &errorCode);

गुप्त:
    शून्य clear();

    तार्किक ensure_ऊपरी_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    शून्य mask_मूल्यावली(uint32_t mask);
    UChar32 find_ऊपरी_Start() ध्रुव;
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

    uint32_t orig_Initial_मूल्य;
    uint32_t initial_मूल्य;
    uint32_t error_मूल्य;
    UChar32 ऊपरी_Start;
    uint32_t ऊपरी_मूल्य;
#ifdef UCPTRIE_DEBUG
सार्वजनिक:
    ध्रुव वर्ण *name;
#endif
गुप्त:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_मूल्य, uint32_t err_मूल्य, UErrorCode &errorCode) :
        orig_Initial_मूल्य(ini_मूल्य), initial_मूल्य(ini_मूल्य), error_मूल्य(err_मूल्य),
        ऊपरी_Start(0), ऊपरी_मूल्य(initial_मूल्य)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    यदि (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(ध्रुव MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_मूल्य(other.orig_Initial_मूल्य), initial_मूल्य(other.initial_मूल्य),
        error_मूल्य(other.error_मूल्य),
        ऊपरी_Start(other.ऊपरी_Start), ऊपरी_मूल्य(other.ऊपरी_मूल्य)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    int32_t iCapacity = ऊपरी_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    यदि (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
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
    uint32_t error_मूल्य = ucpmap_get(map, -1);
    uint32_t initial_मूल्य = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नूतन MutableCodePointTrie(initial_मूल्य, error_मूल्य, errorCode),
        errorCode);
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    यावत् ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        यदि (value != initial_मूल्य) {
            यदि (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अन्यथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    यदि (U_SUCCESS(errorCode)) {
        प्रत्यागम mutableTrie.orphan();
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(ध्रुव UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मूल्य;
    uint32_t initial_मूल्य;
    विकल्प (trie->मूल्य_Width) {
    पक्ष UCPTRIE_मूल्य_BITS_16:
        error_मूल्य = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_मूल्य_NEG_DATA_OFFSET];
        initial_मूल्य = trie->data.ptr16[trie->dataLength - UCPTRIE_ऊपरी_मूल्य_NEG_DATA_OFFSET];
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_32:
        error_मूल्य = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_मूल्य_NEG_DATA_OFFSET];
        initial_मूल्य = trie->data.ptr32[trie->dataLength - UCPTRIE_ऊपरी_मूल्य_NEG_DATA_OFFSET];
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_8:
        error_मूल्य = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_मूल्य_NEG_DATA_OFFSET];
        initial_मूल्य = trie->data.ptr8[trie->dataLength - UCPTRIE_ऊपरी_मूल्य_NEG_DATA_OFFSET];
        भङ्ग;
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नूतन MutableCodePointTrie(initial_मूल्य, error_मूल्य, errorCode),
        errorCode);
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    यावत् ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        यदि (value != initial_मूल्य) {
            यदि (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अन्यथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    यदि (U_SUCCESS(errorCode)) {
        प्रत्यागम mutableTrie.orphan();
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

शून्य MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    ऊपरी_मूल्य = initial_मूल्य = orig_Initial_मूल्य;
    ऊपरी_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) ध्रुव {
    यदि (static_cast<uint32_t>(c) > MAX_UNICODE) {
        प्रत्यागम error_मूल्य;
    }
    यदि (c >= ऊपरी_Start) {
        प्रत्यागम ऊपरी_मूल्य;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    यदि (flags[i] == ALL_SAME) {
        प्रत्यागम index[i];
    } अन्यथा {
        प्रत्यागम data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

अन्तःस्थ uint32_t maybe_Filter_मूल्य(uint32_t value, uint32_t initial_मूल्य, uint32_t null_मूल्य,
                                 UCPMap_मूल्य_Filter *filter, ध्रुव शून्य *context) {
    यदि (value == initial_मूल्य) {
        value = null_मूल्य;
    } अन्यथा यदि (filter != nullptr) {
        value = filter(context, value);
    }
    प्रत्यागम value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_मूल्य_Filter *filter, ध्रुव शून्य *context,
        uint32_t *p_मूल्य) ध्रुव {
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE) {
        प्रत्यागम U_SENTINEL;
    }
    यदि (start >= ऊपरी_Start) {
        यदि (p_मूल्य != nullptr) {
            uint32_t value = ऊपरी_मूल्य;
            यदि (filter != nullptr) { value = filter(context, value); }
            *p_मूल्य = value;
        }
        प्रत्यागम MAX_UNICODE;
    }
    uint32_t null_मूल्य = initial_मूल्य;
    यदि (filter != nullptr) { null_मूल्य = filter(context, null_मूल्य); }
    UChar32 c = start;
    uint32_t trie_मूल्य, value;
    तार्किक have_मूल्य = असत्य;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    कुरु {
        यदि (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            यदि (have_मूल्य) {
                यदि (trieValue2 != trie_मूल्य) {
                    यदि (filter == nullptr ||
                            maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य,
                                             filter, context) != value) {
                        प्रत्यागम c - 1;
                    }
                    trie_मूल्य = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मूल्य = trieValue2;
                value = maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य, filter, context);
                यदि (p_मूल्य != nullptr) { *p_मूल्य = value; }
                have_मूल्य = सत्य;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } अन्यथा /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            यदि (have_मूल्य) {
                यदि (trieValue2 != trie_मूल्य) {
                    यदि (filter == nullptr ||
                            maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य,
                                             filter, context) != value) {
                        प्रत्यागम c - 1;
                    }
                    trie_मूल्य = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मूल्य = trieValue2;
                value = maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य, filter, context);
                यदि (p_मूल्य != nullptr) { *p_मूल्य = value; }
                have_मूल्य = सत्य;
            }
            यावत् ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                यदि (trieValue2 != trie_मूल्य) {
                    यदि (filter == nullptr ||
                            maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य,
                                             filter, context) != value) {
                        प्रत्यागम c - 1;
                    }
                }
                trie_मूल्य = trieValue2;  // may or may not help
            }
        }
        ++i;
    } यावत् (c < ऊपरी_Start);
    U_ASSERT(have_मूल्य);
    यदि (maybe_Filter_मूल्य(ऊपरी_मूल्य, initial_मूल्य, null_मूल्य,
                         filter, context) != value) {
        प्रत्यागम c - 1;
    } अन्यथा {
        प्रत्यागम MAX_UNICODE;
    }
}

शून्य
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    यावत् (block < limit) {
        *block++ = value;
    }
}

तार्किक MutableCodePointTrie::ensure_ऊपरी_Start(UChar32 c) {
    यदि (c >= ऊपरी_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = ऊपरी_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        यदि (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            यदि (newIndex == nullptr) { प्रत्यागम असत्य; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        कुरु {
            flags[i] = ALL_SAME;
            index[i] = initial_मूल्य;
        } यावत्(++i < iLimit);
        ऊपरी_Start = c;
    }
    प्रत्यागम सत्य;
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
            प्रत्यागम -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        यदि (newData == nullptr) {
            प्रत्यागम -1;
        }
        uprv_memcpy(newData, data, (size_t)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_शीर्ष;
    प्रत्यागम newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    यदि (flags[i] == MIXED) {
        प्रत्यागम index[i];
    }
    यदि (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        यदि (newBlock < 0) { प्रत्यागम newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        कुरु {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } यावत् (iStart < iLimit);
        प्रत्यागम index[i];
    } अन्यथा {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        यदि (newBlock < 0) { प्रत्यागम newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        प्रत्यागम newBlock;
    }
}

शून्य MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    यदि (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    int32_t block;
    यदि (!ensure_ऊपरी_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

शून्य
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    यावत् (block < pLimit) {
        *block++ = value;
    }
}

शून्य MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    यदि (!ensure_ऊपरी_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }

    UChar32 limit = end + 1;
    यदि (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        यदि (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        यदि (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } अन्यथा {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            प्रत्यागम;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    यावत् (start < limit) {
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
            प्रत्यागम;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

शून्य MutableCodePointTrie::mask_मूल्यावली(uint32_t mask) {
    initial_मूल्य &= mask;
    error_मूल्य &= mask;
    ऊपरी_मूल्य &= mask;
    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    प्रति (int32_t i = 0; i < iLimit; ++i) {
        यदि (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    प्रति (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
तार्किक equalBlocks(ध्रुव UIntA *s, ध्रुव UIntB *t, int32_t length) {
    यावत् (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    प्रत्यागम length == 0;
}

तार्किक all_मूल्यावली_Same_As(ध्रुव uint32_t *p, int32_t length, uint32_t value) {
    ध्रुव uint32_t *pLimit = p + length;
    यावत् (p < pLimit && *p == value) { ++p; }
    प्रत्यागम p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(ध्रुव uint16_t *p, int32_t pStart, int32_t length,
                      ध्रुव uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    यावत् (pStart <= length) {
        यदि (equalBlocks(p + pStart, q, blockLength)) {
            प्रत्यागम pStart;
        }
        ++pStart;
    }
    प्रत्यागम -1;
}

int32_t findAllSameBlock(ध्रुव uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    प्रति (int32_t block = start; block <= limit; ++block) {
        यदि (p[block] == value) {
            प्रति (int32_t i = 1;; ++i) {
                यदि (i == blockLength) {
                    प्रत्यागम block;
                }
                यदि (p[block + i] != value) {
                    block += i;
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
template<typename UIntA, typename UIntB>
int32_t getOverlap(ध्रुव UIntA *p, int32_t length,
                   ध्रुव UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    यावत् (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    प्रत्यागम overlap;
}

int32_t getAllSameOverlap(ध्रुव uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    यावत् (min < i && p[i - 1] == value) { --i; }
    प्रत्यागम length - i;
}

तार्किक isStartOfSomeFastBlock(uint32_t dataOffset, ध्रुव uint32_t index[], int32_t fastILimit) {
    प्रति (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        यदि (index[i] == dataOffset) {
            प्रत्यागम सत्य;
        }
    }
    प्रत्यागम असत्य;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_ऊपरी_Start() ध्रुव {
    int32_t i = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    यावत् (i > 0) {
        तार्किक match;
        यदि (flags[--i] == ALL_SAME) {
            match = index[i] == ऊपरी_मूल्य;
        } अन्यथा /* MIXED */ {
            ध्रुव uint32_t *p = data + index[i];
            प्रति (int32_t j = 0;; ++j) {
                यदि (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = सत्य;
                    भङ्ग;
                }
                यदि (p[j] != ऊपरी_मूल्य) {
                    match = असत्य;
                    भङ्ग;
                }
            }
        }
        यदि (!match) {
            प्रत्यागम (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    प्रत्यागम 0;
}

वर्ग AllSameBlocks {
सार्वजनिक:
    स्थिर constexpr int32_t NEW_UNIQUE = -1;
    स्थिर constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        यदि (mostRecent >= 0 && मूल्यावली[mostRecent] == value) {
            refCounts[mostRecent] += count;
            प्रत्यागम indexes[mostRecent];
        }
        प्रति (int32_t i = 0; i < length; ++i) {
            यदि (मूल्यावली[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                प्रत्यागम indexes[i];
            }
        }
        यदि (length == CAPACITY) {
            प्रत्यागम OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        मूल्यावली[length] = value;
        refCounts[length++] = count;
        प्रत्यागम NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    शून्य add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_गिनती = I_LIMIT;
        प्रति (int32_t i = 0; i < length; ++i) {
            U_ASSERT(मूल्यावली[i] != value);
            यदि (refCounts[i] < least_गिनती) {
                least = i;
                least_गिनती = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        मूल्यावली[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() ध्रुव {
        यदि (length == 0) { प्रत्यागम -1; }
        int32_t max = -1;
        int32_t max_गिनती = 0;
        प्रति (int32_t i = 0; i < length; ++i) {
            यदि (refCounts[i] > max_गिनती) {
                max = i;
                max_गिनती = refCounts[i];
            }
        }
        प्रत्यागम indexes[max];
    }

गुप्त:
    स्थिर constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t मूल्यावली[CAPACITY];
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
                प्रत्यागम असत्य;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        प्रत्यागम सत्य;
    }

    template<typename UInt>
    शून्य extend(ध्रुव UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        यदि (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } अन्यथा {
            start = minStart;  // Begin with the first full block.
        }
        प्रति (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(ध्रुव UIntA *data, ध्रुव UIntB *blockData, int32_t blockStart) ध्रुव {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        यदि (entryIndex >= 0) {
            प्रत्यागम (table[entryIndex] & mask) - 1;
        } अन्यथा {
            प्रत्यागम -1;
        }
    }

    int32_t findAllSameBlock(ध्रुव uint32_t *data, uint32_t block_मूल्य) ध्रुव {
        uint32_t hashCode = makeHashCode(block_मूल्य);
        int32_t entryIndex = findEntry(data, block_मूल्य, hashCode);
        यदि (entryIndex >= 0) {
            प्रत्यागम (table[entryIndex] & mask) - 1;
        } अन्यथा {
            प्रत्यागम -1;
        }
    }

गुप्त:
    template<typename UInt>
    uint32_t makeHashCode(ध्रुव UInt *blockData, int32_t blockStart) ध्रुव {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        कुरु {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } यावत् (blockStart < blockLimit);
        प्रत्यागम hashCode;
    }

    uint32_t makeHashCode(uint32_t block_मूल्य) ध्रुव {
        uint32_t hashCode = block_मूल्य;
        प्रति (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_मूल्य;
        }
        प्रत्यागम hashCode;
    }

    template<typename UInt>
    शून्य addEntry(ध्रुव UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        यदि (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(ध्रुव UIntA *data, ध्रुव UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) ध्रुव {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        प्रति (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            यदि (entry == 0) {
                प्रत्यागम ~entryIndex;
            }
            यदि ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                यदि (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    प्रत्यागम entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(ध्रुव uint32_t *data, uint32_t block_मूल्य, uint32_t hashCode) ध्रुव {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        प्रति (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            यदि (entry == 0) {
                प्रत्यागम ~entryIndex;
            }
            यदि ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                यदि (all_मूल्यावली_Same_As(data + dataIndex, blockLength, block_मूल्य)) {
                    प्रत्यागम entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    अन्तःस्थ int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) ध्रुव {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        प्रत्यागम (entryIndex + initialEntryIndex) % length;
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
    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    प्रति (int32_t i = 0; i < iLimit; i += inc) {
        यदि (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        यदि (flags[i] == MIXED) {
            // Really mixed?
            ध्रुव uint32_t *p = data + value;
            value = *p;
            यदि (all_मूल्यावली_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } अन्यथा {
                newDataCapacity += blockLength;
                अनुवर्त;
            }
        } अन्यथा {
            U_ASSERT(flags[i] == ALL_SAME);
            यदि (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                तार्किक allSame = सत्य;
                int32_t next_i = i + inc;
                प्रति (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    यदि (index[j] != value) {
                        allSame = असत्य;
                        भङ्ग;
                    }
                }
                यदि (!allSame) {
                    // Turn it into a MIXED block.
                    यदि (getDataBlock(i) < 0) {
                        प्रत्यागम -1;
                    }
                    newDataCapacity += blockLength;
                    अनुवर्त;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        यदि (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            यदि (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = सत्य;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            प्रति (int32_t j = 0;; j += jInc) {
                यदि (j == i) {
                    allSameBlocks.add(i, inc, value);
                    भङ्ग;
                }
                यदि (j == fastILimit) {
                    jInc = 1;
                }
                यदि (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
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
            index[i] = other;
        } अन्यथा {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    प्रत्यागम newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(व्यंजक) व्यंजक
#अन्यथा
#   define DEBUG_DO(व्यंजक)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_मूल्य(वर्ण s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (वर्ण)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (वर्ण)(0x80 + (value & 0x3F));
    प्रत्यागम length + 3;
}

शून्य printBlock(ध्रुव uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_मूल्य) {
    वर्ण s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    प्रति (i = 0; i < overlap; ++i) {
        length = append_मूल्य(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    प्रति (; i < blockLength; ++i) {
        यदि (block != nullptr) {
            value = block[i];
        }
        यदि (value == initial_मूल्य) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_मूल्य(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    यदि (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } अन्यथा यदि (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } अन्यथा {
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
    int32_t गिनती_Same=0, sumOverlaps=0;
    तार्किक printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    प्रति (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        यदि (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_मूल्य);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    यदि (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    प्रति (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        यदि (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            यदि (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम 0;
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
            यावत् (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            यदि (n >= 0) {
                DEBUG_DO(++गिनती_Same);
                index[i] = n;
            } अन्यथा {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                यदि (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_मूल्य);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                यावत् (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } अन्यथा यदि (flags[i] == MIXED) {
            ध्रुव uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            यदि (n >= 0) {
                DEBUG_DO(++गिनती_Same);
                index[i] = n;
            } अन्यथा {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                यदि (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_मूल्य);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                यावत् (n < blockLength) {
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
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)dataLength, (long)newDataLength, (long)गिनती_Same, (long)sumOverlaps);
#endif
    प्रत्यागम newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    यदि ((ऊपरी_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        प्रत्यागम fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    प्रति (int32_t i = 0, j = 0; i < fastILimit; ++j) {
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
        यावत् (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    यदि (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
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
    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    प्रति (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        तार्किक isNull = सत्य;
        कुरु {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            यदि (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = असत्य;
            }
        } यावत् (++j < jLimit);
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
        प्रत्यागम 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    यदि (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
    }
    MixedBlocks longI3Blocks;
    यदि (hasLongI3Blocks) {
        यदि (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    प्रति (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
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
                यावत् (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
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
            कुरु {
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
            } यावत् (j < jLimit);
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
                    यावत् (n < INDEX_3_18BIT_BLOCK_LENGTH) {
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
        प्रत्यागम 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    प्रति (int32_t i = 0; i < i2Length; i += blockLength) {
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
            यावत् (n < blockLength) {
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

    प्रत्यागम indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((ऊपरी_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    ऊपरी_मूल्य = get(MAX_UNICODE);
    int32_t real_ऊपरी_Start = find_ऊपरी_Start();
    real_ऊपरी_Start = (real_ऊपरी_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    यदि (real_ऊपरी_Start == UNICODE_LIMIT) {
        ऊपरी_मूल्य = initial_मूल्य;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_ऊपरी_Start, (long)ऊपरी_मूल्य, (long)initial_मूल्य);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    यदि (real_ऊपरी_Start < fastLimit) {
        प्रति (int32_t i = (real_ऊपरी_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = ऊपरी_मूल्य;
        }
        ऊपरी_Start = fastLimit;
    } अन्यथा {
        ऊपरी_Start = real_ऊपरी_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    प्रति (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    यदि (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    यदि (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
    }
    uprv_memcpy(newData, asciiData, परिमाण(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    यदि (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    यदि (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        यदि (data[dataNullOffset] != initial_मूल्य) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_मूल्य, (long)data[dataNullOffset]);
        }
#endif
        initial_मूल्य = data[dataNullOffset];
    } अन्यथा {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    ऊपरी_Start = real_ऊपरी_Start;
    प्रत्यागम indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_मूल्य_Width मूल्य_Width, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    यदि (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मूल्य_Width < UCPTRIE_मूल्य_BITS_16 || UCPTRIE_मूल्य_BITS_8 < मूल्य_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    विकल्प (मूल्य_Width) {
    पक्ष UCPTRIE_मूल्य_BITS_32:
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_16:
        mask_मूल्यावली(0xffff);
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_8:
        mask_मूल्यावली(0xff);
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    यदि (U_FAILURE(errorCode)) {
        clear();
        प्रत्यागम nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    यदि (मूल्य_Width == UCPTRIE_मूल्य_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    यदि (मूल्य_Width == UCPTRIE_मूल्य_BITS_16) {
        यदि (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_मूल्य;
        }
        यदि (data[dataLength - 1] != error_मूल्य || data[dataLength - 2] != ऊपरी_मूल्य) {
            data[dataLength++] = ऊपरी_मूल्य;
            data[dataLength++] = error_मूल्य;
        }
        length += dataLength * 2;
    } अन्यथा यदि (मूल्य_Width == UCPTRIE_मूल्य_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        यदि (data[dataLength - 1] != error_मूल्य || data[dataLength - 2] != ऊपरी_मूल्य) {
            यदि (data[dataLength - 1] != ऊपरी_मूल्य) {
                data[dataLength++] = ऊपरी_मूल्य;
            }
            data[dataLength++] = error_मूल्य;
        }
        length += dataLength * 4;
    } अन्यथा {
        int32_t and3 = (length + dataLength) & 3;
        यदि (and3 == 0 && data[dataLength - 1] == error_मूल्य && data[dataLength - 2] == ऊपरी_मूल्य) {
            // all set
        } अन्यथा यदि(and3 == 3 && data[dataLength - 1] == ऊपरी_मूल्य) {
            data[dataLength++] = error_मूल्य;
        } अन्यथा {
            यावत् (and3 != 2) {
                data[dataLength++] = ऊपरी_मूल्य;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = ऊपरी_मूल्य;
            data[dataLength++] = error_मूल्य;
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
        प्रत्यागम nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, परिमाण(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->ऊपरी_Start = ऊपरी_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ऊपरी_Start = (ऊपरी_Start + 0xfff) >> 12;
    trie->type = type;
    trie->मूल्य_Width = मूल्य_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_मूल्य = initial_मूल्य;

    bytes += परिमाण(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    यदि (ऊपरी_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        प्रति (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } अन्यथा {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    ध्रुव uint32_t *p = data;
    विकल्प (मूल्य_Width) {
    पक्ष UCPTRIE_मूल्य_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        प्रति (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)dataLength * 4);
        भङ्ग;
    पक्ष UCPTRIE_मूल्य_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        प्रति (int32_t i = dataLength; i > 0; --i) {
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
    प्रत्यागम trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_मूल्य, uint32_t error_मूल्य, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        नूतन MutableCodePointTrie(initial_मूल्य, error_मूल्य, *pErrorCode), *pErrorCode);
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(ध्रुव UMutableCPTrie *other, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    यदि (other == nullptr) {
        प्रत्यागम nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        नूतन MutableCodePointTrie(*reinterpret_cast<ध्रुव MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(ध्रुव UCPMap *map, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    यदि (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(ध्रुव UCPTrie *trie, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    यदि (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(ध्रुव UMutableCPTrie *trie, UChar32 c) {
    प्रत्यागम reinterpret_cast<ध्रुव MutableCodePointTrie *>(trie)->get(c);
}

नामाकाश {

UChar32 getRange(ध्रुव शून्य *trie, UChar32 start,
                 UCPMap_मूल्य_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मूल्य) {
    प्रत्यागम reinterpret_cast<ध्रुव MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_मूल्य);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(ध्रुव UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                        UCPMap_मूल्य_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मूल्य) {
    प्रत्यागम ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मूल्य,
                                    filter, context, p_मूल्य);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI शून्य U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_मूल्य_Width मूल्य_Width,
                              UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, मूल्य_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC शून्य umutablecptrie_setName(UMutableCPTrie *trie, ध्रुव वर्ण *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
