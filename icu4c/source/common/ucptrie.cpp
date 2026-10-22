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
ucptrie_open_From_द्विमान(UCPTrieType type,
                       UCPTrie_मूल्य_Width मूल्य_Width,
                       स्थिरांक शून्य* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    जर (U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }

    जर (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            मूल्य_Width < UCPTRIE_मूल्य_BITS_ANY || UCPTRIE_मूल्य_BITS_8 < मूल्य_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    // Enough data for a trie header?
    जर (length < (int32_t)आकार(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        परत nullptr;
    }

    // Check the signature.
    स्थिरांक UCPTrieHeader *header = (स्थिरांक UCPTrieHeader *)data;
    जर (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        परत nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t मूल्य_Width_Int = options & UCPTRIE_OPTIONS_मूल्य_BITS_MASK;
    जर (typeInt > UCPTRIE_TYPE_SMALL || मूल्य_Width_Int > UCPTRIE_मूल्य_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        परत nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_मूल्य_Width actual_मूल्य_Width = (UCPTrie_मूल्य_Width)मूल्य_Width_Int;
    जर (type < 0) {
        type = actualType;
    }
    जर (मूल्य_Width < 0) {
        मूल्य_Width = actual_मूल्य_Width;
    }
    जर (type != actualType || मूल्य_Width != actual_मूल्य_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        परत nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, आकार(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.वरचा_Start = header->shifted_वरचा_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_वरचा_Start = (tempTrie.वरचा_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.मूल्य_Width = मूल्य_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)आकार(UCPTrieHeader) + tempTrie.indexLength * 2;
    जर (मूल्य_Width == UCPTRIE_मूल्य_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } अन्यथा जर (मूल्य_Width == UCPTRIE_मूल्य_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } अन्यथा {
        actualLength += tempTrie.dataLength;
    }
    जर (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        परत nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(आकार(UCPTrie));
    जर (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    uprv_memcpy(trie, &tempTrie, आकार(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    स्थिरांक uint16_t *p16 = (स्थिरांक uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_मूल्य_Offset = trie->dataNullOffset;
    जर (null_मूल्य_Offset >= trie->dataLength) {
        null_मूल्य_Offset = trie->dataLength - UCPTRIE_वरचा_मूल्य_NEG_DATA_OFFSET;
    }
    निवड (मूल्य_Width) {
    स्थिती UCPTRIE_मूल्य_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_मूल्य = trie->data.ptr16[null_मूल्य_Offset];
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_32:
        trie->data.ptr32 = (स्थिरांक uint32_t *)p16;
        trie->null_मूल्य = trie->data.ptr32[null_मूल्य_Offset];
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_8:
        trie->data.ptr8 = (स्थिरांक uint8_t *)p16;
        trie->null_मूल्य = trie->data.ptr8[null_मूल्य_Offset];
        थांबा;
    पूर्वनिर्धारित:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        परत nullptr;
    }

    जर (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    परत trie;
}

U_CAPI शून्य U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(स्थिरांक UCPTrie *trie) {
    परत (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_मूल्य_Width U_EXPORT2
ucptrie_get_मूल्य_Width(स्थिरांक UCPTrie *trie) {
    परत (UCPTrie_मूल्य_Width)trie->मूल्य_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(स्थिरांक UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    जर (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->वरचा_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } अन्यथा {
        U_ASSERT((uint32_t)c < (uint32_t)trie->वरचा_Start && trie->वरचा_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    जर ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } अन्यथा {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    परत dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(स्थिरांक UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    जर (c >= trie->वरचा_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        परत trie->dataLength - UCPTRIE_वरचा_मूल्य_NEG_DATA_OFFSET;
    }
    परत ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(स्थिरांक UCPTrie *trie, UChar32 c,
                            स्थिरांक uint8_t *start, स्थिरांक uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    जर ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } अन्यथा {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    परत (idx << 3) | i;
}

नामाकाश {

ओळीत uint32_t get_मूल्य(UCPTrieData data, UCPTrie_मूल्य_Width मूल्य_Width, int32_t dataIndex) {
    निवड (मूल्य_Width) {
    स्थिती UCPTRIE_मूल्य_BITS_16:
        परत data.ptr16[dataIndex];
    स्थिती UCPTRIE_मूल्य_BITS_32:
        परत data.ptr32[dataIndex];
    स्थिती UCPTRIE_मूल्य_BITS_8:
        परत data.ptr8[dataIndex];
    पूर्वनिर्धारित:
        // Unreachable if the trie is properly initialized.
        परत 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(स्थिरांक UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    जर ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } अन्यथा {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    परत get_मूल्य(trie->data, (UCPTrie_मूल्य_Width)trie->मूल्य_Width, dataIndex);
}

नामाकाश {

constexpr int32_t MAX_UNICODE = 0x10ffff;

ओळीत uint32_t maybe_Filter_मूल्य(uint32_t value, uint32_t trie_Null_मूल्य, uint32_t null_मूल्य,
                                 UCPMap_मूल्य_Filter *filter, स्थिरांक शून्य *context) {
    जर (value == trie_Null_मूल्य) {
        value = null_मूल्य;
    } अन्यथा जर (filter != nullptr) {
        value = filter(context, value);
    }
    परत value;
}

UChar32 getRange(स्थिरांक शून्य *t, UChar32 start,
                 UCPMap_मूल्य_Filter *filter, स्थिरांक शून्य *context, uint32_t *p_मूल्य) {
    जर (static_cast<uint32_t>(start) > MAX_UNICODE) {
        परत U_SENTINEL;
    }
    स्थिरांक UCPTrie *trie = reinterpret_cast<स्थिरांक UCPTrie *>(t);
    UCPTrie_मूल्य_Width मूल्य_Width = static_cast<UCPTrie_मूल्य_Width>(trie->मूल्य_Width);
    जर (start >= trie->वरचा_Start) {
        जर (p_मूल्य != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_वरचा_मूल्य_NEG_DATA_OFFSET;
            uint32_t value = get_मूल्य(trie->data, मूल्य_Width, di);
            जर (filter != nullptr) { value = filter(context, value); }
            *p_मूल्य = value;
        }
        परत MAX_UNICODE;
    }

    uint32_t null_मूल्य = trie->null_मूल्य;
    जर (filter != nullptr) { null_मूल्य = filter(context, null_मूल्य); }
    स्थिरांक uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_मूल्य, value = null_मूल्य;
    तार्किक have_मूल्य = असत्य;
    करा {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        जर (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } अन्यथा {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            जर (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->वरचा_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } अन्यथा {
                U_ASSERT(c < trie->वरचा_Start && trie->वरचा_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            जर (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                सुरूठेवा;
            }
            prevI3Block = i3Block;
            जर (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                जर (have_मूल्य) {
                    जर (null_मूल्य != value) {
                        परत c - 1;
                    }
                } अन्यथा {
                    trie_मूल्य = trie->null_मूल्य;
                    value = null_मूल्य;
                    जर (p_मूल्य != nullptr) { *p_मूल्य = null_मूल्य; }
                    have_मूल्य = सत्य;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                सुरूठेवा;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        करा {
            int32_t block;
            जर ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } अन्यथा {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            जर (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } अन्यथा {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                जर (block == trie->dataNullOffset) {
                    // This is the data null block.
                    जर (have_मूल्य) {
                        जर (null_मूल्य != value) {
                            परत c - 1;
                        }
                    } अन्यथा {
                        trie_मूल्य = trie->null_मूल्य;
                        value = null_मूल्य;
                        जर (p_मूल्य != nullptr) { *p_मूल्य = null_मूल्य; }
                        have_मूल्य = सत्य;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } अन्यथा {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_मूल्य(trie->data, मूल्य_Width, di);
                    जर (have_मूल्य) {
                        जर (trieValue2 != trie_मूल्य) {
                            जर (filter == nullptr ||
                                    maybe_Filter_मूल्य(trieValue2, trie->null_मूल्य, null_मूल्य,
                                                     filter, context) != value) {
                                परत c - 1;
                            }
                            trie_मूल्य = trieValue2;  // may or may not help
                        }
                    } अन्यथा {
                        trie_मूल्य = trieValue2;
                        value = maybe_Filter_मूल्य(trieValue2, trie->null_मूल्य, null_मूल्य,
                                                 filter, context);
                        जर (p_मूल्य != nullptr) { *p_मूल्य = value; }
                        have_मूल्य = सत्य;
                    }
                    पर्यंत ((++c & dataMask) != 0) {
                        trieValue2 = get_मूल्य(trie->data, मूल्य_Width, ++di);
                        जर (trieValue2 != trie_मूल्य) {
                            जर (filter == nullptr ||
                                    maybe_Filter_मूल्य(trieValue2, trie->null_मूल्य, null_मूल्य,
                                                     filter, context) != value) {
                                परत c - 1;
                            }
                            trie_मूल्य = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } पर्यंत (++i3 < i3BlockLength);
    } पर्यंत (c < trie->वरचा_Start);
    U_ASSERT(have_मूल्य);
    int32_t di = trie->dataLength - UCPTRIE_वरचा_मूल्य_NEG_DATA_OFFSET;
    uint32_t वरचा_मूल्य = get_मूल्य(trie->data, मूल्य_Width, di);
    जर (maybe_Filter_मूल्य(वरचा_मूल्य, trie->null_मूल्य, null_मूल्य,
                         filter, context) != value) {
        परत c - 1;
    } अन्यथा {
        परत MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         स्थिरांक शून्य *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                         UCPMap_मूल्य_Filter *filter, स्थिरांक शून्य *context, uint32_t *p_मूल्य) {
    जर (option == UCPMAP_RANGE_NORMAL) {
        परत getRange(trie, start, filter, context, p_मूल्य);
    }
    uint32_t value;
    जर (p_मूल्य == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_मूल्य = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_मूल्य);
    जर (end < 0xd7ff || start > surrEnd) {
        परत end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    जर (*p_मूल्य == surrogate_मूल्य) {
        जर (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            परत end;
        }
    } अन्यथा {
        जर (start <= 0xd7ff) {
            परत 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_मूल्य = surrogate_मूल्य;
        जर (end > surrEnd) {
            परत surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    जर (value2 == surrogate_मूल्य) {
        परत end2;
    }
    परत surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(स्थिरांक UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                 UCPMap_मूल्य_Filter *filter, स्थिरांक शून्य *context, uint32_t *p_मूल्य) {
    परत ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मूल्य,
                                    filter, context, p_मूल्य);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_द्विमान(स्थिरांक UCPTrie *trie,
                 शून्य *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    जर (U_FAILURE(*pErrorCode)) {
        परत 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_मूल्य_Width मूल्य_Width = (UCPTrie_मूल्य_Width)trie->मूल्य_Width;
    जर (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मूल्य_Width < UCPTRIE_मूल्य_BITS_16 || UCPTRIE_मूल्य_BITS_8 < मूल्य_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    int32_t length = (int32_t)आकार(UCPTrieHeader) + trie->indexLength * 2;
    निवड (मूल्य_Width) {
    स्थिती UCPTRIE_मूल्य_BITS_16:
        length += trie->dataLength * 2;
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_32:
        length += trie->dataLength * 4;
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_8:
        length += trie->dataLength;
        थांबा;
    पूर्वनिर्धारित:
        // unreachable
        थांबा;
    }
    जर (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        परत length;
    }

    अक्षर *bytes = (अक्षर *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        मूल्य_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_वरचा_Start = trie->वरचा_Start >> UCPTRIE_SHIFT_2;
    bytes += आकार(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    निवड (मूल्य_Width) {
    स्थिती UCPTRIE_मूल्य_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        थांबा;
    पूर्वनिर्धारित:
        // unreachable
        थांबा;
    }
    परत length;
}

नामाकाश {

#ifdef UCPTRIE_DEBUG
दीर्घ मोजणी_Null(स्थिरांक UCPTrie *trie) {
    uint32_t null_मूल्य=trie->null_मूल्य;
    int32_t length=trie->dataLength;
    दीर्घ count=0;
    निवड (trie->मूल्य_Width) {
    स्थिती UCPTRIE_मूल्य_BITS_16:
        साठी(int32_t i=0; i<length; ++i) {
            जर(trie->data.ptr16[i]==null_मूल्य) { ++count; }
        }
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_32:
        साठी(int32_t i=0; i<length; ++i) {
            जर(trie->data.ptr32[i]==null_मूल्य) { ++count; }
        }
        थांबा;
    स्थिती UCPTRIE_मूल्य_BITS_8:
        साठी(int32_t i=0; i<length; ++i) {
            जर(trie->data.ptr8[i]==null_मूल्य) { ++count; }
        }
        थांबा;
    पूर्वनिर्धारित:
        // unreachable
        थांबा;
    }
    परत count;
}

U_CFUNC शून्य
ucptrie_printLengths(स्थिरांक UCPTrie *trie, स्थिरांक अक्षर *which) {
    दीर्घ indexLength=trie->indexLength;
    दीर्घ dataLength=(दीर्घ)trie->dataLength;
    दीर्घ totalLength=(दीर्घ)आकार(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->मूल्य_Width==UCPTRIE_मूल्य_BITS_16 ? 2 :
                        trie->मूल्य_Width==UCPTRIE_मूल्य_BITS_32 ? 4 : 1);
    म_लिखो("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, मोजणी_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(स्थिरांक UCPMap *map, UChar32 c) {
    परत ucptrie_get(reinterpret_cast<स्थिरांक UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(स्थिरांक UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                UCPMap_मूल्य_Filter *filter, स्थिरांक शून्य *context, uint32_t *p_मूल्य) {
    परत ucptrie_getRange(reinterpret_cast<स्थिरांक UCPTrie *>(map), start,
                            option, surrogate_मूल्य,
                            filter, context, p_मूल्य);
}
