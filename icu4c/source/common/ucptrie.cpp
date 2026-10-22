// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_openFromBinary(UCPTrieType type,
                       UCPTrieValueWidth valueWidth,
                       ثابت فراغ* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    ܐܢ (U_FAILURE(*pErrorCode)) {
        ܦܢܐ nullptr;
    }

    ܐܢ (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_ANY || UCPTRIE_VALUE_BITS_8 < valueWidth) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }

    // Enough data for a trie header?
    ܐܢ (length < (int32_t)حجم(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ܦܢܐ nullptr;
    }

    // Check the signature.
    ثابت UCPTrieHeader *header = (ثابت UCPTrieHeader *)data;
    ܐܢ (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ܦܢܐ nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t valueWidthInt = options & UCPTRIE_OPTIONS_VALUE_BITS_MASK;
    ܐܢ (typeInt > UCPTRIE_TYPE_SMALL || valueWidthInt > UCPTRIE_VALUE_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ܦܢܐ nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrieValueWidth actualValueWidth = (UCPTrieValueWidth)valueWidthInt;
    ܐܢ (type < 0) {
        type = actualType;
    }
    ܐܢ (valueWidth < 0) {
        valueWidth = actualValueWidth;
    }
    ܐܢ (type != actualType || valueWidth != actualValueWidth) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ܦܢܐ nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, حجم(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.highStart = header->shiftedHighStart << UCPTRIE_SHIFT_2;
    tempTrie.shifted12HighStart = (tempTrie.highStart + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.valueWidth = valueWidth;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)حجم(UCPTrieHeader) + tempTrie.indexLength * 2;
    ܐܢ (valueWidth == UCPTRIE_VALUE_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } ܐܚܪܢܐ ܐܢ (valueWidth == UCPTRIE_VALUE_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } ܐܚܪܢܐ {
        actualLength += tempTrie.dataLength;
    }
    ܐܢ (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ܦܢܐ nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    ܐܢ (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, حجم(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t nullValueOffset = trie->dataNullOffset;
    ܐܢ (nullValueOffset >= trie->dataLength) {
        nullValueOffset = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    }
    بدل (valueWidth) {
    حالة UCPTRIE_VALUE_BITS_16:
        trie->data.ptr16 = p16;
        trie->nullValue = trie->data.ptr16[nullValueOffset];
        اكسر;
    حالة UCPTRIE_VALUE_BITS_32:
        trie->data.ptr32 = (ثابت uint32_t *)p16;
        trie->nullValue = trie->data.ptr32[nullValueOffset];
        اكسر;
    حالة UCPTRIE_VALUE_BITS_8:
        trie->data.ptr8 = (ثابت uint8_t *)p16;
        trie->nullValue = trie->data.ptr8[nullValueOffset];
        اكسر;
    افتراضي:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ܦܢܐ nullptr;
    }

    ܐܢ (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    ܦܢܐ trie;
}

U_CAPI فراغ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ثابت UCPTrie *trie) {
    ܦܢܐ (UCPTrieType)trie->type;
}

U_CAPI UCPTrieValueWidth U_EXPORT2
ucptrie_getValueWidth(ثابت UCPTrie *trie) {
    ܦܢܐ (UCPTrieValueWidth)trie->valueWidth;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    ܐܢ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->highStart);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } ܐܚܪܢܐ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    ܐܢ ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } ܐܚܪܢܐ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    ܦܢܐ dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ܐܢ (c >= trie->highStart) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ܦܢܐ trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    }
    ܦܢܐ ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *start, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ܐܢ ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } ܐܚܪܢܐ {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    ܦܢܐ (idx << 3) | i;
}

نطاق {

مضمن uint32_t getValue(UCPTrieData data, UCPTrieValueWidth valueWidth, int32_t dataIndex) {
    بدل (valueWidth) {
    حالة UCPTRIE_VALUE_BITS_16:
        ܦܢܐ data.ptr16[dataIndex];
    حالة UCPTRIE_VALUE_BITS_32:
        ܦܢܐ data.ptr32[dataIndex];
    حالة UCPTRIE_VALUE_BITS_8:
        ܦܢܐ data.ptr8[dataIndex];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        ܦܢܐ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    ܐܢ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } ܐܚܪܢܐ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    ܦܢܐ getValue(trie->data, (UCPTrieValueWidth)trie->valueWidth, dataIndex);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybeFilterValue(uint32_t value, uint32_t trieNullValue, uint32_t nullValue,
                                 UCPMapValueFilter *filter, ثابت فراغ *context) {
    ܐܢ (value == trieNullValue) {
        value = nullValue;
    } ܐܚܪܢܐ ܐܢ (filter != nullptr) {
        value = filter(context, value);
    }
    ܦܢܐ value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 start,
                 UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    ܐܢ (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ܦܢܐ U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrieValueWidth valueWidth = static_cast<UCPTrieValueWidth>(trie->valueWidth);
    ܐܢ (start >= trie->highStart) {
        ܐܢ (pValue != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
            uint32_t value = getValue(trie->data, valueWidth, di);
            ܐܢ (filter != nullptr) { value = filter(context, value); }
            *pValue = value;
        }
        ܦܢܐ MAX_UNICODE;
    }

    uint32_t nullValue = trie->nullValue;
    ܐܢ (filter != nullptr) { nullValue = filter(context, nullValue); }
    ثابت uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trieValue, value = nullValue;
    منطقي haveValue = ܕܓܠܐ;
    ܥܒܕ {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        ܐܢ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } ܐܚܪܢܐ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            ܐܢ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->highStart);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } ܐܚܪܢܐ {
                U_ASSERT(c < trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            ܐܢ (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                استمر;
            }
            prevI3Block = i3Block;
            ܐܢ (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                ܐܢ (haveValue) {
                    ܐܢ (nullValue != value) {
                        ܦܢܐ c - 1;
                    }
                } ܐܚܪܢܐ {
                    trieValue = trie->nullValue;
                    value = nullValue;
                    ܐܢ (pValue != nullptr) { *pValue = nullValue; }
                    haveValue = ܫܪܪܐ;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                استمر;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        ܥܒܕ {
            int32_t block;
            ܐܢ ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } ܐܚܪܢܐ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            ܐܢ (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } ܐܚܪܢܐ {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                ܐܢ (block == trie->dataNullOffset) {
                    // This is the data null block.
                    ܐܢ (haveValue) {
                        ܐܢ (nullValue != value) {
                            ܦܢܐ c - 1;
                        }
                    } ܐܚܪܢܐ {
                        trieValue = trie->nullValue;
                        value = nullValue;
                        ܐܢ (pValue != nullptr) { *pValue = nullValue; }
                        haveValue = ܫܪܪܐ;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } ܐܚܪܢܐ {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = getValue(trie->data, valueWidth, di);
                    ܐܢ (haveValue) {
                        ܐܢ (trieValue2 != trieValue) {
                            ܐܢ (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                ܦܢܐ c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    } ܐܚܪܢܐ {
                        trieValue = trieValue2;
                        value = maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                 filter, context);
                        ܐܢ (pValue != nullptr) { *pValue = value; }
                        haveValue = ܫܪܪܐ;
                    }
                    ܟܡܐ ((++c & dataMask) != 0) {
                        trieValue2 = getValue(trie->data, valueWidth, ++di);
                        ܐܢ (trieValue2 != trieValue) {
                            ܐܢ (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                ܦܢܐ c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ܟܡܐ (++i3 < i3BlockLength);
    } ܟܡܐ (c < trie->highStart);
    U_ASSERT(haveValue);
    int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    uint32_t highValue = getValue(trie->data, valueWidth, di);
    ܐܢ (maybeFilterValue(highValue, trie->nullValue, nullValue,
                         filter, context) != value) {
        ܦܢܐ c - 1;
    } ܐܚܪܢܐ {
        ܦܢܐ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogateValue,
                         UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    ܐܢ (option == UCPMAP_RANGE_NORMAL) {
        ܦܢܐ getRange(trie, start, filter, context, pValue);
    }
    uint32_t value;
    ܐܢ (pValue == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        pValue = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, pValue);
    ܐܢ (end < 0xd7ff || start > surrEnd) {
        ܦܢܐ end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ܐܢ (*pValue == surrogateValue) {
        ܐܢ (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ܦܢܐ end;
        }
    } ܐܚܪܢܐ {
        ܐܢ (start <= 0xd7ff) {
            ܦܢܐ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *pValue = surrogateValue;
        ܐܢ (end > surrEnd) {
            ܦܢܐ surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    ܐܢ (value2 == surrogateValue) {
        ܦܢܐ end2;
    }
    ܦܢܐ surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogateValue,
                 UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    ܦܢܐ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogateValue,
                                    filter, context, pValue);
}

U_CAPI int32_t U_EXPORT2
ucptrie_toBinary(ثابت UCPTrie *trie,
                 فراغ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    ܐܢ (U_FAILURE(*pErrorCode)) {
        ܦܢܐ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrieValueWidth valueWidth = (UCPTrieValueWidth)trie->valueWidth;
    ܐܢ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_16 || UCPTRIE_VALUE_BITS_8 < valueWidth ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ 0;
    }

    int32_t length = (int32_t)حجم(UCPTrieHeader) + trie->indexLength * 2;
    بدل (valueWidth) {
    حالة UCPTRIE_VALUE_BITS_16:
        length += trie->dataLength * 2;
        اكسر;
    حالة UCPTRIE_VALUE_BITS_32:
        length += trie->dataLength * 4;
        اكسر;
    حالة UCPTRIE_VALUE_BITS_8:
        length += trie->dataLength;
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    ܐܢ (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        ܦܢܐ length;
    }

    حرف *bytes = (حرف *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        valueWidth);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shiftedHighStart = trie->highStart >> UCPTRIE_SHIFT_2;
    bytes += حجم(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    بدل (valueWidth) {
    حالة UCPTRIE_VALUE_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        اكسر;
    حالة UCPTRIE_VALUE_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        اكسر;
    حالة UCPTRIE_VALUE_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    ܦܢܐ length;
}

نطاق {

#ifdef UCPTRIE_DEBUG
long countNull(ثابت UCPTrie *trie) {
    uint32_t nullValue=trie->nullValue;
    int32_t length=trie->dataLength;
    long count=0;
    بدل (trie->valueWidth) {
    حالة UCPTRIE_VALUE_BITS_16:
        لكل(int32_t i=0; i<length; ++i) {
            ܐܢ(trie->data.ptr16[i]==nullValue) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_VALUE_BITS_32:
        لكل(int32_t i=0; i<length; ++i) {
            ܐܢ(trie->data.ptr32[i]==nullValue) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_VALUE_BITS_8:
        لكل(int32_t i=0; i<length; ++i) {
            ܐܢ(trie->data.ptr8[i]==nullValue) { ++count; }
        }
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    ܦܢܐ count;
}

U_CFUNC فراغ
ucptrie_printLengths(ثابت UCPTrie *trie, ثابت حرف *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)حجم(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->valueWidth==UCPTRIE_VALUE_BITS_16 ? 2 :
                        trie->valueWidth==UCPTRIE_VALUE_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, countNull(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    ܦܢܐ ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogateValue,
                UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    ܦܢܐ ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), start,
                            option, surrogateValue,
                            filter, context, pValue);
}
