// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/umutablecptrie.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ifdef OVERFLOW
#undef OVERFLOW
#endif

U_NAMESPACE_BEGIN

નામાકાશ {

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

વર્ગ AllSameBlocks;
વર્ગ MixedBlocks;

વર્ગ MutableCodePointTrie : જાહેર UMemory {
જાહેર:
    MutableCodePointTrie(uint32_t initial_કિંમત, uint32_t error_કિંમત, UErrorCode &errorCode);
    MutableCodePointTrie(અચલ MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(અચલ MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(અચલ MutableCodePointTrie &other) = delete;

    સ્થિર MutableCodePointTrie *fromUCPMap(અચલ UCPMap *map, UErrorCode &errorCode);
    સ્થિર MutableCodePointTrie *fromUCPTrie(અચલ UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) અચલ;
    int32_t getRange(UChar32 start, UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context,
                     uint32_t *p_કિંમત) અચલ;

    શૂન્ય set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    શૂન્ય setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_કિંમત_Width કિંમત_Width, UErrorCode &errorCode);

ખાનગી:
    શૂન્ય clear();

    તાર્કિક ensure_ઉપલું_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    શૂન્ય mask_કિંમતો(uint32_t mask);
    UChar32 find_ઉપલું_Start() અચલ;
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

    uint32_t orig_Initial_કિંમત;
    uint32_t initial_કિંમત;
    uint32_t error_કિંમત;
    UChar32 ઉપલું_Start;
    uint32_t ઉપલું_કિંમત;
#ifdef UCPTRIE_DEBUG
જાહેર:
    અચલ અક્ષર *name;
#endif
ખાનગી:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_કિંમત, uint32_t err_કિંમત, UErrorCode &errorCode) :
        orig_Initial_કિંમત(ini_કિંમત), initial_કિંમત(ini_કિંમત), error_કિંમત(err_કિંમત),
        ઉપલું_Start(0), ઉપલું_કિંમત(initial_કિંમત)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    જો (U_FAILURE(errorCode)) { પાછા; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    જો (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(અચલ MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_કિંમત(other.orig_Initial_કિંમત), initial_કિંમત(other.initial_કિંમત),
        error_કિંમત(other.error_કિંમત),
        ઉપલું_Start(other.ઉપલું_Start), ઉપલું_કિંમત(other.ઉપલું_કિંમત)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    જો (U_FAILURE(errorCode)) { પાછા; }
    int32_t iCapacity = ઉપલું_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    જો (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = ઉપલું_Start >> UCPTRIE_SHIFT_3;
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

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(અચલ UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_કિંમત = ucpmap_get(map, -1);
    uint32_t initial_કિંમત = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        નવું MutableCodePointTrie(initial_કિંમત, error_કિંમત, errorCode),
        errorCode);
    જો (U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    જ્યારે ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        જો (value != initial_કિંમત) {
            જો (start == end) {
                mutableTrie->set(start, value, errorCode);
            } નહીંતર {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    જો (U_SUCCESS(errorCode)) {
        પાછા mutableTrie.orphan();
    } નહીંતર {
        પાછા nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(અચલ UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_કિંમત;
    uint32_t initial_કિંમત;
    પસંદગી (trie->કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        error_કિંમત = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_કિંમત_NEG_DATA_OFFSET];
        initial_કિંમત = trie->data.ptr16[trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET];
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        error_કિંમત = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_કિંમત_NEG_DATA_OFFSET];
        initial_કિંમત = trie->data.ptr32[trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET];
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        error_કિંમત = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_કિંમત_NEG_DATA_OFFSET];
        initial_કિંમત = trie->data.ptr8[trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET];
        તોડો;
    મૂળભૂત:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        નવું MutableCodePointTrie(initial_કિંમત, error_કિંમત, errorCode),
        errorCode);
    જો (U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    જ્યારે ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        જો (value != initial_કિંમત) {
            જો (start == end) {
                mutableTrie->set(start, value, errorCode);
            } નહીંતર {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    જો (U_SUCCESS(errorCode)) {
        પાછા mutableTrie.orphan();
    } નહીંતર {
        પાછા nullptr;
    }
}

શૂન્ય MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    ઉપલું_કિંમત = initial_કિંમત = orig_Initial_કિંમત;
    ઉપલું_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) અચલ {
    જો (static_cast<uint32_t>(c) > MAX_UNICODE) {
        પાછા error_કિંમત;
    }
    જો (c >= ઉપલું_Start) {
        પાછા ઉપલું_કિંમત;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    જો (flags[i] == ALL_SAME) {
        પાછા index[i];
    } નહીંતર {
        પાછા data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

અંતઃરેખ uint32_t maybe_Filter_કિંમત(uint32_t value, uint32_t initial_કિંમત, uint32_t null_કિંમત,
                                 UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context) {
    જો (value == initial_કિંમત) {
        value = null_કિંમત;
    } નહીંતર જો (filter != nullptr) {
        value = filter(context, value);
    }
    પાછા value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context,
        uint32_t *p_કિંમત) અચલ {
    જો (static_cast<uint32_t>(start) > MAX_UNICODE) {
        પાછા U_SENTINEL;
    }
    જો (start >= ઉપલું_Start) {
        જો (p_કિંમત != nullptr) {
            uint32_t value = ઉપલું_કિંમત;
            જો (filter != nullptr) { value = filter(context, value); }
            *p_કિંમત = value;
        }
        પાછા MAX_UNICODE;
    }
    uint32_t null_કિંમત = initial_કિંમત;
    જો (filter != nullptr) { null_કિંમત = filter(context, null_કિંમત); }
    UChar32 c = start;
    uint32_t trie_કિંમત, value;
    તાર્કિક have_કિંમત = ખોટું;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    કરો {
        જો (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            જો (have_કિંમત) {
                જો (trieValue2 != trie_કિંમત) {
                    જો (filter == nullptr ||
                            maybe_Filter_કિંમત(trieValue2, initial_કિંમત, null_કિંમત,
                                             filter, context) != value) {
                        પાછા c - 1;
                    }
                    trie_કિંમત = trieValue2;  // may or may not help
                }
            } નહીંતર {
                trie_કિંમત = trieValue2;
                value = maybe_Filter_કિંમત(trieValue2, initial_કિંમત, null_કિંમત, filter, context);
                જો (p_કિંમત != nullptr) { *p_કિંમત = value; }
                have_કિંમત = સાચું;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } નહીંતર /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            જો (have_કિંમત) {
                જો (trieValue2 != trie_કિંમત) {
                    જો (filter == nullptr ||
                            maybe_Filter_કિંમત(trieValue2, initial_કિંમત, null_કિંમત,
                                             filter, context) != value) {
                        પાછા c - 1;
                    }
                    trie_કિંમત = trieValue2;  // may or may not help
                }
            } નહીંતર {
                trie_કિંમત = trieValue2;
                value = maybe_Filter_કિંમત(trieValue2, initial_કિંમત, null_કિંમત, filter, context);
                જો (p_કિંમત != nullptr) { *p_કિંમત = value; }
                have_કિંમત = સાચું;
            }
            જ્યારે ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                જો (trieValue2 != trie_કિંમત) {
                    જો (filter == nullptr ||
                            maybe_Filter_કિંમત(trieValue2, initial_કિંમત, null_કિંમત,
                                             filter, context) != value) {
                        પાછા c - 1;
                    }
                }
                trie_કિંમત = trieValue2;  // may or may not help
            }
        }
        ++i;
    } જ્યારે (c < ઉપલું_Start);
    U_ASSERT(have_કિંમત);
    જો (maybe_Filter_કિંમત(ઉપલું_કિંમત, initial_કિંમત, null_કિંમત,
                         filter, context) != value) {
        પાછા c - 1;
    } નહીંતર {
        પાછા MAX_UNICODE;
    }
}

શૂન્ય
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    જ્યારે (block < limit) {
        *block++ = value;
    }
}

તાર્કિક MutableCodePointTrie::ensure_ઉપલું_Start(UChar32 c) {
    જો (c >= ઉપલું_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = ઉપલું_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        જો (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            જો (newIndex == nullptr) { પાછા ખોટું; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        કરો {
            flags[i] = ALL_SAME;
            index[i] = initial_કિંમત;
        } જ્યારે(++i < iLimit);
        ઉપલું_Start = c;
    }
    પાછા સાચું;
}

int32_t MutableCodePointTrie::allocDataBlock(int32_t blockLength) {
    int32_t newBlock = dataLength;
    int32_t new_ટોચ = newBlock + blockLength;
    જો (new_ટોચ > dataCapacity) {
        int32_t capacity;
        જો (dataCapacity < MEDIUM_DATA_LENGTH) {
            capacity = MEDIUM_DATA_LENGTH;
        } નહીંતર જો (dataCapacity < MAX_DATA_LENGTH) {
            capacity = MAX_DATA_LENGTH;
        } નહીંતર {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            પાછા -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        જો (newData == nullptr) {
            પાછા -1;
        }
        uprv_memcpy(newData, data, (size_t)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_ટોચ;
    પાછા newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    જો (flags[i] == MIXED) {
        પાછા index[i];
    }
    જો (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        જો (newBlock < 0) { પાછા newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        કરો {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } જ્યારે (iStart < iLimit);
        પાછા index[i];
    } નહીંતર {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        જો (newBlock < 0) { પાછા newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        પાછા newBlock;
    }
}

શૂન્ય MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }
    જો (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    int32_t block;
    જો (!ensure_ઉપલું_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

શૂન્ય
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    જ્યારે (block < pLimit) {
        *block++ = value;
    }
}

શૂન્ય MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }
    જો (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    જો (!ensure_ઉપલું_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    UChar32 limit = end + 1;
    જો (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        જો (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        જો (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } નહીંતર {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            પાછા;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    જ્યારે (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        જો (flags[i] == ALL_SAME) {
            index[i] = value;
        } નહીંતર /* MIXED */ {
            fillBlock(data + index[i], 0, UCPTRIE_SMALL_DATA_BLOCK_LENGTH, value);
        }
        start += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    }

    જો (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        જો (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

શૂન્ય MutableCodePointTrie::mask_કિંમતો(uint32_t mask) {
    initial_કિંમત &= mask;
    error_કિંમત &= mask;
    ઉપલું_કિંમત &= mask;
    int32_t iLimit = ઉપલું_Start >> UCPTRIE_SHIFT_3;
    માટે (int32_t i = 0; i < iLimit; ++i) {
        જો (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    માટે (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
તાર્કિક equalBlocks(અચલ UIntA *s, અચલ UIntB *t, int32_t length) {
    જ્યારે (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    પાછા length == 0;
}

તાર્કિક all_કિંમતો_Same_As(અચલ uint32_t *p, int32_t length, uint32_t value) {
    અચલ uint32_t *pLimit = p + length;
    જ્યારે (p < pLimit && *p == value) { ++p; }
    પાછા p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(અચલ uint16_t *p, int32_t pStart, int32_t length,
                      અચલ uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    જ્યારે (pStart <= length) {
        જો (equalBlocks(p + pStart, q, blockLength)) {
            પાછા pStart;
        }
        ++pStart;
    }
    પાછા -1;
}

int32_t findAllSameBlock(અચલ uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    માટે (int32_t block = start; block <= limit; ++block) {
        જો (p[block] == value) {
            માટે (int32_t i = 1;; ++i) {
                જો (i == blockLength) {
                    પાછા block;
                }
                જો (p[block + i] != value) {
                    block += i;
                    તોડો;
                }
            }
        }
    }
    પાછા -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
template<typename UIntA, typename UIntB>
int32_t getOverlap(અચલ UIntA *p, int32_t length,
                   અચલ UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    જ્યારે (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    પાછા overlap;
}

int32_t getAllSameOverlap(અચલ uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    જ્યારે (min < i && p[i - 1] == value) { --i; }
    પાછા length - i;
}

તાર્કિક isStartOfSomeFastBlock(uint32_t dataOffset, અચલ uint32_t index[], int32_t fastILimit) {
    માટે (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        જો (index[i] == dataOffset) {
            પાછા સાચું;
        }
    }
    પાછા ખોટું;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_ઉપલું_Start() અચલ {
    int32_t i = ઉપલું_Start >> UCPTRIE_SHIFT_3;
    જ્યારે (i > 0) {
        તાર્કિક match;
        જો (flags[--i] == ALL_SAME) {
            match = index[i] == ઉપલું_કિંમત;
        } નહીંતર /* MIXED */ {
            અચલ uint32_t *p = data + index[i];
            માટે (int32_t j = 0;; ++j) {
                જો (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = સાચું;
                    તોડો;
                }
                જો (p[j] != ઉપલું_કિંમત) {
                    match = ખોટું;
                    તોડો;
                }
            }
        }
        જો (!match) {
            પાછા (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    પાછા 0;
}

વર્ગ AllSameBlocks {
જાહેર:
    સ્થિર constexpr int32_t NEW_UNIQUE = -1;
    સ્થિર constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        જો (mostRecent >= 0 && કિંમતો[mostRecent] == value) {
            refCounts[mostRecent] += count;
            પાછા indexes[mostRecent];
        }
        માટે (int32_t i = 0; i < length; ++i) {
            જો (કિંમતો[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                પાછા indexes[i];
            }
        }
        જો (length == CAPACITY) {
            પાછા OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        કિંમતો[length] = value;
        refCounts[length++] = count;
        પાછા NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    શૂન્ય add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_ગણતરી = I_LIMIT;
        માટે (int32_t i = 0; i < length; ++i) {
            U_ASSERT(કિંમતો[i] != value);
            જો (refCounts[i] < least_ગણતરી) {
                least = i;
                least_ગણતરી = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        કિંમતો[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() અચલ {
        જો (length == 0) { પાછા -1; }
        int32_t max = -1;
        int32_t max_ગણતરી = 0;
        માટે (int32_t i = 0; i < length; ++i) {
            જો (refCounts[i] > max_ગણતરી) {
                max = i;
                max_ગણતરી = refCounts[i];
            }
        }
        પાછા indexes[max];
    }

ખાનગી:
    સ્થિર constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t કિંમતો[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
વર્ગ MixedBlocks {
જાહેર:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    તાર્કિક init(int32_t maxLength, int32_t newBlockLength) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t maxDataIndex = maxLength - newBlockLength + 1;
        int32_t newLength;
        જો (maxDataIndex <= 0xfff) {  // 4k
            newLength = 6007;
            shift = 12;
            mask = 0xfff;
        } નહીંતર જો (maxDataIndex <= 0x7fff) {  // 32k
            newLength = 50021;
            shift = 15;
            mask = 0x7fff;
        } નહીંતર જો (maxDataIndex <= 0x1ffff) {  // 128k
            newLength = 200003;
            shift = 17;
            mask = 0x1ffff;
        } નહીંતર {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            newLength = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        જો (newLength > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(newLength * 4));
            જો (table == nullptr) {
                પાછા ખોટું;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        પાછા સાચું;
    }

    template<typename UInt>
    શૂન્ય extend(અચલ UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        જો (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } નહીંતર {
            start = minStart;  // Begin with the first full block.
        }
        માટે (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(અચલ UIntA *data, અચલ UIntB *blockData, int32_t blockStart) અચલ {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        જો (entryIndex >= 0) {
            પાછા (table[entryIndex] & mask) - 1;
        } નહીંતર {
            પાછા -1;
        }
    }

    int32_t findAllSameBlock(અચલ uint32_t *data, uint32_t block_કિંમત) અચલ {
        uint32_t hashCode = makeHashCode(block_કિંમત);
        int32_t entryIndex = findEntry(data, block_કિંમત, hashCode);
        જો (entryIndex >= 0) {
            પાછા (table[entryIndex] & mask) - 1;
        } નહીંતર {
            પાછા -1;
        }
    }

ખાનગી:
    template<typename UInt>
    uint32_t makeHashCode(અચલ UInt *blockData, int32_t blockStart) અચલ {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        કરો {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } જ્યારે (blockStart < blockLimit);
        પાછા hashCode;
    }

    uint32_t makeHashCode(uint32_t block_કિંમત) અચલ {
        uint32_t hashCode = block_કિંમત;
        માટે (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_કિંમત;
        }
        પાછા hashCode;
    }

    template<typename UInt>
    શૂન્ય addEntry(અચલ UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        જો (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(અચલ UIntA *data, અચલ UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) અચલ {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        માટે (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            જો (entry == 0) {
                પાછા ~entryIndex;
            }
            જો ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                જો (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    પાછા entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(અચલ uint32_t *data, uint32_t block_કિંમત, uint32_t hashCode) અચલ {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        માટે (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            જો (entry == 0) {
                પાછા ~entryIndex;
            }
            જો ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                જો (all_કિંમતો_Same_As(data + dataIndex, blockLength, block_કિંમત)) {
                    પાછા entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    અંતઃરેખ int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) અચલ {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        પાછા (entryIndex + initialEntryIndex) % length;
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
    તાર્કિક overflow = ખોટું;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t newDataCapacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    newDataCapacity += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    // Add room for special values (errorValue, highValue) and padding.
    newDataCapacity += 4;
    int32_t iLimit = ઉપલું_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    માટે (int32_t i = 0; i < iLimit; i += inc) {
        જો (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        જો (flags[i] == MIXED) {
            // Really mixed?
            અચલ uint32_t *p = data + value;
            value = *p;
            જો (all_કિંમતો_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } નહીંતર {
                newDataCapacity += blockLength;
                ચાલુ;
            }
        } નહીંતર {
            U_ASSERT(flags[i] == ALL_SAME);
            જો (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                તાર્કિક allSame = સાચું;
                int32_t next_i = i + inc;
                માટે (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    જો (index[j] != value) {
                        allSame = ખોટું;
                        તોડો;
                    }
                }
                જો (!allSame) {
                    // Turn it into a MIXED block.
                    જો (getDataBlock(i) < 0) {
                        પાછા -1;
                    }
                    newDataCapacity += blockLength;
                    ચાલુ;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        જો (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            જો (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = સાચું;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            માટે (int32_t j = 0;; j += jInc) {
                જો (j == i) {
                    allSameBlocks.add(i, inc, value);
                    તોડો;
                }
                જો (j == fastILimit) {
                    jInc = 1;
                }
                જો (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    તોડો;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        જો (other >= 0) {
            flags[i] = SAME_AS;
            index[i] = other;
        } નહીંતર {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    પાછા newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(પદાવલિ) પદાવલિ
#નહીંતર
#   define DEBUG_DO(પદાવલિ)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_કિંમત(અક્ષર s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (અક્ષર)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (અક્ષર)(0x80 + (value & 0x3F));
    પાછા length + 3;
}

શૂન્ય printBlock(અચલ uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_કિંમત) {
    અક્ષર s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    માટે (i = 0; i < overlap; ++i) {
        length = append_કિંમત(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    માટે (; i < blockLength; ++i) {
        જો (block != nullptr) {
            value = block[i];
        }
        જો (value == initial_કિંમત) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_કિંમત(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    જો (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } નહીંતર જો (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } નહીંતર {
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
    int32_t ગણતરી_Same=0, sumOverlaps=0;
    તાર્કિક printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    માટે (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        જો (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_કિંમત);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    જો (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = ઉપલું_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    માટે (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        જો (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            જો (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા 0;
            }
            mixedBlocks.extend(newData, 0, 0, newDataLength);
        }
        જો (flags[i] == ALL_SAME) {
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
            જ્યારે (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            જો (n >= 0) {
                DEBUG_DO(++ગણતરી_Same);
                index[i] = n;
            } નહીંતર {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                જો (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_કિંમત);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                જ્યારે (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } નહીંતર જો (flags[i] == MIXED) {
            અચલ uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            જો (n >= 0) {
                DEBUG_DO(++ગણતરી_Same);
                index[i] = n;
            } નહીંતર {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                જો (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_કિંમત);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                જ્યારે (n < blockLength) {
                    newData[newDataLength++] = block[n++];
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } નહીંતર /* SAME_AS */ {
            uint32_t j = index[i];
            index[i] = index[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)dataLength, (long)newDataLength, (long)ગણતરી_Same, (long)sumOverlaps);
#endif
    પાછા newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    જો ((ઉપલું_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        પાછા fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    માટે (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = index[i];
        fastIndex[j] = static_cast<uint16_t>(i3);
        જો (i3 == static_cast<uint32_t>(dataNullOffset)) {
            જો (i3FirstNull < 0) {
                i3FirstNull = j;
            } નહીંતર જો (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                index3NullOffset = i3FirstNull;
            }
        } નહીંતર {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t iNext = i + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        જ્યારે (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    જો (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
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
    તાર્કિક hasLongI3Blocks = ખોટું;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = ઉપલું_Start >> UCPTRIE_SHIFT_3;
    માટે (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        તાર્કિક isNull = સાચું;
        કરો {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            જો (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = ખોટું;
            }
        } જ્યારે (++j < jLimit);
        જો (isNull) {
            flags[i] = I3_NULL;
            જો (i3FirstNull < 0) {
                જો (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                } નહીંતર {
                    index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                    hasLongI3Blocks = સાચું;
                }
                i3FirstNull = 0;
            }
        } નહીંતર {
            જો (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fastIndex, index, i);
                જો (n >= 0) {
                    flags[i] = I3_BMP;
                    index[i] = n;
                } નહીંતર {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                }
            } નહીંતર {
                flags[i] = I3_18;
                index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                hasLongI3Blocks = સાચું;
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
    જો (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    જો (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
    }
    MixedBlocks longI3Blocks;
    જો (hasLongI3Blocks) {
        જો (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    માટે (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
        int32_t i3;
        uint8_t f = flags[i];
        જો (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = dataNullOffset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        જો (f == I3_NULL) {
            i3 = index3NullOffset;
        } નહીંતર જો (f == I3_BMP) {
            i3 = index[i];
        } નહીંતર જો (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, index, i);
            જો (n >= 0) {
                i3 = n;
            } નહીંતર {
                જો (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } નહીંતર {
                    n = getOverlap(index16, indexLength,
                                   index, i, UCPTRIE_INDEX_3_BLOCK_LENGTH);
                }
                i3 = indexLength - n;
                int32_t prevIndexLength = indexLength;
                જ્યારે (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                    index16[indexLength++] = index[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                જો (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        } નહીંતર {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
            int32_t k = indexLength;
            કરો {
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
            } જ્યારે (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, indexLength);
            જો (n >= 0) {
                i3 = n | 0x8000;
            } નહીંતર {
                જો (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } નહીંતર {
                    n = getOverlap(index16, indexLength,
                                   index16, indexLength, INDEX_3_18BIT_BLOCK_LENGTH);
                }
                i3 = (indexLength - n) | 0x8000;
                int32_t prevIndexLength = indexLength;
                જો (n > 0) {
                    int32_t start = indexLength;
                    જ્યારે (n < INDEX_3_18BIT_BLOCK_LENGTH) {
                        index16[indexLength++] = index16[start + n++];
                    }
                } નહીંતર {
                    indexLength += INDEX_3_18BIT_BLOCK_LENGTH;
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                જો (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        }
        જો (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2Length++] = i3;
    }
    U_ASSERT(i2Length == index2Capacity);
    U_ASSERT(indexLength <= index3Start + index3Capacity);

    જો (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    જો (indexLength >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    માટે (int32_t i = 0; i < i2Length; i += blockLength) {
        int32_t n;
        જો ((i2Length - i) >= blockLength) {
            // normal block
            U_ASSERT(blockLength == UCPTRIE_INDEX_2_BLOCK_LENGTH);
            n = mixedBlocks.findBlock(index16, index2, i);
        } નહીંતર {
            // highStart is inside the last index-2 block. Shorten it.
            blockLength = i2Length - i;
            n = findSameBlock(index16, index3Start, indexLength,
                              index2, i, blockLength);
        }
        int32_t i2;
        જો (n >= 0) {
            i2 = n;
        } નહીંતર {
            જો (indexLength == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } નહીંતર {
                n = getOverlap(index16, indexLength, index2, i, blockLength);
            }
            i2 = indexLength - n;
            int32_t prevIndexLength = indexLength;
            જ્યારે (n < blockLength) {
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

    પાછા indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((ઉપલું_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    ઉપલું_કિંમત = get(MAX_UNICODE);
    int32_t real_ઉપલું_Start = find_ઉપલું_Start();
    real_ઉપલું_Start = (real_ઉપલું_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    જો (real_ઉપલું_Start == UNICODE_LIMIT) {
        ઉપલું_કિંમત = initial_કિંમત;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_ઉપલું_Start, (long)ઉપલું_કિંમત, (long)initial_કિંમત);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    જો (real_ઉપલું_Start < fastLimit) {
        માટે (int32_t i = (real_ઉપલું_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = ઉપલું_કિંમત;
        }
        ઉપલું_Start = fastLimit;
    } નહીંતર {
        ઉપલું_Start = real_ઉપલું_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    માટે (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    જો (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    જો (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
    }
    uprv_memcpy(newData, asciiData, કદ(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    જો (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    જો (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        જો (data[dataNullOffset] != initial_કિંમત) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_કિંમત, (long)data[dataNullOffset]);
        }
#endif
        initial_કિંમત = data[dataNullOffset];
    } નહીંતર {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    ઉપલું_Start = real_ઉપલું_Start;
    પાછા indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_કિંમત_Width કિંમત_Width, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    જો (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            કિંમત_Width < UCPTRIE_કિંમત_BITS_16 || UCPTRIE_કિંમત_BITS_8 < કિંમત_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    પસંદગી (કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        mask_કિંમતો(0xffff);
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        mask_કિંમતો(0xff);
        તોડો;
    મૂળભૂત:
        તોડો;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    જો (U_FAILURE(errorCode)) {
        clear();
        પાછા nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    જો (કિંમત_Width == UCPTRIE_કિંમત_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    જો (કિંમત_Width == UCPTRIE_કિંમત_BITS_16) {
        જો (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_કિંમત;
        }
        જો (data[dataLength - 1] != error_કિંમત || data[dataLength - 2] != ઉપલું_કિંમત) {
            data[dataLength++] = ઉપલું_કિંમત;
            data[dataLength++] = error_કિંમત;
        }
        length += dataLength * 2;
    } નહીંતર જો (કિંમત_Width == UCPTRIE_કિંમત_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        જો (data[dataLength - 1] != error_કિંમત || data[dataLength - 2] != ઉપલું_કિંમત) {
            જો (data[dataLength - 1] != ઉપલું_કિંમત) {
                data[dataLength++] = ઉપલું_કિંમત;
            }
            data[dataLength++] = error_કિંમત;
        }
        length += dataLength * 4;
    } નહીંતર {
        int32_t and3 = (length + dataLength) & 3;
        જો (and3 == 0 && data[dataLength - 1] == error_કિંમત && data[dataLength - 2] == ઉપલું_કિંમત) {
            // all set
        } નહીંતર જો(and3 == 3 && data[dataLength - 1] == ઉપલું_કિંમત) {
            data[dataLength++] = error_કિંમત;
        } નહીંતર {
            જ્યારે (and3 != 2) {
                data[dataLength++] = ઉપલું_કિંમત;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = ઉપલું_કિંમત;
            data[dataLength++] = error_કિંમત;
        }
        length += dataLength;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += કદ(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    જો (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        પાછા nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, કદ(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->ઉપલું_Start = ઉપલું_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ઉપલું_Start = (ઉપલું_Start + 0xfff) >> 12;
    trie->type = type;
    trie->કિંમત_Width = કિંમત_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_કિંમત = initial_કિંમત;

    bytes += કદ(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    જો (ઉપલું_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        માટે (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } નહીંતર {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    અચલ uint32_t *p = data;
    પસંદગી (કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        માટે (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)dataLength * 4);
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        માટે (int32_t i = dataLength; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        તોડો;
    મૂળભૂત:
        // Will not occur, valueWidth checked at the beginning.
        તોડો;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    પાછા trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_કિંમત, uint32_t error_કિંમત, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        નવું MutableCodePointTrie(initial_કિંમત, error_કિંમત, *pErrorCode), *pErrorCode);
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    પાછા reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(અચલ UMutableCPTrie *other, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો (other == nullptr) {
        પાછા nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        નવું MutableCodePointTrie(*reinterpret_cast<અચલ MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    પાછા reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI શૂન્ય U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(અચલ UCPMap *map, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    પાછા reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(અચલ UCPTrie *trie, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    પાછા reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(અચલ UMutableCPTrie *trie, UChar32 c) {
    પાછા reinterpret_cast<અચલ MutableCodePointTrie *>(trie)->get(c);
}

નામાકાશ {

UChar32 getRange(અચલ શૂન્ય *trie, UChar32 start,
                 UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context, uint32_t *p_કિંમત) {
    પાછા reinterpret_cast<અચલ MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_કિંમત);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(અચલ UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_કિંમત,
                        UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context, uint32_t *p_કિંમત) {
    પાછા ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_કિંમત,
                                    filter, context, p_કિંમત);
}

U_CAPI શૂન્ય U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI શૂન્ય U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_કિંમત_Width કિંમત_Width,
                              UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    પાછા reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, કિંમત_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC શૂન્ય umutablecptrie_setName(UMutableCPTrie *trie, અચલ અક્ષર *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
