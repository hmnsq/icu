// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/umutablecptrie.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#ifdef OVERFLOW
#undef OVERFLOW
#endif

U_NAMESPACE_BEGIN

நாமாகாஶ {

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

வகுப்பு AllSameBlocks;
வகுப்பு MixedBlocks;

வகுப்பு MutableCodePointTrie : பொது UMemory {
பொது:
    MutableCodePointTrie(uint32_t initial_மதிப்பு, uint32_t error_மதிப்பு, UErrorCode &errorCode);
    MutableCodePointTrie(மாறிலி MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(மாறிலி MutableCodePointTrie &other) = delete;
    ~MutableCodePointTrie();

    MutableCodePointTrie &operator=(மாறிலி MutableCodePointTrie &other) = delete;

    நிலையான MutableCodePointTrie *fromUCPMap(மாறிலி UCPMap *map, UErrorCode &errorCode);
    நிலையான MutableCodePointTrie *fromUCPTrie(மாறிலி UCPTrie *trie, UErrorCode &errorCode);

    uint32_t get(UChar32 c) மாறிலி;
    int32_t getRange(UChar32 start, UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context,
                     uint32_t *p_மதிப்பு) மாறிலி;

    வெறுமை set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    வெறுமை setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_மதிப்பு_Width மதிப்பு_Width, UErrorCode &errorCode);

தனிப்பட்ட:
    வெறுமை clear();

    தருக்க ensure_உயர்வு_Start(UChar32 c);
    int32_t allocDataBlock(int32_t blockLength);
    int32_t getDataBlock(int32_t i);

    வெறுமை mask_மதிப்புகள்(uint32_t mask);
    UChar32 find_உயர்வு_Start() மாறிலி;
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

    uint32_t orig_Initial_மதிப்பு;
    uint32_t initial_மதிப்பு;
    uint32_t error_மதிப்பு;
    UChar32 உயர்வு_Start;
    uint32_t உயர்வு_மதிப்பு;
#ifdef UCPTRIE_DEBUG
பொது:
    மாறிலி எழுத்து *name;
#endif
தனிப்பட்ட:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_மதிப்பு, uint32_t err_மதிப்பு, UErrorCode &errorCode) :
        orig_Initial_மதிப்பு(ini_மதிப்பு), initial_மதிப்பு(ini_மதிப்பு), error_மதிப்பு(err_மதிப்பு),
        உயர்வு_Start(0), உயர்வு_மதிப்பு(initial_மதிப்பு)
#ifdef UCPTRIE_DEBUG
        , name("open")
#endif
        {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    index = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    data = static_cast<uint32_t*>(uprv_malloc(INITIAL_DATA_LENGTH * 4));
    எனில் (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    indexCapacity = BMP_I_LIMIT;
    dataCapacity = INITIAL_DATA_LENGTH;
}

MutableCodePointTrie::MutableCodePointTrie(மாறிலி MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        dataNullOffset(other.dataNullOffset),
        orig_Initial_மதிப்பு(other.orig_Initial_மதிப்பு), initial_மதிப்பு(other.initial_மதிப்பு),
        error_மதிப்பு(other.error_மதிப்பு),
        உயர்வு_Start(other.உயர்வு_Start), உயர்வு_மதிப்பு(other.உயர்வு_மதிப்பு)
#ifdef UCPTRIE_DEBUG
        , name("mutable clone")
#endif
        {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    int32_t iCapacity = உயர்வு_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    index = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    data = static_cast<uint32_t*>(uprv_malloc(other.dataCapacity * 4));
    எனில் (index == nullptr || data == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    indexCapacity = iCapacity;
    dataCapacity = other.dataCapacity;

    int32_t iLimit = உயர்வு_Start >> UCPTRIE_SHIFT_3;
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

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(மாறிலி UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_மதிப்பு = ucpmap_get(map, -1);
    uint32_t initial_மதிப்பு = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        புதிய MutableCodePointTrie(initial_மதிப்பு, error_மதிப்பு, errorCode),
        errorCode);
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    போது ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        எனில் (value != initial_மதிப்பு) {
            எனில் (start == end) {
                mutableTrie->set(start, value, errorCode);
            } இல்லையெனில் {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    எனில் (U_SUCCESS(errorCode)) {
        திரும்பு mutableTrie.orphan();
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(மாறிலி UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_மதிப்பு;
    uint32_t initial_மதிப்பு;
    தேர்வு (trie->மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        error_மதிப்பு = trie->data.ptr16[trie->dataLength - UCPTRIE_ERROR_மதிப்பு_NEG_DATA_OFFSET];
        initial_மதிப்பு = trie->data.ptr16[trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET];
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        error_மதிப்பு = trie->data.ptr32[trie->dataLength - UCPTRIE_ERROR_மதிப்பு_NEG_DATA_OFFSET];
        initial_மதிப்பு = trie->data.ptr32[trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET];
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        error_மதிப்பு = trie->data.ptr8[trie->dataLength - UCPTRIE_ERROR_மதிப்பு_NEG_DATA_OFFSET];
        initial_மதிப்பு = trie->data.ptr8[trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET];
        நிறுத்து;
    இயல்பு:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        புதிய MutableCodePointTrie(initial_மதிப்பு, error_மதிப்பு, errorCode),
        errorCode);
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    போது ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        எனில் (value != initial_மதிப்பு) {
            எனில் (start == end) {
                mutableTrie->set(start, value, errorCode);
            } இல்லையெனில் {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    எனில் (U_SUCCESS(errorCode)) {
        திரும்பு mutableTrie.orphan();
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

வெறுமை MutableCodePointTrie::clear() {
    index3NullOffset = dataNullOffset = -1;
    dataLength = 0;
    உயர்வு_மதிப்பு = initial_மதிப்பு = orig_Initial_மதிப்பு;
    உயர்வு_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::get(UChar32 c) மாறிலி {
    எனில் (static_cast<uint32_t>(c) > MAX_UNICODE) {
        திரும்பு error_மதிப்பு;
    }
    எனில் (c >= உயர்வு_Start) {
        திரும்பு உயர்வு_மதிப்பு;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    எனில் (flags[i] == ALL_SAME) {
        திரும்பு index[i];
    } இல்லையெனில் {
        திரும்பு data[index[i] + (c & UCPTRIE_SMALL_DATA_MASK)];
    }
}

உள்வரி uint32_t maybe_Filter_மதிப்பு(uint32_t value, uint32_t initial_மதிப்பு, uint32_t null_மதிப்பு,
                                 UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context) {
    எனில் (value == initial_மதிப்பு) {
        value = null_மதிப்பு;
    } இல்லையெனில் எனில் (filter != nullptr) {
        value = filter(context, value);
    }
    திரும்பு value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context,
        uint32_t *p_மதிப்பு) மாறிலி {
    எனில் (static_cast<uint32_t>(start) > MAX_UNICODE) {
        திரும்பு U_SENTINEL;
    }
    எனில் (start >= உயர்வு_Start) {
        எனில் (p_மதிப்பு != nullptr) {
            uint32_t value = உயர்வு_மதிப்பு;
            எனில் (filter != nullptr) { value = filter(context, value); }
            *p_மதிப்பு = value;
        }
        திரும்பு MAX_UNICODE;
    }
    uint32_t null_மதிப்பு = initial_மதிப்பு;
    எனில் (filter != nullptr) { null_மதிப்பு = filter(context, null_மதிப்பு); }
    UChar32 c = start;
    uint32_t trie_மதிப்பு, value;
    தருக்க have_மதிப்பு = பொய்;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    செய் {
        எனில் (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = index[i];
            எனில் (have_மதிப்பு) {
                எனில் (trieValue2 != trie_மதிப்பு) {
                    எனில் (filter == nullptr ||
                            maybe_Filter_மதிப்பு(trieValue2, initial_மதிப்பு, null_மதிப்பு,
                                             filter, context) != value) {
                        திரும்பு c - 1;
                    }
                    trie_மதிப்பு = trieValue2;  // may or may not help
                }
            } இல்லையெனில் {
                trie_மதிப்பு = trieValue2;
                value = maybe_Filter_மதிப்பு(trieValue2, initial_மதிப்பு, null_மதிப்பு, filter, context);
                எனில் (p_மதிப்பு != nullptr) { *p_மதிப்பு = value; }
                have_மதிப்பு = உண்மை;
            }
            c = (c + UCPTRIE_SMALL_DATA_BLOCK_LENGTH) & ~UCPTRIE_SMALL_DATA_MASK;
        } இல்லையெனில் /* MIXED */ {
            int32_t di = index[i] + (c & UCPTRIE_SMALL_DATA_MASK);
            uint32_t trieValue2 = data[di];
            எனில் (have_மதிப்பு) {
                எனில் (trieValue2 != trie_மதிப்பு) {
                    எனில் (filter == nullptr ||
                            maybe_Filter_மதிப்பு(trieValue2, initial_மதிப்பு, null_மதிப்பு,
                                             filter, context) != value) {
                        திரும்பு c - 1;
                    }
                    trie_மதிப்பு = trieValue2;  // may or may not help
                }
            } இல்லையெனில் {
                trie_மதிப்பு = trieValue2;
                value = maybe_Filter_மதிப்பு(trieValue2, initial_மதிப்பு, null_மதிப்பு, filter, context);
                எனில் (p_மதிப்பு != nullptr) { *p_மதிப்பு = value; }
                have_மதிப்பு = உண்மை;
            }
            போது ((++c & UCPTRIE_SMALL_DATA_MASK) != 0) {
                trieValue2 = data[++di];
                எனில் (trieValue2 != trie_மதிப்பு) {
                    எனில் (filter == nullptr ||
                            maybe_Filter_மதிப்பு(trieValue2, initial_மதிப்பு, null_மதிப்பு,
                                             filter, context) != value) {
                        திரும்பு c - 1;
                    }
                }
                trie_மதிப்பு = trieValue2;  // may or may not help
            }
        }
        ++i;
    } போது (c < உயர்வு_Start);
    U_ASSERT(have_மதிப்பு);
    எனில் (maybe_Filter_மதிப்பு(உயர்வு_மதிப்பு, initial_மதிப்பு, null_மதிப்பு,
                         filter, context) != value) {
        திரும்பு c - 1;
    } இல்லையெனில் {
        திரும்பு MAX_UNICODE;
    }
}

வெறுமை
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    போது (block < limit) {
        *block++ = value;
    }
}

தருக்க MutableCodePointTrie::ensure_உயர்வு_Start(UChar32 c) {
    எனில் (c >= உயர்வு_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
        int32_t i = உயர்வு_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        எனில் (iLimit > indexCapacity) {
            uint32_t* newIndex = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            எனில் (newIndex == nullptr) { திரும்பு பொய்; }
            uprv_memcpy(newIndex, index, i * 4);
            uprv_free(index);
            index = newIndex;
            indexCapacity = I_LIMIT;
        }
        செய் {
            flags[i] = ALL_SAME;
            index[i] = initial_மதிப்பு;
        } போது(++i < iLimit);
        உயர்வு_Start = c;
    }
    திரும்பு உண்மை;
}

int32_t MutableCodePointTrie::allocDataBlock(int32_t blockLength) {
    int32_t newBlock = dataLength;
    int32_t new_உச்சி = newBlock + blockLength;
    எனில் (new_உச்சி > dataCapacity) {
        int32_t capacity;
        எனில் (dataCapacity < MEDIUM_DATA_LENGTH) {
            capacity = MEDIUM_DATA_LENGTH;
        } இல்லையெனில் எனில் (dataCapacity < MAX_DATA_LENGTH) {
            capacity = MAX_DATA_LENGTH;
        } இல்லையெனில் {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            திரும்பு -1;
        }
        uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        எனில் (newData == nullptr) {
            திரும்பு -1;
        }
        uprv_memcpy(newData, data, (size_t)dataLength * 4);
        uprv_free(data);
        data = newData;
        dataCapacity = capacity;
    }
    dataLength = new_உச்சி;
    திரும்பு newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::getDataBlock(int32_t i) {
    எனில் (flags[i] == MIXED) {
        திரும்பு index[i];
    }
    எனில் (i < BMP_I_LIMIT) {
        int32_t newBlock = allocDataBlock(UCPTRIE_FAST_DATA_BLOCK_LENGTH);
        எனில் (newBlock < 0) { திரும்பு newBlock; }
        int32_t iStart = i & ~(SMALL_DATA_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        செய் {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(data + newBlock, index[iStart]);
            flags[iStart] = MIXED;
            index[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        } போது (iStart < iLimit);
        திரும்பு index[i];
    } இல்லையெனில் {
        int32_t newBlock = allocDataBlock(UCPTRIE_SMALL_DATA_BLOCK_LENGTH);
        எனில் (newBlock < 0) { திரும்பு newBlock; }
        writeBlock(data + newBlock, index[i]);
        flags[i] = MIXED;
        index[i] = newBlock;
        திரும்பு newBlock;
    }
}

வெறுமை MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில் (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    int32_t block;
    எனில் (!ensure_உயர்வு_Start(c) || (block = getDataBlock(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    data[block + (c & UCPTRIE_SMALL_DATA_MASK)] = value;
}

வெறுமை
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    போது (block < pLimit) {
        *block++ = value;
    }
}

வெறுமை MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில் (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    எனில் (!ensure_உயர்வு_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    UChar32 limit = end + 1;
    எனில் (start & UCPTRIE_SMALL_DATA_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        எனில் (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }

        UChar32 nextStart = (start + UCPTRIE_SMALL_DATA_MASK) & ~UCPTRIE_SMALL_DATA_MASK;
        எனில் (nextStart <= limit) {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, UCPTRIE_SMALL_DATA_BLOCK_LENGTH,
                      value);
            start = nextStart;
        } இல்லையெனில் {
            fillBlock(data + block, start & UCPTRIE_SMALL_DATA_MASK, limit & UCPTRIE_SMALL_DATA_MASK,
                      value);
            திரும்பு;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_DATA_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_DATA_MASK;

    // Iterate over all-value blocks.
    போது (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        எனில் (flags[i] == ALL_SAME) {
            index[i] = value;
        } இல்லையெனில் /* MIXED */ {
            fillBlock(data + index[i], 0, UCPTRIE_SMALL_DATA_BLOCK_LENGTH, value);
        }
        start += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    }

    எனில் (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = getDataBlock(start >> UCPTRIE_SHIFT_3);
        எனில் (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }

        fillBlock(data + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

வெறுமை MutableCodePointTrie::mask_மதிப்புகள்(uint32_t mask) {
    initial_மதிப்பு &= mask;
    error_மதிப்பு &= mask;
    உயர்வு_மதிப்பு &= mask;
    int32_t iLimit = உயர்வு_Start >> UCPTRIE_SHIFT_3;
    க்கு (int32_t i = 0; i < iLimit; ++i) {
        எனில் (flags[i] == ALL_SAME) {
            index[i] &= mask;
        }
    }
    க்கு (int32_t i = 0; i < dataLength; ++i) {
        data[i] &= mask;
    }
}

template<typename UIntA, typename UIntB>
தருக்க equalBlocks(மாறிலி UIntA *s, மாறிலி UIntB *t, int32_t length) {
    போது (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    திரும்பு length == 0;
}

தருக்க all_மதிப்புகள்_Same_As(மாறிலி uint32_t *p, int32_t length, uint32_t value) {
    மாறிலி uint32_t *pLimit = p + length;
    போது (p < pLimit && *p == value) { ++p; }
    திரும்பு p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(மாறிலி uint16_t *p, int32_t pStart, int32_t length,
                      மாறிலி uint16_t *q, int32_t qStart, int32_t blockLength) {
    // Ensure that we do not even partially get past length.
    length -= blockLength;

    q += qStart;
    போது (pStart <= length) {
        எனில் (equalBlocks(p + pStart, q, blockLength)) {
            திரும்பு pStart;
        }
        ++pStart;
    }
    திரும்பு -1;
}

int32_t findAllSameBlock(மாறிலி uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t blockLength) {
    // Ensure that we do not even partially get past limit.
    limit -= blockLength;

    க்கு (int32_t block = start; block <= limit; ++block) {
        எனில் (p[block] == value) {
            க்கு (int32_t i = 1;; ++i) {
                எனில் (i == blockLength) {
                    திரும்பு block;
                }
                எனில் (p[block + i] != value) {
                    block += i;
                    நிறுத்து;
                }
            }
        }
    }
    திரும்பு -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
template<typename UIntA, typename UIntB>
int32_t getOverlap(மாறிலி UIntA *p, int32_t length,
                   மாறிலி UIntB *q, int32_t qStart, int32_t blockLength) {
    int32_t overlap = blockLength - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    போது (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    திரும்பு overlap;
}

int32_t getAllSameOverlap(மாறிலி uint32_t *p, int32_t length, uint32_t value,
                          int32_t blockLength) {
    int32_t min = length - (blockLength - 1);
    int32_t i = length;
    போது (min < i && p[i - 1] == value) { --i; }
    திரும்பு length - i;
}

தருக்க isStartOfSomeFastBlock(uint32_t dataOffset, மாறிலி uint32_t index[], int32_t fastILimit) {
    க்கு (int32_t i = 0; i < fastILimit; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        எனில் (index[i] == dataOffset) {
            திரும்பு உண்மை;
        }
    }
    திரும்பு பொய்;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_உயர்வு_Start() மாறிலி {
    int32_t i = உயர்வு_Start >> UCPTRIE_SHIFT_3;
    போது (i > 0) {
        தருக்க match;
        எனில் (flags[--i] == ALL_SAME) {
            match = index[i] == உயர்வு_மதிப்பு;
        } இல்லையெனில் /* MIXED */ {
            மாறிலி uint32_t *p = data + index[i];
            க்கு (int32_t j = 0;; ++j) {
                எனில் (j == UCPTRIE_SMALL_DATA_BLOCK_LENGTH) {
                    match = உண்மை;
                    நிறுத்து;
                }
                எனில் (p[j] != உயர்வு_மதிப்பு) {
                    match = பொய்;
                    நிறுத்து;
                }
            }
        }
        எனில் (!match) {
            திரும்பு (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    திரும்பு 0;
}

வகுப்பு AllSameBlocks {
பொது:
    நிலையான constexpr int32_t NEW_UNIQUE = -1;
    நிலையான constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t index, int32_t count, uint32_t value) {
        எனில் (mostRecent >= 0 && மதிப்புகள்[mostRecent] == value) {
            refCounts[mostRecent] += count;
            திரும்பு indexes[mostRecent];
        }
        க்கு (int32_t i = 0; i < length; ++i) {
            எனில் (மதிப்புகள்[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                திரும்பு indexes[i];
            }
        }
        எனில் (length == CAPACITY) {
            திரும்பு OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = index;
        மதிப்புகள்[length] = value;
        refCounts[length++] = count;
        திரும்பு NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    வெறுமை add(int32_t index, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_மொத்தம் = I_LIMIT;
        க்கு (int32_t i = 0; i < length; ++i) {
            U_ASSERT(மதிப்புகள்[i] != value);
            எனில் (refCounts[i] < least_மொத்தம்) {
                least = i;
                least_மொத்தம் = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = index;
        மதிப்புகள்[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() மாறிலி {
        எனில் (length == 0) { திரும்பு -1; }
        int32_t max = -1;
        int32_t max_மொத்தம் = 0;
        க்கு (int32_t i = 0; i < length; ++i) {
            எனில் (refCounts[i] > max_மொத்தம்) {
                max = i;
                max_மொத்தம் = refCounts[i];
            }
        }
        திரும்பு indexes[max];
    }

தனிப்பட்ட:
    நிலையான constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t மதிப்புகள்[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
வகுப்பு MixedBlocks {
பொது:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    தருக்க init(int32_t maxLength, int32_t newBlockLength) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t maxDataIndex = maxLength - newBlockLength + 1;
        int32_t newLength;
        எனில் (maxDataIndex <= 0xfff) {  // 4k
            newLength = 6007;
            shift = 12;
            mask = 0xfff;
        } இல்லையெனில் எனில் (maxDataIndex <= 0x7fff) {  // 32k
            newLength = 50021;
            shift = 15;
            mask = 0x7fff;
        } இல்லையெனில் எனில் (maxDataIndex <= 0x1ffff) {  // 128k
            newLength = 200003;
            shift = 17;
            mask = 0x1ffff;
        } இல்லையெனில் {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            newLength = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        எனில் (newLength > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(newLength * 4));
            எனில் (table == nullptr) {
                திரும்பு பொய்;
            }
            capacity = newLength;
        }
        length = newLength;
        uprv_memset(table, 0, length * 4);

        blockLength = newBlockLength;
        திரும்பு உண்மை;
    }

    template<typename UInt>
    வெறுமை extend(மாறிலி UInt *data, int32_t minStart, int32_t prevDataLength, int32_t newDataLength) {
        int32_t start = prevDataLength - blockLength;
        எனில் (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } இல்லையெனில் {
            start = minStart;  // Begin with the first full block.
        }
        க்கு (int32_t end = newDataLength - blockLength; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(data, start);
            addEntry(data, start, hashCode, start);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findBlock(மாறிலி UIntA *data, மாறிலி UIntB *blockData, int32_t blockStart) மாறிலி {
        uint32_t hashCode = makeHashCode(blockData, blockStart);
        int32_t entryIndex = findEntry(data, blockData, blockStart, hashCode);
        எனில் (entryIndex >= 0) {
            திரும்பு (table[entryIndex] & mask) - 1;
        } இல்லையெனில் {
            திரும்பு -1;
        }
    }

    int32_t findAllSameBlock(மாறிலி uint32_t *data, uint32_t block_மதிப்பு) மாறிலி {
        uint32_t hashCode = makeHashCode(block_மதிப்பு);
        int32_t entryIndex = findEntry(data, block_மதிப்பு, hashCode);
        எனில் (entryIndex >= 0) {
            திரும்பு (table[entryIndex] & mask) - 1;
        } இல்லையெனில் {
            திரும்பு -1;
        }
    }

தனிப்பட்ட:
    template<typename UInt>
    uint32_t makeHashCode(மாறிலி UInt *blockData, int32_t blockStart) மாறிலி {
        int32_t blockLimit = blockStart + blockLength;
        uint32_t hashCode = blockData[blockStart++];
        செய் {
            hashCode = 37 * hashCode + blockData[blockStart++];
        } போது (blockStart < blockLimit);
        திரும்பு hashCode;
    }

    uint32_t makeHashCode(uint32_t block_மதிப்பு) மாறிலி {
        uint32_t hashCode = block_மதிப்பு;
        க்கு (int32_t i = 1; i < blockLength; ++i) {
            hashCode = 37 * hashCode + block_மதிப்பு;
        }
        திரும்பு hashCode;
    }

    template<typename UInt>
    வெறுமை addEntry(மாறிலி UInt *data, int32_t blockStart, uint32_t hashCode, int32_t dataIndex) {
        U_ASSERT(0 <= dataIndex && dataIndex < (int32_t)mask);
        int32_t entryIndex = findEntry(data, data, blockStart, hashCode);
        எனில் (entryIndex < 0) {
            table[~entryIndex] = (hashCode << shift) | (dataIndex + 1);
        }
    }

    template<typename UIntA, typename UIntB>
    int32_t findEntry(மாறிலி UIntA *data, மாறிலி UIntB *blockData, int32_t blockStart,
                      uint32_t hashCode) மாறிலி {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        க்கு (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            எனில் (entry == 0) {
                திரும்பு ~entryIndex;
            }
            எனில் ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                எனில் (equalBlocks(data + dataIndex, blockData + blockStart, blockLength)) {
                    திரும்பு entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    int32_t findEntry(மாறிலி uint32_t *data, uint32_t block_மதிப்பு, uint32_t hashCode) மாறிலி {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initialEntryIndex = (hashCode % (length - 1)) + 1;  // 1..length-1
        க்கு (int32_t entryIndex = initialEntryIndex;;) {
            uint32_t entry = table[entryIndex];
            எனில் (entry == 0) {
                திரும்பு ~entryIndex;
            }
            எனில் ((entry & ~mask) == shiftedHashCode) {
                int32_t dataIndex = (entry & mask) - 1;
                எனில் (all_மதிப்புகள்_Same_As(data + dataIndex, blockLength, block_மதிப்பு)) {
                    திரும்பு entryIndex;
                }
            }
            entryIndex = nextIndex(initialEntryIndex, entryIndex);
        }
    }

    உள்வரி int32_t nextIndex(int32_t initialEntryIndex, int32_t entryIndex) மாறிலி {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        திரும்பு (entryIndex + initialEntryIndex) % length;
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
    தருக்க overflow = பொய்;
#endif

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t newDataCapacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    newDataCapacity += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
    // Add room for special values (errorValue, highValue) and padding.
    newDataCapacity += 4;
    int32_t iLimit = உயர்வு_Start >> UCPTRIE_SHIFT_3;
    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    க்கு (int32_t i = 0; i < iLimit; i += inc) {
        எனில் (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
        }
        uint32_t value = index[i];
        எனில் (flags[i] == MIXED) {
            // Really mixed?
            மாறிலி uint32_t *p = data + value;
            value = *p;
            எனில் (all_மதிப்புகள்_Same_As(p + 1, blockLength - 1, value)) {
                flags[i] = ALL_SAME;
                index[i] = value;
                // Fall through to ALL_SAME handling.
            } இல்லையெனில் {
                newDataCapacity += blockLength;
                தொடர்;
            }
        } இல்லையெனில் {
            U_ASSERT(flags[i] == ALL_SAME);
            எனில் (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                தருக்க allSame = உண்மை;
                int32_t next_i = i + inc;
                க்கு (int32_t j = i + 1; j < next_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    எனில் (index[j] != value) {
                        allSame = பொய்;
                        நிறுத்து;
                    }
                }
                எனில் (!allSame) {
                    // Turn it into a MIXED block.
                    எனில் (getDataBlock(i) < 0) {
                        திரும்பு -1;
                    }
                    newDataCapacity += blockLength;
                    தொடர்;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        எனில் (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#ifdef UCPTRIE_DEBUG
            எனில் (!overflow) {
                puts("UCPTrie AllSameBlocks overflow");
                overflow = உண்மை;
            }
#endif
            int32_t jInc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
            க்கு (int32_t j = 0;; j += jInc) {
                எனில் (j == i) {
                    allSameBlocks.add(i, inc, value);
                    நிறுத்து;
                }
                எனில் (j == fastILimit) {
                    jInc = 1;
                }
                எனில் (flags[j] == ALL_SAME && index[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    நிறுத்து;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        எனில் (other >= 0) {
            flags[i] = SAME_AS;
            index[i] = other;
        } இல்லையெனில் {
            // New unique same-value block.
            newDataCapacity += blockLength;
        }
    }
    திரும்பு newDataCapacity;
}

#ifdef UCPTRIE_DEBUG
#   define DEBUG_DO(கோவை) கோவை
#இல்லையெனில்
#   define DEBUG_DO(கோவை)
#endif

#ifdef UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_மதிப்பு(எழுத்து s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (எழுத்து)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (எழுத்து)(0x80 + (value & 0x3F));
    திரும்பு length + 3;
}

வெறுமை printBlock(மாறிலி uint32_t *block, int32_t blockLength, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_மதிப்பு) {
    எழுத்து s[UCPTRIE_FAST_DATA_BLOCK_LENGTH * 3 + 3];
    int32_t length = 0;
    int32_t i;
    க்கு (i = 0; i < overlap; ++i) {
        length = append_மதிப்பு(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    க்கு (; i < blockLength; ++i) {
        எனில் (block != nullptr) {
            value = block[i];
        }
        எனில் (value == initial_மதிப்பு) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_மதிப்பு(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    எனில் (start <= 0xffff) {
        printf("    %04lX  %s|\n", (long)start, s);
    } இல்லையெனில் எனில் (start <= 0xfffff) {
        printf("   %5lX  %s|\n", (long)start, s);
    } இல்லையெனில் {
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
    int32_t மொத்தம்_Same=0, sumOverlaps=0;
    தருக்க printData = dataLength == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        dataLength == 50400 /* zh.txt~stroke */;
#endif

    // The linear ASCII data has been copied into newData already.
    int32_t newDataLength = 0;
    க்கு (int32_t i = 0; newDataLength < ASCII_LIMIT;
            newDataLength += UCPTRIE_FAST_DATA_BLOCK_LENGTH, i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK) {
        index[i] = newDataLength;
#ifdef UCPTRIE_DEBUG
        எனில் (printData) {
            printBlock(newData + newDataLength, UCPTRIE_FAST_DATA_BLOCK_LENGTH, 0, newDataLength, 0, initial_மதிப்பு);
        }
#endif
    }

    int32_t blockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
    எனில் (!mixedBlocks.init(newDataCapacity, blockLength)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    mixedBlocks.extend(newData, 0, 0, newDataLength);

    int32_t iLimit = உயர்வு_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
    int32_t fastLength = 0;
    க்கு (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        எனில் (i == fastILimit) {
            blockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            inc = 1;
            fastLength = newDataLength;
            எனில் (!mixedBlocks.init(newDataCapacity, blockLength)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு 0;
            }
            mixedBlocks.extend(newData, 0, 0, newDataLength);
        }
        எனில் (flags[i] == ALL_SAME) {
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
            போது (n >= 0 && i == dataNullIndex && i >= fastILimit && n < fastLength &&
                    isStartOfSomeFastBlock(n, index, fastILimit)) {
                n = findAllSameBlock(newData, n + 1, newDataLength, value, blockLength);
            }
            எனில் (n >= 0) {
                DEBUG_DO(++மொத்தம்_Same);
                index[i] = n;
            } இல்லையெனில் {
                n = getAllSameOverlap(newData, newDataLength, value, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                எனில் (printData) {
                    printBlock(nullptr, blockLength, value, i << UCPTRIE_SHIFT_3, n, initial_மதிப்பு);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                போது (n < blockLength) {
                    newData[newDataLength++] = value;
                    ++n;
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } இல்லையெனில் எனில் (flags[i] == MIXED) {
            மாறிலி uint32_t *block = data + index[i];
            int32_t n = mixedBlocks.findBlock(newData, block, 0);
            எனில் (n >= 0) {
                DEBUG_DO(++மொத்தம்_Same);
                index[i] = n;
            } இல்லையெனில் {
                n = getOverlap(newData, newDataLength, block, 0, blockLength);
                DEBUG_DO(sumOverlaps += n);
#ifdef UCPTRIE_DEBUG
                எனில் (printData) {
                    printBlock(block, blockLength, 0, i << UCPTRIE_SHIFT_3, n, initial_மதிப்பு);
                }
#endif
                index[i] = newDataLength - n;
                int32_t prevDataLength = newDataLength;
                போது (n < blockLength) {
                    newData[newDataLength++] = block[n++];
                }
                mixedBlocks.extend(newData, 0, prevDataLength, newDataLength);
            }
        } இல்லையெனில் /* SAME_AS */ {
            uint32_t j = index[i];
            index[i] = index[j];
        }
    }

#ifdef UCPTRIE_DEBUG
    /* we saved some space */
    printf("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)dataLength, (long)newDataLength, (long)மொத்தம்_Same, (long)sumOverlaps);
#endif
    திரும்பு newDataLength;
}

int32_t MutableCodePointTrie::compactIndex(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fastIndexLength = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    எனில் ((உயர்வு_Start >> UCPTRIE_FAST_SHIFT) <= fastIndexLength) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        திரும்பு fastIndexLength;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fastIndex[UCPTRIE_BMP_INDEX_LENGTH];  // fastIndexLength
    int32_t i3FirstNull = -1;
    க்கு (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = index[i];
        fastIndex[j] = static_cast<uint16_t>(i3);
        எனில் (i3 == static_cast<uint32_t>(dataNullOffset)) {
            எனில் (i3FirstNull < 0) {
                i3FirstNull = j;
            } இல்லையெனில் எனில் (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                index3NullOffset = i3FirstNull;
            }
        } இல்லையெனில் {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t iNext = i + SMALL_DATA_BLOCKS_PER_BMP_BLOCK;
        போது (++i < iNext) {
            i3 += UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
            index[i] = i3;
        }
    }

    எனில் (!mixedBlocks.init(fastIndexLength, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
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
    தருக்க hasLongI3Blocks = பொய்;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = உயர்வு_Start >> UCPTRIE_SHIFT_3;
    க்கு (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
        uint32_t oredI3 = 0;
        தருக்க isNull = உண்மை;
        செய் {
            uint32_t i3 = index[j];
            oredI3 |= i3;
            எனில் (i3 != static_cast<uint32_t>(dataNullOffset)) {
                isNull = பொய்;
            }
        } போது (++j < jLimit);
        எனில் (isNull) {
            flags[i] = I3_NULL;
            எனில் (i3FirstNull < 0) {
                எனில் (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                } இல்லையெனில் {
                    index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                    hasLongI3Blocks = உண்மை;
                }
                i3FirstNull = 0;
            }
        } இல்லையெனில் {
            எனில் (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fastIndex, index, i);
                எனில் (n >= 0) {
                    flags[i] = I3_BMP;
                    index[i] = n;
                } இல்லையெனில் {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_INDEX_3_BLOCK_LENGTH;
                }
            } இல்லையெனில் {
                flags[i] = I3_18;
                index3Capacity += INDEX_3_18BIT_BLOCK_LENGTH;
                hasLongI3Blocks = உண்மை;
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
    எனில் (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    uprv_memcpy(index16, fastIndex, fastIndexLength * 2);

    எனில் (!mixedBlocks.init(index16Capacity, UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    MixedBlocks longI3Blocks;
    எனில் (hasLongI3Blocks) {
        எனில் (!longI3Blocks.init(index16Capacity, INDEX_3_18BIT_BLOCK_LENGTH)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2Length = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fastIndexLength + index1Length;
    int32_t indexLength = index3Start;
    க்கு (int32_t i = iStart; i < iLimit; i += UCPTRIE_INDEX_3_BLOCK_LENGTH) {
        int32_t i3;
        uint8_t f = flags[i];
        எனில் (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = dataNullOffset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        எனில் (f == I3_NULL) {
            i3 = index3NullOffset;
        } இல்லையெனில் எனில் (f == I3_BMP) {
            i3 = index[i];
        } இல்லையெனில் எனில் (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, index, i);
            எனில் (n >= 0) {
                i3 = n;
            } இல்லையெனில் {
                எனில் (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } இல்லையெனில் {
                    n = getOverlap(index16, indexLength,
                                   index, i, UCPTRIE_INDEX_3_BLOCK_LENGTH);
                }
                i3 = indexLength - n;
                int32_t prevIndexLength = indexLength;
                போது (n < UCPTRIE_INDEX_3_BLOCK_LENGTH) {
                    index16[indexLength++] = index[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                எனில் (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        } இல்லையெனில் {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_INDEX_3_BLOCK_LENGTH;
            int32_t k = indexLength;
            செய் {
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
            } போது (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, indexLength);
            எனில் (n >= 0) {
                i3 = n | 0x8000;
            } இல்லையெனில் {
                எனில் (indexLength == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } இல்லையெனில் {
                    n = getOverlap(index16, indexLength,
                                   index16, indexLength, INDEX_3_18BIT_BLOCK_LENGTH);
                }
                i3 = (indexLength - n) | 0x8000;
                int32_t prevIndexLength = indexLength;
                எனில் (n > 0) {
                    int32_t start = indexLength;
                    போது (n < INDEX_3_18BIT_BLOCK_LENGTH) {
                        index16[indexLength++] = index16[start + n++];
                    }
                } இல்லையெனில் {
                    indexLength += INDEX_3_18BIT_BLOCK_LENGTH;
                }
                mixedBlocks.extend(index16, index3Start, prevIndexLength, indexLength);
                எனில் (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, prevIndexLength, indexLength);
                }
            }
        }
        எனில் (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2Length++] = i3;
    }
    U_ASSERT(i2Length == index2Capacity);
    U_ASSERT(indexLength <= index3Start + index3Capacity);

    எனில் (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    எனில் (indexLength >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_INDEX_3_BLOCK_LENGTH)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_INDEX_2_BLOCK_LENGTH == UCPTRIE_INDEX_3_BLOCK_LENGTH,
                  "must re-init mixedBlocks");
    int32_t blockLength = UCPTRIE_INDEX_2_BLOCK_LENGTH;
    int32_t i1 = fastIndexLength;
    க்கு (int32_t i = 0; i < i2Length; i += blockLength) {
        int32_t n;
        எனில் ((i2Length - i) >= blockLength) {
            // normal block
            U_ASSERT(blockLength == UCPTRIE_INDEX_2_BLOCK_LENGTH);
            n = mixedBlocks.findBlock(index16, index2, i);
        } இல்லையெனில் {
            // highStart is inside the last index-2 block. Shorten it.
            blockLength = i2Length - i;
            n = findSameBlock(index16, index3Start, indexLength,
                              index2, i, blockLength);
        }
        int32_t i2;
        எனில் (n >= 0) {
            i2 = n;
        } இல்லையெனில் {
            எனில் (indexLength == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } இல்லையெனில் {
                n = getOverlap(index16, indexLength, index2, i, blockLength);
            }
            i2 = indexLength - n;
            int32_t prevIndexLength = indexLength;
            போது (n < blockLength) {
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

    திரும்பு indexLength;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((உயர்வு_Start & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
    உயர்வு_மதிப்பு = get(MAX_UNICODE);
    int32_t real_உயர்வு_Start = find_உயர்வு_Start();
    real_உயர்வு_Start = (real_உயர்வு_Start + (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
    எனில் (real_உயர்வு_Start == UNICODE_LIMIT) {
        உயர்வு_மதிப்பு = initial_மதிப்பு;
    }

#ifdef UCPTRIE_DEBUG
    printf("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)real_உயர்வு_Start, (long)உயர்வு_மதிப்பு, (long)initial_மதிப்பு);
#endif

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    எனில் (real_உயர்வு_Start < fastLimit) {
        க்கு (int32_t i = (real_உயர்வு_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            index[i] = உயர்வு_மதிப்பு;
        }
        உயர்வு_Start = fastLimit;
    } இல்லையெனில் {
        உயர்வு_Start = real_உயர்வு_Start;
    }

    uint32_t asciiData[ASCII_LIMIT];
    க்கு (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        asciiData[i] = get(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t newDataCapacity = compactWholeDataBlocks(fastILimit, allSameBlocks);
    எனில் (newDataCapacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    uint32_t* newData = static_cast<uint32_t*>(uprv_malloc(newDataCapacity * 4));
    எனில் (newData == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    uprv_memcpy(newData, asciiData, அளவு(asciiData));

    int32_t dataNullIndex = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t newDataLength = compactData(fastILimit, newData, newDataCapacity,
                                        dataNullIndex, mixedBlocks, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0; }
    U_ASSERT(newDataLength <= newDataCapacity);
    uprv_free(data);
    data = newData;
    dataCapacity = newDataCapacity;
    dataLength = newDataLength;
    எனில் (dataLength > (0x3ffff + UCPTRIE_SMALL_DATA_BLOCK_LENGTH)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    எனில் (dataNullIndex >= 0) {
        dataNullOffset = index[dataNullIndex];
#ifdef UCPTRIE_DEBUG
        எனில் (data[dataNullOffset] != initial_மதிப்பு) {
            printf("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (long)initial_மதிப்பு, (long)data[dataNullOffset]);
        }
#endif
        initial_மதிப்பு = data[dataNullOffset];
    } இல்லையெனில் {
        dataNullOffset = UCPTRIE_NO_DATA_NULL_OFFSET;
    }

    int32_t indexLength = compactIndex(fastILimit, mixedBlocks, errorCode);
    உயர்வு_Start = real_உயர்வு_Start;
    திரும்பு indexLength;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_மதிப்பு_Width மதிப்பு_Width, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    எனில் (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            மதிப்பு_Width < UCPTRIE_மதிப்பு_BITS_16 || UCPTRIE_மதிப்பு_BITS_8 < மதிப்பு_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    தேர்வு (மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        mask_மதிப்புகள்(0xffff);
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        mask_மதிப்புகள்(0xff);
        நிறுத்து;
    இயல்பு:
        நிறுத்து;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t indexLength = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    எனில் (U_FAILURE(errorCode)) {
        clear();
        திரும்பு nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    எனில் (மதிப்பு_Width == UCPTRIE_மதிப்பு_BITS_32 && (indexLength & 1) != 0) {
        index16[indexLength++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = indexLength * 2;
    எனில் (மதிப்பு_Width == UCPTRIE_மதிப்பு_BITS_16) {
        எனில் (((indexLength ^ dataLength) & 1) != 0) {
            // padding
            data[dataLength++] = error_மதிப்பு;
        }
        எனில் (data[dataLength - 1] != error_மதிப்பு || data[dataLength - 2] != உயர்வு_மதிப்பு) {
            data[dataLength++] = உயர்வு_மதிப்பு;
            data[dataLength++] = error_மதிப்பு;
        }
        length += dataLength * 2;
    } இல்லையெனில் எனில் (மதிப்பு_Width == UCPTRIE_மதிப்பு_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        எனில் (data[dataLength - 1] != error_மதிப்பு || data[dataLength - 2] != உயர்வு_மதிப்பு) {
            எனில் (data[dataLength - 1] != உயர்வு_மதிப்பு) {
                data[dataLength++] = உயர்வு_மதிப்பு;
            }
            data[dataLength++] = error_மதிப்பு;
        }
        length += dataLength * 4;
    } இல்லையெனில் {
        int32_t and3 = (length + dataLength) & 3;
        எனில் (and3 == 0 && data[dataLength - 1] == error_மதிப்பு && data[dataLength - 2] == உயர்வு_மதிப்பு) {
            // all set
        } இல்லையெனில் எனில்(and3 == 3 && data[dataLength - 1] == உயர்வு_மதிப்பு) {
            data[dataLength++] = error_மதிப்பு;
        } இல்லையெனில் {
            போது (and3 != 2) {
                data[dataLength++] = உயர்வு_மதிப்பு;
                and3 = (and3 + 1) & 3;
            }
            data[dataLength++] = உயர்வு_மதிப்பு;
            data[dataLength++] = error_மதிப்பு;
        }
        length += dataLength;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += அளவு(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    எனில் (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        clear();
        திரும்பு nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, அளவு(UCPTrie));
    trie->indexLength = indexLength;
    trie->dataLength = dataLength;

    trie->உயர்வு_Start = உயர்வு_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_உயர்வு_Start = (உயர்வு_Start + 0xfff) >> 12;
    trie->type = type;
    trie->மதிப்பு_Width = மதிப்பு_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->dataNullOffset = dataNullOffset;
    trie->null_மதிப்பு = initial_மதிப்பு;

    bytes += அளவு(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->index = dest16;

    எனில் (உயர்வு_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        க்கு (int32_t i = 0, j = 0; j < indexLength; i += SMALL_DATA_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(index[i]); // dest16[j]
        }
    } இல்லையெனில் {
        uprv_memcpy(dest16, index16, indexLength * 2);
        dest16 += indexLength;
    }
    bytes += indexLength * 2;

    // Write the data array.
    மாறிலி uint32_t *p = data;
    தேர்வு (மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        // Write 16-bit data values.
        trie->data.ptr16 = dest16;
        க்கு (int32_t i = dataLength; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        // Write 32-bit data values.
        trie->data.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (size_t)dataLength * 4);
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        // Write 8-bit data values.
        trie->data.ptr8 = bytes;
        க்கு (int32_t i = dataLength; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        நிறுத்து;
    இயல்பு:
        // Will not occur, valueWidth checked at the beginning.
        நிறுத்து;
    }

#ifdef UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#endif

    clear();
    திரும்பு trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_மதிப்பு, uint32_t error_மதிப்பு, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        புதிய MutableCodePointTrie(initial_மதிப்பு, error_மதிப்பு, *pErrorCode), *pErrorCode);
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(மாறிலி UMutableCPTrie *other, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில் (other == nullptr) {
        திரும்பு nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        புதிய MutableCodePointTrie(*reinterpret_cast<மாறிலி MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI வெறுமை U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    delete reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(மாறிலி UCPMap *map, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில் (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(மாறிலி UCPTrie *trie, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில் (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(மாறிலி UMutableCPTrie *trie, UChar32 c) {
    திரும்பு reinterpret_cast<மாறிலி MutableCodePointTrie *>(trie)->get(c);
}

நாமாகாஶ {

UChar32 getRange(மாறிலி வெறுமை *trie, UChar32 start,
                 UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context, uint32_t *p_மதிப்பு) {
    திரும்பு reinterpret_cast<மாறிலி MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_மதிப்பு);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(மாறிலி UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_மதிப்பு,
                        UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context, uint32_t *p_மதிப்பு) {
    திரும்பு ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_மதிப்பு,
                                    filter, context, p_மதிப்பு);
}

U_CAPI வெறுமை U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI வெறுமை U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_மதிப்பு_Width மதிப்பு_Width,
                              UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, மதிப்பு_Width, *pErrorCode);
}

#ifdef UCPTRIE_DEBUG
U_CFUNC வெறுமை umutablecptrie_setName(UMutableCPTrie *trie, மாறிலி எழுத்து *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#endif
