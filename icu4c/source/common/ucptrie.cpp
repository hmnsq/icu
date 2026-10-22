// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_இருமம்(UCPTrieType type,
                       UCPTrie_மதிப்பு_Width மதிப்பு_Width,
                       மாறிலி வெறுமை* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    எனில் (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            மதிப்பு_Width < UCPTRIE_மதிப்பு_BITS_ANY || UCPTRIE_மதிப்பு_BITS_8 < மதிப்பு_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    // Enough data for a trie header?
    எனில் (length < (int32_t)அளவு(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    // Check the signature.
    மாறிலி UCPTrieHeader *header = (மாறிலி UCPTrieHeader *)data;
    எனில் (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t மதிப்பு_Width_Int = options & UCPTRIE_OPTIONS_மதிப்பு_BITS_MASK;
    எனில் (typeInt > UCPTRIE_TYPE_SMALL || மதிப்பு_Width_Int > UCPTRIE_மதிப்பு_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_மதிப்பு_Width actual_மதிப்பு_Width = (UCPTrie_மதிப்பு_Width)மதிப்பு_Width_Int;
    எனில் (type < 0) {
        type = actualType;
    }
    எனில் (மதிப்பு_Width < 0) {
        மதிப்பு_Width = actual_மதிப்பு_Width;
    }
    எனில் (type != actualType || மதிப்பு_Width != actual_மதிப்பு_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, அளவு(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.உயர்வு_Start = header->shifted_உயர்வு_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_உயர்வு_Start = (tempTrie.உயர்வு_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.மதிப்பு_Width = மதிப்பு_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)அளவு(UCPTrieHeader) + tempTrie.indexLength * 2;
    எனில் (மதிப்பு_Width == UCPTRIE_மதிப்பு_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } இல்லையெனில் எனில் (மதிப்பு_Width == UCPTRIE_மதிப்பு_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } இல்லையெனில் {
        actualLength += tempTrie.dataLength;
    }
    எனில் (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        திரும்பு nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(அளவு(UCPTrie));
    எனில் (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memcpy(trie, &tempTrie, அளவு(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    மாறிலி uint16_t *p16 = (மாறிலி uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_மதிப்பு_Offset = trie->dataNullOffset;
    எனில் (null_மதிப்பு_Offset >= trie->dataLength) {
        null_மதிப்பு_Offset = trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET;
    }
    தேர்வு (மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_மதிப்பு = trie->data.ptr16[null_மதிப்பு_Offset];
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        trie->data.ptr32 = (மாறிலி uint32_t *)p16;
        trie->null_மதிப்பு = trie->data.ptr32[null_மதிப்பு_Offset];
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        trie->data.ptr8 = (மாறிலி uint8_t *)p16;
        trie->null_மதிப்பு = trie->data.ptr8[null_மதிப்பு_Offset];
        நிறுத்து;
    இயல்பு:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    எனில் (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    திரும்பு trie;
}

U_CAPI வெறுமை U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(மாறிலி UCPTrie *trie) {
    திரும்பு (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_மதிப்பு_Width U_EXPORT2
ucptrie_get_மதிப்பு_Width(மாறிலி UCPTrie *trie) {
    திரும்பு (UCPTrie_மதிப்பு_Width)trie->மதிப்பு_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(மாறிலி UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    எனில் (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->உயர்வு_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } இல்லையெனில் {
        U_ASSERT((uint32_t)c < (uint32_t)trie->உயர்வு_Start && trie->உயர்வு_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    எனில் ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } இல்லையெனில் {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    திரும்பு dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(மாறிலி UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    எனில் (c >= trie->உயர்வு_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        திரும்பு trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET;
    }
    திரும்பு ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(மாறிலி UCPTrie *trie, UChar32 c,
                            மாறிலி uint8_t *start, மாறிலி uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    எனில் ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } இல்லையெனில் {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    திரும்பு (idx << 3) | i;
}

நாமாகாஶ {

உள்வரி uint32_t get_மதிப்பு(UCPTrieData data, UCPTrie_மதிப்பு_Width மதிப்பு_Width, int32_t dataIndex) {
    தேர்வு (மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        திரும்பு data.ptr16[dataIndex];
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        திரும்பு data.ptr32[dataIndex];
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        திரும்பு data.ptr8[dataIndex];
    இயல்பு:
        // Unreachable if the trie is properly initialized.
        திரும்பு 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(மாறிலி UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    எனில் ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } இல்லையெனில் {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    திரும்பு get_மதிப்பு(trie->data, (UCPTrie_மதிப்பு_Width)trie->மதிப்பு_Width, dataIndex);
}

நாமாகாஶ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

உள்வரி uint32_t maybe_Filter_மதிப்பு(uint32_t value, uint32_t trie_Null_மதிப்பு, uint32_t null_மதிப்பு,
                                 UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context) {
    எனில் (value == trie_Null_மதிப்பு) {
        value = null_மதிப்பு;
    } இல்லையெனில் எனில் (filter != nullptr) {
        value = filter(context, value);
    }
    திரும்பு value;
}

UChar32 getRange(மாறிலி வெறுமை *t, UChar32 start,
                 UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context, uint32_t *p_மதிப்பு) {
    எனில் (static_cast<uint32_t>(start) > MAX_UNICODE) {
        திரும்பு U_SENTINEL;
    }
    மாறிலி UCPTrie *trie = reinterpret_cast<மாறிலி UCPTrie *>(t);
    UCPTrie_மதிப்பு_Width மதிப்பு_Width = static_cast<UCPTrie_மதிப்பு_Width>(trie->மதிப்பு_Width);
    எனில் (start >= trie->உயர்வு_Start) {
        எனில் (p_மதிப்பு != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET;
            uint32_t value = get_மதிப்பு(trie->data, மதிப்பு_Width, di);
            எனில் (filter != nullptr) { value = filter(context, value); }
            *p_மதிப்பு = value;
        }
        திரும்பு MAX_UNICODE;
    }

    uint32_t null_மதிப்பு = trie->null_மதிப்பு;
    எனில் (filter != nullptr) { null_மதிப்பு = filter(context, null_மதிப்பு); }
    மாறிலி uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_மதிப்பு, value = null_மதிப்பு;
    தருக்க have_மதிப்பு = பொய்;
    செய் {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        எனில் (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } இல்லையெனில் {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            எனில் (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->உயர்வு_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } இல்லையெனில் {
                U_ASSERT(c < trie->உயர்வு_Start && trie->உயர்வு_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            எனில் (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                தொடர்;
            }
            prevI3Block = i3Block;
            எனில் (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                எனில் (have_மதிப்பு) {
                    எனில் (null_மதிப்பு != value) {
                        திரும்பு c - 1;
                    }
                } இல்லையெனில் {
                    trie_மதிப்பு = trie->null_மதிப்பு;
                    value = null_மதிப்பு;
                    எனில் (p_மதிப்பு != nullptr) { *p_மதிப்பு = null_மதிப்பு; }
                    have_மதிப்பு = உண்மை;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                தொடர்;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        செய் {
            int32_t block;
            எனில் ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } இல்லையெனில் {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            எனில் (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } இல்லையெனில் {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                எனில் (block == trie->dataNullOffset) {
                    // This is the data null block.
                    எனில் (have_மதிப்பு) {
                        எனில் (null_மதிப்பு != value) {
                            திரும்பு c - 1;
                        }
                    } இல்லையெனில் {
                        trie_மதிப்பு = trie->null_மதிப்பு;
                        value = null_மதிப்பு;
                        எனில் (p_மதிப்பு != nullptr) { *p_மதிப்பு = null_மதிப்பு; }
                        have_மதிப்பு = உண்மை;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } இல்லையெனில் {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_மதிப்பு(trie->data, மதிப்பு_Width, di);
                    எனில் (have_மதிப்பு) {
                        எனில் (trieValue2 != trie_மதிப்பு) {
                            எனில் (filter == nullptr ||
                                    maybe_Filter_மதிப்பு(trieValue2, trie->null_மதிப்பு, null_மதிப்பு,
                                                     filter, context) != value) {
                                திரும்பு c - 1;
                            }
                            trie_மதிப்பு = trieValue2;  // may or may not help
                        }
                    } இல்லையெனில் {
                        trie_மதிப்பு = trieValue2;
                        value = maybe_Filter_மதிப்பு(trieValue2, trie->null_மதிப்பு, null_மதிப்பு,
                                                 filter, context);
                        எனில் (p_மதிப்பு != nullptr) { *p_மதிப்பு = value; }
                        have_மதிப்பு = உண்மை;
                    }
                    போது ((++c & dataMask) != 0) {
                        trieValue2 = get_மதிப்பு(trie->data, மதிப்பு_Width, ++di);
                        எனில் (trieValue2 != trie_மதிப்பு) {
                            எனில் (filter == nullptr ||
                                    maybe_Filter_மதிப்பு(trieValue2, trie->null_மதிப்பு, null_மதிப்பு,
                                                     filter, context) != value) {
                                திரும்பு c - 1;
                            }
                            trie_மதிப்பு = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } போது (++i3 < i3BlockLength);
    } போது (c < trie->உயர்வு_Start);
    U_ASSERT(have_மதிப்பு);
    int32_t di = trie->dataLength - UCPTRIE_உயர்வு_மதிப்பு_NEG_DATA_OFFSET;
    uint32_t உயர்வு_மதிப்பு = get_மதிப்பு(trie->data, மதிப்பு_Width, di);
    எனில் (maybe_Filter_மதிப்பு(உயர்வு_மதிப்பு, trie->null_மதிப்பு, null_மதிப்பு,
                         filter, context) != value) {
        திரும்பு c - 1;
    } இல்லையெனில் {
        திரும்பு MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         மாறிலி வெறுமை *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_மதிப்பு,
                         UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context, uint32_t *p_மதிப்பு) {
    எனில் (option == UCPMAP_RANGE_NORMAL) {
        திரும்பு getRange(trie, start, filter, context, p_மதிப்பு);
    }
    uint32_t value;
    எனில் (p_மதிப்பு == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_மதிப்பு = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_மதிப்பு);
    எனில் (end < 0xd7ff || start > surrEnd) {
        திரும்பு end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    எனில் (*p_மதிப்பு == surrogate_மதிப்பு) {
        எனில் (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            திரும்பு end;
        }
    } இல்லையெனில் {
        எனில் (start <= 0xd7ff) {
            திரும்பு 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_மதிப்பு = surrogate_மதிப்பு;
        எனில் (end > surrEnd) {
            திரும்பு surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    எனில் (value2 == surrogate_மதிப்பு) {
        திரும்பு end2;
    }
    திரும்பு surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(மாறிலி UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_மதிப்பு,
                 UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context, uint32_t *p_மதிப்பு) {
    திரும்பு ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_மதிப்பு,
                                    filter, context, p_மதிப்பு);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_இருமம்(மாறிலி UCPTrie *trie,
                 வெறுமை *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_மதிப்பு_Width மதிப்பு_Width = (UCPTrie_மதிப்பு_Width)trie->மதிப்பு_Width;
    எனில் (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            மதிப்பு_Width < UCPTRIE_மதிப்பு_BITS_16 || UCPTRIE_மதிப்பு_BITS_8 < மதிப்பு_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    int32_t length = (int32_t)அளவு(UCPTrieHeader) + trie->indexLength * 2;
    தேர்வு (மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        length += trie->dataLength * 2;
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        length += trie->dataLength * 4;
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        length += trie->dataLength;
        நிறுத்து;
    இயல்பு:
        // unreachable
        நிறுத்து;
    }
    எனில் (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு length;
    }

    எழுத்து *bytes = (எழுத்து *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        மதிப்பு_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_உயர்வு_Start = trie->உயர்வு_Start >> UCPTRIE_SHIFT_2;
    bytes += அளவு(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    தேர்வு (மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        நிறுத்து;
    இயல்பு:
        // unreachable
        நிறுத்து;
    }
    திரும்பு length;
}

நாமாகாஶ {

#ifdef UCPTRIE_DEBUG
long மொத்தம்_Null(மாறிலி UCPTrie *trie) {
    uint32_t null_மதிப்பு=trie->null_மதிப்பு;
    int32_t length=trie->dataLength;
    long count=0;
    தேர்வு (trie->மதிப்பு_Width) {
    நிலை UCPTRIE_மதிப்பு_BITS_16:
        க்கு(int32_t i=0; i<length; ++i) {
            எனில்(trie->data.ptr16[i]==null_மதிப்பு) { ++count; }
        }
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_32:
        க்கு(int32_t i=0; i<length; ++i) {
            எனில்(trie->data.ptr32[i]==null_மதிப்பு) { ++count; }
        }
        நிறுத்து;
    நிலை UCPTRIE_மதிப்பு_BITS_8:
        க்கு(int32_t i=0; i<length; ++i) {
            எனில்(trie->data.ptr8[i]==null_மதிப்பு) { ++count; }
        }
        நிறுத்து;
    இயல்பு:
        // unreachable
        நிறுத்து;
    }
    திரும்பு count;
}

U_CFUNC வெறுமை
ucptrie_printLengths(மாறிலி UCPTrie *trie, மாறிலி எழுத்து *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)அளவு(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->மதிப்பு_Width==UCPTRIE_மதிப்பு_BITS_16 ? 2 :
                        trie->மதிப்பு_Width==UCPTRIE_மதிப்பு_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, மொத்தம்_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(மாறிலி UCPMap *map, UChar32 c) {
    திரும்பு ucptrie_get(reinterpret_cast<மாறிலி UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(மாறிலி UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_மதிப்பு,
                UCPMap_மதிப்பு_Filter *filter, மாறிலி வெறுமை *context, uint32_t *p_மதிப்பு) {
    திரும்பு ucptrie_getRange(reinterpret_cast<மாறிலி UCPTrie *>(map), start,
                            option, surrogate_மதிப்பு,
                            filter, context, p_மதிப்பு);
}
