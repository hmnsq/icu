// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_द्विमान(UCPTrieType type,
                       UCPTrie_मानम्_Width मानम्_Width,
                       ध्रुव शून्य* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }

    सचे (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            मानम्_Width < UCPTRIE_मानम्_BITS_ANY || UCPTRIE_मानम्_BITS_8 < मानम्_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }

    // Enough data for a trie header?
    सचे (length < (int32_t)परिमाण(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        पटिनिवत्त nullptr;
    }

    // Check the signature.
    ध्रुव UCPTrieHeader *header = (ध्रुव UCPTrieHeader *)data;
    सचे (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        पटिनिवत्त nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t मानम्_Width_Int = options & UCPTRIE_OPTIONS_मानम्_BITS_MASK;
    सचे (typeInt > UCPTRIE_TYPE_SMALL || मानम्_Width_Int > UCPTRIE_मानम्_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        पटिनिवत्त nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_मानम्_Width actual_मानम्_Width = (UCPTrie_मानम्_Width)मानम्_Width_Int;
    सचे (type < 0) {
        type = actualType;
    }
    सचे (मानम्_Width < 0) {
        मानम्_Width = actual_मानम्_Width;
    }
    सचे (type != actualType || मानम्_Width != actual_मानम्_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        पटिनिवत्त nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, परिमाण(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.उच्चः_Start = header->shifted_उच्चः_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_उच्चः_Start = (tempTrie.उच्चः_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.मानम्_Width = मानम्_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)परिमाण(UCPTrieHeader) + tempTrie.indexLength * 2;
    सचे (मानम्_Width == UCPTRIE_मानम्_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } अञ्ञथा सचे (मानम्_Width == UCPTRIE_मानम्_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } अञ्ञथा {
        actualLength += tempTrie.dataLength;
    }
    सचे (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        पटिनिवत्त nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(परिमाण(UCPTrie));
    सचे (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    uprv_memcpy(trie, &tempTrie, परिमाण(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ध्रुव uint16_t *p16 = (ध्रुव uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_मानम्_Offset = trie->dataNullOffset;
    सचे (null_मानम्_Offset >= trie->dataLength) {
        null_मानम्_Offset = trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET;
    }
    विकल्प (मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_मानम् = trie->data.ptr16[null_मानम्_Offset];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        trie->data.ptr32 = (ध्रुव uint32_t *)p16;
        trie->null_मानम् = trie->data.ptr32[null_मानम्_Offset];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        trie->data.ptr8 = (ध्रुव uint8_t *)p16;
        trie->null_मानम् = trie->data.ptr8[null_मानम्_Offset];
        भङ्ग;
    स्वभाव:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        पटिनिवत्त nullptr;
    }

    सचे (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    पटिनिवत्त trie;
}

U_CAPI शून्य U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ध्रुव UCPTrie *trie) {
    पटिनिवत्त (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_मानम्_Width U_EXPORT2
ucptrie_get_मानम्_Width(ध्रुव UCPTrie *trie) {
    पटिनिवत्त (UCPTrie_मानम्_Width)trie->मानम्_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ध्रुव UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    सचे (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->उच्चः_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } अञ्ञथा {
        U_ASSERT((uint32_t)c < (uint32_t)trie->उच्चः_Start && trie->उच्चः_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    सचे ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } अञ्ञथा {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    पटिनिवत्त dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ध्रुव UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    सचे (c >= trie->उच्चः_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        पटिनिवत्त trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET;
    }
    पटिनिवत्त ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ध्रुव UCPTrie *trie, UChar32 c,
                            ध्रुव uint8_t *start, ध्रुव uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    सचे ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } अञ्ञथा {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    पटिनिवत्त (idx << 3) | i;
}

नामाकाश {

अन्तःस्थ uint32_t get_मानम्(UCPTrieData data, UCPTrie_मानम्_Width मानम्_Width, int32_t dataIndex) {
    विकल्प (मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        पटिनिवत्त data.ptr16[dataIndex];
    पक्ष UCPTRIE_मानम्_BITS_32:
        पटिनिवत्त data.ptr32[dataIndex];
    पक्ष UCPTRIE_मानम्_BITS_8:
        पटिनिवत्त data.ptr8[dataIndex];
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        पटिनिवत्त 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ध्रुव UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    सचे ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } अञ्ञथा {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    पटिनिवत्त get_मानम्(trie->data, (UCPTrie_मानम्_Width)trie->मानम्_Width, dataIndex);
}

नामाकाश {

constexpr int32_t MAX_UNICODE = 0x10ffff;

अन्तःस्थ uint32_t maybe_Filter_मानम्(uint32_t value, uint32_t trie_Null_मानम्, uint32_t null_मानम्,
                                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context) {
    सचे (value == trie_Null_मानम्) {
        value = null_मानम्;
    } अञ्ञथा सचे (filter != nullptr) {
        value = filter(context, value);
    }
    पटिनिवत्त value;
}

UChar32 getRange(ध्रुव शून्य *t, UChar32 start,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    सचे (static_cast<uint32_t>(start) > MAX_UNICODE) {
        पटिनिवत्त U_SENTINEL;
    }
    ध्रुव UCPTrie *trie = reinterpret_cast<ध्रुव UCPTrie *>(t);
    UCPTrie_मानम्_Width मानम्_Width = static_cast<UCPTrie_मानम्_Width>(trie->मानम्_Width);
    सचे (start >= trie->उच्चः_Start) {
        सचे (p_मानम् != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET;
            uint32_t value = get_मानम्(trie->data, मानम्_Width, di);
            सचे (filter != nullptr) { value = filter(context, value); }
            *p_मानम् = value;
        }
        पटिनिवत्त MAX_UNICODE;
    }

    uint32_t null_मानम् = trie->null_मानम्;
    सचे (filter != nullptr) { null_मानम् = filter(context, null_मानम्); }
    ध्रुव uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_मानम्, value = null_मानम्;
    तार्किक have_मानम् = मिच्छा;
    करोहि {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        सचे (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } अञ्ञथा {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            सचे (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->उच्चः_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } अञ्ञथा {
                U_ASSERT(c < trie->उच्चः_Start && trie->उच्चः_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            सचे (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                अनुवर्त;
            }
            prevI3Block = i3Block;
            सचे (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                सचे (have_मानम्) {
                    सचे (null_मानम् != value) {
                        पटिनिवत्त c - 1;
                    }
                } अञ्ञथा {
                    trie_मानम् = trie->null_मानम्;
                    value = null_मानम्;
                    सचे (p_मानम् != nullptr) { *p_मानम् = null_मानम्; }
                    have_मानम् = सच्च;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                अनुवर्त;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        करोहि {
            int32_t block;
            सचे ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } अञ्ञथा {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            सचे (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } अञ्ञथा {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                सचे (block == trie->dataNullOffset) {
                    // This is the data null block.
                    सचे (have_मानम्) {
                        सचे (null_मानम् != value) {
                            पटिनिवत्त c - 1;
                        }
                    } अञ्ञथा {
                        trie_मानम् = trie->null_मानम्;
                        value = null_मानम्;
                        सचे (p_मानम् != nullptr) { *p_मानम् = null_मानम्; }
                        have_मानम् = सच्च;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } अञ्ञथा {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_मानम्(trie->data, मानम्_Width, di);
                    सचे (have_मानम्) {
                        सचे (trieValue2 != trie_मानम्) {
                            सचे (filter == nullptr ||
                                    maybe_Filter_मानम्(trieValue2, trie->null_मानम्, null_मानम्,
                                                     filter, context) != value) {
                                पटिनिवत्त c - 1;
                            }
                            trie_मानम् = trieValue2;  // may or may not help
                        }
                    } अञ्ञथा {
                        trie_मानम् = trieValue2;
                        value = maybe_Filter_मानम्(trieValue2, trie->null_मानम्, null_मानम्,
                                                 filter, context);
                        सचे (p_मानम् != nullptr) { *p_मानम् = value; }
                        have_मानम् = सच्च;
                    }
                    याव ((++c & dataMask) != 0) {
                        trieValue2 = get_मानम्(trie->data, मानम्_Width, ++di);
                        सचे (trieValue2 != trie_मानम्) {
                            सचे (filter == nullptr ||
                                    maybe_Filter_मानम्(trieValue2, trie->null_मानम्, null_मानम्,
                                                     filter, context) != value) {
                                पटिनिवत्त c - 1;
                            }
                            trie_मानम् = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } याव (++i3 < i3BlockLength);
    } याव (c < trie->उच्चः_Start);
    U_ASSERT(have_मानम्);
    int32_t di = trie->dataLength - UCPTRIE_उच्चः_मानम्_NEG_DATA_OFFSET;
    uint32_t उच्चः_मानम् = get_मानम्(trie->data, मानम्_Width, di);
    सचे (maybe_Filter_मानम्(उच्चः_मानम्, trie->null_मानम्, null_मानम्,
                         filter, context) != value) {
        पटिनिवत्त c - 1;
    } अञ्ञथा {
        पटिनिवत्त MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ध्रुव शून्य *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_मानम्,
                         UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    सचे (option == UCPMAP_RANGE_NORMAL) {
        पटिनिवत्त getRange(trie, start, filter, context, p_मानम्);
    }
    uint32_t value;
    सचे (p_मानम् == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_मानम् = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_मानम्);
    सचे (end < 0xd7ff || start > surrEnd) {
        पटिनिवत्त end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    सचे (*p_मानम् == surrogate_मानम्) {
        सचे (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            पटिनिवत्त end;
        }
    } अञ्ञथा {
        सचे (start <= 0xd7ff) {
            पटिनिवत्त 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_मानम् = surrogate_मानम्;
        सचे (end > surrEnd) {
            पटिनिवत्त surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    सचे (value2 == surrogate_मानम्) {
        पटिनिवत्त end2;
    }
    पटिनिवत्त surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ध्रुव UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_मानम्,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    पटिनिवत्त ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मानम्,
                                    filter, context, p_मानम्);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_द्विमान(ध्रुव UCPTrie *trie,
                 शून्य *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) {
        पटिनिवत्त 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_मानम्_Width मानम्_Width = (UCPTrie_मानम्_Width)trie->मानम्_Width;
    सचे (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मानम्_Width < UCPTRIE_मानम्_BITS_16 || UCPTRIE_मानम्_BITS_8 < मानम्_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }

    int32_t length = (int32_t)परिमाण(UCPTrieHeader) + trie->indexLength * 2;
    विकल्प (मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        length += trie->dataLength * 2;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        length += trie->dataLength * 4;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        length += trie->dataLength;
        भङ्ग;
    स्वभाव:
        // unreachable
        भङ्ग;
    }
    सचे (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        पटिनिवत्त length;
    }

    वर्ण *bytes = (वर्ण *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        मानम्_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_उच्चः_Start = trie->उच्चः_Start >> UCPTRIE_SHIFT_2;
    bytes += परिमाण(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    विकल्प (मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        भङ्ग;
    स्वभाव:
        // unreachable
        भङ्ग;
    }
    पटिनिवत्त length;
}

नामाकाश {

#ifdef UCPTRIE_DEBUG
long गणनम्_Null(ध्रुव UCPTrie *trie) {
    uint32_t null_मानम्=trie->null_मानम्;
    int32_t length=trie->dataLength;
    long count=0;
    विकल्प (trie->मानम्_Width) {
    पक्ष UCPTRIE_मानम्_BITS_16:
        अत्थाय(int32_t i=0; i<length; ++i) {
            सचे(trie->data.ptr16[i]==null_मानम्) { ++count; }
        }
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_32:
        अत्थाय(int32_t i=0; i<length; ++i) {
            सचे(trie->data.ptr32[i]==null_मानम्) { ++count; }
        }
        भङ्ग;
    पक्ष UCPTRIE_मानम्_BITS_8:
        अत्थाय(int32_t i=0; i<length; ++i) {
            सचे(trie->data.ptr8[i]==null_मानम्) { ++count; }
        }
        भङ्ग;
    स्वभाव:
        // unreachable
        भङ्ग;
    }
    पटिनिवत्त count;
}

U_CFUNC शून्य
ucptrie_printLengths(ध्रुव UCPTrie *trie, ध्रुव वर्ण *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)परिमाण(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->मानम्_Width==UCPTRIE_मानम्_BITS_16 ? 2 :
                        trie->मानम्_Width==UCPTRIE_मानम्_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, गणनम्_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ध्रुव UCPMap *map, UChar32 c) {
    पटिनिवत्त ucptrie_get(reinterpret_cast<ध्रुव UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ध्रुव UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_मानम्,
                UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    पटिनिवत्त ucptrie_getRange(reinterpret_cast<ध्रुव UCPTrie *>(map), start,
                            option, surrogate_मानम्,
                            filter, context, p_मानम्);
}
