// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ദ്വിമാന(UCPTrieType type,
                       UCPTrie_മൂല്യം_Width മൂല്യം_Width,
                       സ്ഥിരം ശൂന്യം* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            മൂല്യം_Width < UCPTRIE_മൂല്യം_BITS_ANY || UCPTRIE_മൂല്യം_BITS_8 < മൂല്യം_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    // Enough data for a trie header?
    എങ്കിൽ (length < (int32_t)വലുപ്പം(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    // Check the signature.
    സ്ഥിരം UCPTrieHeader *header = (സ്ഥിരം UCPTrieHeader *)data;
    എങ്കിൽ (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t മൂല്യം_Width_Int = options & UCPTRIE_OPTIONS_മൂല്യം_BITS_MASK;
    എങ്കിൽ (typeInt > UCPTRIE_TYPE_SMALL || മൂല്യം_Width_Int > UCPTRIE_മൂല്യം_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_മൂല്യം_Width actual_മൂല്യം_Width = (UCPTrie_മൂല്യം_Width)മൂല്യം_Width_Int;
    എങ്കിൽ (type < 0) {
        type = actualType;
    }
    എങ്കിൽ (മൂല്യം_Width < 0) {
        മൂല്യം_Width = actual_മൂല്യം_Width;
    }
    എങ്കിൽ (type != actualType || മൂല്യം_Width != actual_മൂല്യം_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, വലുപ്പം(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.മുകളിലെ_Start = header->shifted_മുകളിലെ_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_മുകളിലെ_Start = (tempTrie.മുകളിലെ_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.മൂല്യം_Width = മൂല്യം_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)വലുപ്പം(UCPTrieHeader) + tempTrie.indexLength * 2;
    എങ്കിൽ (മൂല്യം_Width == UCPTRIE_മൂല്യം_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } അല്ലെങ്കിൽ എങ്കിൽ (മൂല്യം_Width == UCPTRIE_മൂല്യം_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } അല്ലെങ്കിൽ {
        actualLength += tempTrie.dataLength;
    }
    എങ്കിൽ (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        തിരികെ nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(വലുപ്പം(UCPTrie));
    എങ്കിൽ (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, വലുപ്പം(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    സ്ഥിരം uint16_t *p16 = (സ്ഥിരം uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_മൂല്യം_Offset = trie->dataNullOffset;
    എങ്കിൽ (null_മൂല്യം_Offset >= trie->dataLength) {
        null_മൂല്യം_Offset = trie->dataLength - UCPTRIE_മുകളിലെ_മൂല്യം_NEG_DATA_OFFSET;
    }
    തിരഞ്ഞെടുപ്പ് (മൂല്യം_Width) {
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_മൂല്യം = trie->data.ptr16[null_മൂല്യം_Offset];
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
        trie->data.ptr32 = (സ്ഥിരം uint32_t *)p16;
        trie->null_മൂല്യം = trie->data.ptr32[null_മൂല്യം_Offset];
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
        trie->data.ptr8 = (സ്ഥിരം uint8_t *)p16;
        trie->null_മൂല്യം = trie->data.ptr8[null_മൂല്യം_Offset];
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    എങ്കിൽ (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    തിരികെ trie;
}

U_CAPI ശൂന്യം U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(സ്ഥിരം UCPTrie *trie) {
    തിരികെ (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_മൂല്യം_Width U_EXPORT2
ucptrie_get_മൂല്യം_Width(സ്ഥിരം UCPTrie *trie) {
    തിരികെ (UCPTrie_മൂല്യം_Width)trie->മൂല്യം_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(സ്ഥിരം UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    എങ്കിൽ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->മുകളിലെ_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } അല്ലെങ്കിൽ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->മുകളിലെ_Start && trie->മുകളിലെ_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    എങ്കിൽ ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } അല്ലെങ്കിൽ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    തിരികെ dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(സ്ഥിരം UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    എങ്കിൽ (c >= trie->മുകളിലെ_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        തിരികെ trie->dataLength - UCPTRIE_മുകളിലെ_മൂല്യം_NEG_DATA_OFFSET;
    }
    തിരികെ ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(സ്ഥിരം UCPTrie *trie, UChar32 c,
                            സ്ഥിരം uint8_t *start, സ്ഥിരം uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    എങ്കിൽ ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } അല്ലെങ്കിൽ {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    തിരികെ (idx << 3) | i;
}

നാമാകാശ {

വരിയിൽ uint32_t get_മൂല്യം(UCPTrieData data, UCPTrie_മൂല്യം_Width മൂല്യം_Width, int32_t dataIndex) {
    തിരഞ്ഞെടുപ്പ് (മൂല്യം_Width) {
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
        തിരികെ data.ptr16[dataIndex];
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
        തിരികെ data.ptr32[dataIndex];
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
        തിരികെ data.ptr8[dataIndex];
    സ്ഥിരസ്ഥിതി:
        // Unreachable if the trie is properly initialized.
        തിരികെ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(സ്ഥിരം UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    എങ്കിൽ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } അല്ലെങ്കിൽ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    തിരികെ get_മൂല്യം(trie->data, (UCPTrie_മൂല്യം_Width)trie->മൂല്യം_Width, dataIndex);
}

നാമാകാശ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

വരിയിൽ uint32_t maybe_Filter_മൂല്യം(uint32_t value, uint32_t trie_Null_മൂല്യം, uint32_t null_മൂല്യം,
                                 UCPMap_മൂല്യം_Filter *filter, സ്ഥിരം ശൂന്യം *context) {
    എങ്കിൽ (value == trie_Null_മൂല്യം) {
        value = null_മൂല്യം;
    } അല്ലെങ്കിൽ എങ്കിൽ (filter != nullptr) {
        value = filter(context, value);
    }
    തിരികെ value;
}

UChar32 getRange(സ്ഥിരം ശൂന്യം *t, UChar32 start,
                 UCPMap_മൂല്യം_Filter *filter, സ്ഥിരം ശൂന്യം *context, uint32_t *p_മൂല്യം) {
    എങ്കിൽ (static_cast<uint32_t>(start) > MAX_UNICODE) {
        തിരികെ U_SENTINEL;
    }
    സ്ഥിരം UCPTrie *trie = reinterpret_cast<സ്ഥിരം UCPTrie *>(t);
    UCPTrie_മൂല്യം_Width മൂല്യം_Width = static_cast<UCPTrie_മൂല്യം_Width>(trie->മൂല്യം_Width);
    എങ്കിൽ (start >= trie->മുകളിലെ_Start) {
        എങ്കിൽ (p_മൂല്യം != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_മുകളിലെ_മൂല്യം_NEG_DATA_OFFSET;
            uint32_t value = get_മൂല്യം(trie->data, മൂല്യം_Width, di);
            എങ്കിൽ (filter != nullptr) { value = filter(context, value); }
            *p_മൂല്യം = value;
        }
        തിരികെ MAX_UNICODE;
    }

    uint32_t null_മൂല്യം = trie->null_മൂല്യം;
    എങ്കിൽ (filter != nullptr) { null_മൂല്യം = filter(context, null_മൂല്യം); }
    സ്ഥിരം uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_മൂല്യം, value = null_മൂല്യം;
    തർക്കം have_മൂല്യം = അസത്യം;
    ചെയ്യുക {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        എങ്കിൽ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } അല്ലെങ്കിൽ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            എങ്കിൽ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->മുകളിലെ_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } അല്ലെങ്കിൽ {
                U_ASSERT(c < trie->മുകളിലെ_Start && trie->മുകളിലെ_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            എങ്കിൽ (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                തുടരുക;
            }
            prevI3Block = i3Block;
            എങ്കിൽ (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                എങ്കിൽ (have_മൂല്യം) {
                    എങ്കിൽ (null_മൂല്യം != value) {
                        തിരികെ c - 1;
                    }
                } അല്ലെങ്കിൽ {
                    trie_മൂല്യം = trie->null_മൂല്യം;
                    value = null_മൂല്യം;
                    എങ്കിൽ (p_മൂല്യം != nullptr) { *p_മൂല്യം = null_മൂല്യം; }
                    have_മൂല്യം = സത്യം;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                തുടരുക;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        ചെയ്യുക {
            int32_t block;
            എങ്കിൽ ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } അല്ലെങ്കിൽ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            എങ്കിൽ (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } അല്ലെങ്കിൽ {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                എങ്കിൽ (block == trie->dataNullOffset) {
                    // This is the data null block.
                    എങ്കിൽ (have_മൂല്യം) {
                        എങ്കിൽ (null_മൂല്യം != value) {
                            തിരികെ c - 1;
                        }
                    } അല്ലെങ്കിൽ {
                        trie_മൂല്യം = trie->null_മൂല്യം;
                        value = null_മൂല്യം;
                        എങ്കിൽ (p_മൂല്യം != nullptr) { *p_മൂല്യം = null_മൂല്യം; }
                        have_മൂല്യം = സത്യം;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } അല്ലെങ്കിൽ {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_മൂല്യം(trie->data, മൂല്യം_Width, di);
                    എങ്കിൽ (have_മൂല്യം) {
                        എങ്കിൽ (trieValue2 != trie_മൂല്യം) {
                            എങ്കിൽ (filter == nullptr ||
                                    maybe_Filter_മൂല്യം(trieValue2, trie->null_മൂല്യം, null_മൂല്യം,
                                                     filter, context) != value) {
                                തിരികെ c - 1;
                            }
                            trie_മൂല്യം = trieValue2;  // may or may not help
                        }
                    } അല്ലെങ്കിൽ {
                        trie_മൂല്യം = trieValue2;
                        value = maybe_Filter_മൂല്യം(trieValue2, trie->null_മൂല്യം, null_മൂല്യം,
                                                 filter, context);
                        എങ്കിൽ (p_മൂല്യം != nullptr) { *p_മൂല്യം = value; }
                        have_മൂല്യം = സത്യം;
                    }
                    ആകുമ്പോൾ ((++c & dataMask) != 0) {
                        trieValue2 = get_മൂല്യം(trie->data, മൂല്യം_Width, ++di);
                        എങ്കിൽ (trieValue2 != trie_മൂല്യം) {
                            എങ്കിൽ (filter == nullptr ||
                                    maybe_Filter_മൂല്യം(trieValue2, trie->null_മൂല്യം, null_മൂല്യം,
                                                     filter, context) != value) {
                                തിരികെ c - 1;
                            }
                            trie_മൂല്യം = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ആകുമ്പോൾ (++i3 < i3BlockLength);
    } ആകുമ്പോൾ (c < trie->മുകളിലെ_Start);
    U_ASSERT(have_മൂല്യം);
    int32_t di = trie->dataLength - UCPTRIE_മുകളിലെ_മൂല്യം_NEG_DATA_OFFSET;
    uint32_t മുകളിലെ_മൂല്യം = get_മൂല്യം(trie->data, മൂല്യം_Width, di);
    എങ്കിൽ (maybe_Filter_മൂല്യം(മുകളിലെ_മൂല്യം, trie->null_മൂല്യം, null_മൂല്യം,
                         filter, context) != value) {
        തിരികെ c - 1;
    } അല്ലെങ്കിൽ {
        തിരികെ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         സ്ഥിരം ശൂന്യം *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_മൂല്യം,
                         UCPMap_മൂല്യം_Filter *filter, സ്ഥിരം ശൂന്യം *context, uint32_t *p_മൂല്യം) {
    എങ്കിൽ (option == UCPMAP_RANGE_NORMAL) {
        തിരികെ getRange(trie, start, filter, context, p_മൂല്യം);
    }
    uint32_t value;
    എങ്കിൽ (p_മൂല്യം == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_മൂല്യം = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_മൂല്യം);
    എങ്കിൽ (end < 0xd7ff || start > surrEnd) {
        തിരികെ end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    എങ്കിൽ (*p_മൂല്യം == surrogate_മൂല്യം) {
        എങ്കിൽ (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            തിരികെ end;
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (start <= 0xd7ff) {
            തിരികെ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_മൂല്യം = surrogate_മൂല്യം;
        എങ്കിൽ (end > surrEnd) {
            തിരികെ surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    എങ്കിൽ (value2 == surrogate_മൂല്യം) {
        തിരികെ end2;
    }
    തിരികെ surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(സ്ഥിരം UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_മൂല്യം,
                 UCPMap_മൂല്യം_Filter *filter, സ്ഥിരം ശൂന്യം *context, uint32_t *p_മൂല്യം) {
    തിരികെ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_മൂല്യം,
                                    filter, context, p_മൂല്യം);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_ദ്വിമാന(സ്ഥിരം UCPTrie *trie,
                 ശൂന്യം *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_മൂല്യം_Width മൂല്യം_Width = (UCPTrie_മൂല്യം_Width)trie->മൂല്യം_Width;
    എങ്കിൽ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            മൂല്യം_Width < UCPTRIE_മൂല്യം_BITS_16 || UCPTRIE_മൂല്യം_BITS_8 < മൂല്യം_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    int32_t length = (int32_t)വലുപ്പം(UCPTrieHeader) + trie->indexLength * 2;
    തിരഞ്ഞെടുപ്പ് (മൂല്യം_Width) {
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
        length += trie->dataLength * 2;
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
        length += trie->dataLength * 4;
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
        length += trie->dataLength;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        // unreachable
        നിർത്തുക;
    }
    എങ്കിൽ (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ length;
    }

    അക്ഷരം *bytes = (അക്ഷരം *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        മൂല്യം_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_മുകളിലെ_Start = trie->മുകളിലെ_Start >> UCPTRIE_SHIFT_2;
    bytes += വലുപ്പം(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    തിരഞ്ഞെടുപ്പ് (മൂല്യം_Width) {
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        // unreachable
        നിർത്തുക;
    }
    തിരികെ length;
}

നാമാകാശ {

#ifdef UCPTRIE_DEBUG
long എണ്ണം_Null(സ്ഥിരം UCPTrie *trie) {
    uint32_t null_മൂല്യം=trie->null_മൂല്യം;
    int32_t length=trie->dataLength;
    long count=0;
    തിരഞ്ഞെടുപ്പ് (trie->മൂല്യം_Width) {
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_16:
        വേണ്ടി(int32_t i=0; i<length; ++i) {
            എങ്കിൽ(trie->data.ptr16[i]==null_മൂല്യം) { ++count; }
        }
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_32:
        വേണ്ടി(int32_t i=0; i<length; ++i) {
            എങ്കിൽ(trie->data.ptr32[i]==null_മൂല്യം) { ++count; }
        }
        നിർത്തുക;
    അവസ്ഥ UCPTRIE_മൂല്യം_BITS_8:
        വേണ്ടി(int32_t i=0; i<length; ++i) {
            എങ്കിൽ(trie->data.ptr8[i]==null_മൂല്യം) { ++count; }
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        // unreachable
        നിർത്തുക;
    }
    തിരികെ count;
}

U_CFUNC ശൂന്യം
ucptrie_printLengths(സ്ഥിരം UCPTrie *trie, സ്ഥിരം അക്ഷരം *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)വലുപ്പം(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->മൂല്യം_Width==UCPTRIE_മൂല്യം_BITS_16 ? 2 :
                        trie->മൂല്യം_Width==UCPTRIE_മൂല്യം_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, എണ്ണം_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(സ്ഥിരം UCPMap *map, UChar32 c) {
    തിരികെ ucptrie_get(reinterpret_cast<സ്ഥിരം UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(സ്ഥിരം UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_മൂല്യം,
                UCPMap_മൂല്യം_Filter *filter, സ്ഥിരം ശൂന്യം *context, uint32_t *p_മൂല്യം) {
    തിരികെ ucptrie_getRange(reinterpret_cast<സ്ഥിരം UCPTrie *>(map), start,
                            option, surrogate_മൂല്യം,
                            filter, context, p_മൂല്യം);
}
