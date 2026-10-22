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
ucptrie_open_From_द्विआधारी(UCPTrieType type,
                       UCPTrie_मान_Width मान_Width,
                       अचर शून्य* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }

    यदि (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            मान_Width < UCPTRIE_मान_BITS_ANY || UCPTRIE_मान_BITS_8 < मान_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    // Enough data for a trie header?
    यदि (length < (int32_t)परिमाण(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    // Check the signature.
    अचर UCPTrieHeader *header = (अचर UCPTrieHeader *)data;
    यदि (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t मान_Width_Int = options & UCPTRIE_OPTIONS_मान_BITS_MASK;
    यदि (typeInt > UCPTRIE_TYPE_SMALL || मान_Width_Int > UCPTRIE_मान_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_मान_Width actual_मान_Width = (UCPTrie_मान_Width)मान_Width_Int;
    यदि (type < 0) {
        type = actualType;
    }
    यदि (मान_Width < 0) {
        मान_Width = actual_मान_Width;
    }
    यदि (type != actualType || मान_Width != actual_मान_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
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

    tempTrie.माथिल्लो_Start = header->shifted_माथिल्लो_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_माथिल्लो_Start = (tempTrie.माथिल्लो_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.मान_Width = मान_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)परिमाण(UCPTrieHeader) + tempTrie.indexLength * 2;
    यदि (मान_Width == UCPTRIE_मान_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } अन्यथा यदि (मान_Width == UCPTRIE_मान_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } अन्यथा {
        actualLength += tempTrie.dataLength;
    }
    यदि (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        फर्काऊ nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(परिमाण(UCPTrie));
    यदि (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, परिमाण(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    अचर uint16_t *p16 = (अचर uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_मान_Offset = trie->dataNullOffset;
    यदि (null_मान_Offset >= trie->dataLength) {
        null_मान_Offset = trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET;
    }
    विकल्प (मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_मान = trie->data.ptr16[null_मान_Offset];
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        trie->data.ptr32 = (अचर uint32_t *)p16;
        trie->null_मान = trie->data.ptr32[null_मान_Offset];
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        trie->data.ptr8 = (अचर uint8_t *)p16;
        trie->null_मान = trie->data.ptr8[null_मान_Offset];
        रोक;
    स्वभाव:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    यदि (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    फर्काऊ trie;
}

U_CAPI शून्य U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(अचर UCPTrie *trie) {
    फर्काऊ (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_मान_Width U_EXPORT2
ucptrie_get_मान_Width(अचर UCPTrie *trie) {
    फर्काऊ (UCPTrie_मान_Width)trie->मान_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(अचर UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    यदि (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->माथिल्लो_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } अन्यथा {
        U_ASSERT((uint32_t)c < (uint32_t)trie->माथिल्लो_Start && trie->माथिल्लो_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    यदि ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } अन्यथा {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    फर्काऊ dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(अचर UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    यदि (c >= trie->माथिल्लो_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        फर्काऊ trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET;
    }
    फर्काऊ ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(अचर UCPTrie *trie, UChar32 c,
                            अचर uint8_t *start, अचर uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    यदि ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } अन्यथा {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    फर्काऊ (idx << 3) | i;
}

नामाकाश {

अन्तःस्थ uint32_t get_मान(UCPTrieData data, UCPTrie_मान_Width मान_Width, int32_t dataIndex) {
    विकल्प (मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        फर्काऊ data.ptr16[dataIndex];
    पक्ष UCPTRIE_मान_BITS_32:
        फर्काऊ data.ptr32[dataIndex];
    पक्ष UCPTRIE_मान_BITS_8:
        फर्काऊ data.ptr8[dataIndex];
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        फर्काऊ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(अचर UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    यदि ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } अन्यथा {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    फर्काऊ get_मान(trie->data, (UCPTrie_मान_Width)trie->मान_Width, dataIndex);
}

नामाकाश {

constexpr int32_t MAX_UNICODE = 0x10ffff;

अन्तःस्थ uint32_t maybe_Filter_मान(uint32_t value, uint32_t trie_Null_मान, uint32_t null_मान,
                                 UCPMap_मान_Filter *filter, अचर शून्य *context) {
    यदि (value == trie_Null_मान) {
        value = null_मान;
    } अन्यथा यदि (filter != nullptr) {
        value = filter(context, value);
    }
    फर्काऊ value;
}

UChar32 getRange(अचर शून्य *t, UChar32 start,
                 UCPMap_मान_Filter *filter, अचर शून्य *context, uint32_t *p_मान) {
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE) {
        फर्काऊ U_SENTINEL;
    }
    अचर UCPTrie *trie = reinterpret_cast<अचर UCPTrie *>(t);
    UCPTrie_मान_Width मान_Width = static_cast<UCPTrie_मान_Width>(trie->मान_Width);
    यदि (start >= trie->माथिल्लो_Start) {
        यदि (p_मान != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET;
            uint32_t value = get_मान(trie->data, मान_Width, di);
            यदि (filter != nullptr) { value = filter(context, value); }
            *p_मान = value;
        }
        फर्काऊ MAX_UNICODE;
    }

    uint32_t null_मान = trie->null_मान;
    यदि (filter != nullptr) { null_मान = filter(context, null_मान); }
    अचर uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_मान, value = null_मान;
    तार्किक have_मान = असत्य;
    गर {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        यदि (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } अन्यथा {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            यदि (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->माथिल्लो_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } अन्यथा {
                U_ASSERT(c < trie->माथिल्लो_Start && trie->माथिल्लो_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            यदि (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                जारी;
            }
            prevI3Block = i3Block;
            यदि (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                यदि (have_मान) {
                    यदि (null_मान != value) {
                        फर्काऊ c - 1;
                    }
                } अन्यथा {
                    trie_मान = trie->null_मान;
                    value = null_मान;
                    यदि (p_मान != nullptr) { *p_मान = null_मान; }
                    have_मान = सत्य;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                जारी;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        गर {
            int32_t block;
            यदि ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } अन्यथा {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            यदि (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } अन्यथा {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                यदि (block == trie->dataNullOffset) {
                    // This is the data null block.
                    यदि (have_मान) {
                        यदि (null_मान != value) {
                            फर्काऊ c - 1;
                        }
                    } अन्यथा {
                        trie_मान = trie->null_मान;
                        value = null_मान;
                        यदि (p_मान != nullptr) { *p_मान = null_मान; }
                        have_मान = सत्य;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } अन्यथा {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_मान(trie->data, मान_Width, di);
                    यदि (have_मान) {
                        यदि (trieValue2 != trie_मान) {
                            यदि (filter == nullptr ||
                                    maybe_Filter_मान(trieValue2, trie->null_मान, null_मान,
                                                     filter, context) != value) {
                                फर्काऊ c - 1;
                            }
                            trie_मान = trieValue2;  // may or may not help
                        }
                    } अन्यथा {
                        trie_मान = trieValue2;
                        value = maybe_Filter_मान(trieValue2, trie->null_मान, null_मान,
                                                 filter, context);
                        यदि (p_मान != nullptr) { *p_मान = value; }
                        have_मान = सत्य;
                    }
                    जबसम्म ((++c & dataMask) != 0) {
                        trieValue2 = get_मान(trie->data, मान_Width, ++di);
                        यदि (trieValue2 != trie_मान) {
                            यदि (filter == nullptr ||
                                    maybe_Filter_मान(trieValue2, trie->null_मान, null_मान,
                                                     filter, context) != value) {
                                फर्काऊ c - 1;
                            }
                            trie_मान = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } जबसम्म (++i3 < i3BlockLength);
    } जबसम्म (c < trie->माथिल्लो_Start);
    U_ASSERT(have_मान);
    int32_t di = trie->dataLength - UCPTRIE_माथिल्लो_मान_NEG_DATA_OFFSET;
    uint32_t माथिल्लो_मान = get_मान(trie->data, मान_Width, di);
    यदि (maybe_Filter_मान(माथिल्लो_मान, trie->null_मान, null_मान,
                         filter, context) != value) {
        फर्काऊ c - 1;
    } अन्यथा {
        फर्काऊ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         अचर शून्य *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_मान,
                         UCPMap_मान_Filter *filter, अचर शून्य *context, uint32_t *p_मान) {
    यदि (option == UCPMAP_RANGE_NORMAL) {
        फर्काऊ getRange(trie, start, filter, context, p_मान);
    }
    uint32_t value;
    यदि (p_मान == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_मान = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_मान);
    यदि (end < 0xd7ff || start > surrEnd) {
        फर्काऊ end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    यदि (*p_मान == surrogate_मान) {
        यदि (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            फर्काऊ end;
        }
    } अन्यथा {
        यदि (start <= 0xd7ff) {
            फर्काऊ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_मान = surrogate_मान;
        यदि (end > surrEnd) {
            फर्काऊ surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    यदि (value2 == surrogate_मान) {
        फर्काऊ end2;
    }
    फर्काऊ surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(अचर UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_मान,
                 UCPMap_मान_Filter *filter, अचर शून्य *context, uint32_t *p_मान) {
    फर्काऊ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मान,
                                    filter, context, p_मान);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_द्विआधारी(अचर UCPTrie *trie,
                 शून्य *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_मान_Width मान_Width = (UCPTrie_मान_Width)trie->मान_Width;
    यदि (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मान_Width < UCPTRIE_मान_BITS_16 || UCPTRIE_मान_BITS_8 < मान_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    int32_t length = (int32_t)परिमाण(UCPTrieHeader) + trie->indexLength * 2;
    विकल्प (मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        length += trie->dataLength * 2;
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        length += trie->dataLength * 4;
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        length += trie->dataLength;
        रोक;
    स्वभाव:
        // unreachable
        रोक;
    }
    यदि (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        फर्काऊ length;
    }

    वर्ण *bytes = (वर्ण *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        मान_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_माथिल्लो_Start = trie->माथिल्लो_Start >> UCPTRIE_SHIFT_2;
    bytes += परिमाण(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    विकल्प (मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        रोक;
    स्वभाव:
        // unreachable
        रोक;
    }
    फर्काऊ length;
}

नामाकाश {

#ifdef UCPTRIE_DEBUG
दीर्घ गन्ती_Null(अचर UCPTrie *trie) {
    uint32_t null_मान=trie->null_मान;
    int32_t length=trie->dataLength;
    दीर्घ count=0;
    विकल्प (trie->मान_Width) {
    पक्ष UCPTRIE_मान_BITS_16:
        लागि(int32_t i=0; i<length; ++i) {
            यदि(trie->data.ptr16[i]==null_मान) { ++count; }
        }
        रोक;
    पक्ष UCPTRIE_मान_BITS_32:
        लागि(int32_t i=0; i<length; ++i) {
            यदि(trie->data.ptr32[i]==null_मान) { ++count; }
        }
        रोक;
    पक्ष UCPTRIE_मान_BITS_8:
        लागि(int32_t i=0; i<length; ++i) {
            यदि(trie->data.ptr8[i]==null_मान) { ++count; }
        }
        रोक;
    स्वभाव:
        // unreachable
        रोक;
    }
    फर्काऊ count;
}

U_CFUNC शून्य
ucptrie_printLengths(अचर UCPTrie *trie, अचर वर्ण *which) {
    दीर्घ indexLength=trie->indexLength;
    दीर्घ dataLength=(दीर्घ)trie->dataLength;
    दीर्घ totalLength=(दीर्घ)परिमाण(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->मान_Width==UCPTRIE_मान_BITS_16 ? 2 :
                        trie->मान_Width==UCPTRIE_मान_BITS_32 ? 4 : 1);
    म_लिखो("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, गन्ती_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(अचर UCPMap *map, UChar32 c) {
    फर्काऊ ucptrie_get(reinterpret_cast<अचर UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(अचर UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_मान,
                UCPMap_मान_Filter *filter, अचर शून्य *context, uint32_t *p_मान) {
    फर्काऊ ucptrie_getRange(reinterpret_cast<अचर UCPTrie *>(map), start,
                            option, surrogate_मान,
                            filter, context, p_मान);
}
