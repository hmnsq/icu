// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// umutablecptrie.cpp (inspired by utrie2_builder.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#अगर_घोषित UCPTRIE_DEBUG
#   include <stdio.h>
#पूर्ण_अगर

#समावेश "unicode/utypes.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/umutablecptrie.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "ucptrie_impl.h"

// ICU-20235 In case Microsoft math.h has defined this, undefine it.
#अगर_घोषित OVERFLOW
#अघोषित OVERFLOW
#पूर्ण_अगर

U_NAMESPACE_BEGIN

namespace {

constexpr int32_t MAX_UNICODE = 0x10ffff;

constexpr int32_t UNICODE_LIMIT = 0x110000;
constexpr int32_t BMP_LIMIT = 0x10000;
constexpr int32_t ASCII_LIMIT = 0x80;

constexpr int32_t I_LIMIT = UNICODE_LIMIT >> UCPTRIE_SHIFT_3;
constexpr int32_t BMP_I_LIMIT = BMP_LIMIT >> UCPTRIE_SHIFT_3;
constexpr int32_t ASCII_I_LIMIT = ASCII_LIMIT >> UCPTRIE_SHIFT_3;

constexpr int32_t SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK = (1 << (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3));

// Flag values for data blocks.
constexpr uint8_t ALL_SAME = 0;
constexpr uint8_t MIXED = 1;
constexpr uint8_t SAME_AS = 2;

/** Start with allocation of 16k data entries. */
constexpr int32_t INITIAL_तथ्य_लंबाई = static_cast<int32_t>(1) << 14;

/** Grow about 8x each time. */
constexpr int32_t MEDIUM_तथ्य_लंबाई = static_cast<int32_t>(1) << 17;

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points.
 */
constexpr int32_t MAX_तथ्य_लंबाई = UNICODE_LIMIT;

// Flag values for index-3 blocks while compacting/building.
constexpr uint8_t I3_NULL = 0;
constexpr uint8_t I3_BMP = 1;
constexpr uint8_t I3_16 = 2;
constexpr uint8_t I3_18 = 3;

constexpr int32_t सूचकांक_3_18_BIT_BLOCK_लंबाई = UCPTRIE_सूचकांक_3_BLOCK_लंबाई + UCPTRIE_सूचकांक_3_BLOCK_लंबाई / 8;

श्रेणी AllSameBlocks;
श्रेणी MixedBlocks;

श्रेणी MutableCodePointTrie : खुला UMemory {
खुला:
    MutableCodePointTrie(uint32_t initial_मूल्य, uint32_t error_मूल्य, UErrorCode &errorCode);
    MutableCodePointTrie(स्थिर MutableCodePointTrie &other, UErrorCode &errorCode);
    MutableCodePointTrie(स्थिर MutableCodePointTrie &other) = मिटाओ;
    ~MutableCodePointTrie();

    MutableCodePointTrie &चालक=(स्थिर MutableCodePointTrie &other) = मिटाओ;

    अटल MutableCodePointTrie *fromUCPMap(स्थिर UCPMap *map, UErrorCode &errorCode);
    अटल MutableCodePointTrie *fromUCPTrie(स्थिर UCPTrie *trie, UErrorCode &errorCode);

    uint32_t लो(UChar32 c) स्थिर;
    int32_t getRange(UChar32 start, UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context,
                     uint32_t *p_मूल्य) स्थिर;

    व्योम set(UChar32 c, uint32_t value, UErrorCode &errorCode);
    व्योम setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode);

    UCPTrie *build(UCPTrieType type, UCPTrie_मूल्य_Width मूल्य_Width, UErrorCode &errorCode);

निजी:
    व्योम सफा();

    bool ensure_ऊपरी_Start(UChar32 c);
    int32_t alloc_तथ्य_Block(int32_t block_लंबाई);
    int32_t get_तथ्य_Block(int32_t i);

    व्योम mask_मूल्यावली(uint32_t mask);
    UChar32 find_ऊपरी_Start() स्थिर;
    int32_t compact_Whole_तथ्य_Blocks(int32_t fastILimit, AllSameBlocks &allSameBlocks);
    int32_t compact_तथ्य(
            int32_t fastILimit, uint32_t *new_तथ्य, int32_t new_तथ्य_Capacity,
            int32_t तथ्य_Null_सूचकांक, MixedBlocks &mixedBlocks, UErrorCode &errorCode);
    int32_t compact_सूचकांक(int32_t fastILimit, MixedBlocks &mixedBlocks, UErrorCode &errorCode);
    int32_t compactTrie(int32_t fastILimit, UErrorCode &errorCode);

    uint32_t *सूचकांक = nullptr;
    int32_t सूचकांक_Capacity = 0;
    int32_t index3NullOffset = -1;
    uint32_t *तथ्य = nullptr;
    int32_t तथ्य_Capacity = 0;
    int32_t तथ्य_लंबाई = 0;
    int32_t तथ्य_Null_Offset = -1;

    uint32_t orig_Initial_मूल्य;
    uint32_t initial_मूल्य;
    uint32_t error_मूल्य;
    UChar32 ऊपरी_Start;
    uint32_t ऊपरी_मूल्य;
#अगर_घोषित UCPTRIE_DEBUG
खुला:
    स्थिर अक्षर *name;
#पूर्ण_अगर
निजी:
    /** Temporary array while building the final data. */
    uint16_t *index16 = nullptr;
    uint8_t flags[UNICODE_LIMIT >> UCPTRIE_SHIFT_3];
};

MutableCodePointTrie::MutableCodePointTrie(uint32_t ini_मूल्य, uint32_t err_मूल्य, UErrorCode &errorCode) :
        orig_Initial_मूल्य(ini_मूल्य), initial_मूल्य(ini_मूल्य), error_मूल्य(err_मूल्य),
        ऊपरी_Start(0), ऊपरी_मूल्य(initial_मूल्य)
#अगर_घोषित UCPTRIE_DEBUG
        , name("open")
