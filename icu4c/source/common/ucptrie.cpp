// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ଦ୍ୱିମିକ(UCPTrieType type,
                       UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width,
                       ଅଚଳ ଶୂନ୍ୟ* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            ମୂଲ୍ୟ_Width < UCPTRIE_ମୂଲ୍ୟ_BITS_ANY || UCPTRIE_ମୂଲ୍ୟ_BITS_8 < ମୂଲ୍ୟ_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    // Enough data for a trie header?
    ଯଦି (length < (int32_t)ଆକାର(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    // Check the signature.
    ଅଚଳ UCPTrieHeader *header = (ଅଚଳ UCPTrieHeader *)data;
    ଯଦି (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t ମୂଲ୍ୟ_Width_Int = options & UCPTRIE_OPTIONS_ମୂଲ୍ୟ_BITS_MASK;
    ଯଦି (typeInt > UCPTRIE_TYPE_SMALL || ମୂଲ୍ୟ_Width_Int > UCPTRIE_ମୂଲ୍ୟ_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_ମୂଲ୍ୟ_Width actual_ମୂଲ୍ୟ_Width = (UCPTrie_ମୂଲ୍ୟ_Width)ମୂଲ୍ୟ_Width_Int;
    ଯଦି (type < 0) {
        type = actualType;
    }
    ଯଦି (ମୂଲ୍ୟ_Width < 0) {
        ମୂଲ୍ୟ_Width = actual_ମୂଲ୍ୟ_Width;
    }
    ଯଦି (type != actualType || ମୂଲ୍ୟ_Width != actual_ମୂଲ୍ୟ_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, ଆକାର(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.ଉପର_Start = header->shifted_ଉପର_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_ଉପର_Start = (tempTrie.ଉପର_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.ମୂଲ୍ୟ_Width = ମୂଲ୍ୟ_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)ଆକାର(UCPTrieHeader) + tempTrie.indexLength * 2;
    ଯଦି (ମୂଲ୍ୟ_Width == UCPTRIE_ମୂଲ୍ୟ_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } ଅନ୍ୟଥା ଯଦି (ମୂଲ୍ୟ_Width == UCPTRIE_ମୂଲ୍ୟ_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } ଅନ୍ୟଥା {
        actualLength += tempTrie.dataLength;
    }
    ଯଦି (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ଫେରନ୍ତୁ nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(ଆକାର(UCPTrie));
    ଯଦି (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, ଆକାର(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ଅଚଳ uint16_t *p16 = (ଅଚଳ uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_ମୂଲ୍ୟ_Offset = trie->dataNullOffset;
    ଯଦି (null_ମୂଲ୍ୟ_Offset >= trie->dataLength) {
        null_ମୂଲ୍ୟ_Offset = trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET;
    }
    ବାଛ (ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_ମୂଲ୍ୟ = trie->data.ptr16[null_ମୂଲ୍ୟ_Offset];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        trie->data.ptr32 = (ଅଚଳ uint32_t *)p16;
        trie->null_ମୂଲ୍ୟ = trie->data.ptr32[null_ମୂଲ୍ୟ_Offset];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        trie->data.ptr8 = (ଅଚଳ uint8_t *)p16;
        trie->null_ମୂଲ୍ୟ = trie->data.ptr8[null_ମୂଲ୍ୟ_Offset];
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    ଫେରନ୍ତୁ trie;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ଅଚଳ UCPTrie *trie) {
    ଫେରନ୍ତୁ (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_ମୂଲ୍ୟ_Width U_EXPORT2
ucptrie_get_ମୂଲ୍ୟ_Width(ଅଚଳ UCPTrie *trie) {
    ଫେରନ୍ତୁ (UCPTrie_ମୂଲ୍ୟ_Width)trie->ମୂଲ୍ୟ_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ଅଚଳ UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    ଯଦି (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ଉପର_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } ଅନ୍ୟଥା {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ଉପର_Start && trie->ଉପର_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    ଯଦି ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } ଅନ୍ୟଥା {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    ଫେରନ୍ତୁ dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ଅଚଳ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ଯଦି (c >= trie->ଉପର_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ଫେରନ୍ତୁ trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET;
    }
    ଫେରନ୍ତୁ ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ଅଚଳ UCPTrie *trie, UChar32 c,
                            ଅଚଳ uint8_t *start, ଅଚଳ uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ଯଦି ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } ଅନ୍ୟଥା {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    ଫେରନ୍ତୁ (idx << 3) | i;
}

ନାମାକାଶ {

ଧାଡ଼ିଭିତରେ uint32_t get_ମୂଲ୍ୟ(UCPTrieData data, UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width, int32_t dataIndex) {
    ବାଛ (ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        ଫେରନ୍ତୁ data.ptr16[dataIndex];
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        ଫେରନ୍ତୁ data.ptr32[dataIndex];
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        ଫେରନ୍ତୁ data.ptr8[dataIndex];
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // Unreachable if the trie is properly initialized.
        ଫେରନ୍ତୁ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ଅଚଳ UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    ଯଦି ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } ଅନ୍ୟଥା {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    ଫେରନ୍ତୁ get_ମୂଲ୍ୟ(trie->data, (UCPTrie_ମୂଲ୍ୟ_Width)trie->ମୂଲ୍ୟ_Width, dataIndex);
}

ନାମାକାଶ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

ଧାଡ଼ିଭିତରେ uint32_t maybe_Filter_ମୂଲ୍ୟ(uint32_t value, uint32_t trie_Null_ମୂଲ୍ୟ, uint32_t null_ମୂଲ୍ୟ,
                                 UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context) {
    ଯଦି (value == trie_Null_ମୂଲ୍ୟ) {
        value = null_ମୂଲ୍ୟ;
    } ଅନ୍ୟଥା ଯଦି (filter != nullptr) {
        value = filter(context, value);
    }
    ଫେରନ୍ତୁ value;
}

UChar32 getRange(ଅଚଳ ଶୂନ୍ୟ *t, UChar32 start,
                 UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context, uint32_t *p_ମୂଲ୍ୟ) {
    ଯଦି (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
    ଅଚଳ UCPTrie *trie = reinterpret_cast<ଅଚଳ UCPTrie *>(t);
    UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width = static_cast<UCPTrie_ମୂଲ୍ୟ_Width>(trie->ମୂଲ୍ୟ_Width);
    ଯଦି (start >= trie->ଉପର_Start) {
        ଯଦି (p_ମୂଲ୍ୟ != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET;
            uint32_t value = get_ମୂଲ୍ୟ(trie->data, ମୂଲ୍ୟ_Width, di);
            ଯଦି (filter != nullptr) { value = filter(context, value); }
            *p_ମୂଲ୍ୟ = value;
        }
        ଫେରନ୍ତୁ MAX_UNICODE;
    }

    uint32_t null_ମୂଲ୍ୟ = trie->null_ମୂଲ୍ୟ;
    ଯଦି (filter != nullptr) { null_ମୂଲ୍ୟ = filter(context, null_ମୂଲ୍ୟ); }
    ଅଚଳ uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_ମୂଲ୍ୟ, value = null_ମୂଲ୍ୟ;
    ତାର୍କିକ have_ମୂଲ୍ୟ = ମିଥ୍ୟା;
    କର {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        ଯଦି (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } ଅନ୍ୟଥା {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            ଯଦି (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ଉପର_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } ଅନ୍ୟଥା {
                U_ASSERT(c < trie->ଉପର_Start && trie->ଉପର_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            ଯଦି (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                ଜାରି;
            }
            prevI3Block = i3Block;
            ଯଦି (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                ଯଦି (have_ମୂଲ୍ୟ) {
                    ଯଦି (null_ମୂଲ୍ୟ != value) {
                        ଫେରନ୍ତୁ c - 1;
                    }
                } ଅନ୍ୟଥା {
                    trie_ମୂଲ୍ୟ = trie->null_ମୂଲ୍ୟ;
                    value = null_ମୂଲ୍ୟ;
                    ଯଦି (p_ମୂଲ୍ୟ != nullptr) { *p_ମୂଲ୍ୟ = null_ମୂଲ୍ୟ; }
                    have_ମୂଲ୍ୟ = ସତ୍ୟ;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                ଜାରି;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        କର {
            int32_t block;
            ଯଦି ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } ଅନ୍ୟଥା {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            ଯଦି (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } ଅନ୍ୟଥା {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                ଯଦି (block == trie->dataNullOffset) {
                    // This is the data null block.
                    ଯଦି (have_ମୂଲ୍ୟ) {
                        ଯଦି (null_ମୂଲ୍ୟ != value) {
                            ଫେରନ୍ତୁ c - 1;
                        }
                    } ଅନ୍ୟଥା {
                        trie_ମୂଲ୍ୟ = trie->null_ମୂଲ୍ୟ;
                        value = null_ମୂଲ୍ୟ;
                        ଯଦି (p_ମୂଲ୍ୟ != nullptr) { *p_ମୂଲ୍ୟ = null_ମୂଲ୍ୟ; }
                        have_ମୂଲ୍ୟ = ସତ୍ୟ;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } ଅନ୍ୟଥା {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_ମୂଲ୍ୟ(trie->data, ମୂଲ୍ୟ_Width, di);
                    ଯଦି (have_ମୂଲ୍ୟ) {
                        ଯଦି (trieValue2 != trie_ମୂଲ୍ୟ) {
                            ଯଦି (filter == nullptr ||
                                    maybe_Filter_ମୂଲ୍ୟ(trieValue2, trie->null_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                                                     filter, context) != value) {
                                ଫେରନ୍ତୁ c - 1;
                            }
                            trie_ମୂଲ୍ୟ = trieValue2;  // may or may not help
                        }
                    } ଅନ୍ୟଥା {
                        trie_ମୂଲ୍ୟ = trieValue2;
                        value = maybe_Filter_ମୂଲ୍ୟ(trieValue2, trie->null_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                                                 filter, context);
                        ଯଦି (p_ମୂଲ୍ୟ != nullptr) { *p_ମୂଲ୍ୟ = value; }
                        have_ମୂଲ୍ୟ = ସତ୍ୟ;
                    }
                    ଯେତେବେଳେ ((++c & dataMask) != 0) {
                        trieValue2 = get_ମୂଲ୍ୟ(trie->data, ମୂଲ୍ୟ_Width, ++di);
                        ଯଦି (trieValue2 != trie_ମୂଲ୍ୟ) {
                            ଯଦି (filter == nullptr ||
                                    maybe_Filter_ମୂଲ୍ୟ(trieValue2, trie->null_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                                                     filter, context) != value) {
                                ଫେରନ୍ତୁ c - 1;
                            }
                            trie_ମୂଲ୍ୟ = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ଯେତେବେଳେ (++i3 < i3BlockLength);
    } ଯେତେବେଳେ (c < trie->ଉପର_Start);
    U_ASSERT(have_ମୂଲ୍ୟ);
    int32_t di = trie->dataLength - UCPTRIE_ଉପର_ମୂଲ୍ୟ_NEG_DATA_OFFSET;
    uint32_t ଉପର_ମୂଲ୍ୟ = get_ମୂଲ୍ୟ(trie->data, ମୂଲ୍ୟ_Width, di);
    ଯଦି (maybe_Filter_ମୂଲ୍ୟ(ଉପର_ମୂଲ୍ୟ, trie->null_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ,
                         filter, context) != value) {
        ଫେରନ୍ତୁ c - 1;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ଅଚଳ ଶୂନ୍ୟ *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_ମୂଲ୍ୟ,
                         UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context, uint32_t *p_ମୂଲ୍ୟ) {
    ଯଦି (option == UCPMAP_RANGE_NORMAL) {
        ଫେରନ୍ତୁ getRange(trie, start, filter, context, p_ମୂଲ୍ୟ);
    }
    uint32_t value;
    ଯଦି (p_ମୂଲ୍ୟ == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_ମୂଲ୍ୟ = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_ମୂଲ୍ୟ);
    ଯଦି (end < 0xd7ff || start > surrEnd) {
        ଫେରନ୍ତୁ end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ଯଦି (*p_ମୂଲ୍ୟ == surrogate_ମୂଲ୍ୟ) {
        ଯଦି (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ଫେରନ୍ତୁ end;
        }
    } ଅନ୍ୟଥା {
        ଯଦି (start <= 0xd7ff) {
            ଫେରନ୍ତୁ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_ମୂଲ୍ୟ = surrogate_ମୂଲ୍ୟ;
        ଯଦି (end > surrEnd) {
            ଫେରନ୍ତୁ surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    ଯଦି (value2 == surrogate_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ end2;
    }
    ଫେରନ୍ତୁ surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ଅଚଳ UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_ମୂଲ୍ୟ,
                 UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context, uint32_t *p_ମୂଲ୍ୟ) {
    ଫେରନ୍ତୁ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_ମୂଲ୍ୟ,
                                    filter, context, p_ମୂଲ୍ୟ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_ଦ୍ୱିମିକ(ଅଚଳ UCPTrie *trie,
                 ଶୂନ୍ୟ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width = (UCPTrie_ମୂଲ୍ୟ_Width)trie->ମୂଲ୍ୟ_Width;
    ଯଦି (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ମୂଲ୍ୟ_Width < UCPTRIE_ମୂଲ୍ୟ_BITS_16 || UCPTRIE_ମୂଲ୍ୟ_BITS_8 < ମୂଲ୍ୟ_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    int32_t length = (int32_t)ଆକାର(UCPTrieHeader) + trie->indexLength * 2;
    ବାଛ (ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        length += trie->dataLength * 2;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        length += trie->dataLength * 4;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        length += trie->dataLength;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // unreachable
        ଭାଙ୍ଗ;
    }
    ଯଦି (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ length;
    }

    ଅକ୍ଷର *bytes = (ଅକ୍ଷର *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        ମୂଲ୍ୟ_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_ଉପର_Start = trie->ଉପର_Start >> UCPTRIE_SHIFT_2;
    bytes += ଆକାର(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    ବାଛ (ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // unreachable
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ length;
}

ନାମାକାଶ {

#ifdef UCPTRIE_DEBUG
long ଗଣନା_Null(ଅଚଳ UCPTrie *trie) {
    uint32_t null_ମୂଲ୍ୟ=trie->null_ମୂଲ୍ୟ;
    int32_t length=trie->dataLength;
    long count=0;
    ବାଛ (trie->ମୂଲ୍ୟ_Width) {
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_16:
        ପାଇଁ(int32_t i=0; i<length; ++i) {
            ଯଦି(trie->data.ptr16[i]==null_ମୂଲ୍ୟ) { ++count; }
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_32:
        ପାଇଁ(int32_t i=0; i<length; ++i) {
            ଯଦି(trie->data.ptr32[i]==null_ମୂଲ୍ୟ) { ++count; }
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCPTRIE_ମୂଲ୍ୟ_BITS_8:
        ପାଇଁ(int32_t i=0; i<length; ++i) {
            ଯଦି(trie->data.ptr8[i]==null_ମୂଲ୍ୟ) { ++count; }
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // unreachable
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ count;
}

U_CFUNC ଶୂନ୍ୟ
ucptrie_printLengths(ଅଚଳ UCPTrie *trie, ଅଚଳ ଅକ୍ଷର *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)ଆକାର(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->ମୂଲ୍ୟ_Width==UCPTRIE_ମୂଲ୍ୟ_BITS_16 ? 2 :
                        trie->ମୂଲ୍ୟ_Width==UCPTRIE_ମୂଲ୍ୟ_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, ଗଣନା_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ଅଚଳ UCPMap *map, UChar32 c) {
    ଫେରନ୍ତୁ ucptrie_get(reinterpret_cast<ଅଚଳ UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ଅଚଳ UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_ମୂଲ୍ୟ,
                UCPMap_ମୂଲ୍ୟ_Filter *filter, ଅଚଳ ଶୂନ୍ୟ *context, uint32_t *p_ମୂଲ୍ୟ) {
    ଫେରନ୍ତୁ ucptrie_getRange(reinterpret_cast<ଅଚଳ UCPTrie *>(map), start,
                            option, surrogate_ମୂଲ୍ୟ,
                            filter, context, p_ମୂଲ୍ୟ);
}
