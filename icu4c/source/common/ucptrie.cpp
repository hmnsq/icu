// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#include "unicode/utypes.h"
#include "unicode/ucptrie.h"
#include "unicode/utf.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "uassert.h"
#include "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_openFromBinary(UCPTrieType type,
                       UCPTrieValueWidth valueWidth,
                       const void* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_ANY || UCPTRIE_VALUE_BITS_8 < valueWidth) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }

    // Enough data for a trie header?
    ប្រសិនបើ (length < (int32_t)parimaa_na(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ត្រឡប់ nullptr;
    }

    // Check the signature.
    const UCPTrieHeader *header = (const UCPTrieHeader *)data;
    ប្រសិនបើ (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ត្រឡប់ nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t valueWidthInt = options & UCPTRIE_OPTIONS_VALUE_BITS_MASK;
    ប្រសិនបើ (typeInt > UCPTRIE_TYPE_SMALL || valueWidthInt > UCPTRIE_VALUE_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ត្រឡប់ nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrieValueWidth actualValueWidth = (UCPTrieValueWidth)valueWidthInt;
    ប្រសិនបើ (type < 0) {
        type = actualType;
    }
    ប្រសិនបើ (valueWidth < 0) {
        valueWidth = actualValueWidth;
    }
    ប្រសិនបើ (type != actualType || valueWidth != actualValueWidth) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ត្រឡប់ nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, parimaa_na(tempTrie));
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
    int32_t actualLength = (int32_t)parimaa_na(UCPTrieHeader) + tempTrie.indexLength * 2;
    ប្រសិនបើ (valueWidth == UCPTRIE_VALUE_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } បើមិនដូច្នេះ ប្រសិនបើ (valueWidth == UCPTRIE_VALUE_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } បើមិនដូច្នេះ {
        actualLength += tempTrie.dataLength;
    }
    ប្រសិនបើ (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ត្រឡប់ nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(parimaa_na(UCPTrie));
    ប្រសិនបើ (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, parimaa_na(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    const uint16_t *p16 = (const uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t nullValueOffset = trie->dataNullOffset;
    ប្រសិនបើ (nullValueOffset >= trie->dataLength) {
        nullValueOffset = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    }
    switch (valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        trie->data.ptr16 = p16;
        trie->nullValue = trie->data.ptr16[nullValueOffset];
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_32:
        trie->data.ptr32 = (const uint32_t *)p16;
        trie->nullValue = trie->data.ptr32[nullValueOffset];
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_8:
        trie->data.ptr8 = (const uint8_t *)p16;
        trie->nullValue = trie->data.ptr8[nullValueOffset];
        bha_nga;
    default:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ត្រឡប់ nullptr;
    }

    ប្រសិនបើ (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    ត្រឡប់ trie;
}

U_CAPI void U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(const UCPTrie *trie) {
    ត្រឡប់ (UCPTrieType)trie->type;
}

U_CAPI UCPTrieValueWidth U_EXPORT2
ucptrie_getValueWidth(const UCPTrie *trie) {
    ត្រឡប់ (UCPTrieValueWidth)trie->valueWidth;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(const UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    ប្រសិនបើ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->highStart);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } បើមិនដូច្នេះ {
        U_ASSERT((uint32_t)c < (uint32_t)trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    ប្រសិនបើ ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } បើមិនដូច្នេះ {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    ត្រឡប់ dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(const UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ប្រសិនបើ (c >= trie->highStart) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ត្រឡប់ trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    }
    ត្រឡប់ ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(const UCPTrie *trie, UChar32 c,
                            const uint8_t *start, const uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ប្រសិនបើ ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } បើមិនដូច្នេះ {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    ត្រឡប់ (idx << 3) | i;
}

namespace {

anta_hstha uint32_t getValue(UCPTrieData data, UCPTrieValueWidth valueWidth, int32_t dataIndex) {
    switch (valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        ត្រឡប់ data.ptr16[dataIndex];
    pak_sa UCPTRIE_VALUE_BITS_32:
        ត្រឡប់ data.ptr32[dataIndex];
    pak_sa UCPTRIE_VALUE_BITS_8:
        ត្រឡប់ data.ptr8[dataIndex];
    default:
        // Unreachable if the trie is properly initialized.
        ត្រឡប់ 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(const UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    ប្រសិនបើ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } បើមិនដូច្នេះ {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    ត្រឡប់ getValue(trie->data, (UCPTrieValueWidth)trie->valueWidth, dataIndex);
}

namespace {

constexpr int32_t MAX_UNICODE = 0x10ffff;

anta_hstha uint32_t maybeFilterValue(uint32_t value, uint32_t trieNullValue, uint32_t nullValue,
                                 UCPMapValueFilter *filter, const void *context) {
    ប្រសិនបើ (value == trieNullValue) {
        value = nullValue;
    } បើមិនដូច្នេះ ប្រសិនបើ (filter != nullptr) {
        value = filter(context, value);
    }
    ត្រឡប់ value;
}

UChar32 getRange(const void *t, UChar32 start,
                 UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    ប្រសិនបើ (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ត្រឡប់ U_SENTINEL;
    }
    const UCPTrie *trie = reinterpret_cast<const UCPTrie *>(t);
    UCPTrieValueWidth valueWidth = static_cast<UCPTrieValueWidth>(trie->valueWidth);
    ប្រសិនបើ (start >= trie->highStart) {
        ប្រសិនបើ (pValue != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
            uint32_t value = getValue(trie->data, valueWidth, di);
            ប្រសិនបើ (filter != nullptr) { value = filter(context, value); }
            *pValue = value;
        }
        ត្រឡប់ MAX_UNICODE;
    }

    uint32_t nullValue = trie->nullValue;
    ប្រសិនបើ (filter != nullptr) { nullValue = filter(context, nullValue); }
    const uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trieValue, value = nullValue;
    bool haveValue = false;
    do {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        ប្រសិនបើ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } បើមិនដូច្នេះ {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            ប្រសិនបើ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->highStart);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } បើមិនដូច្នេះ {
                U_ASSERT(c < trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            ប្រសិនបើ (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                continue;
            }
            prevI3Block = i3Block;
            ប្រសិនបើ (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                ប្រសិនបើ (haveValue) {
                    ប្រសិនបើ (nullValue != value) {
                        ត្រឡប់ c - 1;
                    }
                } បើមិនដូច្នេះ {
                    trieValue = trie->nullValue;
                    value = nullValue;
                    ប្រសិនបើ (pValue != nullptr) { *pValue = nullValue; }
                    haveValue = true;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                continue;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        do {
            int32_t block;
            ប្រសិនបើ ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } បើមិនដូច្នេះ {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            ប្រសិនបើ (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } បើមិនដូច្នេះ {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                ប្រសិនបើ (block == trie->dataNullOffset) {
                    // This is the data null block.
                    ប្រសិនបើ (haveValue) {
                        ប្រសិនបើ (nullValue != value) {
                            ត្រឡប់ c - 1;
                        }
                    } បើមិនដូច្នេះ {
                        trieValue = trie->nullValue;
                        value = nullValue;
                        ប្រសិនបើ (pValue != nullptr) { *pValue = nullValue; }
                        haveValue = true;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } បើមិនដូច្នេះ {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = getValue(trie->data, valueWidth, di);
                    ប្រសិនបើ (haveValue) {
                        ប្រសិនបើ (trieValue2 != trieValue) {
                            ប្រសិនបើ (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                ត្រឡប់ c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    } បើមិនដូច្នេះ {
                        trieValue = trieValue2;
                        value = maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                 filter, context);
                        ប្រសិនបើ (pValue != nullptr) { *pValue = value; }
                        haveValue = true;
                    }
                    ខណៈពេល ((++c & dataMask) != 0) {
                        trieValue2 = getValue(trie->data, valueWidth, ++di);
                        ប្រសិនបើ (trieValue2 != trieValue) {
                            ប្រសិនបើ (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                ត្រឡប់ c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ខណៈពេល (++i3 < i3BlockLength);
    } ខណៈពេល (c < trie->highStart);
    U_ASSERT(haveValue);
    int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    uint32_t highValue = getValue(trie->data, valueWidth, di);
    ប្រសិនបើ (maybeFilterValue(highValue, trie->nullValue, nullValue,
                         filter, context) != value) {
        ត្រឡប់ c - 1;
    } បើមិនដូច្នេះ {
        ត្រឡប់ MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         const void *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogateValue,
                         UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    ប្រសិនបើ (option == UCPMAP_RANGE_NORMAL) {
        ត្រឡប់ getRange(trie, start, filter, context, pValue);
    }
    uint32_t value;
    ប្រសិនបើ (pValue == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        pValue = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, pValue);
    ប្រសិនបើ (end < 0xd7ff || start > surrEnd) {
        ត្រឡប់ end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ប្រសិនបើ (*pValue == surrogateValue) {
        ប្រសិនបើ (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ត្រឡប់ end;
        }
    } បើមិនដូច្នេះ {
        ប្រសិនបើ (start <= 0xd7ff) {
            ត្រឡប់ 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *pValue = surrogateValue;
        ប្រសិនបើ (end > surrEnd) {
            ត្រឡប់ surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    ប្រសិនបើ (value2 == surrogateValue) {
        ត្រឡប់ end2;
    }
    ត្រឡប់ surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(const UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogateValue,
                 UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    ត្រឡប់ ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogateValue,
                                    filter, context, pValue);
}

U_CAPI int32_t U_EXPORT2
ucptrie_toBinary(const UCPTrie *trie,
                 void *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
        ត្រឡប់ 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrieValueWidth valueWidth = (UCPTrieValueWidth)trie->valueWidth;
    ប្រសិនបើ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_16 || UCPTRIE_VALUE_BITS_8 < valueWidth ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }

    int32_t length = (int32_t)parimaa_na(UCPTrieHeader) + trie->indexLength * 2;
    switch (valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        length += trie->dataLength * 2;
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_32:
        length += trie->dataLength * 4;
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_8:
        length += trie->dataLength;
        bha_nga;
    default:
        // unreachable
        bha_nga;
    }
    ប្រសិនបើ (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        ត្រឡប់ length;
    }

    var_na *bytes = (var_na *)data;
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
    bytes += parimaa_na(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    switch (valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        bha_nga;
    default:
        // unreachable
        bha_nga;
    }
    ត្រឡប់ length;
}

namespace {

#ifdef UCPTRIE_DEBUG
long countNull(const UCPTrie *trie) {
    uint32_t nullValue=trie->nullValue;
    int32_t length=trie->dataLength;
    long count=0;
    switch (trie->valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        សម្រាប់(int32_t i=0; i<length; ++i) {
            ប្រសិនបើ(trie->data.ptr16[i]==nullValue) { ++count; }
        }
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_32:
        សម្រាប់(int32_t i=0; i<length; ++i) {
            ប្រសិនបើ(trie->data.ptr32[i]==nullValue) { ++count; }
        }
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_8:
        សម្រាប់(int32_t i=0; i<length; ++i) {
            ប្រសិនបើ(trie->data.ptr8[i]==nullValue) { ++count; }
        }
        bha_nga;
    default:
        // unreachable
        bha_nga;
    }
    ត្រឡប់ count;
}

U_CFUNC void
ucptrie_printLengths(const UCPTrie *trie, const var_na *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)parimaa_na(UCPTrieHeader)+indexLength*2+
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
ucpmap_get(const UCPMap *map, UChar32 c) {
    ត្រឡប់ ucptrie_get(reinterpret_cast<const UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(const UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogateValue,
                UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    ត្រឡប់ ucptrie_getRange(reinterpret_cast<const UCPTrie *>(map), start,
                            option, surrogateValue,
                            filter, context, pValue);
}
