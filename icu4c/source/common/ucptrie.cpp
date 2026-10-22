// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#अगर_घोषित UCPTRIE_DEBUG
#   include <stdio.h>
#पूर्ण_अगर

#समावेश "unicode/utypes.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_द्विआधारी(UCPTrieType type,
                       UCPTrie_मूल्य_Width मूल्य_Width,
                       स्थिर व्योम* तथ्य U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_लंबाई,
                       UErrorCode* pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    अगर (length <= 0 || (U_POINTER_MASK_LSB(तथ्य, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            मूल्य_Width < UCPTRIE_मूल्य_BITS_ANY || UCPTRIE_मूल्य_BITS_8 < मूल्य_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    // Enough data for a trie header?
    अगर (length < (int32_t)माप(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    // Check the signature.
    स्थिर UCPTrieHeader *header = (स्थिर UCPTrieHeader *)तथ्य;
    अगर (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t मूल्य_Width_Int = options & UCPTRIE_OPTIONS_मूल्य_BITS_MASK;
    अगर (typeInt > UCPTRIE_TYPE_SMALL || मूल्य_Width_Int > UCPTRIE_मूल्य_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_मूल्य_Width actual_मूल्य_Width = (UCPTrie_मूल्य_Width)मूल्य_Width_Int;
    अगर (type < 0) {
        type = actualType;
    }
    अगर (मूल्य_Width < 0) {
        मूल्य_Width = actual_मूल्य_Width;
    }
    अगर (type != actualType || मूल्य_Width != actual_मूल्य_Width) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    // Get the length values and offsets.
    UCPTrie अस्थायी_Trie;
    uprv_memset(&अस्थायी_Trie, 0, माप(अस्थायी_Trie));
    अस्थायी_Trie.सूचकांक_लंबाई = header->सूचकांक_लंबाई;
    अस्थायी_Trie.तथ्य_लंबाई =
        ((options & UCPTRIE_OPTIONS_तथ्य_लंबाई_MASK) << 4) | header->तथ्य_लंबाई;
    अस्थायी_Trie.index3NullOffset = header->index3NullOffset;
    अस्थायी_Trie.तथ्य_Null_Offset =
        ((options & UCPTRIE_OPTIONS_तथ्य_NULL_OFFSET_MASK) << 8) | header->तथ्य_Null_Offset;

    अस्थायी_Trie.ऊपरी_Start = header->shifted_ऊपरी_Start << UCPTRIE_SHIFT_2;
    अस्थायी_Trie.shifted12_ऊपरी_Start = (अस्थायी_Trie.ऊपरी_Start + 0xfff) >> 12;
    अस्थायी_Trie.type = type;
    अस्थायी_Trie.मूल्य_Width = मूल्य_Width;

    // Calculate the actual length.
    int32_t actual_लंबाई = (int32_t)माप(UCPTrieHeader) + अस्थायी_Trie.सूचकांक_लंबाई * 2;
    अगर (मूल्य_Width == UCPTRIE_मूल्य_BITS_16) {
        actual_लंबाई += अस्थायी_Trie.तथ्य_लंबाई * 2;
    } अन्यथा अगर (मूल्य_Width == UCPTRIE_मूल्य_BITS_32) {
        actual_लंबाई += अस्थायी_Trie.तथ्य_लंबाई * 4;
    } अन्यथा {
        actual_लंबाई += अस्थायी_Trie.तथ्य_लंबाई;
    }
    अगर (length < actual_लंबाई) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        वापस nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(माप(UCPTrie));
    अगर (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(trie, &अस्थायी_Trie, माप(अस्थायी_Trie));
#अगर_घोषित UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#पूर्ण_अगर

    // Set the pointers to its index and data arrays.
    स्थिर uint16_t *p16 = (स्थिर uint16_t *)(header + 1);
    trie->सूचकांक = p16;
    p16 += trie->सूचकांक_लंबाई;

    // Get the data.
    int32_t null_मूल्य_Offset = trie->तथ्य_Null_Offset;
    अगर (null_मूल्य_Offset >= trie->तथ्य_लंबाई) {
        null_मूल्य_Offset = trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET;
    }
    चयन (मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        trie->तथ्य.ptr16 = p16;
        trie->null_मूल्य = trie->तथ्य.ptr16[null_मूल्य_Offset];
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        trie->तथ्य.ptr32 = (स्थिर uint32_t *)p16;
        trie->null_मूल्य = trie->तथ्य.ptr32[null_मूल्य_Offset];
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        trie->तथ्य.ptr8 = (स्थिर uint8_t *)p16;
        trie->null_मूल्य = trie->तथ्य.ptr8[null_मूल्य_Offset];
        अवरोध;
    शेष:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    अगर (p_Actual_लंबाई != nullptr) {
        *p_Actual_लंबाई = actual_लंबाई;
    }
    वापस trie;
}

U_CAPI व्योम U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(स्थिर UCPTrie *trie) {
    वापस (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_मूल्य_Width U_EXPORT2
ucptrie_get_मूल्य_Width(स्थिर UCPTrie *trie) {
    वापस (UCPTrie_मूल्य_Width)trie->मूल्य_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internal_Small_सूचकांक(स्थिर UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    अगर (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ऊपरी_Start);
        i1 += UCPTRIE_BMP_सूचकांक_लंबाई - UCPTRIE_OMITTED_BMP_सूचकांक_1_लंबाई;
    } अन्यथा {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ऊपरी_Start && trie->ऊपरी_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_सूचकांक_लंबाई;
    }
    int32_t i3Block = trie->सूचकांक[
        (int32_t)trie->सूचकांक[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_सूचकांक_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_सूचकांक_3_MASK;
    int32_t तथ्य_Block;
    अगर ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        तथ्य_Block = trie->सूचकांक[i3Block + i3];
    } अन्यथा {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        तथ्य_Block = ((int32_t)trie->सूचकांक[i3Block++] << (2 + (2 * i3))) & 0x30000;
        तथ्य_Block |= trie->सूचकांक[i3Block + i3];
    }
    वापस तथ्य_Block + (c & UCPTRIE_SMALL_तथ्य_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internal_Small_U8_सूचकांक(स्थिर UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    अगर (c >= trie->ऊपरी_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        वापस trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET;
    }
    वापस ucptrie_internal_Small_सूचकांक(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internal_U8_पूर्व_सूचकांक(स्थिर UCPTrie *trie, UChar32 c,
                            स्थिर uint8_t *start, स्थिर uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    अगर ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } अन्यथा {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_पूर्व_Char_Safe_Body(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_सूचकांक(trie, 0xffff, c);
    वापस (idx << 3) | i;
}

namespace {

अंतरभूत uint32_t get_मूल्य(UCPTrie_तथ्य तथ्य, UCPTrie_मूल्य_Width मूल्य_Width, int32_t तथ्य_सूचकांक) {
    चयन (मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        वापस तथ्य.ptr16[तथ्य_सूचकांक];
    हाल UCPTRIE_मूल्य_BITS_32:
        वापस तथ्य.ptr32[तथ्य_सूचकांक];
    हाल UCPTRIE_मूल्य_BITS_8:
        वापस तथ्य.ptr8[तथ्य_सूचकांक];
    शेष:
        // Unreachable if the trie is properly initialized.
        वापस 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(स्थिर UCPTrie *trie, UChar32 c) {
    int32_t तथ्य_सूचकांक;
    अगर ((uint32_t)c <= 0x7f) {
        // linear ASCII
        तथ्य_सूचकांक = c;
    } अन्यथा {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        तथ्य_सूचकांक = UCPTRIE_CP_सूचकांक(trie, fastMax, c);
    }
    वापस get_मूल्य(trie->तथ्य, (UCPTrie_मूल्य_Width)trie->मूल्य_Width, तथ्य_सूचकांक);
}

namespace {

constexpr int32_t MAX_UNICODE = 0x10ffff;

अंतरभूत uint32_t maybe_Filter_मूल्य(uint32_t value, uint32_t trie_Null_मूल्य, uint32_t null_मूल्य,
                                 UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context) {
    अगर (value == trie_Null_मूल्य) {
        value = null_मूल्य;
    } अन्यथा अगर (filter != nullptr) {
        value = filter(context, value);
    }
    वापस value;
}

UChar32 getRange(स्थिर व्योम *t, UChar32 start,
                 UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context, uint32_t *p_मूल्य) {
    अगर (static_cast<uint32_t>(start) > MAX_UNICODE) {
        वापस U_SENTINEL;
    }
    स्थिर UCPTrie *trie = reinterpret_cast<स्थिर UCPTrie *>(t);
    UCPTrie_मूल्य_Width मूल्य_Width = static_cast<UCPTrie_मूल्य_Width>(trie->मूल्य_Width);
    अगर (start >= trie->ऊपरी_Start) {
        अगर (p_मूल्य != nullptr) {
            int32_t di = trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET;
            uint32_t value = get_मूल्य(trie->तथ्य, मूल्य_Width, di);
            अगर (filter != nullptr) { value = filter(context, value); }
            *p_मूल्य = value;
        }
        वापस MAX_UNICODE;
    }

    uint32_t null_मूल्य = trie->null_मूल्य;
    अगर (filter != nullptr) { null_मूल्य = filter(context, null_मूल्य); }
    स्थिर uint16_t *सूचकांक = trie->सूचकांक;

    int32_t पूर्व_I3_Block = -1;
    int32_t पूर्व_Block = -1;
    UChar32 c = start;
    uint32_t trie_मूल्य, value = null_मूल्य;
    bool have_मूल्य = false;
    करो {
        int32_t i3Block;
        int32_t i3;
        int32_t i3_Block_लंबाई;
        int32_t तथ्य_Block_लंबाई;
        अगर (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3_Block_लंबाई = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_सूचकांक_लंबाई : UCPTRIE_SMALL_सूचकांक_लंबाई;
            तथ्य_Block_लंबाई = UCPTRIE_FAST_तथ्य_BLOCK_लंबाई;
        } अन्यथा {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            अगर (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ऊपरी_Start);
                i1 += UCPTRIE_BMP_सूचकांक_लंबाई - UCPTRIE_OMITTED_BMP_सूचकांक_1_लंबाई;
            } अन्यथा {
                U_ASSERT(c < trie->ऊपरी_Start && trie->ऊपरी_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_सूचकांक_लंबाई;
            }
            i3Block = trie->सूचकांक[
                static_cast<int32_t>(trie->सूचकांक[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_सूचकांक_2_MASK)];
            अगर (i3Block == पूर्व_I3_Block && (c - start) >= UCPTRIE_CP_PER_सूचकांक_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_सूचकांक_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_सूचकांक_2_ENTRY;
                जारी;
            }
            पूर्व_I3_Block = i3Block;
            अगर (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                अगर (have_मूल्य) {
                    अगर (null_मूल्य != value) {
                        वापस c - 1;
                    }
                } अन्यथा {
                    trie_मूल्य = trie->null_मूल्य;
                    value = null_मूल्य;
                    अगर (p_मूल्य != nullptr) { *p_मूल्य = null_मूल्य; }
                    have_मूल्य = true;
                }
                पूर्व_Block = trie->तथ्य_Null_Offset;
                c = (c + UCPTRIE_CP_PER_सूचकांक_2_ENTRY) & ~(UCPTRIE_CP_PER_सूचकांक_2_ENTRY - 1);
                जारी;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_सूचकांक_3_MASK;
            i3_Block_लंबाई = UCPTRIE_सूचकांक_3_BLOCK_लंबाई;
            तथ्य_Block_लंबाई = UCPTRIE_SMALL_तथ्य_BLOCK_लंबाई;
        }
        // Enumerate data blocks for one index-3 block.
        करो {
            int32_t block;
            अगर ((i3Block & 0x8000) == 0) {
                block = सूचकांक[i3Block + i3];
            } अन्यथा {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(सूचकांक[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= सूचकांक[group + gi];
            }
            अगर (block == पूर्व_Block && (c - start) >= तथ्य_Block_लंबाई) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (तथ्य_Block_लंबाई - 1)) == 0);
                c += तथ्य_Block_लंबाई;
            } अन्यथा {
                int32_t तथ्य_Mask = तथ्य_Block_लंबाई - 1;
                पूर्व_Block = block;
                अगर (block == trie->तथ्य_Null_Offset) {
                    // This is the data null block.
                    अगर (have_मूल्य) {
                        अगर (null_मूल्य != value) {
                            वापस c - 1;
                        }
                    } अन्यथा {
                        trie_मूल्य = trie->null_मूल्य;
                        value = null_मूल्य;
                        अगर (p_मूल्य != nullptr) { *p_मूल्य = null_मूल्य; }
                        have_मूल्य = true;
                    }
                    c = (c + तथ्य_Block_लंबाई) & ~तथ्य_Mask;
                } अन्यथा {
                    int32_t di = block + (c & तथ्य_Mask);
                    uint32_t trieValue2 = get_मूल्य(trie->तथ्य, मूल्य_Width, di);
                    अगर (have_मूल्य) {
                        अगर (trieValue2 != trie_मूल्य) {
                            अगर (filter == nullptr ||
                                    maybe_Filter_मूल्य(trieValue2, trie->null_मूल्य, null_मूल्य,
                                                     filter, context) != value) {
                                वापस c - 1;
                            }
                            trie_मूल्य = trieValue2;  // may or may not help
                        }
                    } अन्यथा {
                        trie_मूल्य = trieValue2;
                        value = maybe_Filter_मूल्य(trieValue2, trie->null_मूल्य, null_मूल्य,
                                                 filter, context);
                        अगर (p_मूल्य != nullptr) { *p_मूल्य = value; }
                        have_मूल्य = true;
                    }
                    जबतक ((++c & तथ्य_Mask) != 0) {
                        trieValue2 = get_मूल्य(trie->तथ्य, मूल्य_Width, ++di);
                        अगर (trieValue2 != trie_मूल्य) {
                            अगर (filter == nullptr ||
                                    maybe_Filter_मूल्य(trieValue2, trie->null_मूल्य, null_मूल्य,
                                                     filter, context) != value) {
                                वापस c - 1;
                            }
                            trie_मूल्य = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } जबतक (++i3 < i3_Block_लंबाई);
    } जबतक (c < trie->ऊपरी_Start);
    U_ASSERT(have_मूल्य);
    int32_t di = trie->तथ्य_लंबाई - UCPTRIE_ऊपरी_मूल्य_NEG_तथ्य_OFFSET;
    uint32_t ऊपरी_मूल्य = get_मूल्य(trie->तथ्य, मूल्य_Width, di);
    अगर (maybe_Filter_मूल्य(ऊपरी_मूल्य, trie->null_मूल्य, null_मूल्य,
                         filter, context) != value) {
        वापस c - 1;
    } अन्यथा {
        वापस MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         स्थिर व्योम *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                         UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context, uint32_t *p_मूल्य) {
    अगर (option == UCPMAP_RANGE_NORMAL) {
        वापस getRange(trie, start, filter, context, p_मूल्य);
    }
    uint32_t value;
    अगर (p_मूल्य == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_मूल्य = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_मूल्य);
    अगर (end < 0xd7ff || start > surrEnd) {
        वापस end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    अगर (*p_मूल्य == surrogate_मूल्य) {
        अगर (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            वापस end;
        }
    } अन्यथा {
        अगर (start <= 0xd7ff) {
            वापस 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_मूल्य = surrogate_मूल्य;
        अगर (end > surrEnd) {
            वापस surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    अगर (value2 == surrogate_मूल्य) {
        वापस end2;
    }
    वापस surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(स्थिर UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                 UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context, uint32_t *p_मूल्य) {
    वापस ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_मूल्य,
                                    filter, context, p_मूल्य);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_द्विआधारी(स्थिर UCPTrie *trie,
                 व्योम *तथ्य, int32_t capacity,
                 UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_मूल्य_Width मूल्य_Width = (UCPTrie_मूल्य_Width)trie->मूल्य_Width;
    अगर (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मूल्य_Width < UCPTRIE_मूल्य_BITS_16 || UCPTRIE_मूल्य_BITS_8 < मूल्य_Width ||
            capacity < 0 ||
            (capacity > 0 && (तथ्य == nullptr || (U_POINTER_MASK_LSB(तथ्य, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    int32_t length = (int32_t)माप(UCPTrieHeader) + trie->सूचकांक_लंबाई * 2;
    चयन (मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        length += trie->तथ्य_लंबाई * 2;
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        length += trie->तथ्य_लंबाई * 4;
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        length += trie->तथ्य_लंबाई;
        अवरोध;
    शेष:
        // unreachable
        अवरोध;
    }
    अगर (capacity < length) {
        *pErrorCode = U_बफर_OVERFLOW_ERROR;
        वापस length;
    }

    अक्षर *bytes = (अक्षर *)तथ्य;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->तथ्य_लंबाई & 0xf0000) >> 4) |
        ((trie->तथ्य_Null_Offset & 0xf0000) >> 8) |
        (trie->type << 6) |
        मूल्य_Width);
    header->सूचकांक_लंबाई = (uint16_t)trie->सूचकांक_लंबाई;
    header->तथ्य_लंबाई = (uint16_t)trie->तथ्य_लंबाई;
    header->index3NullOffset = trie->index3NullOffset;
    header->तथ्य_Null_Offset = (uint16_t)trie->तथ्य_Null_Offset;
    header->shifted_ऊपरी_Start = trie->ऊपरी_Start >> UCPTRIE_SHIFT_2;
    bytes += माप(UCPTrieHeader);

    uprv_memcpy(bytes, trie->सूचकांक, trie->सूचकांक_लंबाई * 2);
    bytes += trie->सूचकांक_लंबाई * 2;

    चयन (मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        uprv_memcpy(bytes, trie->तथ्य.ptr16, trie->तथ्य_लंबाई * 2);
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        uprv_memcpy(bytes, trie->तथ्य.ptr32, trie->तथ्य_लंबाई * 4);
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        uprv_memcpy(bytes, trie->तथ्य.ptr8, trie->तथ्य_लंबाई);
        अवरोध;
    शेष:
        // unreachable
        अवरोध;
    }
    वापस length;
}

namespace {

#अगर_घोषित UCPTRIE_DEBUG
दीर्घ गिनती_Null(स्थिर UCPTrie *trie) {
    uint32_t null_मूल्य=trie->null_मूल्य;
    int32_t length=trie->तथ्य_लंबाई;
    दीर्घ count=0;
    चयन (trie->मूल्य_Width) {
    हाल UCPTRIE_मूल्य_BITS_16:
        क्रम(int32_t i=0; i<length; ++i) {
            अगर(trie->तथ्य.ptr16[i]==null_मूल्य) { ++count; }
        }
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_32:
        क्रम(int32_t i=0; i<length; ++i) {
            अगर(trie->तथ्य.ptr32[i]==null_मूल्य) { ++count; }
        }
        अवरोध;
    हाल UCPTRIE_मूल्य_BITS_8:
        क्रम(int32_t i=0; i<length; ++i) {
            अगर(trie->तथ्य.ptr8[i]==null_मूल्य) { ++count; }
        }
        अवरोध;
    शेष:
        // unreachable
        अवरोध;
    }
    वापस count;
}

U_CFUNC व्योम
ucptrie_printLengths(स्थिर UCPTrie *trie, स्थिर अक्षर *which) {
    दीर्घ सूचकांक_लंबाई=trie->सूचकांक_लंबाई;
    दीर्घ तथ्य_लंबाई=(दीर्घ)trie->तथ्य_लंबाई;
    दीर्घ योग_लंबाई=(दीर्घ)माप(UCPTrieHeader)+सूचकांक_लंबाई*2+
            तथ्य_लंबाई*(trie->मूल्य_Width==UCPTRIE_मूल्य_BITS_16 ? 2 :
                        trie->मूल्य_Width==UCPTRIE_मूल्य_BITS_32 ? 4 : 1);
    म_लिखो("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, सूचकांक_लंबाई, तथ्य_लंबाई, गिनती_Null(trie), योग_लंबाई);
}
#पूर्ण_अगर

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(स्थिर UCPMap *map, UChar32 c) {
    वापस ucptrie_get(reinterpret_cast<स्थिर UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(स्थिर UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_मूल्य,
                UCPMap_मूल्य_Filter *filter, स्थिर व्योम *context, uint32_t *p_मूल्य) {
    वापस ucptrie_getRange(reinterpret_cast<स्थिर UCPTrie *>(map), start,
                            option, surrogate_मूल्य,
                            filter, context, p_मूल्य);
}
