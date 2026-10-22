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
    ถ้า (U_FAILURE(*pErrorCode)) {
        คืน nullptr;
    }

    ถ้า (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_ANY || UCPTRIE_VALUE_BITS_8 < valueWidth) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }

    // Enough data for a trie header?
    ถ้า (length < (int32_t)parimaa_na(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        คืน nullptr;
    }

    // Check the signature.
    const UCPTrieHeader *header = (const UCPTrieHeader *)data;
    ถ้า (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        คืน nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t valueWidthInt = options & UCPTRIE_OPTIONS_VALUE_BITS_MASK;
    ถ้า (typeInt > UCPTRIE_TYPE_SMALL || valueWidthInt > UCPTRIE_VALUE_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        คืน nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrieValueWidth actualValueWidth = (UCPTrieValueWidth)valueWidthInt;
    ถ้า (type < 0) {
        type = actualType;
    }
    ถ้า (valueWidth < 0) {
        valueWidth = actualValueWidth;
    }
    ถ้า (type != actualType || valueWidth != actualValueWidth) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        คืน nullptr;
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
    ถ้า (valueWidth == UCPTRIE_VALUE_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } มิฉะนั้น ถ้า (valueWidth == UCPTRIE_VALUE_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } มิฉะนั้น {
        actualLength += tempTrie.dataLength;
    }
    ถ้า (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        คืน nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(parimaa_na(UCPTrie));
    ถ้า (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน nullptr;
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
    ถ้า (nullValueOffset >= trie->dataLength) {
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
        คืน nullptr;
    }

    ถ้า (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    คืน trie;
}

U_CAPI void U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(const UCPTrie *trie) {
    คืน (UCPTrieType)trie->type;
}

U_CAPI UCPTrieValueWidth U_EXPORT2
ucptrie_getValueWidth(const UCPTrie *trie) {
    คืน (UCPTrieValueWidth)trie->valueWidth;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(const UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    ถ้า (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->highStart);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } มิฉะนั้น {
        U_ASSERT((uint32_t)c < (uint32_t)trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    ถ้า ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } มิฉะนั้น {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    คืน dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(const UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ถ้า (c >= trie->highStart) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        คืน trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    }
    คืน ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(const UCPTrie *trie, UChar32 c,
                            const uint8_t *start, const uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ถ้า ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } มิฉะนั้น {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    คืน (idx << 3) | i;
}

namespace {

anta_hstha uint32_t getValue(UCPTrieData data, UCPTrieValueWidth valueWidth, int32_t dataIndex) {
    switch (valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        คืน data.ptr16[dataIndex];
    pak_sa UCPTRIE_VALUE_BITS_32:
        คืน data.ptr32[dataIndex];
    pak_sa UCPTRIE_VALUE_BITS_8:
        คืน data.ptr8[dataIndex];
    default:
        // Unreachable if the trie is properly initialized.
        คืน 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(const UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    ถ้า ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } มิฉะนั้น {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    คืน getValue(trie->data, (UCPTrieValueWidth)trie->valueWidth, dataIndex);
}

namespace {

constexpr int32_t MAX_UNICODE = 0x10ffff;

anta_hstha uint32_t maybeFilterValue(uint32_t value, uint32_t trieNullValue, uint32_t nullValue,
                                 UCPMapValueFilter *filter, const void *context) {
    ถ้า (value == trieNullValue) {
        value = nullValue;
    } มิฉะนั้น ถ้า (filter != nullptr) {
        value = filter(context, value);
    }
    คืน value;
}

UChar32 getRange(const void *t, UChar32 start,
                 UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    ถ้า (static_cast<uint32_t>(start) > MAX_UNICODE) {
        คืน U_SENTINEL;
    }
    const UCPTrie *trie = reinterpret_cast<const UCPTrie *>(t);
    UCPTrieValueWidth valueWidth = static_cast<UCPTrieValueWidth>(trie->valueWidth);
    ถ้า (start >= trie->highStart) {
        ถ้า (pValue != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
            uint32_t value = getValue(trie->data, valueWidth, di);
            ถ้า (filter != nullptr) { value = filter(context, value); }
            *pValue = value;
        }
        คืน MAX_UNICODE;
    }

    uint32_t nullValue = trie->nullValue;
    ถ้า (filter != nullptr) { nullValue = filter(context, nullValue); }
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
        ถ้า (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } มิฉะนั้น {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            ถ้า (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->highStart);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } มิฉะนั้น {
                U_ASSERT(c < trie->highStart && trie->highStart > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            ถ้า (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                continue;
            }
            prevI3Block = i3Block;
            ถ้า (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                ถ้า (haveValue) {
                    ถ้า (nullValue != value) {
                        คืน c - 1;
                    }
                } มิฉะนั้น {
                    trieValue = trie->nullValue;
                    value = nullValue;
                    ถ้า (pValue != nullptr) { *pValue = nullValue; }
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
            ถ้า ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } มิฉะนั้น {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            ถ้า (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } มิฉะนั้น {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                ถ้า (block == trie->dataNullOffset) {
                    // This is the data null block.
                    ถ้า (haveValue) {
                        ถ้า (nullValue != value) {
                            คืน c - 1;
                        }
                    } มิฉะนั้น {
                        trieValue = trie->nullValue;
                        value = nullValue;
                        ถ้า (pValue != nullptr) { *pValue = nullValue; }
                        haveValue = true;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } มิฉะนั้น {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = getValue(trie->data, valueWidth, di);
                    ถ้า (haveValue) {
                        ถ้า (trieValue2 != trieValue) {
                            ถ้า (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                คืน c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    } มิฉะนั้น {
                        trieValue = trieValue2;
                        value = maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                 filter, context);
                        ถ้า (pValue != nullptr) { *pValue = value; }
                        haveValue = true;
                    }
                    ขณะ ((++c & dataMask) != 0) {
                        trieValue2 = getValue(trie->data, valueWidth, ++di);
                        ถ้า (trieValue2 != trieValue) {
                            ถ้า (filter == nullptr ||
                                    maybeFilterValue(trieValue2, trie->nullValue, nullValue,
                                                     filter, context) != value) {
                                คืน c - 1;
                            }
                            trieValue = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } ขณะ (++i3 < i3BlockLength);
    } ขณะ (c < trie->highStart);
    U_ASSERT(haveValue);
    int32_t di = trie->dataLength - UCPTRIE_HIGH_VALUE_NEG_DATA_OFFSET;
    uint32_t highValue = getValue(trie->data, valueWidth, di);
    ถ้า (maybeFilterValue(highValue, trie->nullValue, nullValue,
                         filter, context) != value) {
        คืน c - 1;
    } มิฉะนั้น {
        คืน MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         const void *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogateValue,
                         UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    ถ้า (option == UCPMAP_RANGE_NORMAL) {
        คืน getRange(trie, start, filter, context, pValue);
    }
    uint32_t value;
    ถ้า (pValue == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        pValue = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, pValue);
    ถ้า (end < 0xd7ff || start > surrEnd) {
        คืน end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ถ้า (*pValue == surrogateValue) {
        ถ้า (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            คืน end;
        }
    } มิฉะนั้น {
        ถ้า (start <= 0xd7ff) {
            คืน 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *pValue = surrogateValue;
        ถ้า (end > surrEnd) {
            คืน surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    ถ้า (value2 == surrogateValue) {
        คืน end2;
    }
    คืน surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(const UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogateValue,
                 UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    คืน ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogateValue,
                                    filter, context, pValue);
}

U_CAPI int32_t U_EXPORT2
ucptrie_toBinary(const UCPTrie *trie,
                 void *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    ถ้า (U_FAILURE(*pErrorCode)) {
        คืน 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrieValueWidth valueWidth = (UCPTrieValueWidth)trie->valueWidth;
    ถ้า (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            valueWidth < UCPTRIE_VALUE_BITS_16 || UCPTRIE_VALUE_BITS_8 < valueWidth ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน 0;
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
    ถ้า (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        คืน length;
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
    คืน length;
}

namespace {

#ifdef UCPTRIE_DEBUG
long countNull(const UCPTrie *trie) {
    uint32_t nullValue=trie->nullValue;
    int32_t length=trie->dataLength;
    long count=0;
    switch (trie->valueWidth) {
    pak_sa UCPTRIE_VALUE_BITS_16:
        สำหรับ(int32_t i=0; i<length; ++i) {
            ถ้า(trie->data.ptr16[i]==nullValue) { ++count; }
        }
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_32:
        สำหรับ(int32_t i=0; i<length; ++i) {
            ถ้า(trie->data.ptr32[i]==nullValue) { ++count; }
        }
        bha_nga;
    pak_sa UCPTRIE_VALUE_BITS_8:
        สำหรับ(int32_t i=0; i<length; ++i) {
            ถ้า(trie->data.ptr8[i]==nullValue) { ++count; }
        }
        bha_nga;
    default:
        // unreachable
        bha_nga;
    }
    คืน count;
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
    คืน ucptrie_get(reinterpret_cast<const UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(const UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogateValue,
                UCPMapValueFilter *filter, const void *context, uint32_t *pValue) {
    คืน ucptrie_getRange(reinterpret_cast<const UCPTrie *>(map), start,
                            option, surrogateValue,
                            filter, context, pValue);
}
