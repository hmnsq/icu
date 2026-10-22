// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#ifdef UCPTRIE_DEBUG
#   include <stdio.h>
#endif

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_দ্বিমিক(UCPTrieType type,
                       UCPTrie_মান_Width মান_Width,
                       ধ্রুবক শূন্য* তথ্য U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_দৈর্ঘ্য,
                       UErrorCode* pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    যদি (length <= 0 || (U_POINTER_MASK_LSB(তথ্য, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            মান_Width < UCPTRIE_মান_BITS_ANY || UCPTRIE_মান_BITS_8 < মান_Width) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    // Enough data for a trie header?
    যদি (length < (int32_t)আকার(UCPTrieHeader)) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    // Check the signature.
    ধ্রুবক UCPTrieHeader *header = (ধ্রুবক UCPTrieHeader *)তথ্য;
    যদি (header->signature != UCPTRIE_SIG) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    int32_t options = header->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t মান_Width_Int = options & UCPTRIE_OPTIONS_মান_BITS_MASK;
    যদি (typeInt > UCPTRIE_TYPE_SMALL || মান_Width_Int > UCPTRIE_মান_BITS_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_MASK) != 0) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
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
        ফেরত nullptr;
    }

    // Get the length values and offsets.
    UCPTrie অস্থায়ী_Trie;
    uprv_memset(&অস্থায়ী_Trie, 0, আকার(অস্থায়ী_Trie));
    অস্থায়ী_Trie.সূচক_দৈর্ঘ্য = header->সূচক_দৈর্ঘ্য;
    অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য =
        ((options & UCPTRIE_OPTIONS_তথ্য_দৈর্ঘ্য_MASK) << 4) | header->তথ্য_দৈর্ঘ্য;
    অস্থায়ী_Trie.index3NullOffset = header->index3NullOffset;
    অস্থায়ী_Trie.তথ্য_Null_Offset =
        ((options & UCPTRIE_OPTIONS_তথ্য_NULL_OFFSET_MASK) << 8) | header->তথ্য_Null_Offset;

    অস্থায়ী_Trie.উপরের_Start = header->shifted_উপরের_Start << UCPTRIE_SHIFT_2;
    অস্থায়ী_Trie.shifted12_উপরের_Start = (অস্থায়ী_Trie.উপরের_Start + 0xfff) >> 12;
    অস্থায়ী_Trie.type = type;
    অস্থায়ী_Trie.মান_Width = মান_Width;

    // Calculate the actual length.
    int32_t actual_দৈর্ঘ্য = (int32_t)আকার(UCPTrieHeader) + অস্থায়ী_Trie.সূচক_দৈর্ঘ্য * 2;
    যদি (মান_Width == UCPTRIE_মান_BITS_16) {
        actual_দৈর্ঘ্য += অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য * 2;
    } অন্যথা যদি (মান_Width == UCPTRIE_মান_BITS_32) {
        actual_দৈর্ঘ্য += অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য * 4;
    } অন্যথা {
        actual_দৈর্ঘ্য += অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য;
    }
    যদি (length < actual_দৈর্ঘ্য) {
        *pErrorCode = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        ফেরত nullptr;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(আকার(UCPTrie));
    যদি (trie == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memcpy(trie, &অস্থায়ী_Trie, আকার(অস্থায়ী_Trie));
#ifdef UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#endif

    // Set the pointers to its index and data arrays.
    ধ্রুবক uint16_t *p16 = (ধ্রুবক uint16_t *)(header + 1);
    trie->সূচক = p16;
    p16 += trie->সূচক_দৈর্ঘ্য;

    // Get the data.
    int32_t null_মান_Offset = trie->তথ্য_Null_Offset;
    যদি (null_মান_Offset >= trie->তথ্য_দৈর্ঘ্য) {
        null_মান_Offset = trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET;
    }
    নির্বাচন (মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        trie->তথ্য.ptr16 = p16;
        trie->null_মান = trie->তথ্য.ptr16[null_মান_Offset];
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        trie->তথ্য.ptr32 = (ধ্রুবক uint32_t *)p16;
        trie->null_মান = trie->তথ্য.ptr32[null_মান_Offset];
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        trie->তথ্য.ptr8 = (ধ্রুবক uint8_t *)p16;
        trie->null_মান = trie->তথ্য.ptr8[null_মান_Offset];
        বিরতি;
    পূর্বনির্ধারিত:
        // Unreachable because valueWidth was checked above.
        *pErrorCode = U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    যদি (p_Actual_দৈর্ঘ্য != nullptr) {
        *p_Actual_দৈর্ঘ্য = actual_দৈর্ঘ্য;
    }
    ফেরত trie;
}

U_CAPI শূন্য U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ধ্রুবক UCPTrie *trie) {
    ফেরত (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_মান_Width U_EXPORT2
ucptrie_get_মান_Width(ধ্রুবক UCPTrie *trie) {
    ফেরত (UCPTrie_মান_Width)trie->মান_Width;
}

U_CAPI int32_t U_EXPORT2
ucptrie_internal_Small_সূচক(ধ্রুবক UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_SHIFT_1;
    যদি (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->উপরের_Start);
        i1 += UCPTRIE_BMP_সূচক_দৈর্ঘ্য - UCPTRIE_OMITTED_BMP_সূচক_1_দৈর্ঘ্য;
    } অন্যথা {
        U_ASSERT((uint32_t)c < (uint32_t)trie->উপরের_Start && trie->উপরের_Start > UCPTRIE_SMALL_LIMIT);
        i1 += UCPTRIE_SMALL_সূচক_দৈর্ঘ্য;
    }
    int32_t i3Block = trie->সূচক[
        (int32_t)trie->সূচক[i1] + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_সূচক_2_MASK)];
    int32_t i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_সূচক_3_MASK;
    int32_t তথ্য_Block;
    যদি ((i3Block & 0x8000) == 0) {
        // 16-bit indexes
        তথ্য_Block = trie->সূচক[i3Block + i3];
    } অন্যথা {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3Block = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        তথ্য_Block = ((int32_t)trie->সূচক[i3Block++] << (2 + (2 * i3))) & 0x30000;
        তথ্য_Block |= trie->সূচক[i3Block + i3];
    }
    ফেরত তথ্য_Block + (c & UCPTRIE_SMALL_তথ্য_MASK);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internal_Small_U8_সূচক(ধ্রুবক UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    যদি (c >= trie->উপরের_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        ফেরত trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET;
    }
    ফেরত ucptrie_internal_Small_সূচক(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_internal_U8_পূর্ব_সূচক(ধ্রুবক UCPTrie *trie, UChar32 c,
                            ধ্রুবক uint8_t *start, ধ্রুবক uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    যদি ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } অন্যথা {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_পূর্ব_Char_Safe_Body(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_সূচক(trie, 0xffff, c);
    ফেরত (idx << 3) | i;
}

নামাকাশ {

অন্তঃরেখ uint32_t get_মান(UCPTrie_তথ্য তথ্য, UCPTrie_মান_Width মান_Width, int32_t তথ্য_সূচক) {
    নির্বাচন (মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        ফেরত তথ্য.ptr16[তথ্য_সূচক];
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        ফেরত তথ্য.ptr32[তথ্য_সূচক];
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        ফেরত তথ্য.ptr8[তথ্য_সূচক];
    পূর্বনির্ধারিত:
        // Unreachable if the trie is properly initialized.
        ফেরত 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ধ্রুবক UCPTrie *trie, UChar32 c) {
    int32_t তথ্য_সূচক;
    যদি ((uint32_t)c <= 0x7f) {
        // linear ASCII
        তথ্য_সূচক = c;
    } অন্যথা {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        তথ্য_সূচক = UCPTRIE_CP_সূচক(trie, fastMax, c);
    }
    ফেরত get_মান(trie->তথ্য, (UCPTrie_মান_Width)trie->মান_Width, তথ্য_সূচক);
}

নামাকাশ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

অন্তঃরেখ uint32_t maybe_Filter_মান(uint32_t value, uint32_t trie_Null_মান, uint32_t null_মান,
                                 UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context) {
    যদি (value == trie_Null_মান) {
        value = null_মান;
    } অন্যথা যদি (filter != nullptr) {
        value = filter(context, value);
    }
    ফেরত value;
}

UChar32 getRange(ধ্রুবক শূন্য *t, UChar32 start,
                 UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context, uint32_t *p_মান) {
    যদি (static_cast<uint32_t>(start) > MAX_UNICODE) {
        ফেরত U_SENTINEL;
    }
    ধ্রুবক UCPTrie *trie = reinterpret_cast<ধ্রুবক UCPTrie *>(t);
    UCPTrie_মান_Width মান_Width = static_cast<UCPTrie_মান_Width>(trie->মান_Width);
    যদি (start >= trie->উপরের_Start) {
        যদি (p_মান != nullptr) {
            int32_t di = trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET;
            uint32_t value = get_মান(trie->তথ্য, মান_Width, di);
            যদি (filter != nullptr) { value = filter(context, value); }
            *p_মান = value;
        }
        ফেরত MAX_UNICODE;
    }

    uint32_t null_মান = trie->null_মান;
    যদি (filter != nullptr) { null_মান = filter(context, null_মান); }
    ধ্রুবক uint16_t *সূচক = trie->সূচক;

    int32_t পূর্ব_I3_Block = -1;
    int32_t পূর্ব_Block = -1;
    UChar32 c = start;
    uint32_t trie_মান, value = null_মান;
    যৌক্তিক have_মান = মিথ্যা;
    করো {
        int32_t i3Block;
        int32_t i3;
        int32_t i3_Block_দৈর্ঘ্য;
        int32_t তথ্য_Block_দৈর্ঘ্য;
        যদি (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3Block = 0;
            i3 = c >> UCPTRIE_FAST_SHIFT;
            i3_Block_দৈর্ঘ্য = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_সূচক_দৈর্ঘ্য : UCPTRIE_SMALL_সূচক_দৈর্ঘ্য;
            তথ্য_Block_দৈর্ঘ্য = UCPTRIE_FAST_তথ্য_BLOCK_দৈর্ঘ্য;
        } অন্যথা {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_SHIFT_1;
            যদি (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->উপরের_Start);
                i1 += UCPTRIE_BMP_সূচক_দৈর্ঘ্য - UCPTRIE_OMITTED_BMP_সূচক_1_দৈর্ঘ্য;
            } অন্যথা {
                U_ASSERT(c < trie->উপরের_Start && trie->উপরের_Start > UCPTRIE_SMALL_LIMIT);
                i1 += UCPTRIE_SMALL_সূচক_দৈর্ঘ্য;
            }
            i3Block = trie->সূচক[
                static_cast<int32_t>(trie->সূচক[i1]) + ((c >> UCPTRIE_SHIFT_2) & UCPTRIE_সূচক_2_MASK)];
            যদি (i3Block == পূর্ব_I3_Block && (c - start) >= UCPTRIE_CP_PER_সূচক_2_ENTRY) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_PER_সূচক_2_ENTRY - 1)) == 0);
                c += UCPTRIE_CP_PER_সূচক_2_ENTRY;
                চালু;
            }
            পূর্ব_I3_Block = i3Block;
            যদি (i3Block == trie->index3NullOffset) {
                // This is the index-3 null block.
                যদি (have_মান) {
                    যদি (null_মান != value) {
                        ফেরত c - 1;
                    }
                } অন্যথা {
                    trie_মান = trie->null_মান;
                    value = null_মান;
                    যদি (p_মান != nullptr) { *p_মান = null_মান; }
                    have_মান = সত্য;
                }
                পূর্ব_Block = trie->তথ্য_Null_Offset;
                c = (c + UCPTRIE_CP_PER_সূচক_2_ENTRY) & ~(UCPTRIE_CP_PER_সূচক_2_ENTRY - 1);
                চালু;
            }
            i3 = (c >> UCPTRIE_SHIFT_3) & UCPTRIE_সূচক_3_MASK;
            i3_Block_দৈর্ঘ্য = UCPTRIE_সূচক_3_BLOCK_দৈর্ঘ্য;
            তথ্য_Block_দৈর্ঘ্য = UCPTRIE_SMALL_তথ্য_BLOCK_দৈর্ঘ্য;
        }
        // Enumerate data blocks for one index-3 block.
        করো {
            int32_t block;
            যদি ((i3Block & 0x8000) == 0) {
                block = সূচক[i3Block + i3];
            } অন্যথা {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t group = (i3Block & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                block = (static_cast<int32_t>(সূচক[group++]) << (2 + (2 * gi))) & 0x30000;
                block |= সূচক[group + gi];
            }
            যদি (block == পূর্ব_Block && (c - start) >= তথ্য_Block_দৈর্ঘ্য) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (তথ্য_Block_দৈর্ঘ্য - 1)) == 0);
                c += তথ্য_Block_দৈর্ঘ্য;
            } অন্যথা {
                int32_t তথ্য_Mask = তথ্য_Block_দৈর্ঘ্য - 1;
                পূর্ব_Block = block;
                যদি (block == trie->তথ্য_Null_Offset) {
                    // This is the data null block.
                    যদি (have_মান) {
                        যদি (null_মান != value) {
                            ফেরত c - 1;
                        }
                    } অন্যথা {
                        trie_মান = trie->null_মান;
                        value = null_মান;
                        যদি (p_মান != nullptr) { *p_মান = null_মান; }
                        have_মান = সত্য;
                    }
                    c = (c + তথ্য_Block_দৈর্ঘ্য) & ~তথ্য_Mask;
                } অন্যথা {
                    int32_t di = block + (c & তথ্য_Mask);
                    uint32_t trieValue2 = get_মান(trie->তথ্য, মান_Width, di);
                    যদি (have_মান) {
                        যদি (trieValue2 != trie_মান) {
                            যদি (filter == nullptr ||
                                    maybe_Filter_মান(trieValue2, trie->null_মান, null_মান,
                                                     filter, context) != value) {
                                ফেরত c - 1;
                            }
                            trie_মান = trieValue2;  // may or may not help
                        }
                    } অন্যথা {
                        trie_মান = trieValue2;
                        value = maybe_Filter_মান(trieValue2, trie->null_মান, null_মান,
                                                 filter, context);
                        যদি (p_মান != nullptr) { *p_মান = value; }
                        have_মান = সত্য;
                    }
                    যখন ((++c & তথ্য_Mask) != 0) {
                        trieValue2 = get_মান(trie->তথ্য, মান_Width, ++di);
                        যদি (trieValue2 != trie_মান) {
                            যদি (filter == nullptr ||
                                    maybe_Filter_মান(trieValue2, trie->null_মান, null_মান,
                                                     filter, context) != value) {
                                ফেরত c - 1;
                            }
                            trie_মান = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } যখন (++i3 < i3_Block_দৈর্ঘ্য);
    } যখন (c < trie->উপরের_Start);
    U_ASSERT(have_মান);
    int32_t di = trie->তথ্য_দৈর্ঘ্য - UCPTRIE_উপরের_মান_NEG_তথ্য_OFFSET;
    uint32_t উপরের_মান = get_মান(trie->তথ্য, মান_Width, di);
    যদি (maybe_Filter_মান(উপরের_মান, trie->null_মান, null_মান,
                         filter, context) != value) {
        ফেরত c - 1;
    } অন্যথা {
        ফেরত MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_internalGetRange(UCPTrieGetRange *getRange,
                         ধ্রুবক শূন্য *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_মান,
                         UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context, uint32_t *p_মান) {
    যদি (option == UCPMAP_RANGE_NORMAL) {
        ফেরত getRange(trie, start, filter, context, p_মান);
    }
    uint32_t value;
    যদি (p_মান == nullptr) {
        // We need to examine the range value even if the caller does not want it.
        p_মান = &value;
    }
    UChar32 surrEnd = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 end = getRange(trie, start, filter, context, p_মান);
    যদি (end < 0xd7ff || start > surrEnd) {
        ফেরত end;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    যদি (*p_মান == surrogate_মান) {
        যদি (end >= surrEnd) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            ফেরত end;
        }
    } অন্যথা {
        যদি (start <= 0xd7ff) {
            ফেরত 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_মান = surrogate_মান;
        যদি (end > surrEnd) {
            ফেরত surrEnd;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surrEnd + 1, filter, context, &value2);
    যদি (value2 == surrogate_মান) {
        ফেরত end2;
    }
    ফেরত surrEnd;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ধ্রুবক UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_মান,
                 UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context, uint32_t *p_মান) {
    ফেরত ucptrie_internalGetRange(getRange, trie, start,
                                    option, surrogate_মান,
                                    filter, context, p_মান);
}

U_CAPI int32_t U_EXPORT2
ucptrie_to_দ্বিমিক(ধ্রুবক UCPTrie *trie,
                 শূন্য *তথ্য, int32_t capacity,
                 UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_মান_Width মান_Width = (UCPTrie_মান_Width)trie->মান_Width;
    যদি (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            মান_Width < UCPTRIE_মান_BITS_16 || UCPTRIE_মান_BITS_8 < মান_Width ||
            capacity < 0 ||
            (capacity > 0 && (তথ্য == nullptr || (U_POINTER_MASK_LSB(তথ্য, 3) != 0)))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    int32_t length = (int32_t)আকার(UCPTrieHeader) + trie->সূচক_দৈর্ঘ্য * 2;
    নির্বাচন (মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        length += trie->তথ্য_দৈর্ঘ্য * 2;
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        length += trie->তথ্য_দৈর্ঘ্য * 4;
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        length += trie->তথ্য_দৈর্ঘ্য;
        বিরতি;
    পূর্বনির্ধারিত:
        // unreachable
        বিরতি;
    }
    যদি (capacity < length) {
        *pErrorCode = U_বাফার_OVERFLOW_ERROR;
        ফেরত length;
    }

    অক্ষর *bytes = (অক্ষর *)তথ্য;
    UCPTrieHeader *header = (UCPTrieHeader *)bytes;
    header->signature = UCPTRIE_SIG;  // "Tri3"
    header->options = (uint16_t)(
        ((trie->তথ্য_দৈর্ঘ্য & 0xf0000) >> 4) |
        ((trie->তথ্য_Null_Offset & 0xf0000) >> 8) |
        (trie->type << 6) |
        মান_Width);
    header->সূচক_দৈর্ঘ্য = (uint16_t)trie->সূচক_দৈর্ঘ্য;
    header->তথ্য_দৈর্ঘ্য = (uint16_t)trie->তথ্য_দৈর্ঘ্য;
    header->index3NullOffset = trie->index3NullOffset;
    header->তথ্য_Null_Offset = (uint16_t)trie->তথ্য_Null_Offset;
    header->shifted_উপরের_Start = trie->উপরের_Start >> UCPTRIE_SHIFT_2;
    bytes += আকার(UCPTrieHeader);

    uprv_memcpy(bytes, trie->সূচক, trie->সূচক_দৈর্ঘ্য * 2);
    bytes += trie->সূচক_দৈর্ঘ্য * 2;

    নির্বাচন (মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        uprv_memcpy(bytes, trie->তথ্য.ptr16, trie->তথ্য_দৈর্ঘ্য * 2);
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        uprv_memcpy(bytes, trie->তথ্য.ptr32, trie->তথ্য_দৈর্ঘ্য * 4);
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        uprv_memcpy(bytes, trie->তথ্য.ptr8, trie->তথ্য_দৈর্ঘ্য);
        বিরতি;
    পূর্বনির্ধারিত:
        // unreachable
        বিরতি;
    }
    ফেরত length;
}

নামাকাশ {

#ifdef UCPTRIE_DEBUG
long গুনতি_Null(ধ্রুবক UCPTrie *trie) {
    uint32_t null_মান=trie->null_মান;
    int32_t length=trie->তথ্য_দৈর্ঘ্য;
    long count=0;
    নির্বাচন (trie->মান_Width) {
    ক্ষেত্র UCPTRIE_মান_BITS_16:
        জন্য(int32_t i=0; i<length; ++i) {
            যদি(trie->তথ্য.ptr16[i]==null_মান) { ++count; }
        }
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_32:
        জন্য(int32_t i=0; i<length; ++i) {
            যদি(trie->তথ্য.ptr32[i]==null_মান) { ++count; }
        }
        বিরতি;
    ক্ষেত্র UCPTRIE_মান_BITS_8:
        জন্য(int32_t i=0; i<length; ++i) {
            যদি(trie->তথ্য.ptr8[i]==null_মান) { ++count; }
        }
        বিরতি;
    পূর্বনির্ধারিত:
        // unreachable
        বিরতি;
    }
    ফেরত count;
}

U_CFUNC শূন্য
ucptrie_printLengths(ধ্রুবক UCPTrie *trie, ধ্রুবক অক্ষর *which) {
    long সূচক_দৈর্ঘ্য=trie->সূচক_দৈর্ঘ্য;
    long তথ্য_দৈর্ঘ্য=(long)trie->তথ্য_দৈর্ঘ্য;
    long যোগ_দৈর্ঘ্য=(long)আকার(UCPTrieHeader)+সূচক_দৈর্ঘ্য*2+
            তথ্য_দৈর্ঘ্য*(trie->মান_Width==UCPTRIE_মান_BITS_16 ? 2 :
                        trie->মান_Width==UCPTRIE_মান_BITS_32 ? 4 : 1);
    printf("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, সূচক_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য, গুনতি_Null(trie), যোগ_দৈর্ঘ্য);
}
#endif

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ধ্রুবক UCPMap *map, UChar32 c) {
    ফেরত ucptrie_get(reinterpret_cast<ধ্রুবক UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ধ্রুবক UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_মান,
                UCPMap_মান_Filter *filter, ধ্রুবক শূন্য *context, uint32_t *p_মান) {
    ফেরত ucptrie_getRange(reinterpret_cast<ধ্রুবক UCPTrie *>(map), start,
                            option, surrogate_মান,
                            filter, context, p_মান);
}
