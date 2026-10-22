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
    𐡄𐡍 (U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }

    𐡄𐡍 (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_ANY || UCPTRIE_VALUE_BITS_8 < valueWidth) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    // Enough data for a trie header?
    𐡄𐡍 (length < (int32_t)حجم(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }

    // Check the signature.
    ثابت UCPTrieHeader *header = (ثابت UCPTrieHeader *)data;
    𐡄𐡍 (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t valueWidthInt = options & UCPTRIE_OPTIONS_VALUE_BITS_MASK;
    𐡄𐡍 (typeInt > UCPTRIE_TYPE_SMALL || valueWidthInt > UCPTRIE_VALUE_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrieValueWidth actualValueWidth = (UCPTrieValueWidth)valueWidthInt;
    𐡄𐡍 (type < 0) {
        type = actualType;
    }
    𐡄𐡍 (valueWidth < 0) {
        valueWidth = actualValueWidth;
    }
    𐡄𐡍 (type != actualType || valueWidth != actualValueWidth) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
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
    𐡄𐡍 (valueWidth == UCPTRIE_VALUE_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } وإلا 𐡄𐡍 (valueWidth == UCPTRIE_VALUE_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } وإلا {
        actualLength += tempTrie.dataLength;
    }
    𐡄𐡍 (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        أرجع nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    𐡄𐡍 (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
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
    𐡄𐡍 (nullValueOffset >= trie->dataLength) {
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
        أرجع nullptr;
    }

    𐡄𐡍 (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ثابت UCPTrie *trie) {
    أرجع (UCPTrieType)trie->type;
}

U_CAPI UCPTrieValueWidth U_EXPORT2
ucptrie_getValueWidth(ثابت UCPTrie *trie) {
    أرجع (UCPTrieValueWidth)trie->valueWidth;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    𐡄𐡍 (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->highStart);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } وإلا {
        U_ASSERT((uint32_t)c < (uint32_t)trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    𐡄𐡍 ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } وإلا {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    أرجع dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    𐡄𐡍 (c >= trie->highStart) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        أرجع trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    }
    أرجع ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *start, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    𐡄𐡍 ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } وإلا {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    أرجع (idx << 3) | i;
}

نطاق {

مضمن uint32_t getValue(UCPTrieData data, UCPTrieValueWidth valueWidth, int32_t dataIndex) {
    بدل (valueWidth) {
    حالة UCPTRIE_VALUE_BITS_16:
        أرجع data.ptr16[dataIndex];
    حالة UCPTRIE_VALUE_BITS_32:
        أرجع data.ptr32[dataIndex];
    حالة UCPTRIE_VALUE_BITS_8:
        أرجع data.ptr8[dataIndex];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        أرجع 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    𐡄𐡍 ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } وإلا {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    أرجع getValue(trie->data, (UCPTrieValueWidth)trie->valueWidth, dataIndex);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybeFilterValue(uint32_t value, uint32_t trieNullValue, uint32_t nullValue,
                                 UCPMapValueFilter *filter, ثابت فراغ *context) {
    𐡄𐡍 (value == trieNullValue) {
        value = nullValue;
    } وإلا 𐡄𐡍 (filter != nullptr) {
        value = filter(context, value);
    }
    أرجع value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 start,
                 UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    𐡄𐡍 (static_cast<uint32_t>(start) > MAX_UNICODE) {
        أرجع U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrieValueWidth valueWidth = static_cast<UCPTrieValueWidth>(trie->valueWidth);
    𐡄𐡍 (start >= trie->highStart) {
        𐡄𐡍 (pValue != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
            uint32_t value = getValue(trie->data, valueWidth, di);
            𐡄𐡍 (filter != nullptr) { value = filter(context, value); }
            *pValue = value;
        }
        أرجع MAX_UNICODE;
    }

    uint32_t nullValue = trie->nullValue;
    𐡄𐡍 (filter != nullptr) { nullValue = filter(context, nullValue); }
    ثابت uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trieValue, value = nullValue;
    منطقي haveValue = خطأ;
    𐡏𐡁𐡃 {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        𐡄𐡍 (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } وإلا {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            𐡄𐡍 (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->highStart);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } وإلا {
                U_ASSERT(c < trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            𐡄𐡍 (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                استمر;
            }
            prevI3Block = i3Block;
            𐡄𐡍 (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                𐡄𐡍 (haveValue) {
                    𐡄𐡍 (nullValue != value) {
                        أرجع c - 1;
                    }
                } وإلا {
                    trieValue = trie->nullValue;
                    value = nullValue;
                    𐡄𐡍 (pValue != nullptr) { *pValue = nullValue; }
                    haveValue = 𐡒𐡔𐡈;
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
        𐡏𐡁𐡃 {
            int32_t block;
            𐡄𐡍 ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } وإلا {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            𐡄𐡍 (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } وإلا {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                𐡄𐡍 (block == trie->dataNullOffset) {
                    // This is the data null block.
                    𐡄𐡍 (haveValue) {
                        𐡄𐡍 (nullValue != value) {
                            أرجع c - 1;
                        }
                    } وإلا {
                        trieValue = trie->nullValue;
                        value = nullValue;
                        𐡄𐡍 (pValue != nullptr) { *pValue = nullValue; }
                        haveValue = 𐡒𐡔𐡈;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } وإلا {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = getValue(trie->data, valueWidth, di);
                    𐡄𐡍 (haveValue) {
                        𐡄𐡍 (trieValue2 != trieValue) {
                            𐡄𐡍 (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    } وإلا {
                        trieValue = trieValue2;
                        value = maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                 filter, context);
                        𐡄𐡍 (pValue != nullptr) { *pValue = value; }
                        haveValue = 𐡒𐡔𐡈;
                    }
                    طالما ((++c & dataMask) != 0) {
                        trieValue2 = getValue(trie->data, valueWidth, ++di);
                        𐡄𐡍 (trieValue2 != trieValue) {
                            𐡄𐡍 (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                أرجع c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } طالما (++i3 < i3BlockLength);
    } طالما (c < trie->highStart);
    U_ASSERT(haveValue);
    int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    uint32_t highValue = getValue(trie->data, valueWidth, di);
    𐡄𐡍 (maybeFilterValue(highValue, trie->nullValue, nullValue,
                         filter, context) != value) {
        أرجع c - 1;
    } وإلا {
        أرجع MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogateValue,
                         UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    𐡄𐡍 (option == UCPMAP_RANGE_NORMAL) {
        أرجع getRange(trie, start, filter, context, pValue);
    }
    uint32_t value;
    𐡄𐡍 (pValue == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        pValue = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, pValue);
    𐡄𐡍 (end < 0xd7ff || start > surrEnd) {
        أرجع end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    𐡄𐡍 (*pValue == surrogateValue) {
        𐡄𐡍 (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            أرجع end;
        }
    } وإلا {
        𐡄𐡍 (start <= 0xd7ff) {
            أرجع 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *pValue = surrogateValue;
        𐡄𐡍 (end > surrEnd) {
            أرجع surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    𐡄𐡍 (value2 == surrogateValue) {
        أرجع end2;
    }
    أرجع surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogateValue,
                 UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    أرجع ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogateValue,
                                    filter, context, pValue);
}

U_CAPI int32_t U_EXPORT2
ucptrie_toBinary(ثابت UCPTrie *trie,
                 فراغ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    𐡄𐡍 (U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrieValueWidth valueWidth = (UCPTrieValueWidth)trie->valueWidth;
    𐡄𐡍 (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_16 || UCPTRIE_VALUE_BITS_8 < valueWidth ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
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
    𐡄𐡍 (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        أرجع length;
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
    أرجع length;
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
            𐡄𐡍(trie->data.ptr16[i]==nullValue) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_VALUE_BITS_32:
        لكل(int32_t i=0; i<length; ++i) {
            𐡄𐡍(trie->data.ptr32[i]==nullValue) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_VALUE_BITS_8:
        لكل(int32_t i=0; i<length; ++i) {
            𐡄𐡍(trie->data.ptr8[i]==nullValue) { ++count; }
        }
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    أرجع count;
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
    أرجع ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogateValue,
                UCPMapValueFilter *filter, ثابت فراغ *context, uint32_t *pValue) {
    أرجع ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), start,
                            option, surrogateValue,
                            filter, context, pValue);
}
