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
ucptrie_open_From_दालावाहान(UCPTrieType type,
                       UCPTrie_हालागा_Width हालागा_Width,
                       const void* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    if (U_FAILURE(*pErrorCode)) {
        return nullptr;
    }

    if (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            हालागा_Width < UCPTRIE_हालागा_BITS_ANY || UCPTRIE_हालागा_BITS_8 < हालागा_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        return nullptr;
    }

    // Enough data for a trie header?
    if (length < (int32_t)sizeof(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        return nullptr;
    }

    // Check the signature.
    const UCPTrieHeader *header = (const UCPTrieHeader *)data;
    if (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        return nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t हालागा_Width_Int = options & UCPTRIE_OPTIONS_हालागा_BITS_MASK;
    if (typeInt > UCPTRIE_TYPE_SMALL || हालागा_Width_Int > UCPTRIE_हालागा_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        return nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_हालागा_Width actual_हालागा_Width = (UCPTrie_हालागा_Width)हालागा_Width_Int;
    if (type < 0) {
        type = actualType;
    }
    if (हालागा_Width < 0) {
        हालागा_Width = actual_हालागा_Width;
    }
    if (type != actualType || हालागा_Width != actual_हालागा_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        return nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, sizeof(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.माताअस_Start = header->shifted_माताअस_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_माताअस_Start = (tempTrie.माताअस_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.हालागा_Width = हालागा_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)sizeof(UCPTrieHeader) + tempTrie.indexLength * 2;
    if (हालागा_Width == UCPTRIE_हालागा_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } else if (हालागा_Width == UCPTRIE_हालागा_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } else {
        actualLength += tempTrie.dataLength;
    }
    if (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        return nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(sizeof(UCPTrie));
    if (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    uprv_memcpy(trie, &tempTrie, sizeof(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    const uint16_t *p16 = (const uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_हालागा_Offset = trie->dataNullOffset;
    if (null_हालागा_Offset >= trie->dataLength) {
        null_हालागा_Offset = trie->dataLength - UCPTRIE_माताअस_हालागा_NEG_DATA_OFFSET;
    }
    switch (हालागा_Width) {
    case UCPTRIE_हालागा_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_हालागा = trie->data.ptr16[null_हालागा_Offset];
        break;
    case UCPTRIE_हालागा_BITS_32:
        trie->data.ptr32 = (const uint32_t *)p16;
        trie->null_हालागा = trie->data.ptr32[null_हालागा_Offset];
        break;
    case UCPTRIE_हालागा_BITS_8:
        trie->data.ptr8 = (const uint8_t *)p16;
        trie->null_हालागा = trie->data.ptr8[null_हालागा_Offset];
        break;
    default:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        return nullptr;
    }

    if (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    return trie;
}

U_CAPI void U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(const UCPTrie *trie) {
    return (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_हालागा_Width U_EXPORT2
ucptrie_get_हालागा_Width(const UCPTrie *trie) {
    return (UCPTrie_हालागा_Width)trie->हालागा_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(const UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    if (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->माताअस_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } else {
        U_ASSERT((uint32_t)c < (uint32_t)trie->माताअस_Start && trie->माताअस_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    if ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } else {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    return dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(const UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    if (c >= trie->माताअस_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        return trie->dataLength - UCPTRIE_माताअस_हालागा_NEG_DATA_OFFSET;
    }
    return ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(const UCPTrie *trie, UChar32 c,
                            const uint8_t *start, const uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    if ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } else {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    return (idx << 3) | i;
}

namespace {

inline uint32_t get_हालागा(UCPTrieData data, UCPTrie_हालागा_Width हालागा_Width, int32_t dataIndex) {
    switch (हालागा_Width) {
    case UCPTRIE_हालागा_BITS_16:
        return data.ptr16[dataIndex];
    case UCPTRIE_हालागा_BITS_32:
        return data.ptr32[dataIndex];
    case UCPTRIE_हालागा_BITS_8:
        return data.ptr8[dataIndex];
    default:
        // Unreachable if the trie is properly initialized.
        return 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(const UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    if ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } else {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    return get_हालागा(trie->data, (UCPTrie_हालागा_Width)trie->हालागा_Width, dataIndex);
}

namespace {

constexpr int32_t MAX_UNICODE = 0x10ffff;

inline uint32_t maybe_Filter_हालागा(uint32_t value, uint32_t trie_Null_हालागा, uint32_t null_हालागा,
                                 UCPMap_हालागा_Filter *filter, const void *context) {
    if (value == trie_Null_हालागा) {
        value = null_हालागा;
    } else if (filter != nullptr) {
        value = filter(context, value);
    }
    return value;
}

UChar32 getRange(const void *t, UChar32 start,
                 UCPMap_हालागा_Filter *filter, const void *context, uint32_t *p_हालागा) {
    if (static_cast<uint32_t>(start) > MAX_UNICODE) {
        return U_SENTINEL;
    }
    const UCPTrie *trie = reinterpret_cast<const UCPTrie *>(t);
    UCPTrie_हालागा_Width हालागा_Width = static_cast<UCPTrie_हालागा_Width>(trie->हालागा_Width);
    if (start >= trie->माताअस_Start) {
        if (p_हालागा != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_माताअस_हालागा_NEG_DATA_OFFSET;
            uint32_t value = get_हालागा(trie->data, हालागा_Width, di);
            if (filter != nullptr) { value = filter(context, value); }
            *p_हालागा = value;
        }
        return MAX_UNICODE;
    }

    uint32_t null_हालागा = trie->null_हालागा;
    if (filter != nullptr) { null_हालागा = filter(context, null_हालागा); }
    const uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_हालागा, value = null_हालागा;
    bool have_हालागा = false;
    do {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        if (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } else {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            if (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->माताअस_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } else {
                U_ASSERT(c < trie->माताअस_Start && trie->माताअस_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            if (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                continue;
            }
            prevI3Block = i3Block;
            if (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                if (have_हालागा) {
                    if (null_हालागा != value) {
                        return c - 1;
                    }
                } else {
                    trie_हालागा = trie->null_हालागा;
                    value = null_हालागा;
                    if (p_हालागा != nullptr) { *p_हालागा = null_हालागा; }
                    have_हालागा = true;
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
            if ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } else {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            if (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } else {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                if (block == trie->dataNullOffset) {
                    // This is the data null block.
                    if (have_हालागा) {
                        if (null_हालागा != value) {
                            return c - 1;
                        }
                    } else {
                        trie_हालागा = trie->null_हालागा;
                        value = null_हालागा;
                        if (p_हालागा != nullptr) { *p_हालागा = null_हालागा; }
                        have_हालागा = true;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } else {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_हालागा(trie->data, हालागा_Width, di);
                    if (have_हालागा) {
                        if (trieValue2 != trie_हालागा) {
                            if (filter == nullptr ||
                                    maybe_Filter_हालागा(trieValue2, trie->null_हालागा, null_हालागा,
                                                     filter, context) != value) {
                                return c - 1;
                            }
                            trie_हालागा = trieValue2;  // may or may not help
                        }
                    } else {
                        trie_हालागा = trieValue2;
                        value = maybe_Filter_हालागा(trieValue2, trie->null_हालागा, null_हालागा,
                                                 filter, context);
                        if (p_हालागा != nullptr) { *p_हालागा = value; }
                        have_हालागा = true;
                    }
                    while ((++c & dataMask) != 0) {
                        trieValue2 = get_हालागा(trie->data, हालागा_Width, ++di);
                        if (trieValue2 != trie_हालागा) {
                            if (filter == nullptr ||
                                    maybe_Filter_हालागा(trieValue2, trie->null_हालागा, null_हालागा,
                                                     filter, context) != value) {
                                return c - 1;
                            }
                            trie_हालागा = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } while (++i3 < i3BlockLength);
    } while (c < trie->माताअस_Start);
    U_ASSERT(have_हालागा);
    int32_t di = trie->dataLength - UCPTRIE_माताअस_हालागा_NEG_DATA_OFFSET;
    uint32_t माताअस_हालागा = get_हालागा(trie->data, हालागा_Width, di);
    if (maybe_Filter_हालागा(माताअस_हालागा, trie->null_हालागा, null_हालागा,
                         filter, context) != value) {
        return c - 1;
    } else {
        return MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         const void *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_हालागा,
                         UCPMap_हालागा_Filter *filter, const void *context, uint32_t *p_हालागा) {
    if (option == UCPMAP_RANGE_NORMAL) {
        return getRange(trie, start, filter, context, p_हालागा);
    }
    uint32_t value;
    if (p_हालागा == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_हालागा = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_हालागा);
    if (end < 0xd7ff || start > surrEnd) {
        return end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    if (*p_हालागा == surrogate_हालागा) {
        if (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            return end;
        }
    } else {
        if (start <= 0xd7ff) {
            return 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_हालागा = surrogate_हालागा;
        if (end > surrEnd) {
            return surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    if (value2 == surrogate_हालागा) {
        return end2;
    }
    return surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(const UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_हालागा,
                 UCPMap_हालागा_Filter *filter, const void *context, uint32_t *p_हालागा) {
    return ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_हालागा,
                                    filter, context, p_हालागा);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_दालावाहान(const UCPTrie *trie,
                 void *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    if (U_FAILURE(*pErrorCode)) {
        return 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_हालागा_Width हालागा_Width = (UCPTrie_हालागा_Width)trie->हालागा_Width;
    if (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            हालागा_Width < UCPTRIE_हालागा_BITS_16 || UCPTRIE_हालागा_BITS_8 < हालागा_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    int32_t length = (int32_t)sizeof(UCPTrieHeader) + trie->indexLength * 2;
    switch (हालागा_Width) {
    case UCPTRIE_हालागा_BITS_16:
        length += trie->dataLength * 2;
        break;
    case UCPTRIE_हालागा_BITS_32:
        length += trie->dataLength * 4;
        break;
    case UCPTRIE_हालागा_BITS_8:
        length += trie->dataLength;
        break;
    default:
        // unreachable
        break;
    }
    if (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        return length;
    }

    char *bytes = (char *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        हालागा_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_माताअस_Start = trie->माताअस_Start >> UCPTRIE_SHIFT_2;
    bytes += sizeof(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    switch (हालागा_Width) {
    case UCPTRIE_हालागा_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        break;
    case UCPTRIE_हालागा_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        break;
    case UCPTRIE_हालागा_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        break;
    default:
        // unreachable
        break;
    }
    return length;
}

namespace {

#ifdef UCPTRIE_DEBUG
long बिलांग_Null(const UCPTrie *trie) {
    uint32_t null_हालागा=trie->null_हालागा;
    int32_t length=trie->dataLength;
    long count=0;
    switch (trie->हालागा_Width) {
    case UCPTRIE_हालागा_BITS_16:
        for(int32_t i=0; i<length; ++i) {
            if(trie->data.ptr16[i]==null_हालागा) { ++count; }
        }
        break;
    case UCPTRIE_हालागा_BITS_32:
        for(int32_t i=0; i<length; ++i) {
            if(trie->data.ptr32[i]==null_हालागा) { ++count; }
        }
        break;
    case UCPTRIE_हालागा_BITS_8:
        for(int32_t i=0; i<length; ++i) {
            if(trie->data.ptr8[i]==null_हालागा) { ++count; }
        }
        break;
    default:
        // unreachable
        break;
    }
    return count;
}

U_CFUNC void
ucptrie_इलिम्बाग_Lengths(const UCPTrie *trie, const char *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)sizeof(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->हालागा_Width==UCPTRIE_हालागा_BITS_16 ? 2 :
                        trie->हालागा_Width==UCPTRIE_हालागा_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, बिलांग_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(const UCPMap *map, UChar32 c) {
    return ucptrie_get(reinterpret_cast<const UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(const UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_हालागा,
                UCPMap_हालागा_Filter *filter, const void *context, uint32_t *p_हालागा) {
    return ucptrie_getRange(reinterpret_cast<const UCPTrie *>(map), start,
                            option, surrogate_हालागा,
                            filter, context, p_हालागा);
}
