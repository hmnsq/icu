// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ದ್ವಿಮಾನ(UCPTrieType type,
                       UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width,
                       ಧ್ರುವ ಶೂನ್ಯ* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            ಮೌಲ್ಯ_Width < UCPTRIE_ಮೌಲ್ಯ_BITS_ANY || UCPTRIE_ಮೌಲ್ಯ_BITS_8 < ಮೌಲ್ಯ_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Enough data for a trie header?
    ಯದಿ (length < (int32_t)ಪರಿಮಾಣ(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Check the signature.
    ಧ್ರುವ UCPTrieHeader *header = (ಧ್ರುವ UCPTrieHeader *)data;
    ಯದಿ (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t ಮೌಲ್ಯ_Width_Int = options & UCPTRIE_OPTIONS_ಮೌಲ್ಯ_BITS_MASK;
    ಯದಿ (typeInt > UCPTRIE_TYPE_SMALL || ಮೌಲ್ಯ_Width_Int > UCPTRIE_ಮೌಲ್ಯ_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_ಮೌಲ್ಯ_Width actual_ಮೌಲ್ಯ_Width = (UCPTrie_ಮೌಲ್ಯ_Width)ಮೌಲ್ಯ_Width_Int;
    ಯದಿ (type < 0) {
        type = actualType;
    }
    ಯದಿ (ಮೌಲ್ಯ_Width < 0) {
        ಮೌಲ್ಯ_Width = actual_ಮೌಲ್ಯ_Width;
    }
    ಯದಿ (type != actualType || ಮೌಲ್ಯ_Width != actual_ಮೌಲ್ಯ_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, ಪರಿಮಾಣ(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.ಮೇಲಿನ_Start = header->shifted_ಮೇಲಿನ_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_ಮೇಲಿನ_Start = (tempTrie.ಮೇಲಿನ_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.ಮೌಲ್ಯ_Width = ಮೌಲ್ಯ_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)ಪರಿಮಾಣ(UCPTrieHeader) + tempTrie.indexLength * 2;
    ಯದಿ (ಮೌಲ್ಯ_Width == UCPTRIE_ಮೌಲ್ಯ_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } ಅನ್ಯಥಾ ಯದಿ (ಮೌಲ್ಯ_Width == UCPTRIE_ಮೌಲ್ಯ_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } ಅನ್ಯಥಾ {
        actualLength += tempTrie.dataLength;
    }
    ಯದಿ (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(ಪರಿಮಾಣ(UCPTrie));
    ಯದಿ (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, ಪರಿಮಾಣ(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ಧ್ರುವ uint16_t *p16 = (ಧ್ರುವ uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_ಮೌಲ್ಯ_Offset = trie->dataNullOffset;
    ಯದಿ (null_ಮೌಲ್ಯ_Offset >= trie->dataLength) {
        null_ಮೌಲ್ಯ_Offset = trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET;
    }
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_ಮೌಲ್ಯ = trie->data.ptr16[null_ಮೌಲ್ಯ_Offset];
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        trie->data.ptr32 = (ಧ್ರುವ uint32_t *)p16;
        trie->null_ಮೌಲ್ಯ = trie->data.ptr32[null_ಮೌಲ್ಯ_Offset];
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        trie->data.ptr8 = (ಧ್ರುವ uint8_t *)p16;
        trie->null_ಮೌಲ್ಯ = trie->data.ptr8[null_ಮೌಲ್ಯ_Offset];
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    ಪ್ರತ್ಯಾಗಮ trie;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ಧ್ರುವ UCPTrie *trie) {
    ಪ್ರತ್ಯಾಗಮ (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_ಮೌಲ್ಯ_Width U_EXPORT2
ucptrie_get_ಮೌಲ್ಯ_Width(ಧ್ರುವ UCPTrie *trie) {
    ಪ್ರತ್ಯಾಗಮ (UCPTrie_ಮೌಲ್ಯ_Width)trie->ಮೌಲ್ಯ_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ಧ್ರುವ UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    ಯದಿ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ಮೇಲಿನ_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } ಅನ್ಯಥಾ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ಮೇಲಿನ_Start && trie->ಮೇಲಿನ_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    ಯದಿ ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } ಅನ್ಯಥಾ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    ಪ್ರತ್ಯಾಗಮ dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ಧ್ರುವ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ಯದಿ (c >= trie->ಮೇಲಿನ_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ಪ್ರತ್ಯಾಗಮ trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET;
    }
    ಪ್ರತ್ಯಾಗಮ ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ಧ್ರುವ UCPTrie *trie, UChar32 c,
                            ಧ್ರುವ uint8_t *start, ಧ್ರುವ uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ಯದಿ ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } ಅನ್ಯಥಾ {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    ಪ್ರತ್ಯಾಗಮ (idx << 3) | i;
}

ನಾಮಾಕಾಶ {

ಅನ್ತಃಸ್ಥ uint32_t get_ಮೌಲ್ಯ(UCPTrieData data, UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width, int32_t dataIndex) {
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        ಪ್ರತ್ಯಾಗಮ data.ptr16[dataIndex];
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        ಪ್ರತ್ಯಾಗಮ data.ptr32[dataIndex];
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        ಪ್ರತ್ಯಾಗಮ data.ptr8[dataIndex];
    ಸ್ವಭಾವ:
        // Unreachable if the trie is properly initialized.
        ಪ್ರತ್ಯಾಗಮ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ಧ್ರುವ UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    ಯದಿ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } ಅನ್ಯಥಾ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    ಪ್ರತ್ಯಾಗಮ get_ಮೌಲ್ಯ(trie->data, (UCPTrie_ಮೌಲ್ಯ_Width)trie->ಮೌಲ್ಯ_Width, dataIndex);
}

ನಾಮಾಕಾಶ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

ಅನ್ತಃಸ್ಥ uint32_t maybe_Filter_ಮೌಲ್ಯ(uint32_t value, uint32_t trie_Null_ಮೌಲ್ಯ, uint32_t null_ಮೌಲ್ಯ,
                                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context) {
    ಯದಿ (value == trie_Null_ಮೌಲ್ಯ) {
        value = null_ಮೌಲ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ (filter != nullptr) {
        value = filter(context, value);
    }
    ಪ್ರತ್ಯಾಗಮ value;
}

UChar32 getRange(ಧ್ರುವ ಶೂನ್ಯ *t, UChar32 start,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಯದಿ (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
    ಧ್ರುವ UCPTrie *trie = reinterpret_cast<ಧ್ರುವ UCPTrie *>(t);
    UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width = static_cast<UCPTrie_ಮೌಲ್ಯ_Width>(trie->ಮೌಲ್ಯ_Width);
    ಯದಿ (start >= trie->ಮೇಲಿನ_Start) {
        ಯದಿ (p_ಮೌಲ್ಯ != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET;
            uint32_t value = get_ಮೌಲ್ಯ(trie->data, ಮೌಲ್ಯ_Width, di);
            ಯದಿ (filter != nullptr) { value = filter(context, value); }
            *p_ಮೌಲ್ಯ = value;
        }
        ಪ್ರತ್ಯಾಗಮ MAX_UNICODE;
    }

    uint32_t null_ಮೌಲ್ಯ = trie->null_ಮೌಲ್ಯ;
    ಯದಿ (filter != nullptr) { null_ಮೌಲ್ಯ = filter(context, null_ಮೌಲ್ಯ); }
    ಧ್ರುವ uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_ಮೌಲ್ಯ, value = null_ಮೌಲ್ಯ;
    ತಾರ್ಕಿಕ have_ಮೌಲ್ಯ = ಅಸತ್ಯ;
    ಕುರು {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        ಯದಿ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } ಅನ್ಯಥಾ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            ಯದಿ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ಮೇಲಿನ_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } ಅನ್ಯಥಾ {
                U_ASSERT(c < trie->ಮೇಲಿನ_Start && trie->ಮೇಲಿನ_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            ಯದಿ (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                ಅನುವರ್ತ;
            }
            prevI3Block = i3Block;
            ಯದಿ (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                ಯದಿ (have_ಮೌಲ್ಯ) {
                    ಯದಿ (null_ಮೌಲ್ಯ != value) {
                        ಪ್ರತ್ಯಾಗಮ c - 1;
                    }
                } ಅನ್ಯಥಾ {
                    trie_ಮೌಲ್ಯ = trie->null_ಮೌಲ್ಯ;
                    value = null_ಮೌಲ್ಯ;
                    ಯದಿ (p_ಮೌಲ್ಯ != nullptr) { *p_ಮೌಲ್ಯ = null_ಮೌಲ್ಯ; }
                    have_ಮೌಲ್ಯ = ಸತ್ಯ;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                ಅನುವರ್ತ;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        ಕುರು {
            int32_t block;
            ಯದಿ ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } ಅನ್ಯಥಾ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            ಯದಿ (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } ಅನ್ಯಥಾ {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                ಯದಿ (block == trie->dataNullOffset) {
                    // This is the data null block.
                    ಯದಿ (have_ಮೌಲ್ಯ) {
                        ಯದಿ (null_ಮೌಲ್ಯ != value) {
                            ಪ್ರತ್ಯಾಗಮ c - 1;
                        }
                    } ಅನ್ಯಥಾ {
                        trie_ಮೌಲ್ಯ = trie->null_ಮೌಲ್ಯ;
                        value = null_ಮೌಲ್ಯ;
                        ಯದಿ (p_ಮೌಲ್ಯ != nullptr) { *p_ಮೌಲ್ಯ = null_ಮೌಲ್ಯ; }
                        have_ಮೌಲ್ಯ = ಸತ್ಯ;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } ಅನ್ಯಥಾ {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_ಮೌಲ್ಯ(trie->data, ಮೌಲ್ಯ_Width, di);
                    ಯದಿ (have_ಮೌಲ್ಯ) {
                        ಯದಿ (trieValue2 != trie_ಮೌಲ್ಯ) {
                            ಯದಿ (filter == nullptr ||
                                    maybe_Filter_ಮೌಲ್ಯ(trieValue2, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                                     filter, context) != value) {
                                ಪ್ರತ್ಯಾಗಮ c - 1;
                            }
                            trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                        }
                    } ಅನ್ಯಥಾ {
                        trie_ಮೌಲ್ಯ = trieValue2;
                        value = maybe_Filter_ಮೌಲ್ಯ(trieValue2, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                                 filter, context);
                        ಯದಿ (p_ಮೌಲ್ಯ != nullptr) { *p_ಮೌಲ್ಯ = value; }
                        have_ಮೌಲ್ಯ = ಸತ್ಯ;
                    }
                    ಯಾವತ್ ((++c & dataMask) != 0) {
                        trieValue2 = get_ಮೌಲ್ಯ(trie->data, ಮೌಲ್ಯ_Width, ++di);
                        ಯದಿ (trieValue2 != trie_ಮೌಲ್ಯ) {
                            ಯದಿ (filter == nullptr ||
                                    maybe_Filter_ಮೌಲ್ಯ(trieValue2, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                                                     filter, context) != value) {
                                ಪ್ರತ್ಯಾಗಮ c - 1;
                            }
                            trie_ಮೌಲ್ಯ = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ಯಾವತ್ (++i3 < i3BlockLength);
    } ಯಾವತ್ (c < trie->ಮೇಲಿನ_Start);
    U_ASSERT(have_ಮೌಲ್ಯ);
    int32_t di = trie->dataLength - UCPTRIE_ಮೇಲಿನ_ಮೌಲ್ಯ_NEG_DATA_OFFSET;
    uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ = get_ಮೌಲ್ಯ(trie->data, ಮೌಲ್ಯ_Width, di);
    ಯದಿ (maybe_Filter_ಮೌಲ್ಯ(ಮೇಲಿನ_ಮೌಲ್ಯ, trie->null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ,
                         filter, context) != value) {
        ಪ್ರತ್ಯಾಗಮ c - 1;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ಧ್ರುವ ಶೂನ್ಯ *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                         UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಯದಿ (option == UCPMAP_RANGE_NORMAL) {
        ಪ್ರತ್ಯಾಗಮ getRange(trie, start, filter, context, p_ಮೌಲ್ಯ);
    }
    uint32_t value;
    ಯದಿ (p_ಮೌಲ್ಯ == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_ಮೌಲ್ಯ = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_ಮೌಲ್ಯ);
    ಯದಿ (end < 0xd7ff || start > surrEnd) {
        ಪ್ರತ್ಯಾಗಮ end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ಯದಿ (*p_ಮೌಲ್ಯ == surrogate_ಮೌಲ್ಯ) {
        ಯದಿ (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ಪ್ರತ್ಯಾಗಮ end;
        }
    } ಅನ್ಯಥಾ {
        ಯದಿ (start <= 0xd7ff) {
            ಪ್ರತ್ಯಾಗಮ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_ಮೌಲ್ಯ = surrogate_ಮೌಲ್ಯ;
        ಯದಿ (end > surrEnd) {
            ಪ್ರತ್ಯಾಗಮ surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    ಯದಿ (value2 == surrogate_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ end2;
    }
    ಪ್ರತ್ಯಾಗಮ surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ಧ್ರುವ UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                 UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಪ್ರತ್ಯಾಗಮ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_ಮೌಲ್ಯ,
                                    filter, context, p_ಮೌಲ್ಯ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_ದ್ವಿಮಾನ(ಧ್ರುವ UCPTrie *trie,
                 ಶೂನ್ಯ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width = (UCPTrie_ಮೌಲ್ಯ_Width)trie->ಮೌಲ್ಯ_Width;
    ಯದಿ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            ಮೌಲ್ಯ_Width < UCPTRIE_ಮೌಲ್ಯ_BITS_16 || UCPTRIE_ಮೌಲ್ಯ_BITS_8 < ಮೌಲ್ಯ_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    int32_t length = (int32_t)ಪರಿಮಾಣ(UCPTrieHeader) + trie->indexLength * 2;
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        length += trie->dataLength * 2;
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        length += trie->dataLength * 4;
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        length += trie->dataLength;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // unreachable
        ಭಙ್ಗ;
    }
    ಯದಿ (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ length;
    }

    ವರ್ಣ *bytes = (ವರ್ಣ *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        ಮೌಲ್ಯ_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_ಮೇಲಿನ_Start = trie->ಮೇಲಿನ_Start >> UCPTRIE_SHIFT_2;
    bytes += ಪರಿಮಾಣ(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    ವಿಕಲ್ಪ (ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // unreachable
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ನಾಮಾಕಾಶ {

#ifdef UCPTRIE_DEBUG
long ಎಣಿಕೆ_Null(ಧ್ರುವ UCPTrie *trie) {
    uint32_t null_ಮೌಲ್ಯ=trie->null_ಮೌಲ್ಯ;
    int32_t length=trie->dataLength;
    long count=0;
    ವಿಕಲ್ಪ (trie->ಮೌಲ್ಯ_Width) {
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_16:
        ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
            ಯದಿ(trie->data.ptr16[i]==null_ಮೌಲ್ಯ) { ++count; }
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_32:
        ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
            ಯದಿ(trie->data.ptr32[i]==null_ಮೌಲ್ಯ) { ++count; }
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UCPTRIE_ಮೌಲ್ಯ_BITS_8:
        ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
            ಯದಿ(trie->data.ptr8[i]==null_ಮೌಲ್ಯ) { ++count; }
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // unreachable
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ count;
}

U_CFUNC ಶೂನ್ಯ
ucptrie_printLengths(ಧ್ರುವ UCPTrie *trie, ಧ್ರುವ ವರ್ಣ *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)ಪರಿಮಾಣ(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->ಮೌಲ್ಯ_Width==UCPTRIE_ಮೌಲ್ಯ_BITS_16 ? 2 :
                        trie->ಮೌಲ್ಯ_Width==UCPTRIE_ಮೌಲ್ಯ_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, ಎಣಿಕೆ_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ಧ್ರುವ UCPMap *map, UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ ucptrie_get(reinterpret_cast<ಧ್ರುವ UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ಧ್ರುವ UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_ಮೌಲ್ಯ,
                UCPMap_ಮೌಲ್ಯ_Filter *filter, ಧ್ರುವ ಶೂನ್ಯ *context, uint32_t *p_ಮೌಲ್ಯ) {
    ಪ್ರತ್ಯಾಗಮ ucptrie_getRange(reinterpret_cast<ಧ್ರುವ UCPTrie *>(map), start,
                            option, surrogate_ಮೌಲ್ಯ,
                            filter, context, p_ಮೌಲ್ಯ);
}
