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
ucptrie_open_From_ثنائي(UCPTrieType type,
                       UCPTrie_قيمة_Width قيمة_Width,
                       ثابت فراغ* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    إذا (U_FAILURE(*pErrorCode)) {
        إرجاع nullptr;
    }

    إذا (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            قيمة_Width < UCPTRIE_قيمة_BITS_ANY || UCPTRIE_قيمة_BITS_8 < قيمة_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع nullptr;
    }

    // Enough data for a trie header?
    إذا (length < (int32_t)حجم(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        إرجاع nullptr;
    }

    // Check the signature.
    ثابت UCPTrieHeader *header = (ثابت UCPTrieHeader *)data;
    إذا (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        إرجاع nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t قيمة_Width_Int = options & UCPTRIE_OPTIONS_قيمة_BITS_MASK;
    إذا (typeInt > UCPTRIE_TYPE_SMALL || قيمة_Width_Int > UCPTRIE_قيمة_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        إرجاع nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_قيمة_Width actual_قيمة_Width = (UCPTrie_قيمة_Width)قيمة_Width_Int;
    إذا (type < 0) {
        type = actualType;
    }
    إذا (قيمة_Width < 0) {
        قيمة_Width = actual_قيمة_Width;
    }
    إذا (type != actualType || قيمة_Width != actual_قيمة_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        إرجاع nullptr;
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

    tempTrie.أعلى_Start = header->shifted_أعلى_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_أعلى_Start = (tempTrie.أعلى_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.قيمة_Width = قيمة_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)حجم(UCPTrieHeader) + tempTrie.indexLength * 2;
    إذا (قيمة_Width == UCPTRIE_قيمة_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } وإلا إذا (قيمة_Width == UCPTRIE_قيمة_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } وإلا {
        actualLength += tempTrie.dataLength;
    }
    إذا (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        إرجاع nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(حجم(UCPTrie));
    إذا (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        إرجاع nullptr;
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
    int32_t null_قيمة_Offset = trie->dataNullOffset;
    إذا (null_قيمة_Offset >= trie->dataLength) {
        null_قيمة_Offset = trie->dataLength - UCPTRIE_أعلى_قيمة_NEG_DATA_OFFSET;
    }
    بدل (قيمة_Width) {
    حالة UCPTRIE_قيمة_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_قيمة = trie->data.ptr16[null_قيمة_Offset];
        اكسر;
    حالة UCPTRIE_قيمة_BITS_32:
        trie->data.ptr32 = (ثابت uint32_t *)p16;
        trie->null_قيمة = trie->data.ptr32[null_قيمة_Offset];
        اكسر;
    حالة UCPTRIE_قيمة_BITS_8:
        trie->data.ptr8 = (ثابت uint8_t *)p16;
        trie->null_قيمة = trie->data.ptr8[null_قيمة_Offset];
        اكسر;
    افتراضي:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        إرجاع nullptr;
    }

    إذا (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    إرجاع trie;
}

U_CAPI فراغ U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ثابت UCPTrie *trie) {
    إرجاع (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_قيمة_Width U_EXPORT2
ucptrie_get_قيمة_Width(ثابت UCPTrie *trie) {
    إرجاع (UCPTrie_قيمة_Width)trie->قيمة_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ثابت UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    إذا (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->أعلى_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } وإلا {
        U_ASSERT((uint32_t)c < (uint32_t)trie->أعلى_Start && trie->أعلى_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    إذا ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } وإلا {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    إرجاع dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ثابت UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    إذا (c >= trie->أعلى_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        إرجاع trie->dataLength - UCPTRIE_أعلى_قيمة_NEG_DATA_OFFSET;
    }
    إرجاع ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ثابت UCPTrie *trie, UChar32 c,
                            ثابت uint8_t *start, ثابت uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    إذا ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } وإلا {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    إرجاع (idx << 3) | i;
}

نطاق {

مضمن uint32_t get_قيمة(UCPTrieData data, UCPTrie_قيمة_Width قيمة_Width, int32_t dataIndex) {
    بدل (قيمة_Width) {
    حالة UCPTRIE_قيمة_BITS_16:
        إرجاع data.ptr16[dataIndex];
    حالة UCPTRIE_قيمة_BITS_32:
        إرجاع data.ptr32[dataIndex];
    حالة UCPTRIE_قيمة_BITS_8:
        إرجاع data.ptr8[dataIndex];
    افتراضي:
        // Unreachable if the trie is properly initialized.
        إرجاع 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ثابت UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    إذا ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } وإلا {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    إرجاع get_قيمة(trie->data, (UCPTrie_قيمة_Width)trie->قيمة_Width, dataIndex);
}

نطاق {

constexpr int32_t MAX_UNICODE = 0x10ffff;

مضمن uint32_t maybe_Filter_قيمة(uint32_t value, uint32_t trie_Null_قيمة, uint32_t null_قيمة,
                                 UCPMap_قيمة_Filter *filter, ثابت فراغ *context) {
    إذا (value == trie_Null_قيمة) {
        value = null_قيمة;
    } وإلا إذا (filter != nullptr) {
        value = filter(context, value);
    }
    إرجاع value;
}

UChar32 getRange(ثابت فراغ *t, UChar32 start,
                 UCPMap_قيمة_Filter *filter, ثابت فراغ *context, uint32_t *p_قيمة) {
    إذا (static_cast<uint32_t>(start) > MAX_UNICODE) {
        إرجاع U_SENTINEL;
    }
    ثابت UCPTrie *trie = reinterpret_cast<ثابت UCPTrie *>(t);
    UCPTrie_قيمة_Width قيمة_Width = static_cast<UCPTrie_قيمة_Width>(trie->قيمة_Width);
    إذا (start >= trie->أعلى_Start) {
        إذا (p_قيمة != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_أعلى_قيمة_NEG_DATA_OFFSET;
            uint32_t value = get_قيمة(trie->data, قيمة_Width, di);
            إذا (filter != nullptr) { value = filter(context, value); }
            *p_قيمة = value;
        }
        إرجاع MAX_UNICODE;
    }

    uint32_t null_قيمة = trie->null_قيمة;
    إذا (filter != nullptr) { null_قيمة = filter(context, null_قيمة); }
    ثابت uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_قيمة, value = null_قيمة;
    منطقي have_قيمة = خطأ;
    افعل {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        إذا (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } وإلا {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            إذا (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->أعلى_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } وإلا {
                U_ASSERT(c < trie->أعلى_Start && trie->أعلى_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            إذا (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                استمر;
            }
            prevI3Block = i3Block;
            إذا (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                إذا (have_قيمة) {
                    إذا (null_قيمة != value) {
                        إرجاع c - 1;
                    }
                } وإلا {
                    trie_قيمة = trie->null_قيمة;
                    value = null_قيمة;
                    إذا (p_قيمة != nullptr) { *p_قيمة = null_قيمة; }
                    have_قيمة = true;
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
        افعل {
            int32_t block;
            إذا ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } وإلا {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            إذا (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } وإلا {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                إذا (block == trie->dataNullOffset) {
                    // This is the data null block.
                    إذا (have_قيمة) {
                        إذا (null_قيمة != value) {
                            إرجاع c - 1;
                        }
                    } وإلا {
                        trie_قيمة = trie->null_قيمة;
                        value = null_قيمة;
                        إذا (p_قيمة != nullptr) { *p_قيمة = null_قيمة; }
                        have_قيمة = true;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } وإلا {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_قيمة(trie->data, قيمة_Width, di);
                    إذا (have_قيمة) {
                        إذا (trieValue2 != trie_قيمة) {
                            إذا (filter == nullptr ||
                                    maybe_Filter_قيمة(trieValue2, trie->null_قيمة, null_قيمة,
                                                     filter, context) != value) {
                                إرجاع c - 1;
                            }
                            trie_قيمة = trieValue2;  // may or may not help
                        }
                    } وإلا {
                        trie_قيمة = trieValue2;
                        value = maybe_Filter_قيمة(trieValue2, trie->null_قيمة, null_قيمة,
                                                 filter, context);
                        إذا (p_قيمة != nullptr) { *p_قيمة = value; }
                        have_قيمة = true;
                    }
                    بينما ((++c & dataMask) != 0) {
                        trieValue2 = get_قيمة(trie->data, قيمة_Width, ++di);
                        إذا (trieValue2 != trie_قيمة) {
                            إذا (filter == nullptr ||
                                    maybe_Filter_قيمة(trieValue2, trie->null_قيمة, null_قيمة,
                                                     filter, context) != value) {
                                إرجاع c - 1;
                            }
                            trie_قيمة = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } بينما (++i3 < i3BlockLength);
    } بينما (c < trie->أعلى_Start);
    U_ASSERT(have_قيمة);
    int32_t di = trie->dataLength - UCPTRIE_أعلى_قيمة_NEG_DATA_OFFSET;
    uint32_t أعلى_قيمة = get_قيمة(trie->data, قيمة_Width, di);
    إذا (maybe_Filter_قيمة(أعلى_قيمة, trie->null_قيمة, null_قيمة,
                         filter, context) != value) {
        إرجاع c - 1;
    } وإلا {
        إرجاع MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ثابت فراغ *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_قيمة,
                         UCPMap_قيمة_Filter *filter, ثابت فراغ *context, uint32_t *p_قيمة) {
    إذا (option == UCPMAP_RANGE_NORMAL) {
        إرجاع getRange(trie, start, filter, context, p_قيمة);
    }
    uint32_t value;
    إذا (p_قيمة == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_قيمة = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_قيمة);
    إذا (end < 0xd7ff || start > surrEnd) {
        إرجاع end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    إذا (*p_قيمة == surrogate_قيمة) {
        إذا (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            إرجاع end;
        }
    } وإلا {
        إذا (start <= 0xd7ff) {
            إرجاع 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_قيمة = surrogate_قيمة;
        إذا (end > surrEnd) {
            إرجاع surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    إذا (value2 == surrogate_قيمة) {
        إرجاع end2;
    }
    إرجاع surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ثابت UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_قيمة,
                 UCPMap_قيمة_Filter *filter, ثابت فراغ *context, uint32_t *p_قيمة) {
    إرجاع ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_قيمة,
                                    filter, context, p_قيمة);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_ثنائي(ثابت UCPTrie *trie,
                 فراغ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    إذا (U_FAILURE(*pErrorCode)) {
        إرجاع 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_قيمة_Width قيمة_Width = (UCPTrie_قيمة_Width)trie->قيمة_Width;
    إذا (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            قيمة_Width < UCPTRIE_قيمة_BITS_16 || UCPTRIE_قيمة_BITS_8 < قيمة_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }

    int32_t length = (int32_t)حجم(UCPTrieHeader) + trie->indexLength * 2;
    بدل (قيمة_Width) {
    حالة UCPTRIE_قيمة_BITS_16:
        length += trie->dataLength * 2;
        اكسر;
    حالة UCPTRIE_قيمة_BITS_32:
        length += trie->dataLength * 4;
        اكسر;
    حالة UCPTRIE_قيمة_BITS_8:
        length += trie->dataLength;
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إذا (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        إرجاع length;
    }

    حرف *bytes = (حرف *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        قيمة_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_أعلى_Start = trie->أعلى_Start >> UCPTRIE_SHIFT_2;
    bytes += حجم(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    بدل (قيمة_Width) {
    حالة UCPTRIE_قيمة_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        اكسر;
    حالة UCPTRIE_قيمة_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        اكسر;
    حالة UCPTRIE_قيمة_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إرجاع length;
}

نطاق {

#ifdef UCPTRIE_DEBUG
long تعداد_Null(ثابت UCPTrie *trie) {
    uint32_t null_قيمة=trie->null_قيمة;
    int32_t length=trie->dataLength;
    long count=0;
    بدل (trie->قيمة_Width) {
    حالة UCPTRIE_قيمة_BITS_16:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data.ptr16[i]==null_قيمة) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_قيمة_BITS_32:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data.ptr32[i]==null_قيمة) { ++count; }
        }
        اكسر;
    حالة UCPTRIE_قيمة_BITS_8:
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data.ptr8[i]==null_قيمة) { ++count; }
        }
        اكسر;
    افتراضي:
        // unreachable
        اكسر;
    }
    إرجاع count;
}

U_CFUNC فراغ
ucptrie_printLengths(ثابت UCPTrie *trie, ثابت حرف *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)حجم(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->قيمة_Width==UCPTRIE_قيمة_BITS_16 ? 2 :
                        trie->قيمة_Width==UCPTRIE_قيمة_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, تعداد_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ثابت UCPMap *map, UChar32 c) {
    إرجاع ucptrie_get(reinterpret_cast<ثابت UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ثابت UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_قيمة,
                UCPMap_قيمة_Filter *filter, ثابت فراغ *context, uint32_t *p_قيمة) {
    إرجاع ucptrie_getRange(reinterpret_cast<ثابت UCPTrie *>(map), start,
                            option, surrogate_قيمة,
                            filter, context, p_قيمة);
}
