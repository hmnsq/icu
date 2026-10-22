// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#شامل "unicode/utypes.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_دودویی(UCPTrieType type,
                       UCPTrie_مقدار_Width مقدار_Width,
                       ثابت تهی* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    اگر (U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }

    اگر (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            مقدار_Width < UCPTRIE_مقدار_BITS_ANY || UCPTRIE_مقدار_BITS_8 < مقدار_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    // Enough data for a trie header?
    اگر (length < (int32_t)اندازه(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    // Check the signature.
    ثابت UCPTrieHeader *header = (ثابت UCPTrieHeader *)data;
    اگر (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t مقدار_Width_Int = options & UCPTRIE_OPTIONS_مقدار_BITS_MASK;
    اگر (typeInt > UCPTRIE_TYPE_SMALL || مقدار_Width_Int > UCPTRIE_مقدار_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_مقدار_Width actual_مقدار_Width = (UCPTrie_مقدار_Width)مقدار_Width_Int;
    اگر (type < 0) {
        type = actualType;
    }
    اگر (مقدار_Width < 0) {
        مقدار_Width = actual_مقدار_Width;
    }
    اگر (type != actualType || مقدار_Width != actual_مقدار_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, اندازه(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.بالا_Start = header->shifted_بالا_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_بالا_Start = (tempTrie.بالا_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.مقدار_Width = مقدار_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)اندازه(UCPTrieHeader) + tempTrie.indexLength * 2;
    اگر (مقدار_Width == UCPTRIE_مقدار_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } وگرنه اگر (مقدار_Width == UCPTRIE_مقدار_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } وگرنه {
        actualLength += tempTrie.dataLength;
    }
    اگر (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        بازگشت nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(اندازه(UCPTrie));
    اگر (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memcpy(trie, &tempTrie, اندازه(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ثابت uint16_t *p16 = (ثابت uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_مقدار_Offset = trie->dataNullOffset;
    اگر (null_مقدار_Offset >= trie->dataLength) {
        null_مقدار_Offset = trie->dataLength - UCPTRIE_بالا_مقدار_NEG_DATA_OFFSET;
    }
    انتخاب (مقدار_Width) {
    حالت UCPTRIE_مقدار_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_مقدار = trie->data.ptr16[null_مقدار_Offset];
        بشکن;
    حالت UCPTRIE_مقدار_BITS_32:
        trie->data.ptr32 = (ثابت uint32_t *)p16;
        trie->null_مقدار = trie->data.ptr32[null_مقدار_Offset];
        بشکن;
    حالت UCPTRIE_مقدار_BITS_8:
        trie->data.ptr8 = (ثابت uint8_t *)p16;
        trie->null_مقدار = trie->data.ptr8[null_مقدار_Offset];
        بشکن;
    پیشفرض:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    اگر (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    بازگشت trie;
}

U_CAPI تهی U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ثابت UCPTrie *trie) {
    بازگشت (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_مقدار_Width U_EXPORT2
ucptrie_get_مقدار_Width(ثابت UCPTrie *trie) {
    بازگشت (UCPTrie_مقدار_Width)trie->مقدار_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    اگر (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->بالا_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } وگرنه {
        U_ASSERT((uint32_t)c < (uint32_t)trie->بالا_Start && trie->بالا_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    اگر ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } وگرنه {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    بازگشت dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    اگر (c >= trie->بالا_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        بازگشت trie->dataLength - UCPTRIE_بالا_مقدار_NEG_DATA_OFFSET;
    }
    بازگشت ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *start, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    اگر ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } وگرنه {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    بازگشت (idx << 3) | i;
}

فضاینام {

درونخطی uint32_t get_مقدار(UCPTrieData data, UCPTrie_مقدار_Width مقدار_Width, int32_t dataIndex) {
    انتخاب (مقدار_Width) {
    حالت UCPTRIE_مقدار_BITS_16:
        بازگشت data.ptr16[dataIndex];
    حالت UCPTRIE_مقدار_BITS_32:
        بازگشت data.ptr32[dataIndex];
    حالت UCPTRIE_مقدار_BITS_8:
        بازگشت data.ptr8[dataIndex];
    پیشفرض:
        // Unreachable if the trie is properly initialized.
        بازگشت 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    اگر ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } وگرنه {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    بازگشت get_مقدار(trie->data, (UCPTrie_مقدار_Width)trie->مقدار_Width, dataIndex);
}

فضاینام {

constexpr int32_t MAX_UNICODE = 0x10ffff;

درونخطی uint32_t maybe_Filter_مقدار(uint32_t value, uint32_t trie_Null_مقدار, uint32_t null_مقدار,
                                 UCPMap_مقدار_Filter *filter, ثابت تهی *context) {
    اگر (value == trie_Null_مقدار) {
        value = null_مقدار;
    } وگرنه اگر (filter != nullptr) {
        value = filter(context, value);
    }
    بازگشت value;
}

UChar32 getRange(ثابت تهی *t, UChar32 start,
                 UCPMap_مقدار_Filter *filter, ثابت تهی *context, uint32_t *p_مقدار) {
    اگر (static_cast<uint32_t>(start) > MAX_UNICODE) {
        بازگشت U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_مقدار_Width مقدار_Width = static_cast<UCPTrie_مقدار_Width>(trie->مقدار_Width);
    اگر (start >= trie->بالا_Start) {
        اگر (p_مقدار != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_بالا_مقدار_NEG_DATA_OFFSET;
            uint32_t value = get_مقدار(trie->data, مقدار_Width, di);
            اگر (filter != nullptr) { value = filter(context, value); }
            *p_مقدار = value;
        }
        بازگشت MAX_UNICODE;
    }

    uint32_t null_مقدار = trie->null_مقدار;
    اگر (filter != nullptr) { null_مقدار = filter(context, null_مقدار); }
    ثابت uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_مقدار, value = null_مقدار;
    منطقی have_مقدار = نادرست;
    انجامبده {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        اگر (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } وگرنه {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            اگر (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->بالا_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } وگرنه {
                U_ASSERT(c < trie->بالا_Start && trie->بالا_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            اگر (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                ادامه;
            }
            prevI3Block = i3Block;
            اگر (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                اگر (have_مقدار) {
                    اگر (null_مقدار != value) {
                        بازگشت c - 1;
                    }
                } وگرنه {
                    trie_مقدار = trie->null_مقدار;
                    value = null_مقدار;
                    اگر (p_مقدار != nullptr) { *p_مقدار = null_مقدار; }
                    have_مقدار = درست;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                ادامه;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        انجامبده {
            int32_t block;
            اگر ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } وگرنه {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            اگر (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } وگرنه {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                اگر (block == trie->dataNullOffset) {
                    // This is the data null block.
                    اگر (have_مقدار) {
                        اگر (null_مقدار != value) {
                            بازگشت c - 1;
                        }
                    } وگرنه {
                        trie_مقدار = trie->null_مقدار;
                        value = null_مقدار;
                        اگر (p_مقدار != nullptr) { *p_مقدار = null_مقدار; }
                        have_مقدار = درست;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } وگرنه {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_مقدار(trie->data, مقدار_Width, di);
                    اگر (have_مقدار) {
                        اگر (trieValue2 != trie_مقدار) {
                            اگر (filter == nullptr ||
                                    maybe_Filter_مقدار(trieValue2, trie->null_مقدار, null_مقدار,
                                                     filter, context) != value) {
                                بازگشت c - 1;
                            }
                            trie_مقدار = trieValue2;  // may or may not help
                        }
                    } وگرنه {
                        trie_مقدار = trieValue2;
                        value = maybe_Filter_مقدار(trieValue2, trie->null_مقدار, null_مقدار,
                                                 filter, context);
                        اگر (p_مقدار != nullptr) { *p_مقدار = value; }
                        have_مقدار = درست;
                    }
                    وقتی ((++c & dataMask) != 0) {
                        trieValue2 = get_مقدار(trie->data, مقدار_Width, ++di);
                        اگر (trieValue2 != trie_مقدار) {
                            اگر (filter == nullptr ||
                                    maybe_Filter_مقدار(trieValue2, trie->null_مقدار, null_مقدار,
                                                     filter, context) != value) {
                                بازگشت c - 1;
                            }
                            trie_مقدار = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } وقتی (++i3 < i3BlockLength);
    } وقتی (c < trie->بالا_Start);
    U_ASSERT(have_مقدار);
    int32_t di = trie->dataLength - UCPTRIE_بالا_مقدار_NEG_DATA_OFFSET;
    uint32_t بالا_مقدار = get_مقدار(trie->data, مقدار_Width, di);
    اگر (maybe_Filter_مقدار(بالا_مقدار, trie->null_مقدار, null_مقدار,
                         filter, context) != value) {
        بازگشت c - 1;
    } وگرنه {
        بازگشت MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ثابت تهی *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_مقدار,
                         UCPMap_مقدار_Filter *filter, ثابت تهی *context, uint32_t *p_مقدار) {
    اگر (option == UCPMAP_RANGE_NORMAL) {
        بازگشت getRange(trie, start, filter, context, p_مقدار);
    }
    uint32_t value;
    اگر (p_مقدار == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_مقدار = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_مقدار);
    اگر (end < 0xd7ff || start > surrEnd) {
        بازگشت end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    اگر (*p_مقدار == surrogate_مقدار) {
        اگر (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            بازگشت end;
        }
    } وگرنه {
        اگر (start <= 0xd7ff) {
            بازگشت 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_مقدار = surrogate_مقدار;
        اگر (end > surrEnd) {
            بازگشت surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    اگر (value2 == surrogate_مقدار) {
        بازگشت end2;
    }
    بازگشت surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_مقدار,
                 UCPMap_مقدار_Filter *filter, ثابت تهی *context, uint32_t *p_مقدار) {
    بازگشت ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_مقدار,
                                    filter, context, p_مقدار);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_دودویی(ثابت UCPTrie *trie,
                 تهی *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    اگر (U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_مقدار_Width مقدار_Width = (UCPTrie_مقدار_Width)trie->مقدار_Width;
    اگر (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            مقدار_Width < UCPTRIE_مقدار_BITS_16 || UCPTRIE_مقدار_BITS_8 < مقدار_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    int32_t length = (int32_t)اندازه(UCPTrieHeader) + trie->indexLength * 2;
    انتخاب (مقدار_Width) {
    حالت UCPTRIE_مقدار_BITS_16:
        length += trie->dataLength * 2;
        بشکن;
    حالت UCPTRIE_مقدار_BITS_32:
        length += trie->dataLength * 4;
        بشکن;
    حالت UCPTRIE_مقدار_BITS_8:
        length += trie->dataLength;
        بشکن;
    پیشفرض:
        // unreachable
        بشکن;
    }
    اگر (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        بازگشت length;
    }

    نویسه *bytes = (نویسه *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        مقدار_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_بالا_Start = trie->بالا_Start >> UCPTRIE_SHIFT_2;
    bytes += اندازه(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    انتخاب (مقدار_Width) {
    حالت UCPTRIE_مقدار_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        بشکن;
    حالت UCPTRIE_مقدار_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        بشکن;
    حالت UCPTRIE_مقدار_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        بشکن;
    پیشفرض:
        // unreachable
        بشکن;
    }
    بازگشت length;
}

فضاینام {

#ifdef UCPTRIE_DEBUG
long شمار_Null(ثابت UCPTrie *trie) {
    uint32_t null_مقدار=trie->null_مقدار;
    int32_t length=trie->dataLength;
    long count=0;
    انتخاب (trie->مقدار_Width) {
    حالت UCPTRIE_مقدار_BITS_16:
        برای(int32_t i=0; i<length; ++i) {
            اگر(trie->data.ptr16[i]==null_مقدار) { ++count; }
        }
        بشکن;
    حالت UCPTRIE_مقدار_BITS_32:
        برای(int32_t i=0; i<length; ++i) {
            اگر(trie->data.ptr32[i]==null_مقدار) { ++count; }
        }
        بشکن;
    حالت UCPTRIE_مقدار_BITS_8:
        برای(int32_t i=0; i<length; ++i) {
            اگر(trie->data.ptr8[i]==null_مقدار) { ++count; }
        }
        بشکن;
    پیشفرض:
        // unreachable
        بشکن;
    }
    بازگشت count;
}

U_CFUNC تهی
ucptrie_printLengths(ثابت UCPTrie *trie, ثابت نویسه *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)اندازه(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->مقدار_Width==UCPTRIE_مقدار_BITS_16 ? 2 :
                        trie->مقدار_Width==UCPTRIE_مقدار_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, شمار_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    بازگشت ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_مقدار,
                UCPMap_مقدار_Filter *filter, ثابت تهی *context, uint32_t *p_مقدار) {
    بازگشت ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), start,
                            option, surrogate_مقدار,
                            filter, context, p_مقدار);
}