#पूर्ण_अगर
        {
    अगर (U_FAILURE(errorCode)) { वापस; }
    सूचकांक = static_cast<uint32_t*>(uprv_malloc(BMP_I_LIMIT * 4));
    तथ्य = static_cast<uint32_t*>(uprv_malloc(INITIAL_तथ्य_लंबाई * 4));
    अगर (सूचकांक == nullptr || तथ्य == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    सूचकांक_Capacity = BMP_I_LIMIT;
    तथ्य_Capacity = INITIAL_तथ्य_लंबाई;
}

MutableCodePointTrie::MutableCodePointTrie(स्थिर MutableCodePointTrie &other, UErrorCode &errorCode) :
        index3NullOffset(other.index3NullOffset),
        तथ्य_Null_Offset(other.तथ्य_Null_Offset),
        orig_Initial_मूल्य(other.orig_Initial_मूल्य), initial_मूल्य(other.initial_मूल्य),
        error_मूल्य(other.error_मूल्य),
        ऊपरी_Start(other.ऊपरी_Start), ऊपरी_मूल्य(other.ऊपरी_मूल्य)
#अगर_घोषित UCPTRIE_DEBUG
        , name("mutable clone")
#पूर्ण_अगर
        {
    अगर (U_FAILURE(errorCode)) { वापस; }
    int32_t iCapacity = ऊपरी_Start <= BMP_LIMIT ? BMP_I_LIMIT : I_LIMIT;
    सूचकांक = static_cast<uint32_t*>(uprv_malloc(iCapacity * 4));
    तथ्य = static_cast<uint32_t*>(uprv_malloc(other.तथ्य_Capacity * 4));
    अगर (सूचकांक == nullptr || तथ्य == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    सूचकांक_Capacity = iCapacity;
    तथ्य_Capacity = other.तथ्य_Capacity;

    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    uprv_memcpy(flags, other.flags, iLimit);
    uprv_memcpy(सूचकांक, other.सूचकांक, iLimit * 4);
    uprv_memcpy(तथ्य, other.तथ्य, (माप_प्रकार)other.तथ्य_लंबाई * 4);
    तथ्य_लंबाई = other.तथ्य_लंबाई;
    U_ASSERT(other.index16 == nullptr);
}

MutableCodePointTrie::~MutableCodePointTrie() {
    uprv_free(सूचकांक);
    uprv_free(तथ्य);
    uprv_free(index16);
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPMap(स्थिर UCPMap *map, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मूल्य = ucpmap_get(map, -1);
    uint32_t initial_मूल्य = ucpmap_get(map, 0x10ffff);
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नया MutableCodePointTrie(initial_मूल्य, error_मूल्य, errorCode),
        errorCode);
    अगर (U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    जबतक ((end = ucpmap_getRange(map, start, UCPMAP_RANGE_NORMAL, 0,
                                  nullptr, nullptr, &value)) >= 0) {
        अगर (value != initial_मूल्य) {
            अगर (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अन्यथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    अगर (U_SUCCESS(errorCode)) {
        वापस mutableTrie.orphan();
    } अन्यथा {
        वापस nullptr;
    }
}

MutableCodePointTrie *MutableCodePointTrie::fromUCPTrie(स्थिर UCPTrie *trie, UErrorCode &errorCode) {
    // Use the highValue as the initialValue to reduce the highStart.
    uint32_t error_मूल्य;
    uint32_t initial_मूल्य;
    चयन (trie->मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        error_मूल्य = trie->तथ्य.ptr16[trie->तथ्य_लंबाई - UCPTRIE_ERROR_मूल्य_NEG_तथ्य_OFFSET];
        initial_मूल्य = trie->तथ्य.ptr16[trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET];
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        error_मूल्य = trie->तथ्य.ptr32[trie->तथ्य_लंबाई - UCPTRIE_ERROR_मूल्य_NEG_तथ्य_OFFSET];
        initial_मूल्य = trie->तथ्य.ptr32[trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET];
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        error_मूल्य = trie->तथ्य.ptr8[trie->तथ्य_लंबाई - UCPTRIE_ERROR_मूल्य_NEG_तथ्य_OFFSET];
        initial_मूल्य = trie->तथ्य.ptr8[trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET];
        अवरोध;
    शेष:
        // Unreachable if the trie is properly initialized.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    LocalPointer<MutableCodePointTrie> mutableTrie(
        नया MutableCodePointTrie(initial_मूल्य, error_मूल्य, errorCode),
        errorCode);
    अगर (U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    UChar32 start = 0, end;
    uint32_t value;
    जबतक ((end = ucptrie_getRange(trie, start, UCPMAP_RANGE_NORMAL, 0,
                                   nullptr, nullptr, &value)) >= 0) {
        अगर (value != initial_मूल्य) {
            अगर (start == end) {
                mutableTrie->set(start, value, errorCode);
            } अन्यथा {
                mutableTrie->setRange(start, end, value, errorCode);
            }
        }
        start = end + 1;
    }
    अगर (U_SUCCESS(errorCode)) {
        वापस mutableTrie.orphan();
    } अन्यथा {
        वापस nullptr;
    }
}

व्योम MutableCodePointTrie::सफा() {
    index3NullOffset = तथ्य_Null_Offset = -1;
    तथ्य_लंबाई = 0;
    ऊपरी_मूल्य = initial_मूल्य = orig_Initial_मूल्य;
    ऊपरी_Start = 0;
    uprv_free(index16);
    index16 = nullptr;
}

uint32_t MutableCodePointTrie::लो(UChar32 c) स्थिर {
    अगर (static_cast<uint32_t>(c) > MAX_UNICODE) {
        वापस error_मूल्य;
    }
    अगर (c >= ऊपरी_Start) {
        वापस ऊपरी_मूल्य;
    }
    int32_t i = c >> UCPTRIE_SHIFT_3;
    अगर (flags[i] == ALL_SAME) {
        वापस सूचकांक[i];
    } अन्यथा {
        वापस तथ्य[सूचकांक[i] + (c & UCPTRIE_SMALL_तथ्य_MASK)];
    }
}

अंतरभूत uint32_t maybe_Filter_मूल्य(uint32_t value, uint32_t initial_मूल्य, uint32_t null_मूल्य,
                                 UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context) {
    अगर (value == initial_मूल्य) {
        value = null_मूल्य;
    } अन्यथा अगर (filter != nullptr) {
        value = filter(context, value);
    }
    वापस value;
}

UChar32 MutableCodePointTrie::getRange(
        UChar32 start, UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context,
        uint32_t *p_मूल्य) स्थिर {
    अगर (static_cast<uint32_t>(start) > MAX_UNICODE) {
        वापस U_SENTINEL;
    }
    अगर (start >= ऊपरी_Start) {
        अगर (p_मूल्य != nullptr) {
            uint32_t value = ऊपरी_मूल्य;
            अगर (filter != nullptr) { value = filter(context, value); }
            *p_मूल्य = value;
        }
        वापस MAX_UNICODE;
    }
    uint32_t null_मूल्य = initial_मूल्य;
    अगर (filter != nullptr) { null_मूल्य = filter(context, null_मूल्य); }
    UChar32 c = start;
    uint32_t trie_मूल्य, value;
    bool have_मूल्य = false;
    int32_t i = c >> UCPTRIE_SHIFT_3;
    करो {
        अगर (flags[i] == ALL_SAME) {
            uint32_t trieValue2 = सूचकांक[i];
            अगर (have_मूल्य) {
                अगर (trieValue2 != trie_मूल्य) {
                    अगर (filter == nullptr ||
                            maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य,
                                             filter, context) != value) {
                        वापस c - 1;
                    }
                    trie_मूल्य = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मूल्य = trieValue2;
                value = maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य, filter, context);
                अगर (p_मूल्य != nullptr) { *p_मूल्य = value; }
                have_मूल्य = true;
            }
            c = (c + UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई) & ~UCPTRIE_SMALL_तथ्य_MASK;
        } अन्यथा /* MIXED */ {
            int32_t di = सूचकांक[i] + (c & UCPTRIE_SMALL_तथ्य_MASK);
            uint32_t trieValue2 = तथ्य[di];
            अगर (have_मूल्य) {
                अगर (trieValue2 != trie_मूल्य) {
                    अगर (filter == nullptr ||
                            maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य,
                                             filter, context) != value) {
                        वापस c - 1;
                    }
                    trie_मूल्य = trieValue2;  // may or may not help
                }
            } अन्यथा {
                trie_मूल्य = trieValue2;
                value = maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य, filter, context);
                अगर (p_मूल्य != nullptr) { *p_मूल्य = value; }
                have_मूल्य = true;
            }
            जबतक ((++c & UCPTRIE_SMALL_तथ्य_MASK) != 0) {
                trieValue2 = तथ्य[++di];
                अगर (trieValue2 != trie_मूल्य) {
                    अगर (filter == nullptr ||
                            maybe_Filter_मूल्य(trieValue2, initial_मूल्य, null_मूल्य,
                                             filter, context) != value) {
                        वापस c - 1;
                    }
                }
                trie_मूल्य = trieValue2;  // may or may not help
            }
        }
        ++i;
    } जबतक (c < ऊपरी_Start);
    U_ASSERT(have_मूल्य);
    अगर (maybe_Filter_मूल्य(ऊपरी_मूल्य, initial_मूल्य, null_मूल्य,
                         filter, context) != value) {
        वापस c - 1;
    } अन्यथा {
        वापस MAX_UNICODE;
    }
}

व्योम
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit = block + UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
    जबतक (block < limit) {
        *block++ = value;
    }
}

bool MutableCodePointTrie::ensure_ऊपरी_Start(UChar32 c) {
    अगर (c >= ऊपरी_Start) {
        // Round up to a UCPTRIE_CP_PER_INDEX_2_ENTRY boundary to simplify compaction.
        c = (c + UCPTRIE_CP_PER_सूचकांक_2_ENTRY) & ~(UCPTRIE_CP_PER_सूचकांक_2_ENTRY - 1);
        int32_t i = ऊपरी_Start >> UCPTRIE_SHIFT_3;
        int32_t iLimit = c >> UCPTRIE_SHIFT_3;
        अगर (iLimit > सूचकांक_Capacity) {
            uint32_t* new_सूचकांक = static_cast<uint32_t*>(uprv_malloc(I_LIMIT * 4));
            अगर (new_सूचकांक == nullptr) { वापस false; }
            uprv_memcpy(new_सूचकांक, सूचकांक, i * 4);
            uprv_free(सूचकांक);
            सूचकांक = new_सूचकांक;
            सूचकांक_Capacity = I_LIMIT;
        }
        करो {
            flags[i] = ALL_SAME;
            सूचकांक[i] = initial_मूल्य;
        } जबतक(++i < iLimit);
        ऊपरी_Start = c;
    }
    वापस true;
}

int32_t MutableCodePointTrie::alloc_तथ्य_Block(int32_t block_लंबाई) {
    int32_t newBlock = तथ्य_लंबाई;
    int32_t new_शीर्ष = newBlock + block_लंबाई;
    अगर (new_शीर्ष > तथ्य_Capacity) {
        int32_t capacity;
        अगर (तथ्य_Capacity < MEDIUM_तथ्य_लंबाई) {
            capacity = MEDIUM_तथ्य_लंबाई;
        } अन्यथा अगर (तथ्य_Capacity < MAX_तथ्य_लंबाई) {
            capacity = MAX_तथ्य_लंबाई;
        } अन्यथा {
            // Should never occur.
            // Either MAX_DATA_LENGTH is incorrect,
            // or the code writes more values than should be possible.
            वापस -1;
        }
        uint32_t* new_तथ्य = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
        अगर (new_तथ्य == nullptr) {
            वापस -1;
        }
        uprv_memcpy(new_तथ्य, तथ्य, (माप_प्रकार)तथ्य_लंबाई * 4);
        uprv_free(तथ्य);
        तथ्य = new_तथ्य;
        तथ्य_Capacity = capacity;
    }
    तथ्य_लंबाई = new_शीर्ष;
    वापस newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
int32_t MutableCodePointTrie::get_तथ्य_Block(int32_t i) {
    अगर (flags[i] == MIXED) {
        वापस सूचकांक[i];
    }
    अगर (i < BMP_I_LIMIT) {
        int32_t newBlock = alloc_तथ्य_Block(UCPTRIE_FAST_तथ्य_BLOCK_लंबाई);
        अगर (newBlock < 0) { वापस newBlock; }
        int32_t iStart = i & ~(SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK -1);
        int32_t iLimit = iStart + SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK;
        करो {
            U_ASSERT(flags[iStart] == ALL_SAME);
            writeBlock(तथ्य + newBlock, सूचकांक[iStart]);
            flags[iStart] = MIXED;
            सूचकांक[iStart++] = newBlock;
            newBlock += UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
        } जबतक (iStart < iLimit);
        वापस सूचकांक[i];
    } अन्यथा {
        int32_t newBlock = alloc_तथ्य_Block(UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई);
        अगर (newBlock < 0) { वापस newBlock; }
        writeBlock(तथ्य + newBlock, सूचकांक[i]);
        flags[i] = MIXED;
        सूचकांक[i] = newBlock;
        वापस newBlock;
    }
}

व्योम MutableCodePointTrie::set(UChar32 c, uint32_t value, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }
    अगर (static_cast<uint32_t>(c) > MAX_UNICODE) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    int32_t block;
    अगर (!ensure_ऊपरी_Start(c) || (block = get_तथ्य_Block(c >> UCPTRIE_SHIFT_3)) < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    तथ्य[block + (c & UCPTRIE_SMALL_तथ्य_MASK)] = value;
}

व्योम
fillBlock(uint32_t *block, UChar32 start, UChar32 limit, uint32_t value) {
    uint32_t *pLimit = block + limit;
    block += start;
    जबतक (block < pLimit) {
        *block++ = value;
    }
}

व्योम MutableCodePointTrie::setRange(UChar32 start, UChar32 end, uint32_t value, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }
    अगर (static_cast<uint32_t>(start) > MAX_UNICODE || static_cast<uint32_t>(end) > MAX_UNICODE || start > end) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर (!ensure_ऊपरी_Start(end)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    UChar32 limit = end + 1;
    अगर (start & UCPTRIE_SMALL_तथ्य_MASK) {
        // Set partial block at [start..following block boundary[.
        int32_t block = get_तथ्य_Block(start >> UCPTRIE_SHIFT_3);
        अगर (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }

        UChar32 अग्र_Start = (start + UCPTRIE_SMALL_तथ्य_MASK) & ~UCPTRIE_SMALL_तथ्य_MASK;
        अगर (अग्र_Start <= limit) {
            fillBlock(तथ्य + block, start & UCPTRIE_SMALL_तथ्य_MASK, UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई,
                      value);
            start = अग्र_Start;
        } अन्यथा {
            fillBlock(तथ्य + block, start & UCPTRIE_SMALL_तथ्य_MASK, limit & UCPTRIE_SMALL_तथ्य_MASK,
                      value);
            वापस;
        }
    }

    // Number of positions in the last, partial block.
    int32_t rest = limit & UCPTRIE_SMALL_तथ्य_MASK;

    // Round down limit to a block boundary.
    limit &= ~UCPTRIE_SMALL_तथ्य_MASK;

    // Iterate over all-value blocks.
    जबतक (start < limit) {
        int32_t i = start >> UCPTRIE_SHIFT_3;
        अगर (flags[i] == ALL_SAME) {
            सूचकांक[i] = value;
        } अन्यथा /* MIXED */ {
            fillBlock(तथ्य + सूचकांक[i], 0, UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई, value);
        }
        start += UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
    }

    अगर (rest > 0) {
        // Set partial block at [last block boundary..limit[.
        int32_t block = get_तथ्य_Block(start >> UCPTRIE_SHIFT_3);
        अगर (block < 0) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }

        fillBlock(तथ्य + block, 0, rest, value);
    }
}

/* compaction --------------------------------------------------------------- */

व्योम MutableCodePointTrie::mask_मूल्यावली(uint32_t mask) {
    initial_मूल्य &= mask;
    error_मूल्य &= mask;
    ऊपरी_मूल्य &= mask;
    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    क्रम (int32_t i = 0; i < iLimit; ++i) {
        अगर (flags[i] == ALL_SAME) {
            सूचकांक[i] &= mask;
        }
    }
    क्रम (int32_t i = 0; i < तथ्य_लंबाई; ++i) {
        तथ्य[i] &= mask;
    }
}

ढाँचा<typename UIntA, typename UIntB>
bool equalBlocks(स्थिर UIntA *s, स्थिर UIntB *t, int32_t length) {
    जबतक (length > 0 && *s == *t) {
        ++s;
        ++t;
        --length;
    }
    वापस length == 0;
}

bool all_मूल्यावली_Same_As(स्थिर uint32_t *p, int32_t length, uint32_t value) {
    स्थिर uint32_t *pLimit = p + length;
    जबतक (p < pLimit && *p == value) { ++p; }
    वापस p == pLimit;
}

/** Search for an identical block. */
int32_t findSameBlock(स्थिर uint16_t *p, int32_t pStart, int32_t length,
                      स्थिर uint16_t *q, int32_t qStart, int32_t block_लंबाई) {
    // Ensure that we do not even partially get past length.
    length -= block_लंबाई;

    q += qStart;
    जबतक (pStart <= length) {
        अगर (equalBlocks(p + pStart, q, block_लंबाई)) {
            वापस pStart;
        }
        ++pStart;
    }
    वापस -1;
}

int32_t findAllSameBlock(स्थिर uint32_t *p, int32_t start, int32_t limit,
                         uint32_t value, int32_t block_लंबाई) {
    // Ensure that we do not even partially get past limit.
    limit -= block_लंबाई;

    क्रम (int32_t block = start; block <= limit; ++block) {
        अगर (p[block] == value) {
            क्रम (int32_t i = 1;; ++i) {
                अगर (i == block_लंबाई) {
                    वापस block;
                }
                अगर (p[block + i] != value) {
                    block += i;
                    अवरोध;
                }
            }
        }
    }
    वापस -1;
}

/**
 * Look for maximum overlap of the beginning of the other block
 * with the previous, adjacent block.
 */
ढाँचा<typename UIntA, typename UIntB>
int32_t getOverlap(स्थिर UIntA *p, int32_t length,
                   स्थिर UIntB *q, int32_t qStart, int32_t block_लंबाई) {
    int32_t overlap = block_लंबाई - 1;
    U_ASSERT(overlap <= length);
    q += qStart;
    जबतक (overlap > 0 && !equalBlocks(p + (length - overlap), q, overlap)) {
        --overlap;
    }
    वापस overlap;
}

int32_t getAllSameOverlap(स्थिर uint32_t *p, int32_t length, uint32_t value,
                          int32_t block_लंबाई) {
    int32_t min = length - (block_लंबाई - 1);
    int32_t i = length;
    जबतक (min < i && p[i - 1] == value) { --i; }
    वापस length - i;
}

bool isStartOfSomeFastBlock(uint32_t तथ्य_Offset, स्थिर uint32_t सूचकांक[], int32_t fastILimit) {
    क्रम (int32_t i = 0; i < fastILimit; i += SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK) {
        अगर (सूचकांक[i] == तथ्य_Offset) {
            वापस true;
        }
    }
    वापस false;
}

/**
 * Finds the start of the last range in the trie by enumerating backward.
 * Indexes for code points higher than this will be omitted.
 */
UChar32 MutableCodePointTrie::find_ऊपरी_Start() स्थिर {
    int32_t i = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    जबतक (i > 0) {
        bool match;
        अगर (flags[--i] == ALL_SAME) {
            match = सूचकांक[i] == ऊपरी_मूल्य;
        } अन्यथा /* MIXED */ {
            स्थिर uint32_t *p = तथ्य + सूचकांक[i];
            क्रम (int32_t j = 0;; ++j) {
                अगर (j == UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई) {
                    match = true;
                    अवरोध;
                }
                अगर (p[j] != ऊपरी_मूल्य) {
                    match = false;
                    अवरोध;
                }
            }
        }
        अगर (!match) {
            वापस (i + 1) << UCPTRIE_SHIFT_3;
        }
    }
    वापस 0;
}

श्रेणी AllSameBlocks {
खुला:
    अटल constexpr int32_t NEW_UNIQUE = -1;
    अटल constexpr int32_t OVERFLOW = -2;

    AllSameBlocks() : length(0), mostRecent(-1) {}

    int32_t findOrAdd(int32_t सूचकांक, int32_t count, uint32_t value) {
        अगर (mostRecent >= 0 && मूल्यावली[mostRecent] == value) {
            refCounts[mostRecent] += count;
            वापस indexes[mostRecent];
        }
        क्रम (int32_t i = 0; i < length; ++i) {
            अगर (मूल्यावली[i] == value) {
                mostRecent = i;
                refCounts[i] += count;
                वापस indexes[i];
            }
        }
        अगर (length == CAPACITY) {
            वापस OVERFLOW;
        }
        mostRecent = length;
        indexes[length] = सूचकांक;
        मूल्यावली[length] = value;
        refCounts[length++] = count;
        वापस NEW_UNIQUE;
    }

    /** Replaces the block which has the lowest reference count. */
    व्योम add(int32_t सूचकांक, int32_t count, uint32_t value) {
        U_ASSERT(length == CAPACITY);
        int32_t least = -1;
        int32_t least_गिनती = I_LIMIT;
        क्रम (int32_t i = 0; i < length; ++i) {
            U_ASSERT(मूल्यावली[i] != value);
            अगर (refCounts[i] < least_गिनती) {
                least = i;
                least_गिनती = refCounts[i];
            }
        }
        U_ASSERT(least >= 0);
        mostRecent = least;
        indexes[least] = सूचकांक;
        मूल्यावली[least] = value;
        refCounts[least] = count;
    }

    int32_t findMostUsed() स्थिर {
        अगर (length == 0) { वापस -1; }
        int32_t max = -1;
        int32_t max_गिनती = 0;
        क्रम (int32_t i = 0; i < length; ++i) {
            अगर (refCounts[i] > max_गिनती) {
                max = i;
                max_गिनती = refCounts[i];
            }
        }
        वापस indexes[max];
    }

निजी:
    अटल constexpr int32_t CAPACITY = 32;

    int32_t length;
    int32_t mostRecent;

    int32_t indexes[CAPACITY];
    uint32_t मूल्यावली[CAPACITY];
    int32_t refCounts[CAPACITY];
};

// Custom hash table for mixed-value blocks to be found anywhere in the
// compacted data or index so far.
श्रेणी MixedBlocks {
खुला:
    MixedBlocks() {}
    ~MixedBlocks() {
        uprv_free(table);
    }

    bool init(int32_t max_लंबाई_2, int32_t new_Block_लंबाई) {
        // We store actual data indexes + 1 to reserve 0 for empty entries.
        int32_t max_तथ्य_सूचकांक = max_लंबाई_2 - new_Block_लंबाई + 1;
        int32_t new_लंबाई;
        अगर (max_तथ्य_सूचकांक <= 0xfff) {  // 4k
            new_लंबाई = 6007;
            shift = 12;
            mask = 0xfff;
        } अन्यथा अगर (max_तथ्य_सूचकांक <= 0x7fff) {  // 32k
            new_लंबाई = 50021;
            shift = 15;
            mask = 0x7fff;
        } अन्यथा अगर (max_तथ्य_सूचकांक <= 0x1ffff) {  // 128k
            new_लंबाई = 200003;
            shift = 17;
            mask = 0x1ffff;
        } अन्यथा {
            // maxDataIndex up to around MAX_DATA_LENGTH, ca. 1.1M
            new_लंबाई = 1500007;
            shift = 21;
            mask = 0x1fffff;
        }
        अगर (new_लंबाई > capacity) {
            uprv_free(table);
            table = static_cast<uint32_t*>(uprv_malloc(new_लंबाई * 4));
            अगर (table == nullptr) {
                वापस false;
            }
            capacity = new_लंबाई;
        }
        length = new_लंबाई;
        uprv_memset(table, 0, length * 4);

        block_लंबाई = new_Block_लंबाई;
        वापस true;
    }

    ढाँचा<typename UInt>
    व्योम extend(स्थिर UInt *तथ्य, int32_t minStart, int32_t पूर्व_तथ्य_लंबाई, int32_t new_तथ्य_लंबाई) {
        int32_t start = पूर्व_तथ्य_लंबाई - block_लंबाई;
        अगर (start >= minStart) {
            ++start;  // Skip the last block that we added last time.
        } अन्यथा {
            start = minStart;  // Begin with the first full block.
        }
        क्रम (int32_t end = new_तथ्य_लंबाई - block_लंबाई; start <= end; ++start) {
            uint32_t hashCode = makeHashCode(तथ्य, start);
            addEntry(तथ्य, start, hashCode, start);
        }
    }

    ढाँचा<typename UIntA, typename UIntB>
    int32_t findBlock(स्थिर UIntA *तथ्य, स्थिर UIntB *block_तथ्य, int32_t blockStart) स्थिर {
        uint32_t hashCode = makeHashCode(block_तथ्य, blockStart);
        int32_t entry_सूचकांक = findEntry(तथ्य, block_तथ्य, blockStart, hashCode);
        अगर (entry_सूचकांक >= 0) {
            वापस (table[entry_सूचकांक] & mask) - 1;
        } अन्यथा {
            वापस -1;
        }
    }

    int32_t findAllSameBlock(स्थिर uint32_t *तथ्य, uint32_t block_मूल्य) स्थिर {
        uint32_t hashCode = makeHashCode(block_मूल्य);
        int32_t entry_सूचकांक = findEntry(तथ्य, block_मूल्य, hashCode);
        अगर (entry_सूचकांक >= 0) {
            वापस (table[entry_सूचकांक] & mask) - 1;
        } अन्यथा {
            वापस -1;
        }
    }

निजी:
    ढाँचा<typename UInt>
    uint32_t makeHashCode(स्थिर UInt *block_तथ्य, int32_t blockStart) स्थिर {
        int32_t blockLimit = blockStart + block_लंबाई;
        uint32_t hashCode = block_तथ्य[blockStart++];
        करो {
            hashCode = 37 * hashCode + block_तथ्य[blockStart++];
        } जबतक (blockStart < blockLimit);
        वापस hashCode;
    }

    uint32_t makeHashCode(uint32_t block_मूल्य) स्थिर {
        uint32_t hashCode = block_मूल्य;
        क्रम (int32_t i = 1; i < block_लंबाई; ++i) {
            hashCode = 37 * hashCode + block_मूल्य;
        }
        वापस hashCode;
    }

    ढाँचा<typename UInt>
    व्योम addEntry(स्थिर UInt *तथ्य, int32_t blockStart, uint32_t hashCode, int32_t तथ्य_सूचकांक) {
        U_ASSERT(0 <= तथ्य_सूचकांक && तथ्य_सूचकांक < (int32_t)mask);
        int32_t entry_सूचकांक = findEntry(तथ्य, तथ्य, blockStart, hashCode);
        अगर (entry_सूचकांक < 0) {
            table[~entry_सूचकांक] = (hashCode << shift) | (तथ्य_सूचकांक + 1);
        }
    }

    ढाँचा<typename UIntA, typename UIntB>
    int32_t findEntry(स्थिर UIntA *तथ्य, स्थिर UIntB *block_तथ्य, int32_t blockStart,
                      uint32_t hashCode) स्थिर {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initial_Entry_सूचकांक = (hashCode % (length - 1)) + 1;  // 1..length-1
        क्रम (int32_t entry_सूचकांक = initial_Entry_सूचकांक;;) {
            uint32_t entry = table[entry_सूचकांक];
            अगर (entry == 0) {
                वापस ~entry_सूचकांक;
            }
            अगर ((entry & ~mask) == shiftedHashCode) {
                int32_t तथ्य_सूचकांक = (entry & mask) - 1;
                अगर (equalBlocks(तथ्य + तथ्य_सूचकांक, block_तथ्य + blockStart, block_लंबाई)) {
                    वापस entry_सूचकांक;
                }
            }
            entry_सूचकांक = अग्र_सूचकांक(initial_Entry_सूचकांक, entry_सूचकांक);
        }
    }

    int32_t findEntry(स्थिर uint32_t *तथ्य, uint32_t block_मूल्य, uint32_t hashCode) स्थिर {
        uint32_t shiftedHashCode = hashCode << shift;
        int32_t initial_Entry_सूचकांक = (hashCode % (length - 1)) + 1;  // 1..length-1
        क्रम (int32_t entry_सूचकांक = initial_Entry_सूचकांक;;) {
            uint32_t entry = table[entry_सूचकांक];
            अगर (entry == 0) {
                वापस ~entry_सूचकांक;
            }
            अगर ((entry & ~mask) == shiftedHashCode) {
                int32_t तथ्य_सूचकांक = (entry & mask) - 1;
                अगर (all_मूल्यावली_Same_As(तथ्य + तथ्य_सूचकांक, block_लंबाई, block_मूल्य)) {
                    वापस entry_सूचकांक;
                }
            }
            entry_सूचकांक = अग्र_सूचकांक(initial_Entry_सूचकांक, entry_सूचकांक);
        }
    }

    अंतरभूत int32_t अग्र_सूचकांक(int32_t initial_Entry_सूचकांक, int32_t entry_सूचकांक) स्थिर {
        // U_ASSERT(0 < initialEntryIndex && initialEntryIndex < length);
        वापस (entry_सूचकांक + initial_Entry_सूचकांक) % length;
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

    int32_t block_लंबाई = 0;
};

int32_t MutableCodePointTrie::compact_Whole_तथ्य_Blocks(int32_t fastILimit, AllSameBlocks &allSameBlocks) {
#अगर_घोषित UCPTRIE_DEBUG
    bool overflow = false;
#पूर्ण_अगर

    // ASCII data will be stored as a linear table, even if the following code
    // does not yet count it that way.
    int32_t new_तथ्य_Capacity = ASCII_LIMIT;
    // Add room for a small data null block in case it would match the start of
    // a fast data block where dataNullOffset must not be set in that case.
    new_तथ्य_Capacity += UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
    // Add room for special values (errorValue, highValue) and padding.
    new_तथ्य_Capacity += 4;
    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    int32_t block_लंबाई = UCPTRIE_FAST_तथ्य_BLOCK_लंबाई;
    int32_t inc = SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK;
    क्रम (int32_t i = 0; i < iLimit; i += inc) {
        अगर (i == fastILimit) {
            block_लंबाई = UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
            inc = 1;
        }
        uint32_t value = सूचकांक[i];
        अगर (flags[i] == MIXED) {
            // Really mixed?
            स्थिर uint32_t *p = तथ्य + value;
            value = *p;
            अगर (all_मूल्यावली_Same_As(p + 1, block_लंबाई - 1, value)) {
                flags[i] = ALL_SAME;
                सूचकांक[i] = value;
                // Fall through to ALL_SAME handling.
            } अन्यथा {
                new_तथ्य_Capacity += block_लंबाई;
                जारी;
            }
        } अन्यथा {
            U_ASSERT(flags[i] == ALL_SAME);
            अगर (inc > 1) {
                // Do all of the fast-range data block's ALL_SAME parts have the same value?
                bool allSame = true;
                int32_t अग्र_i = i + inc;
                क्रम (int32_t j = i + 1; j < अग्र_i; ++j) {
                    U_ASSERT(flags[j] == ALL_SAME);
                    अगर (सूचकांक[j] != value) {
                        allSame = false;
                        अवरोध;
                    }
                }
                अगर (!allSame) {
                    // Turn it into a MIXED block.
                    अगर (get_तथ्य_Block(i) < 0) {
                        वापस -1;
                    }
                    new_तथ्य_Capacity += block_लंबाई;
                    जारी;
                }
            }
        }
        // Is there another ALL_SAME block with the same value?
        int32_t other = allSameBlocks.findOrAdd(i, inc, value);
        अगर (other == AllSameBlocks::OVERFLOW) {
            // The fixed-size array overflowed. Slow check for a duplicate block.
#अगर_घोषित UCPTRIE_DEBUG
            अगर (!overflow) {
                माला_दो("UCPTrie AllSameBlocks overflow");
                overflow = true;
            }
#पूर्ण_अगर
            int32_t jInc = SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK;
            क्रम (int32_t j = 0;; j += jInc) {
                अगर (j == i) {
                    allSameBlocks.add(i, inc, value);
                    अवरोध;
                }
                अगर (j == fastILimit) {
                    jInc = 1;
                }
                अगर (flags[j] == ALL_SAME && सूचकांक[j] == value) {
                    allSameBlocks.add(j, jInc + inc, value);
                    other = j;
                    अवरोध;
                    // We could keep counting blocks with the same value
                    // before we add the first one, which may improve compaction in rare cases,
                    // but it would make it slower.
                }
            }
        }
        अगर (other >= 0) {
            flags[i] = SAME_AS;
            सूचकांक[i] = other;
        } अन्यथा {
            // New unique same-value block.
            new_तथ्य_Capacity += block_लंबाई;
        }
    }
    वापस new_तथ्य_Capacity;
}

#अगर_घोषित UCPTRIE_DEBUG
#   define DEBUG_DO(व्यंजक) व्यंजक
#अन्यथा
#   define DEBUG_DO(व्यंजक)
#पूर्ण_अगर

#अगर_घोषित UCPTRIE_DEBUG
// Braille symbols: U+28xx = UTF-8 E2 A0 80..E2 A3 BF
int32_t append_मूल्य(अक्षर s[], int32_t length, uint32_t value) {
    value ^= value >> 16;
    value ^= value >> 8;
    s[length] = 0xE2;
    s[length + 1] = (अक्षर)(0xA0 + ((value >> 6) & 3));
    s[length + 2] = (अक्षर)(0x80 + (value & 0x3F));
    वापस length + 3;
}

व्योम printBlock(स्थिर uint32_t *block, int32_t block_लंबाई, uint32_t value,
                UChar32 start, int32_t overlap, uint32_t initial_मूल्य) {
    अक्षर s[UCPTRIE_FAST_तथ्य_BLOCK_लंबाई * 3 + 3];
    int32_t length = 0;
    int32_t i;
    क्रम (i = 0; i < overlap; ++i) {
        length = append_मूल्य(s, length, 0);  // Braille blank
    }
    s[length++] = '|';
    क्रम (; i < block_लंबाई; ++i) {
        अगर (block != nullptr) {
            value = block[i];
        }
        अगर (value == initial_मूल्य) {
            value = 0x40;  // Braille lower left dot
        }
        length = append_मूल्य(s, length, value);
    }
    s[length] = 0;
    start += overlap;
    अगर (start <= 0xffff) {
        म_लिखो("    %04lX  %s|\n", (दीर्घ)start, s);
    } अन्यथा अगर (start <= 0xfffff) {
        म_लिखो("   %5lX  %s|\n", (दीर्घ)start, s);
    } अन्यथा {
        म_लिखो("  %6lX  %s|\n", (दीर्घ)start, s);
    }
}
#पूर्ण_अगर

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
int32_t MutableCodePointTrie::compact_तथ्य(
        int32_t fastILimit, uint32_t *new_तथ्य, int32_t new_तथ्य_Capacity,
        int32_t तथ्य_Null_सूचकांक, MixedBlocks &mixedBlocks, UErrorCode &errorCode) {
#अगर_घोषित UCPTRIE_DEBUG
    int32_t गिनती_Same=0, योग_Overlaps=0;
    bool print_तथ्य = तथ्य_लंबाई == 29088 /* line.brk */ ||
        // dataLength == 30048 /* CanonIterData */ ||
        तथ्य_लंबाई == 50400 /* zh.txt~stroke */;
#पूर्ण_अगर

    // The linear ASCII data has been copied into newData already.
    int32_t new_तथ्य_लंबाई = 0;
    क्रम (int32_t i = 0; new_तथ्य_लंबाई < ASCII_LIMIT;
            new_तथ्य_लंबाई += UCPTRIE_FAST_तथ्य_BLOCK_लंबाई, i += SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK) {
        सूचकांक[i] = new_तथ्य_लंबाई;
#अगर_घोषित UCPTRIE_DEBUG
        अगर (print_तथ्य) {
            printBlock(new_तथ्य + new_तथ्य_लंबाई, UCPTRIE_FAST_तथ्य_BLOCK_लंबाई, 0, new_तथ्य_लंबाई, 0, initial_मूल्य);
        }
#पूर्ण_अगर
    }

    int32_t block_लंबाई = UCPTRIE_FAST_तथ्य_BLOCK_लंबाई;
    अगर (!mixedBlocks.init(new_तथ्य_Capacity, block_लंबाई)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    mixedBlocks.extend(new_तथ्य, 0, 0, new_तथ्य_लंबाई);

    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    int32_t inc = SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK;
    int32_t fast_लंबाई = 0;
    क्रम (int32_t i = ASCII_I_LIMIT; i < iLimit; i += inc) {
        अगर (i == fastILimit) {
            block_लंबाई = UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
            inc = 1;
            fast_लंबाई = new_तथ्य_लंबाई;
            अगर (!mixedBlocks.init(new_तथ्य_Capacity, block_लंबाई)) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस 0;
            }
            mixedBlocks.extend(new_तथ्य, 0, 0, new_तथ्य_लंबाई);
        }
        अगर (flags[i] == ALL_SAME) {
            uint32_t value = सूचकांक[i];
            // Find an earlier part of the data array of length blockLength
            // that is filled with this value.
            int32_t n = mixedBlocks.findAllSameBlock(new_तथ्य, value);
            // If we find a match, and the current block is the data null block,
            // and it is not a fast block but matches the start of a fast block,
            // then we need to continue looking.
            // This is because this small block is shorter than the fast block,
            // and not all of the rest of the fast block is filled with this value.
            // Otherwise trie.getRange() would detect that the fast block starts at
            // dataNullOffset and assume incorrectly that it is filled with the null value.
            जबतक (n >= 0 && i == तथ्य_Null_सूचकांक && i >= fastILimit && n < fast_लंबाई &&
                    isStartOfSomeFastBlock(n, सूचकांक, fastILimit)) {
                n = findAllSameBlock(new_तथ्य, n + 1, new_तथ्य_लंबाई, value, block_लंबाई);
            }
            अगर (n >= 0) {
                DEBUG_DO(++गिनती_Same);
                सूचकांक[i] = n;
            } अन्यथा {
                n = getAllSameOverlap(new_तथ्य, new_तथ्य_लंबाई, value, block_लंबाई);
                DEBUG_DO(योग_Overlaps += n);
#अगर_घोषित UCPTRIE_DEBUG
                अगर (print_तथ्य) {
                    printBlock(nullptr, block_लंबाई, value, i << UCPTRIE_SHIFT_3, n, initial_मूल्य);
                }
#पूर्ण_अगर
                सूचकांक[i] = new_तथ्य_लंबाई - n;
                int32_t पूर्व_तथ्य_लंबाई = new_तथ्य_लंबाई;
                जबतक (n < block_लंबाई) {
                    new_तथ्य[new_तथ्य_लंबाई++] = value;
                    ++n;
                }
                mixedBlocks.extend(new_तथ्य, 0, पूर्व_तथ्य_लंबाई, new_तथ्य_लंबाई);
            }
        } अन्यथा अगर (flags[i] == MIXED) {
            स्थिर uint32_t *block = तथ्य + सूचकांक[i];
            int32_t n = mixedBlocks.findBlock(new_तथ्य, block, 0);
            अगर (n >= 0) {
                DEBUG_DO(++गिनती_Same);
                सूचकांक[i] = n;
            } अन्यथा {
                n = getOverlap(new_तथ्य, new_तथ्य_लंबाई, block, 0, block_लंबाई);
                DEBUG_DO(योग_Overlaps += n);
#अगर_घोषित UCPTRIE_DEBUG
                अगर (print_तथ्य) {
                    printBlock(block, block_लंबाई, 0, i << UCPTRIE_SHIFT_3, n, initial_मूल्य);
                }
#पूर्ण_अगर
                सूचकांक[i] = new_तथ्य_लंबाई - n;
                int32_t पूर्व_तथ्य_लंबाई = new_तथ्य_लंबाई;
                जबतक (n < block_लंबाई) {
                    new_तथ्य[new_तथ्य_लंबाई++] = block[n++];
                }
                mixedBlocks.extend(new_तथ्य, 0, पूर्व_तथ्य_लंबाई, new_तथ्य_लंबाई);
            }
        } अन्यथा /* SAME_AS */ {
            uint32_t j = सूचकांक[i];
            सूचकांक[i] = सूचकांक[j];
        }
    }

#अगर_घोषित UCPTRIE_DEBUG
    /* we saved some space */
    म_लिखो("compacting UCPTrie: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (दीर्घ)तथ्य_लंबाई, (दीर्घ)new_तथ्य_लंबाई, (दीर्घ)गिनती_Same, (दीर्घ)योग_Overlaps);
#पूर्ण_अगर
    वापस new_तथ्य_लंबाई;
}

int32_t MutableCodePointTrie::compact_सूचकांक(int32_t fastILimit, MixedBlocks &mixedBlocks,
                                           UErrorCode &errorCode) {
    int32_t fast_सूचकांक_लंबाई = fastILimit >> (UCPTRIE_FAST_SHIFT - UCPTRIE_SHIFT_3);
    अगर ((ऊपरी_Start >> UCPTRIE_FAST_SHIFT) <= fast_सूचकांक_लंबाई) {
        // Only the linear fast index, no multi-stage index tables.
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
        वापस fast_सूचकांक_लंबाई;
    }

    // Condense the fast index table.
    // Also, does it contain an index-3 block with all dataNullOffset?
    uint16_t fast_सूचकांक[UCPTRIE_BMP_सूचकांक_लंबाई];  // fastIndexLength
    int32_t i3FirstNull = -1;
    क्रम (int32_t i = 0, j = 0; i < fastILimit; ++j) {
        uint32_t i3 = सूचकांक[i];
        fast_सूचकांक[j] = static_cast<uint16_t>(i3);
        अगर (i3 == static_cast<uint32_t>(तथ्य_Null_Offset)) {
            अगर (i3FirstNull < 0) {
                i3FirstNull = j;
            } अन्यथा अगर (index3NullOffset < 0 &&
                    (j - i3FirstNull + 1) == UCPTRIE_सूचकांक_3_BLOCK_लंबाई) {
                index3NullOffset = i3FirstNull;
            }
        } अन्यथा {
            i3FirstNull = -1;
        }
        // Set the index entries that compactData() skipped.
        // Needed when the multi-stage index covers the fast index range as well.
        int32_t i_अग्र = i + SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK;
        जबतक (++i < i_अग्र) {
            i3 += UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
            सूचकांक[i] = i3;
        }
    }

    अगर (!mixedBlocks.init(fast_सूचकांक_लंबाई, UCPTRIE_सूचकांक_3_BLOCK_लंबाई)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    mixedBlocks.extend(fast_सूचकांक, 0, 0, fast_सूचकांक_लंबाई);

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
    bool hasLongI3Blocks = false;
    // If the fast index covers the whole BMP, then
    // the multi-stage index is only for supplementary code points.
    // Otherwise, the multi-stage index covers all of Unicode.
    int32_t iStart = fastILimit < BMP_I_LIMIT ? 0 : BMP_I_LIMIT;
    int32_t iLimit = ऊपरी_Start >> UCPTRIE_SHIFT_3;
    क्रम (int32_t i = iStart; i < iLimit;) {
        int32_t j = i;
        int32_t jLimit = i + UCPTRIE_सूचकांक_3_BLOCK_लंबाई;
        uint32_t oredI3 = 0;
        bool isNull = true;
        करो {
            uint32_t i3 = सूचकांक[j];
            oredI3 |= i3;
            अगर (i3 != static_cast<uint32_t>(तथ्य_Null_Offset)) {
                isNull = false;
            }
        } जबतक (++j < jLimit);
        अगर (isNull) {
            flags[i] = I3_NULL;
            अगर (i3FirstNull < 0) {
                अगर (oredI3 <= 0xffff) {
                    index3Capacity += UCPTRIE_सूचकांक_3_BLOCK_लंबाई;
                } अन्यथा {
                    index3Capacity += सूचकांक_3_18_BIT_BLOCK_लंबाई;
                    hasLongI3Blocks = true;
                }
                i3FirstNull = 0;
            }
        } अन्यथा {
            अगर (oredI3 <= 0xffff) {
                int32_t n = mixedBlocks.findBlock(fast_सूचकांक, सूचकांक, i);
                अगर (n >= 0) {
                    flags[i] = I3_BMP;
                    सूचकांक[i] = n;
                } अन्यथा {
                    flags[i] = I3_16;
                    index3Capacity += UCPTRIE_सूचकांक_3_BLOCK_लंबाई;
                }
            } अन्यथा {
                flags[i] = I3_18;
                index3Capacity += सूचकांक_3_18_BIT_BLOCK_लंबाई;
                hasLongI3Blocks = true;
            }
        }
        i = j;
    }

    int32_t index2Capacity = (iLimit - iStart) >> UCPTRIE_SHIFT_2_3;

    // Length of the index-1 table, rounded up.
    int32_t index1_लंबाई = (index2Capacity + UCPTRIE_सूचकांक_2_MASK) >> UCPTRIE_SHIFT_1_2;

    // Index table: Fast index, index-1, index-3, index-2.
    // +1 for possible index table padding.
    int32_t index16Capacity = fast_सूचकांक_लंबाई + index1_लंबाई + index3Capacity + index2Capacity + 1;
    index16 = static_cast<uint16_t*>(uprv_malloc(index16Capacity * 2));
    अगर (index16 == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    uprv_memcpy(index16, fast_सूचकांक, fast_सूचकांक_लंबाई * 2);

    अगर (!mixedBlocks.init(index16Capacity, UCPTRIE_सूचकांक_3_BLOCK_लंबाई)) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    MixedBlocks longI3Blocks;
    अगर (hasLongI3Blocks) {
        अगर (!longI3Blocks.init(index16Capacity, सूचकांक_3_18_BIT_BLOCK_लंबाई)) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस 0;
        }
    }

    // Compact the index-3 table and write an uncompacted version of the index-2 table.
    uint16_t index2[UNICODE_LIMIT >> UCPTRIE_SHIFT_2];  // index2Capacity
    int32_t i2_लंबाई = 0;
    i3FirstNull = index3NullOffset;
    int32_t index3Start = fast_सूचकांक_लंबाई + index1_लंबाई;
    int32_t सूचकांक_लंबाई = index3Start;
    क्रम (int32_t i = iStart; i < iLimit; i += UCPTRIE_सूचकांक_3_BLOCK_लंबाई) {
        int32_t i3;
        uint8_t f = flags[i];
        अगर (f == I3_NULL && i3FirstNull < 0) {
            // First index-3 null block. Write & overlap it like a normal block, then remember it.
            f = तथ्य_Null_Offset <= 0xffff ? I3_16 : I3_18;
            i3FirstNull = 0;
        }
        अगर (f == I3_NULL) {
            i3 = index3NullOffset;
        } अन्यथा अगर (f == I3_BMP) {
            i3 = सूचकांक[i];
        } अन्यथा अगर (f == I3_16) {
            int32_t n = mixedBlocks.findBlock(index16, सूचकांक, i);
            अगर (n >= 0) {
                i3 = n;
            } अन्यथा {
                अगर (सूचकांक_लंबाई == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अन्यथा {
                    n = getOverlap(index16, सूचकांक_लंबाई,
                                   सूचकांक, i, UCPTRIE_सूचकांक_3_BLOCK_लंबाई);
                }
                i3 = सूचकांक_लंबाई - n;
                int32_t पूर्व_सूचकांक_लंबाई = सूचकांक_लंबाई;
                जबतक (n < UCPTRIE_सूचकांक_3_BLOCK_लंबाई) {
                    index16[सूचकांक_लंबाई++] = सूचकांक[i + n++];
                }
                mixedBlocks.extend(index16, index3Start, पूर्व_सूचकांक_लंबाई, सूचकांक_लंबाई);
                अगर (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, पूर्व_सूचकांक_लंबाई, सूचकांक_लंबाई);
                }
            }
        } अन्यथा {
            U_ASSERT(f == I3_18);
            U_ASSERT(hasLongI3Blocks);
            // Encode an index-3 block that contains one or more data indexes exceeding 16 bits.
            int32_t j = i;
            int32_t jLimit = i + UCPTRIE_सूचकांक_3_BLOCK_लंबाई;
            int32_t k = सूचकांक_लंबाई;
            करो {
                ++k;
                uint32_t v = सूचकांक[j++];
                uint32_t upperBits = (v & 0x30000) >> 2;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 4;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 6;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 8;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 10;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 12;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 14;
                index16[k++] = v;
                v = सूचकांक[j++];
                upperBits |= (v & 0x30000) >> 16;
                index16[k++] = v;
                index16[k - 9] = upperBits;
            } जबतक (j < jLimit);
            int32_t n = longI3Blocks.findBlock(index16, index16, सूचकांक_लंबाई);
            अगर (n >= 0) {
                i3 = n | 0x8000;
            } अन्यथा {
                अगर (सूचकांक_लंबाई == index3Start) {
                    // No overlap at the boundary between the index-1 and index-3 tables.
                    n = 0;
                } अन्यथा {
                    n = getOverlap(index16, सूचकांक_लंबाई,
                                   index16, सूचकांक_लंबाई, सूचकांक_3_18_BIT_BLOCK_लंबाई);
                }
                i3 = (सूचकांक_लंबाई - n) | 0x8000;
                int32_t पूर्व_सूचकांक_लंबाई = सूचकांक_लंबाई;
                अगर (n > 0) {
                    int32_t start = सूचकांक_लंबाई;
                    जबतक (n < सूचकांक_3_18_BIT_BLOCK_लंबाई) {
                        index16[सूचकांक_लंबाई++] = index16[start + n++];
                    }
                } अन्यथा {
                    सूचकांक_लंबाई += सूचकांक_3_18_BIT_BLOCK_लंबाई;
                }
                mixedBlocks.extend(index16, index3Start, पूर्व_सूचकांक_लंबाई, सूचकांक_लंबाई);
                अगर (hasLongI3Blocks) {
                    longI3Blocks.extend(index16, index3Start, पूर्व_सूचकांक_लंबाई, सूचकांक_लंबाई);
                }
            }
        }
        अगर (index3NullOffset < 0 && i3FirstNull >= 0) {
            index3NullOffset = i3;
        }
        // Set the index-2 table entry.
        index2[i2_लंबाई++] = i3;
    }
    U_ASSERT(i2_लंबाई == index2Capacity);
    U_ASSERT(सूचकांक_लंबाई <= index3Start + index3Capacity);

    अगर (index3NullOffset < 0) {
        index3NullOffset = UCPTRIE_NO_INDEX3_NULL_OFFSET;
    }
    अगर (सूचकांक_लंबाई >= (UCPTRIE_NO_INDEX3_NULL_OFFSET + UCPTRIE_सूचकांक_3_BLOCK_लंबाई)) {
        // The index-3 offsets exceed 15 bits, or
        // the last one cannot be distinguished from the no-null-block value.
        errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    // Compact the index-2 table and write the index-1 table.
    static_assert(UCPTRIE_सूचकांक_2_BLOCK_लंबाई == UCPTRIE_सूचकांक_3_BLOCK_लंबाई,
                  "must re-init mixedBlocks");
    int32_t block_लंबाई = UCPTRIE_सूचकांक_2_BLOCK_लंबाई;
    int32_t i1 = fast_सूचकांक_लंबाई;
    क्रम (int32_t i = 0; i < i2_लंबाई; i += block_लंबाई) {
        int32_t n;
        अगर ((i2_लंबाई - i) >= block_लंबाई) {
            // normal block
            U_ASSERT(block_लंबाई == UCPTRIE_सूचकांक_2_BLOCK_लंबाई);
            n = mixedBlocks.findBlock(index16, index2, i);
        } अन्यथा {
            // highStart is inside the last index-2 block. Shorten it.
            block_लंबाई = i2_लंबाई - i;
            n = findSameBlock(index16, index3Start, सूचकांक_लंबाई,
                              index2, i, block_लंबाई);
        }
        int32_t i2;
        अगर (n >= 0) {
            i2 = n;
        } अन्यथा {
            अगर (सूचकांक_लंबाई == index3Start) {
                // No overlap at the boundary between the index-1 and index-3/2 tables.
                n = 0;
            } अन्यथा {
                n = getOverlap(index16, सूचकांक_लंबाई, index2, i, block_लंबाई);
            }
            i2 = सूचकांक_लंबाई - n;
            int32_t पूर्व_सूचकांक_लंबाई = सूचकांक_लंबाई;
            जबतक (n < block_लंबाई) {
                index16[सूचकांक_लंबाई++] = index2[i + n++];
            }
            mixedBlocks.extend(index16, index3Start, पूर्व_सूचकांक_लंबाई, सूचकांक_लंबाई);
        }
        // Set the index-1 table entry.
        index16[i1++] = i2;
    }
    U_ASSERT(i1 == index3Start);
    U_ASSERT(सूचकांक_लंबाई <= index16Capacity);

#अगर_घोषित UCPTRIE_DEBUG
    /* we saved some space */
    म_लिखो("compacting UCPTrie: count of 16-bit index words %lu->%lu\n",
            (दीर्घ)iLimit, (दीर्घ)सूचकांक_लंबाई);
#पूर्ण_अगर

    वापस सूचकांक_लंबाई;
}

int32_t MutableCodePointTrie::compactTrie(int32_t fastILimit, UErrorCode &errorCode) {
    // Find the real highStart and round it up.
    U_ASSERT((ऊपरी_Start & (UCPTRIE_CP_PER_सूचकांक_2_ENTRY - 1)) == 0);
    ऊपरी_मूल्य = लो(MAX_UNICODE);
    int32_t real_ऊपरी_Start = find_ऊपरी_Start();
    real_ऊपरी_Start = (real_ऊपरी_Start + (UCPTRIE_CP_PER_सूचकांक_2_ENTRY - 1)) &
        ~(UCPTRIE_CP_PER_सूचकांक_2_ENTRY - 1);
    अगर (real_ऊपरी_Start == UNICODE_LIMIT) {
        ऊपरी_मूल्य = initial_मूल्य;
    }

#अगर_घोषित UCPTRIE_DEBUG
    म_लिखो("UCPTrie: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (दीर्घ)real_ऊपरी_Start, (दीर्घ)ऊपरी_मूल्य, (दीर्घ)initial_मूल्य);
#पूर्ण_अगर

    // We always store indexes and data values for the fast range.
    // Pin highStart to the top of that range while building.
    UChar32 fastLimit = fastILimit << UCPTRIE_SHIFT_3;
    अगर (real_ऊपरी_Start < fastLimit) {
        क्रम (int32_t i = (real_ऊपरी_Start >> UCPTRIE_SHIFT_3); i < fastILimit; ++i) {
            flags[i] = ALL_SAME;
            सूचकांक[i] = ऊपरी_मूल्य;
        }
        ऊपरी_Start = fastLimit;
    } अन्यथा {
        ऊपरी_Start = real_ऊपरी_Start;
    }

    uint32_t ascii_तथ्य[ASCII_LIMIT];
    क्रम (int32_t i = 0; i < ASCII_LIMIT; ++i) {
        ascii_तथ्य[i] = लो(i);
    }

    // First we look for which data blocks have the same value repeated over the whole block,
    // deduplicate such blocks, find a good null data block (for faster enumeration),
    // and get an upper bound for the necessary data array length.
    AllSameBlocks allSameBlocks;
    int32_t new_तथ्य_Capacity = compact_Whole_तथ्य_Blocks(fastILimit, allSameBlocks);
    अगर (new_तथ्य_Capacity < 0) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    uint32_t* new_तथ्य = static_cast<uint32_t*>(uprv_malloc(new_तथ्य_Capacity * 4));
    अगर (new_तथ्य == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    uprv_memcpy(new_तथ्य, ascii_तथ्य, माप(ascii_तथ्य));

    int32_t तथ्य_Null_सूचकांक = allSameBlocks.findMostUsed();

    MixedBlocks mixedBlocks;
    int32_t new_तथ्य_लंबाई = compact_तथ्य(fastILimit, new_तथ्य, new_तथ्य_Capacity,
                                        तथ्य_Null_सूचकांक, mixedBlocks, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस 0; }
    U_ASSERT(new_तथ्य_लंबाई <= new_तथ्य_Capacity);
    uprv_free(तथ्य);
    तथ्य = new_तथ्य;
    तथ्य_Capacity = new_तथ्य_Capacity;
    तथ्य_लंबाई = new_तथ्य_लंबाई;
    अगर (तथ्य_लंबाई > (0x3ffff + UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई)) {
        // The offset of the last data block is too high to be stored in the index table.
        errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    अगर (तथ्य_Null_सूचकांक >= 0) {
        तथ्य_Null_Offset = सूचकांक[तथ्य_Null_सूचकांक];
#अगर_घोषित UCPTRIE_DEBUG
        अगर (तथ्य[तथ्य_Null_Offset] != initial_मूल्य) {
            म_लिखो("UCPTrie initialValue %lx -> more common nullValue %lx\n",
                   (दीर्घ)initial_मूल्य, (दीर्घ)तथ्य[तथ्य_Null_Offset]);
        }
#पूर्ण_अगर
        initial_मूल्य = तथ्य[तथ्य_Null_Offset];
    } अन्यथा {
        तथ्य_Null_Offset = UCPTRIE_NO_तथ्य_NULL_OFFSET;
    }

    int32_t सूचकांक_लंबाई = compact_सूचकांक(fastILimit, mixedBlocks, errorCode);
    ऊपरी_Start = real_ऊपरी_Start;
    वापस सूचकांक_लंबाई;
}

UCPTrie *MutableCodePointTrie::build(UCPTrieType type, UCPTrie_मूल्य_Width मूल्य_Width, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    अगर (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मूल्य_Width < UCPTRIE_मूल्य_BITS_16 || UCPTRIE_मूल्य_BITS_8 < मूल्य_Width) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    // The mutable trie always stores 32-bit values.
    // When we build a UCPTrie for a smaller value width, we first mask off unused bits
    // before compacting the data.
    चयन (मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_32:
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_16:
        mask_मूल्यावली(0xffff);
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        mask_मूल्यावली(0xff);
        अवरोध;
    शेष:
        अवरोध;
    }

    UChar32 fastLimit = type == UCPTRIE_TYPE_FAST ? BMP_LIMIT : UCPTRIE_SMALL_LIMIT;
    int32_t सूचकांक_लंबाई = compactTrie(fastLimit >> UCPTRIE_SHIFT_3, errorCode);
    अगर (U_FAILURE(errorCode)) {
        सफा();
        वापस nullptr;
    }

    // Ensure data table alignment: The index length must be even for uint32_t data.
    अगर (मूल्य_Width == UCPTRIE_मूल्य_BITS_32 && (सूचकांक_लंबाई & 1) != 0) {
        index16[सूचकांक_लंबाई++] = 0xffee;  // arbitrary value
    }

    // Make the total trie structure length a multiple of 4 bytes by padding the data table,
    // and store special values as the last two data values.
    int32_t length = सूचकांक_लंबाई * 2;
    अगर (मूल्य_Width == UCPTRIE_मूल्य_BITS_16) {
        अगर (((सूचकांक_लंबाई ^ तथ्य_लंबाई) & 1) != 0) {
            // padding
            तथ्य[तथ्य_लंबाई++] = error_मूल्य;
        }
        अगर (तथ्य[तथ्य_लंबाई - 1] != error_मूल्य || तथ्य[तथ्य_लंबाई - 2] != ऊपरी_मूल्य) {
            तथ्य[तथ्य_लंबाई++] = ऊपरी_मूल्य;
            तथ्य[तथ्य_लंबाई++] = error_मूल्य;
        }
        length += तथ्य_लंबाई * 2;
    } अन्यथा अगर (मूल्य_Width == UCPTRIE_मूल्य_BITS_32) {
        // 32-bit data words never need padding to a multiple of 4 bytes.
        अगर (तथ्य[तथ्य_लंबाई - 1] != error_मूल्य || तथ्य[तथ्य_लंबाई - 2] != ऊपरी_मूल्य) {
            अगर (तथ्य[तथ्य_लंबाई - 1] != ऊपरी_मूल्य) {
                तथ्य[तथ्य_लंबाई++] = ऊपरी_मूल्य;
            }
            तथ्य[तथ्य_लंबाई++] = error_मूल्य;
        }
        length += तथ्य_लंबाई * 4;
    } अन्यथा {
        int32_t and3 = (length + तथ्य_लंबाई) & 3;
        अगर (and3 == 0 && तथ्य[तथ्य_लंबाई - 1] == error_मूल्य && तथ्य[तथ्य_लंबाई - 2] == ऊपरी_मूल्य) {
            // all set
        } अन्यथा अगर(and3 == 3 && तथ्य[तथ्य_लंबाई - 1] == ऊपरी_मूल्य) {
            तथ्य[तथ्य_लंबाई++] = error_मूल्य;
        } अन्यथा {
            जबतक (and3 != 2) {
                तथ्य[तथ्य_लंबाई++] = ऊपरी_मूल्य;
                and3 = (and3 + 1) & 3;
            }
            तथ्य[तथ्य_लंबाई++] = ऊपरी_मूल्य;
            तथ्य[तथ्य_लंबाई++] = error_मूल्य;
        }
        length += तथ्य_लंबाई;
    }

    // Calculate the total length of the UCPTrie as a single memory block.
    length += माप(UCPTrie);
    U_ASSERT((length & 3) == 0);

    uint8_t* bytes = static_cast<uint8_t*>(uprv_malloc(length));
    अगर (bytes == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        सफा();
        वापस nullptr;
    }
    UCPTrie *trie = reinterpret_cast<UCPTrie *>(bytes);
    uprv_memset(trie, 0, माप(UCPTrie));
    trie->सूचकांक_लंबाई = सूचकांक_लंबाई;
    trie->तथ्य_लंबाई = तथ्य_लंबाई;

    trie->ऊपरी_Start = ऊपरी_Start;
    // Round up shifted12HighStart to a multiple of 0x1000 for easy testing from UTF-8 lead bytes.
    // Runtime code needs to then test for the real highStart as well.
    trie->shifted12_ऊपरी_Start = (ऊपरी_Start + 0xfff) >> 12;
    trie->type = type;
    trie->मूल्य_Width = मूल्य_Width;

    trie->index3NullOffset = index3NullOffset;
    trie->तथ्य_Null_Offset = तथ्य_Null_Offset;
    trie->null_मूल्य = initial_मूल्य;

    bytes += माप(UCPTrie);

    // Fill the index and data arrays.
    uint16_t* dest16 = reinterpret_cast<uint16_t*>(bytes);
    trie->सूचकांक = dest16;

    अगर (ऊपरी_Start <= fastLimit) {
        // Condense only the fast index from the mutable-trie index.
        क्रम (int32_t i = 0, j = 0; j < सूचकांक_लंबाई; i += SMALL_तथ्य_BLOCKS_PER_BMP_BLOCK, ++j) {
            *dest16++ = static_cast<uint16_t>(सूचकांक[i]); // dest16[j]
        }
    } अन्यथा {
        uprv_memcpy(dest16, index16, सूचकांक_लंबाई * 2);
        dest16 += सूचकांक_लंबाई;
    }
    bytes += सूचकांक_लंबाई * 2;

    // Write the data array.
    स्थिर uint32_t *p = तथ्य;
    चयन (मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        // Write 16-bit data values.
        trie->तथ्य.ptr16 = dest16;
        क्रम (int32_t i = तथ्य_लंबाई; i > 0; --i) {
            *dest16++ = static_cast<uint16_t>(*p++);
        }
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        // Write 32-bit data values.
        trie->तथ्य.ptr32 = reinterpret_cast<uint32_t*>(bytes);
        uprv_memcpy(bytes, p, (माप_प्रकार)तथ्य_लंबाई * 4);
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        // Write 8-bit data values.
        trie->तथ्य.ptr8 = bytes;
        क्रम (int32_t i = तथ्य_लंबाई; i > 0; --i) {
            *bytes++ = static_cast<uint8_t>(*p++);
        }
        अवरोध;
    शेष:
        // Will not occur, valueWidth checked at the beginning.
        अवरोध;
    }

#अगर_घोषित UCPTRIE_DEBUG
    trie->name = name;

    ucptrie_printLengths(trie, "");
#पूर्ण_अगर

    सफा();
    वापस trie;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_open(uint32_t initial_मूल्य, uint32_t error_मूल्य, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    LocalPointer<MutableCodePointTrie> trie(
        नया MutableCodePointTrie(initial_मूल्य, error_मूल्य, *pErrorCode), *pErrorCode);
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    वापस reinterpret_cast<UMutableCPTrie *>(trie.orphan());
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_clone(स्थिर UMutableCPTrie *other, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर (other == nullptr) {
        वापस nullptr;
    }
    LocalPointer<MutableCodePointTrie> clone(
        नया MutableCodePointTrie(*reinterpret_cast<स्थिर MutableCodePointTrie *>(other), *pErrorCode), *pErrorCode);
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    वापस reinterpret_cast<UMutableCPTrie *>(clone.orphan());
}

U_CAPI व्योम U_EXPORT2
umutablecptrie_close(UMutableCPTrie *trie) {
    मिटाओ reinterpret_cast<MutableCodePointTrie *>(trie);
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPMap(स्थिर UCPMap *map, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर (map == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    वापस reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPMap(map, *pErrorCode));
}

U_CAPI UMutableCPTrie * U_EXPORT2
umutablecptrie_fromUCPTrie(स्थिर UCPTrie *trie, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर (trie == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    वापस reinterpret_cast<UMutableCPTrie *>(MutableCodePointTrie::fromUCPTrie(trie, *pErrorCode));
}

U_CAPI uint32_t U_EXPORT2
umutablecptrie_get(स्थिर UMutableCPTrie *trie, UChar32 c) {
    वापस reinterpret_cast<स्थिर MutableCodePointTrie *>(trie)->लो(c);
}

namespace {

UChar32 getRange(स्थिर व्योम *trie, UChar32 start,
                 UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context, uint32_t *p_मूल्य) {
    वापस reinterpret_cast<स्थिर MutableCodePointTrie *>(trie)->
        getRange(start, filter, context, p_मूल्य);
}

}  // namespace

U_CAPI UChar32 U_EXPORT2
umutablecptrie_getRange(स्थिर UMutableCPTrie *trie, UChar32 start,
                        UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                        UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context, uint32_t *p_मूल्य) {
    वापस ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मूल्य,
                                    filter, context, p_मूल्य);
}

U_CAPI व्योम U_EXPORT2
umutablecptrie_set(UMutableCPTrie *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->set(c, value, *pErrorCode);
}

U_CAPI व्योम U_EXPORT2
umutablecptrie_setRange(UMutableCPTrie *trie, UChar32 start, UChar32 end,
                   uint32_t value, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस;
    }
    reinterpret_cast<MutableCodePointTrie *>(trie)->setRange(start, end, value, *pErrorCode);
}

/* Compact and internally serialize the trie. */
U_CAPI UCPTrie * U_EXPORT2
umutablecptrie_buildImmutable(UMutableCPTrie *trie, UCPTrieType type, UCPTrie_मूल्य_Width मूल्य_Width,
                              UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    वापस reinterpret_cast<MutableCodePointTrie *>(trie)->build(type, मूल्य_Width, *pErrorCode);
}

#अगर_घोषित UCPTRIE_DEBUG
U_CFUNC व्योम umutablecptrie_setName(UMutableCPTrie *trie, स्थिर अक्षर *name) {
    reinterpret_cast<MutableCodePointTrie *>(trie)->name = name;
}
#पूर्ण_अगर
