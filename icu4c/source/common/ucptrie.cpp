// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/ucptrie.h"
#অন্তর্ভাৱ "unicode/utf.h"
#অন্তর্ভাৱ "unicode/utf8.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_দ্বিমিক(UCPTrieType type,
                       UCPTrie_মান_Width মান_Width,
                       ধ্রুৱ শূন্য* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }

    যদি (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            মান_Width < UCPTRIE_মান_BITS_ANY || UCPTRIE_মান_BITS_8 < মান_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    // Enough data for a trie header?
    যদি (length < (int32_t)পরিমাণ(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ঘূৰাই nullptr;
    }

    // Check the signature.
    ধ্রুৱ UCPTrieHeader *header = (ধ্রুৱ UCPTrieHeader *)data;
    যদি (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ঘূৰাই nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t মান_Width_Int = options & UCPTRIE_OPTIONS_মান_BITS_MASK;
    যদি (typeInt > UCPTRIE_TYPE_SMALL || মান_Width_Int > UCPTRIE_মান_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ঘূৰাই nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_মান_Width actual_মান_Width = (UCPTrie_মান_Width)মান_Width_Int;
    যদি (type < 0) {
        type = actualType;
    }
    যদি (মান_Width < 0) {
        মান_Width = actual_মান_Width;
    }
    যদি (type != actualType || মান_Width != actual_মান_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ঘূৰাই nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, পরিমাণ(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.উপৰেৰ_Start = header->shifted_উপৰেৰ_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_উপৰেৰ_Start = (tempTrie.উপৰেৰ_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.মান_Width = মান_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)পরিমাণ(UCPTrieHeader) + tempTrie.indexLength * 2;
    যদি (মান_Width == UCPTRIE_মান_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } অন্যথা যদি (মান_Width == UCPTRIE_মান_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } অন্যথা {
        actualLength += tempTrie.dataLength;
    }
    যদি (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ঘূৰাই nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(পরিমাণ(UCPTrie));
    যদি (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    uprv_memcpy(trie, &tempTrie, পরিমাণ(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ধ্রুৱ uint16_t *p16 = (ধ্রুৱ uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_মান_Offset = trie->dataNullOffset;
    যদি (null_মান_Offset >= trie->dataLength) {
        null_মান_Offset = trie->dataLength - UCPTRIE_উপৰেৰ_মান_NEG_DATA_OFFSET;
    }
    ৱিকল্প (মান_Width) {
    পক্ষ UCPTRIE_মান_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_মান = trie->data.ptr16[null_মান_Offset];
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_32:
        trie->data.ptr32 = (ধ্রুৱ uint32_t *)p16;
        trie->null_মান = trie->data.ptr32[null_মান_Offset];
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_8:
        trie->data.ptr8 = (ধ্রুৱ uint8_t *)p16;
        trie->null_মান = trie->data.ptr8[null_মান_Offset];
        ভঙ্গ;
    স্ৱভাৱ:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ঘূৰাই nullptr;
    }

    যদি (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    ঘূৰাই trie;
}

U_CAPI শূন্য U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ধ্রুৱ UCPTrie *trie) {
    ঘূৰাই (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_মান_Width U_EXPORT2
ucptrie_get_মান_Width(ধ্রুৱ UCPTrie *trie) {
    ঘূৰাই (UCPTrie_মান_Width)trie->মান_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(ধ্রুৱ UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    যদি (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->উপৰেৰ_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } অন্যথা {
        U_ASSERT((uint32_t)c < (uint32_t)trie->উপৰেৰ_Start && trie->উপৰেৰ_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    যদি ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } অন্যথা {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    ঘূৰাই dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(ধ্রুৱ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    যদি (c >= trie->উপৰেৰ_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ঘূৰাই trie->dataLength - UCPTRIE_উপৰেৰ_মান_NEG_DATA_OFFSET;
    }
    ঘূৰাই ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(ধ্রুৱ UCPTrie *trie, UChar32 c,
                            ধ্রুৱ uint8_t *start, ধ্রুৱ uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    যদি ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } অন্যথা {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    ঘূৰাই (idx << 3) | i;
}

নামাকাশ {

অন্তঃস্থ uint32_t get_মান(UCPTrieData data, UCPTrie_মান_Width মান_Width, int32_t dataIndex) {
    ৱিকল্প (মান_Width) {
    পক্ষ UCPTRIE_মান_BITS_16:
        ঘূৰাই data.ptr16[dataIndex];
    পক্ষ UCPTRIE_মান_BITS_32:
        ঘূৰাই data.ptr32[dataIndex];
    পক্ষ UCPTRIE_মান_BITS_8:
        ঘূৰাই data.ptr8[dataIndex];
    স্ৱভাৱ:
        // Unreachable if the trie is properly initialized.
        ঘূৰাই 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ধ্রুৱ UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    যদি ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } অন্যথা {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    ঘূৰাই get_মান(trie->data, (UCPTrie_মান_Width)trie->মান_Width, dataIndex);
}

নামাকাশ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

অন্তঃস্থ uint32_t maybe_Filter_মান(uint32_t value, uint32_t trie_Null_মান, uint32_t null_মান,
                                 UCPMap_মান_Filter *filter, ধ্রুৱ শূন্য *context) {
    যদি (value == trie_Null_মান) {
        value = null_মান;
    } অন্যথা যদি (filter != nullptr) {
        value = filter(context, value);
    }
    ঘূৰাই value;
}

UChar32 getRange(ধ্রুৱ শূন্য *t, UChar32 start,
                 UCPMap_মান_Filter *filter, ধ্রুৱ শূন্য *context, uint32_t *p_মান) {
    যদি (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ঘূৰাই U_SENTINEL;
    }
    ধ্রুৱ UCPTrie *trie = reinterpret_cast<ধ্রুৱ UCPTrie *>(t);
    UCPTrie_মান_Width মান_Width = static_cast<UCPTrie_মান_Width>(trie->মান_Width);
    যদি (start >= trie->উপৰেৰ_Start) {
        যদি (p_মান != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_উপৰেৰ_মান_NEG_DATA_OFFSET;
            uint32_t value = get_মান(trie->data, মান_Width, di);
            যদি (filter != nullptr) { value = filter(context, value); }
            *p_মান = value;
        }
        ঘূৰাই MAX_UNICODE;
    }

    uint32_t null_মান = trie->null_মান;
    যদি (filter != nullptr) { null_মান = filter(context, null_মান); }
    ধ্রুৱ uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_মান, value = null_মান;
    তার্কিক have_মান = মিছা;
    কৰা {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        যদি (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } অন্যথা {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            যদি (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->উপৰেৰ_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } অন্যথা {
                U_ASSERT(c < trie->উপৰেৰ_Start && trie->উপৰেৰ_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            যদি (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                অনুৱর্ত;
            }
            prevI3Block = i3Block;
            যদি (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                যদি (have_মান) {
                    যদি (null_মান != value) {
                        ঘূৰাই c - 1;
                    }
                } অন্যথা {
                    trie_মান = trie->null_মান;
                    value = null_মান;
                    যদি (p_মান != nullptr) { *p_মান = null_মান; }
                    have_মান = সঁচা;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                অনুৱর্ত;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        কৰা {
            int32_t block;
            যদি ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } অন্যথা {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            যদি (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } অন্যথা {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                যদি (block == trie->dataNullOffset) {
                    // This is the data null block.
                    যদি (have_মান) {
                        যদি (null_মান != value) {
                            ঘূৰাই c - 1;
                        }
                    } অন্যথা {
                        trie_মান = trie->null_মান;
                        value = null_মান;
                        যদি (p_মান != nullptr) { *p_মান = null_মান; }
                        have_মান = সঁচা;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } অন্যথা {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_মান(trie->data, মান_Width, di);
                    যদি (have_মান) {
                        যদি (trieValue2 != trie_মান) {
                            যদি (filter == nullptr ||
                                    maybe_Filter_মান(trieValue2, trie->null_মান, null_মান,
                                                     filter, context) != value) {
                                ঘূৰাই c - 1;
                            }
                            trie_মান = trieValue2;  // may or may not help
                        }
                    } অন্যথা {
                        trie_মান = trieValue2;
                        value = maybe_Filter_মান(trieValue2, trie->null_মান, null_মান,
                                                 filter, context);
                        যদি (p_মান != nullptr) { *p_মান = value; }
                        have_মান = সঁচা;
                    }
                    যেতিয়ালৈ ((++c & dataMask) != 0) {
                        trieValue2 = get_মান(trie->data, মান_Width, ++di);
                        যদি (trieValue2 != trie_মান) {
                            যদি (filter == nullptr ||
                                    maybe_Filter_মান(trieValue2, trie->null_মান, null_মান,
                                                     filter, context) != value) {
                                ঘূৰাই c - 1;
                            }
                            trie_মান = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } যেতিয়ালৈ (++i3 < i3BlockLength);
    } যেতিয়ালৈ (c < trie->উপৰেৰ_Start);
    U_ASSERT(have_মান);
    int32_t di = trie->dataLength - UCPTRIE_উপৰেৰ_মান_NEG_DATA_OFFSET;
    uint32_t উপৰেৰ_মান = get_মান(trie->data, মান_Width, di);
    যদি (maybe_Filter_মান(উপৰেৰ_মান, trie->null_মান, null_মান,
                         filter, context) != value) {
        ঘূৰাই c - 1;
    } অন্যথা {
        ঘূৰাই MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ধ্রুৱ শূন্য *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_মান,
                         UCPMap_মান_Filter *filter, ধ্রুৱ শূন্য *context, uint32_t *p_মান) {
    যদি (option == UCPMAP_RANGE_NORMAL) {
        ঘূৰাই getRange(trie, start, filter, context, p_মান);
    }
    uint32_t value;
    যদি (p_মান == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_মান = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_মান);
    যদি (end < 0xd7ff || start > surrEnd) {
        ঘূৰাই end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    যদি (*p_মান == surrogate_মান) {
        যদি (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ঘূৰাই end;
        }
    } অন্যথা {
        যদি (start <= 0xd7ff) {
            ঘূৰাই 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_মান = surrogate_মান;
        যদি (end > surrEnd) {
            ঘূৰাই surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    যদি (value2 == surrogate_মান) {
        ঘূৰাই end2;
    }
    ঘূৰাই surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ধ্রুৱ UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_মান,
                 UCPMap_মান_Filter *filter, ধ্রুৱ শূন্য *context, uint32_t *p_মান) {
    ঘূৰাই ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_মান,
                                    filter, context, p_মান);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_দ্বিমিক(ধ্রুৱ UCPTrie *trie,
                 শূন্য *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_মান_Width মান_Width = (UCPTrie_মান_Width)trie->মান_Width;
    যদি (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            মান_Width < UCPTRIE_মান_BITS_16 || UCPTRIE_মান_BITS_8 < মান_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    int32_t length = (int32_t)পরিমাণ(UCPTrieHeader) + trie->indexLength * 2;
    ৱিকল্প (মান_Width) {
    পক্ষ UCPTRIE_মান_BITS_16:
        length += trie->dataLength * 2;
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_32:
        length += trie->dataLength * 4;
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_8:
        length += trie->dataLength;
        ভঙ্গ;
    স্ৱভাৱ:
        // unreachable
        ভঙ্গ;
    }
    যদি (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        ঘূৰাই length;
    }

    ৱর্ণ *bytes = (ৱর্ণ *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        মান_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_উপৰেৰ_Start = trie->উপৰেৰ_Start >> UCPTRIE_SHIFT_2;
    bytes += পরিমাণ(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    ৱিকল্প (মান_Width) {
    পক্ষ UCPTRIE_মান_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        ভঙ্গ;
    স্ৱভাৱ:
        // unreachable
        ভঙ্গ;
    }
    ঘূৰাই length;
}

নামাকাশ {

#ifdef UCPTRIE_DEBUG
long গুনতি_Null(ধ্রুৱ UCPTrie *trie) {
    uint32_t null_মান=trie->null_মান;
    int32_t length=trie->dataLength;
    long count=0;
    ৱিকল্প (trie->মান_Width) {
    পক্ষ UCPTRIE_মান_BITS_16:
        বাবে(int32_t i=0; i<length; ++i) {
            যদি(trie->data.ptr16[i]==null_মান) { ++count; }
        }
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_32:
        বাবে(int32_t i=0; i<length; ++i) {
            যদি(trie->data.ptr32[i]==null_মান) { ++count; }
        }
        ভঙ্গ;
    পক্ষ UCPTRIE_মান_BITS_8:
        বাবে(int32_t i=0; i<length; ++i) {
            যদি(trie->data.ptr8[i]==null_মান) { ++count; }
        }
        ভঙ্গ;
    স্ৱভাৱ:
        // unreachable
        ভঙ্গ;
    }
    ঘূৰাই count;
}

U_CFUNC শূন্য
ucptrie_printLengths(ধ্রুৱ UCPTrie *trie, ধ্রুৱ ৱর্ণ *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)পরিমাণ(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->মান_Width==UCPTRIE_মান_BITS_16 ? 2 :
                        trie->মান_Width==UCPTRIE_মান_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, গুনতি_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ধ্রুৱ UCPMap *map, UChar32 c) {
    ঘূৰাই ucptrie_get(reinterpret_cast<ধ্রুৱ UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ধ্রুৱ UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_মান,
                UCPMap_মান_Filter *filter, ধ্রুৱ শূন্য *context, uint32_t *p_মান) {
    ঘূৰাই ucptrie_getRange(reinterpret_cast<ধ্রুৱ UCPTrie *>(map), start,
                            option, surrogate_মান,
                            filter, context, p_মান);
}
