// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/utf.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_દ્વિઅંકી(UCPTrieType type,
                       UCPTrie_કિંમત_Width કિંમત_Width,
                       અચલ શૂન્ય* data U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* pActualLength,
                       UErrorCode* pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }

    જો (length <= 0 || (U_POINTER_MASK_LSB(data, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            કિંમત_Width < UCPTRIE_કિંમત_BITS_ANY || UCPTRIE_કિંમત_BITS_8 < કિંમત_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    // Enough data for a trie header?
    જો (length < (int32_t)કદ(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    // Check the signature.
    અચલ UCPTrieHeader *header = (અચલ UCPTrieHeader *)data;
    જો (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t કિંમત_Width_Int = options & UCPTRIE_OPTIONS_કિંમત_BITS_MASK;
    જો (typeInt > UCPTRIE_TYPE_SMALL || કિંમત_Width_Int > UCPTRIE_કિંમત_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_કિંમત_Width actual_કિંમત_Width = (UCPTrie_કિંમત_Width)કિંમત_Width_Int;
    જો (type < 0) {
        type = actualType;
    }
    જો (કિંમત_Width < 0) {
        કિંમત_Width = actual_કિંમત_Width;
    }
    જો (type != actualType || કિંમત_Width != actual_કિંમત_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    // Get the length values and offsets.
    UCPTrie tempTrie;
    uprv_memset(&tempTrie, 0, કદ(tempTrie));
    tempTrie.indexLength = header->indexLength;
    tempTrie.dataLength =
        ((options & UCPTRIE_OPTIONS_DATA_LENGTH_MASK) << 4) | header->dataLength;
    tempTrie.index3NullOffset = header->index3NullOffset;
    tempTrie.dataNullOffset =
        ((options & UCPTRIE_OPTIONS_DATA_NULL_OFFSET_MASK) << 8) | header->dataNullOffset;

    tempTrie.ઉપલું_Start = header->shifted_ઉપલું_Start << UCPTRIE_SHIFT_2;
    tempTrie.shifted12_ઉપલું_Start = (tempTrie.ઉપલું_Start + 0xfff) >> 12;
    tempTrie.type = type;
    tempTrie.કિંમત_Width = કિંમત_Width;

    // Calculate the actual length.
    int32_t actualLength = (int32_t)કદ(UCPTrieHeader) + tempTrie.indexLength * 2;
    જો (કિંમત_Width == UCPTRIE_કિંમત_BITS_16) {
        actualLength += tempTrie.dataLength * 2;
    } નહીંતર જો (કિંમત_Width == UCPTRIE_કિંમત_BITS_32) {
        actualLength += tempTrie.dataLength * 4;
    } નહીંતર {
        actualLength += tempTrie.dataLength;
    }
    જો (length < actualLength) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        પાછા nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(કદ(UCPTrie));
    જો (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memcpy(trie, &tempTrie, કદ(tempTrie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    અચલ uint16_t *p16 = (અચલ uint16_t *)(header + 1);
    trie->index = p16;
    p16 += trie->indexLength;

    // Get the data.
    int32_t null_કિંમત_Offset = trie->dataNullOffset;
    જો (null_કિંમત_Offset >= trie->dataLength) {
        null_કિંમત_Offset = trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET;
    }
    પસંદગી (કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        trie->data.ptr16 = p16;
        trie->null_કિંમત = trie->data.ptr16[null_કિંમત_Offset];
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        trie->data.ptr32 = (અચલ uint32_t *)p16;
        trie->null_કિંમત = trie->data.ptr32[null_કિંમત_Offset];
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        trie->data.ptr8 = (અચલ uint8_t *)p16;
        trie->null_કિંમત = trie->data.ptr8[null_કિંમત_Offset];
        તોડો;
    મૂળભૂત:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    જો (pActualLength != nullptr) {
        *pActualLength = actualLength;
    }
    પાછા trie;
}

U_CAPI શૂન્ય U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(અચલ UCPTrie *trie) {
    પાછા (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_કિંમત_Width U_EXPORT2
ucptrie_get_કિંમત_Width(અચલ UCPTrie *trie) {
    પાછા (UCPTrie_કિંમત_Width)trie->કિંમત_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallIndex(અચલ UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    જો (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ઉપલું_Start);
        i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
    } નહીંતર {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ઉપલું_Start && trie->ઉપલું_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_INDEX_LENGTH;
    }
    int32_t i3Block = trie->index[
        (int32_t)trie->index[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
    int32_t dataBlock;
    જો ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        dataBlock = trie->index[i3Block + i3];
    } નહીંતર {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        dataBlock = ((int32_t)trie->index[i3Block++] << (2 + (2 * i3))) & 0x30000;
        dataBlock |= trie->index[i3Block + i3];
    }
    પાછા dataBlock + (c & UCPTRIE_SMALL_DATA_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalSmallU8Index(અચલ UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    જો (c >= trie->ઉપલું_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        પાછા trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET;
    }
    પાછા ucptrie_internalSmallIndex(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internalU8PrevIndex(અચલ UCPTrie *trie, UChar32 c,
                            અચલ uint8_t *start, અચલ uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    જો ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } નહીંતર {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = _UCPTRIE_CP_INDEX(trie, 0xffff, c);
    પાછા (idx << 3) | i;
}

નામાકાશ {

અંતઃરેખ uint32_t get_કિંમત(UCPTrieData data, UCPTrie_કિંમત_Width કિંમત_Width, int32_t dataIndex) {
    પસંદગી (કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        પાછા data.ptr16[dataIndex];
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        પાછા data.ptr32[dataIndex];
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        પાછા data.ptr8[dataIndex];
    મૂળભૂત:
        // Unreachable if the trie is properly initialized.
        પાછા 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(અચલ UCPTrie *trie, UChar32 c) {
    int32_t dataIndex;
    જો ((uint32_t)c <= 0x7f) {
        // linear ASCII
        dataIndex = c;
    } નહીંતર {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        dataIndex = _UCPTRIE_CP_INDEX(trie, fastMax, c);
    }
    પાછા get_કિંમત(trie->data, (UCPTrie_કિંમત_Width)trie->કિંમત_Width, dataIndex);
}

નામાકાશ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

અંતઃરેખ uint32_t maybe_Filter_કિંમત(uint32_t value, uint32_t trie_Null_કિંમત, uint32_t null_કિંમત,
                                 UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context) {
    જો (value == trie_Null_કિંમત) {
        value = null_કિંમત;
    } નહીંતર જો (filter != nullptr) {
        value = filter(context, value);
    }
    પાછા value;
}

UChar32 getRange(અચલ શૂન્ય *t, UChar32 start,
                 UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context, uint32_t *p_કિંમત) {
    જો (static_cast<uint32_t>(start) > MAX_UNICODE) {
        પાછા U_SENTINEL;
    }
    અચલ UCPTrie *trie = reinterpret_cast<અચલ UCPTrie *>(t);
    UCPTrie_કિંમત_Width કિંમત_Width = static_cast<UCPTrie_કિંમત_Width>(trie->કિંમત_Width);
    જો (start >= trie->ઉપલું_Start) {
        જો (p_કિંમત != nullptr) {
            int32_t di = trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET;
            uint32_t value = get_કિંમત(trie->data, કિંમત_Width, di);
            જો (filter != nullptr) { value = filter(context, value); }
            *p_કિંમત = value;
        }
        પાછા MAX_UNICODE;
    }

    uint32_t null_કિંમત = trie->null_કિંમત;
    જો (filter != nullptr) { null_કિંમત = filter(context, null_કિંમત); }
    અચલ uint16_t *index = trie->index;

    int32_t prevI3Block = -1;
    int32_t prevBlock = -1;
    UChar32 c = start;
    uint32_t trie_કિંમત, value = null_કિંમત;
    તાર્કિક have_કિંમત = ખોટું;
    કરો {
        int32_t i3Block;
        int32_t i3;
        int32_t i3BlockLength;
        int32_t dataBlockLength;
        જો (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3BlockLength = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_INDEX_LENGTH : UCPTRIE_SMALL_INDEX_LENGTH;
            dataBlockLength = UCPTRIE_FAST_DATA_BLOCK_LENGTH;
        } નહીંતર {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            જો (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ઉપલું_Start);
                i1 += UCPTRIE_BMP_INDEX_LENGTH - UCPTRIE_OMITTED_BMP_INDEX_1_LENGTH;
            } નહીંતર {
                U_ASSERT(c < trie->ઉપલું_Start && trie->ઉપલું_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_INDEX_LENGTH;
            }
            i3Block = trie->index[
                static_cast<int32_t>(trie->index[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_INDEX_2_MASK)];
            જો (i3Block == prevI3Block && (c - start) >= UCPTRIE_CP_PER_INDEX_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_INDEX_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_INDEX_2_ENTRY;
                ચાલુ;
            }
            prevI3Block = i3Block;
            જો (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                જો (have_કિંમત) {
                    જો (null_કિંમત != value) {
                        પાછા c - 1;
                    }
                } નહીંતર {
                    trie_કિંમત = trie->null_કિંમત;
                    value = null_કિંમત;
                    જો (p_કિંમત != nullptr) { *p_કિંમત = null_કિંમત; }
                    have_કિંમત = સાચું;
                }
                prevBlock = trie->dataNullOffset;
                c = (c + UCPTRIE_CP_PER_INDEX_2_ENTRY) & ~(UCPTRIE_CP_PER_INDEX_2_ENTRY - 1);
                ચાલુ;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_INDEX_3_MASK;
            i3BlockLength = UCPTRIE_INDEX_3_BLOCK_LENGTH;
            dataBlockLength = UCPTRIE_SMALL_DATA_BLOCK_LENGTH;
        }
        // Enumerate data blocks for one index-3 block.
        કરો {
            int32_t block;
            જો ((i3Block & 0x8000) == 0) {
                block = index[i3Block + i3];
            } નહીંતર {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(index[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= index[group + gi];
            }
            જો (block == prevBlock && (c - start) >= dataBlockLength) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (dataBlockLength - 1)) == 0);
                c += dataBlockLength;
            } નહીંતર {
                int32_t dataMask = dataBlockLength - 1;
                prevBlock = block;
                જો (block == trie->dataNullOffset) {
                    // This is the data null block.
                    જો (have_કિંમત) {
                        જો (null_કિંમત != value) {
                            પાછા c - 1;
                        }
                    } નહીંતર {
                        trie_કિંમત = trie->null_કિંમત;
                        value = null_કિંમત;
                        જો (p_કિંમત != nullptr) { *p_કિંમત = null_કિંમત; }
                        have_કિંમત = સાચું;
                    }
                    c = (c + dataBlockLength) & ~dataMask;
                } નહીંતર {
                    int32_t di = block + (c & dataMask);
                    uint32_t trieValue2 = get_કિંમત(trie->data, કિંમત_Width, di);
                    જો (have_કિંમત) {
                        જો (trieValue2 != trie_કિંમત) {
                            જો (filter == nullptr ||
                                    maybe_Filter_કિંમત(trieValue2, trie->null_કિંમત, null_કિંમત,
                                                     filter, context) != value) {
                                પાછા c - 1;
                            }
                            trie_કિંમત = trieValue2;  // may or may not help
                        }
                    } નહીંતર {
                        trie_કિંમત = trieValue2;
                        value = maybe_Filter_કિંમત(trieValue2, trie->null_કિંમત, null_કિંમત,
                                                 filter, context);
                        જો (p_કિંમત != nullptr) { *p_કિંમત = value; }
                        have_કિંમત = સાચું;
                    }
                    જ્યારે ((++c & dataMask) != 0) {
                        trieValue2 = get_કિંમત(trie->data, કિંમત_Width, ++di);
                        જો (trieValue2 != trie_કિંમત) {
                            જો (filter == nullptr ||
                                    maybe_Filter_કિંમત(trieValue2, trie->null_કિંમત, null_કિંમત,
                                                     filter, context) != value) {
                                પાછા c - 1;
                            }
                            trie_કિંમત = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } જ્યારે (++i3 < i3BlockLength);
    } જ્યારે (c < trie->ઉપલું_Start);
    U_ASSERT(have_કિંમત);
    int32_t di = trie->dataLength - UCPTRIE_ઉપલું_કિંમત_NEG_DATA_OFFSET;
    uint32_t ઉપલું_કિંમત = get_કિંમત(trie->data, કિંમત_Width, di);
    જો (maybe_Filter_કિંમત(ઉપલું_કિંમત, trie->null_કિંમત, null_કિંમત,
                         filter, context) != value) {
        પાછા c - 1;
    } નહીંતર {
        પાછા MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         અચલ શૂન્ય *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_કિંમત,
                         UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context, uint32_t *p_કિંમત) {
    જો (option == UCPMAP_RANGE_NORMAL) {
        પાછા getRange(trie, start, filter, context, p_કિંમત);
    }
    uint32_t value;
    જો (p_કિંમત == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_કિંમત = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_કિંમત);
    જો (end < 0xd7ff || start > surrEnd) {
        પાછા end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    જો (*p_કિંમત == surrogate_કિંમત) {
        જો (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            પાછા end;
        }
    } નહીંતર {
        જો (start <= 0xd7ff) {
            પાછા 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_કિંમત = surrogate_કિંમત;
        જો (end > surrEnd) {
            પાછા surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    જો (value2 == surrogate_કિંમત) {
        પાછા end2;
    }
    પાછા surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(અચલ UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_કિંમત,
                 UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context, uint32_t *p_કિંમત) {
    પાછા ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_કિંમત,
                                    filter, context, p_કિંમત);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_દ્વિઅંકી(અચલ UCPTrie *trie,
                 શૂન્ય *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_કિંમત_Width કિંમત_Width = (UCPTrie_કિંમત_Width)trie->કિંમત_Width;
    જો (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            કિંમત_Width < UCPTRIE_કિંમત_BITS_16 || UCPTRIE_કિંમત_BITS_8 < કિંમત_Width ||
            capacity < 0 ||
            (capacity > 0 && (data == nullptr || (U_POINTER_MASK_LSB(data, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    int32_t length = (int32_t)કદ(UCPTrieHeader) + trie->indexLength * 2;
    પસંદગી (કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        length += trie->dataLength * 2;
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        length += trie->dataLength * 4;
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        length += trie->dataLength;
        તોડો;
    મૂળભૂત:
        // unreachable
        તોડો;
    }
    જો (capacity < length) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        પાછા length;
    }

    અક્ષર *bytes = (અક્ષર *)data;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->dataLength & 0xf0000) >> 4) |
        ((trie->dataNullOffset & 0xf0000) >> 8) |
        (trie->type << 6) |
        કિંમત_Width);
    header->indexLength = (uint16_t)trie->indexLength;
    header->dataLength = (uint16_t)trie->dataLength;
    header->index3NullOffset = trie->index3NullOffset;
    header->dataNullOffset = (uint16_t)trie->dataNullOffset;
    header->shifted_ઉપલું_Start = trie->ઉપલું_Start >> UCPTRIE_SHIFT_2;
    bytes += કદ(UCPTrieHeader);

    uprv_memcpy(bytes, trie->index, trie->indexLength * 2);
    bytes += trie->indexLength * 2;

    પસંદગી (કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        uprv_memcpy(bytes, trie->data.ptr16, trie->dataLength * 2);
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        uprv_memcpy(bytes, trie->data.ptr32, trie->dataLength * 4);
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        uprv_memcpy(bytes, trie->data.ptr8, trie->dataLength);
        તોડો;
    મૂળભૂત:
        // unreachable
        તોડો;
    }
    પાછા length;
}

નામાકાશ {

#ifdef UCPTRIE_DEBUG
long ગણતરી_Null(અચલ UCPTrie *trie) {
    uint32_t null_કિંમત=trie->null_કિંમત;
    int32_t length=trie->dataLength;
    long count=0;
    પસંદગી (trie->કિંમત_Width) {
    સ્થિતિ UCPTRIE_કિંમત_BITS_16:
        માટે(int32_t i=0; i<length; ++i) {
            જો(trie->data.ptr16[i]==null_કિંમત) { ++count; }
        }
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_32:
        માટે(int32_t i=0; i<length; ++i) {
            જો(trie->data.ptr32[i]==null_કિંમત) { ++count; }
        }
        તોડો;
    સ્થિતિ UCPTRIE_કિંમત_BITS_8:
        માટે(int32_t i=0; i<length; ++i) {
            જો(trie->data.ptr8[i]==null_કિંમત) { ++count; }
        }
        તોડો;
    મૂળભૂત:
        // unreachable
        તોડો;
    }
    પાછા count;
}

U_CFUNC શૂન્ય
ucptrie_printLengths(અચલ UCPTrie *trie, અચલ અક્ષર *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)કદ(UCPTrieHeader)+indexLength*2+
            dataLength*(trie->કિંમત_Width==UCPTRIE_કિંમત_BITS_16 ? 2 :
                        trie->કિંમત_Width==UCPTRIE_કિંમત_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, ગણતરી_Null(trie), totalLength);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(અચલ UCPMap *map, UChar32 c) {
    પાછા ucptrie_get(reinterpret_cast<અચલ UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(અચલ UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_કિંમત,
                UCPMap_કિંમત_Filter *filter, અચલ શૂન્ય *context, uint32_t *p_કિંમત) {
    પાછા ucptrie_getRange(reinterpret_cast<અચલ UCPTrie *>(map), start,
                            option, surrogate_કિંમત,
                            filter, context, p_કિંમત);
}
