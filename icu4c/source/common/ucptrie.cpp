// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#यदि_परिभाषितम् UCPTRIE_DEBUG
#   include <stdio.h>
#यदि_अन्तः

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
                       UCPTrie_मानम्_विस्तारः मानम्_विस्तारः,
                       ध्रुव शून्य* दत्त U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_दीर्घता,
                       UError_संहिता* p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि (length <= 0 || (U_POINTER_आवरणम्_LSB(दत्त, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            मानम्_विस्तारः < UCPTRIE_मानम्_द्वयङ्काः_ANY || UCPTRIE_मानम्_द्वयङ्काः_8 < मानम्_विस्तारः) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    // Enough data for a trie header?
    यदि (length < (int32_t)परिमाण(UCPTrie_शीर्षकम्)) {
        *p_Error_संहिता = U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    // Check the signature.
    ध्रुव UCPTrie_शीर्षकम् *शीर्षकम् = (ध्रुव UCPTrie_शीर्षकम् *)दत्त;
    यदि (शीर्षकम्->signature != UCPTRIE_SIG) {
        *p_Error_संहिता = U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    int32_t options = शीर्षकम्->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t मानम्_विस्तारः_Int = options & UCPTRIE_OPTIONS_मानम्_द्वयङ्काः_आवरणम्;
    यदि (typeInt > UCPTRIE_TYPE_SMALL || मानम्_विस्तारः_Int > UCPTRIE_मानम्_द्वयङ्काः_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_आवरणम्) != 0) {
        *p_Error_संहिता = U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_मानम्_विस्तारः actual_मानम्_विस्तारः = (UCPTrie_मानम्_विस्तारः)मानम्_विस्तारः_Int;
    यदि (type < 0) {
        type = actualType;
    }
    यदि (मानम्_विस्तारः < 0) {
        मानम्_विस्तारः = actual_मानम्_विस्तारः;
    }
    यदि (type != actualType || मानम्_विस्तारः != actual_मानम्_विस्तारः) {
        *p_Error_संहिता = U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    // Get the length values and offsets.
    UCPTrie अस्थायी_Trie;
    uprv_memset(&अस्थायी_Trie, 0, परिमाण(अस्थायी_Trie));
    अस्थायी_Trie.सूचकांक_दीर्घता = शीर्षकम्->सूचकांक_दीर्घता;
    अस्थायी_Trie.दत्त_दीर्घता =
        ((options & UCPTRIE_OPTIONS_दत्त_दीर्घता_आवरणम्) << 4) | शीर्षकम्->दत्त_दीर्घता;
    अस्थायी_Trie.index3_Null_विस्थापनम् = शीर्षकम्->index3_Null_विस्थापनम्;
    अस्थायी_Trie.दत्त_Null_विस्थापनम् =
        ((options & UCPTRIE_OPTIONS_दत्त_NULL_विस्थापनम्_आवरणम्) << 8) | शीर्षकम्->दत्त_Null_विस्थापनम्;

    अस्थायी_Trie.उच्चः_Start = शीर्षकम्->shifted_उच्चः_Start << UCPTRIE_सरणम्_2;
    अस्थायी_Trie.shifted12_उच्चः_Start = (अस्थायी_Trie.उच्चः_Start + 0xfff) >> 12;
    अस्थायी_Trie.type = type;
    अस्थायी_Trie.मानम्_विस्तारः = मानम्_विस्तारः;

    // Calculate the actual length.
    int32_t actual_दीर्घता = (int32_t)परिमाण(UCPTrie_शीर्षकम्) + अस्थायी_Trie.सूचकांक_दीर्घता * 2;
    यदि (मानम्_विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_16) {
        actual_दीर्घता += अस्थायी_Trie.दत्त_दीर्घता * 2;
    } अन्यथा यदि (मानम्_विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_32) {
        actual_दीर्घता += अस्थायी_Trie.दत्त_दीर्घता * 4;
    } अन्यथा {
        actual_दीर्घता += अस्थायी_Trie.दत्त_दीर्घता;
    }
    यदि (length < actual_दीर्घता) {
        *p_Error_संहिता = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        प्रत्यागम शून्यसूचकः;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(परिमाण(UCPTrie));
    यदि (trie == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(trie, &अस्थायी_Trie, परिमाण(अस्थायी_Trie));
#यदि_परिभाषितम् UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#यदि_अन्तः

    // Set the pointers to its index and data arrays.
    ध्रुव uint16_t *p16 = (ध्रुव uint16_t *)(शीर्षकम् + 1);
    trie->सूचकांक = p16;
    p16 += trie->सूचकांक_दीर्घता;

    // Get the data.
    int32_t null_मानम्_विस्थापनम् = trie->दत्त_Null_विस्थापनम्;
    यदि (null_मानम्_विस्थापनम् >= trie->दत्त_दीर्घता) {
        null_मानम्_विस्थापनम् = trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्;
    }
    विकल्प (मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        trie->दत्त.ptr16 = p16;
        trie->null_मानम् = trie->दत्त.ptr16[null_मानम्_विस्थापनम्];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        trie->दत्त.ptr32 = (ध्रुव uint32_t *)p16;
        trie->null_मानम् = trie->दत्त.ptr32[null_मानम्_विस्थापनम्];
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        trie->दत्त.ptr8 = (ध्रुव uint8_t *)p16;
        trie->null_मानम् = trie->दत्त.ptr8[null_मानम्_विस्थापनम्];
        भङ्ग;
    स्वभाव:
        // Unreachable because valueWidth was checked above.
        *p_Error_संहिता = U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि (p_Actual_दीर्घता != शून्यसूचकः) {
        *p_Actual_दीर्घता = actual_दीर्घता;
    }
    प्रत्यागम trie;
}

U_CAPI शून्य U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(ध्रुव UCPTrie *trie) {
    प्रत्यागम (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_मानम्_विस्तारः U_EXPORT2
ucptrie_get_मानम्_विस्तारः(ध्रुव UCPTrie *trie) {
    प्रत्यागम (UCPTrie_मानम्_विस्तारः)trie->मानम्_विस्तारः;
}

U_CAPI int32_t U_EXPORT2
ucptrie_आन्तरिकम्_Small_सूचकांक(ध्रुव UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_सरणम्_1;
    यदि (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->उच्चः_Start);
        i1 += UCPTRIE_BMP_सूचकांक_दीर्घता - UCPTRIE_OMITTED_BMP_सूचकांक_1_दीर्घता;
    } अन्यथा {
        U_ASSERT((uint32_t)c < (uint32_t)trie->उच्चः_Start && trie->उच्चः_Start > UCPTRIE_SMALL_सीमा);
        i1 += UCPTRIE_SMALL_सूचकांक_दीर्घता;
    }
    int32_t i3_खण्डम् = trie->सूचकांक[
        (int32_t)trie->सूचकांक[i1] + ((c >> UCPTRIE_सरणम्_2) & UCPTRIE_सूचकांक_2_आवरणम्)];
    int32_t i3 = (c >> UCPTRIE_सरणम्_3) & UCPTRIE_सूचकांक_3_आवरणम्;
    int32_t दत्त_खण्डम्;
    यदि ((i3_खण्डम् & 0x8000) == 0) {
        // 16-bit indexes
        दत्त_खण्डम् = trie->सूचकांक[i3_खण्डम् + i3];
    } अन्यथा {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_खण्डम् = (i3_खण्डम् & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        दत्त_खण्डम् = ((int32_t)trie->सूचकांक[i3_खण्डम्++] << (2 + (2 * i3))) & 0x30000;
        दत्त_खण्डम् |= trie->सूचकांक[i3_खण्डम् + i3];
    }
    प्रत्यागम दत्त_खण्डम् + (c & UCPTRIE_SMALL_दत्त_आवरणम्);
}

U_CAPI int32_t U_EXPORT2
ucptrie_आन्तरिकम्_Small_U8_सूचकांक(ध्रुव UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    यदि (c >= trie->उच्चः_Start) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        प्रत्यागम trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्;
    }
    प्रत्यागम ucptrie_आन्तरिकम्_Small_सूचकांक(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_आन्तरिकम्_U8_पूर्व_सूचकांक(ध्रुव UCPTrie *trie, UChar32 c,
                            ध्रुव uint8_t *start, ध्रुव uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    यदि ((src - start) <= 7) {
        i = length = (int32_t)(src - start);
    } अन्यथा {
        i = length = 7;
        start = src - 7;
    }
    c = utf8_पूर्व_Char_Safe_Body(start, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_सूचकांक(trie, 0xffff, c);
    प्रत्यागम (idx << 3) | i;
}

नामाकाश {

अन्तःस्थ uint32_t get_मानम्(UCPTrie_दत्त दत्त, UCPTrie_मानम्_विस्तारः मानम्_विस्तारः, int32_t दत्त_सूचकांक) {
    विकल्प (मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        प्रत्यागम दत्त.ptr16[दत्त_सूचकांक];
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        प्रत्यागम दत्त.ptr32[दत्त_सूचकांक];
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        प्रत्यागम दत्त.ptr8[दत्त_सूचकांक];
    स्वभाव:
        // Unreachable if the trie is properly initialized.
        प्रत्यागम 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(ध्रुव UCPTrie *trie, UChar32 c) {
    int32_t दत्त_सूचकांक;
    यदि ((uint32_t)c <= 0x7f) {
        // linear ASCII
        दत्त_सूचकांक = c;
    } अन्यथा {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        दत्त_सूचकांक = UCPTRIE_CP_सूचकांक(trie, fastMax, c);
    }
    प्रत्यागम get_मानम्(trie->दत्त, (UCPTrie_मानम्_विस्तारः)trie->मानम्_विस्तारः, दत्त_सूचकांक);
}

नामाकाश {

constexpr int32_t MAX_UNICODE = 0x10ffff;

अन्तःस्थ uint32_t maybe_Filter_मानम्(uint32_t value, uint32_t trie_Null_मानम्, uint32_t null_मानम्,
                                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context) {
    यदि (value == trie_Null_मानम्) {
        value = null_मानम्;
    } अन्यथा यदि (filter != शून्यसूचकः) {
        value = filter(context, value);
    }
    प्रत्यागम value;
}

UChar32 getRange(ध्रुव शून्य *t, UChar32 start,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    यदि (static_cast<uint32_t>(start) > MAX_UNICODE) {
        प्रत्यागम U_SENTINEL;
    }
    ध्रुव UCPTrie *trie = reinterpret_cast<ध्रुव UCPTrie *>(t);
    UCPTrie_मानम्_विस्तारः मानम्_विस्तारः = static_cast<UCPTrie_मानम्_विस्तारः>(trie->मानम्_विस्तारः);
    यदि (start >= trie->उच्चः_Start) {
        यदि (p_मानम् != शून्यसूचकः) {
            int32_t di = trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्;
            uint32_t value = get_मानम्(trie->दत्त, मानम्_विस्तारः, di);
            यदि (filter != शून्यसूचकः) { value = filter(context, value); }
            *p_मानम् = value;
        }
        प्रत्यागम MAX_UNICODE;
    }

    uint32_t null_मानम् = trie->null_मानम्;
    यदि (filter != शून्यसूचकः) { null_मानम् = filter(context, null_मानम्); }
    ध्रुव uint16_t *सूचकांक = trie->सूचकांक;

    int32_t पूर्व_I3_खण्डम् = -1;
    int32_t पूर्व_खण्डम् = -1;
    UChar32 c = start;
    uint32_t trie_मानम्, value = null_मानम्;
    तार्किक have_मानम् = असत्य;
    कुरु {
        int32_t i3_खण्डम्;
        int32_t i3;
        int32_t i3_खण्डम्_दीर्घता;
        int32_t दत्त_खण्डम्_दीर्घता;
        यदि (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_खण्डम् = 0;
            i3 = c >> UCPTRIE_FAST_सरणम्;
            i3_खण्डम्_दीर्घता = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_सूचकांक_दीर्घता : UCPTRIE_SMALL_सूचकांक_दीर्घता;
            दत्त_खण्डम्_दीर्घता = UCPTRIE_FAST_दत्त_खण्डम्_दीर्घता;
        } अन्यथा {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_सरणम्_1;
            यदि (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->उच्चः_Start);
                i1 += UCPTRIE_BMP_सूचकांक_दीर्घता - UCPTRIE_OMITTED_BMP_सूचकांक_1_दीर्घता;
            } अन्यथा {
                U_ASSERT(c < trie->उच्चः_Start && trie->उच्चः_Start > UCPTRIE_SMALL_सीमा);
                i1 += UCPTRIE_SMALL_सूचकांक_दीर्घता;
            }
            i3_खण्डम् = trie->सूचकांक[
                static_cast<int32_t>(trie->सूचकांक[i1]) + ((c >> UCPTRIE_सरणम्_2) & UCPTRIE_सूचकांक_2_आवरणम्)];
            यदि (i3_खण्डम् == पूर्व_I3_खण्डम् && (c - start) >= UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः - 1)) == 0);
                c += UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः;
                अनुवर्त;
            }
            पूर्व_I3_खण्डम् = i3_खण्डम्;
            यदि (i3_खण्डम् == trie->index3_Null_विस्थापनम्) {
                // This is the index-3 null block.
                यदि (have_मानम्) {
                    यदि (null_मानम् != value) {
                        प्रत्यागम c - 1;
                    }
                } अन्यथा {
                    trie_मानम् = trie->null_मानम्;
                    value = null_मानम्;
                    यदि (p_मानम् != शून्यसूचकः) { *p_मानम् = null_मानम्; }
                    have_मानम् = सत्य;
                }
                पूर्व_खण्डम् = trie->दत्त_Null_विस्थापनम्;
                c = (c + UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः) & ~(UCPTRIE_CP_प्रत्येकम्_सूचकांक_2_प्रविष्टिः - 1);
                अनुवर्त;
            }
            i3 = (c >> UCPTRIE_सरणम्_3) & UCPTRIE_सूचकांक_3_आवरणम्;
            i3_खण्डम्_दीर्घता = UCPTRIE_सूचकांक_3_खण्डम्_दीर्घता;
            दत्त_खण्डम्_दीर्घता = UCPTRIE_SMALL_दत्त_खण्डम्_दीर्घता;
        }
        // Enumerate data blocks for one index-3 block.
        कुरु {
            int32_t खण्डम्;
            यदि ((i3_खण्डम् & 0x8000) == 0) {
                खण्डम् = सूचकांक[i3_खण्डम् + i3];
            } अन्यथा {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t गणः = (i3_खण्डम् & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                खण्डम् = (static_cast<int32_t>(सूचकांक[गणः++]) << (2 + (2 * gi))) & 0x30000;
                खण्डम् |= सूचकांक[गणः + gi];
            }
            यदि (खण्डम् == पूर्व_खण्डम् && (c - start) >= दत्त_खण्डम्_दीर्घता) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (दत्त_खण्डम्_दीर्घता - 1)) == 0);
                c += दत्त_खण्डम्_दीर्घता;
            } अन्यथा {
                int32_t दत्त_आवरणम् = दत्त_खण्डम्_दीर्घता - 1;
                पूर्व_खण्डम् = खण्डम्;
                यदि (खण्डम् == trie->दत्त_Null_विस्थापनम्) {
                    // This is the data null block.
                    यदि (have_मानम्) {
                        यदि (null_मानम् != value) {
                            प्रत्यागम c - 1;
                        }
                    } अन्यथा {
                        trie_मानम् = trie->null_मानम्;
                        value = null_मानम्;
                        यदि (p_मानम् != शून्यसूचकः) { *p_मानम् = null_मानम्; }
                        have_मानम् = सत्य;
                    }
                    c = (c + दत्त_खण्डम्_दीर्घता) & ~दत्त_आवरणम्;
                } अन्यथा {
                    int32_t di = खण्डम् + (c & दत्त_आवरणम्);
                    uint32_t trieValue2 = get_मानम्(trie->दत्त, मानम्_विस्तारः, di);
                    यदि (have_मानम्) {
                        यदि (trieValue2 != trie_मानम्) {
                            यदि (filter == शून्यसूचकः ||
                                    maybe_Filter_मानम्(trieValue2, trie->null_मानम्, null_मानम्,
                                                     filter, context) != value) {
                                प्रत्यागम c - 1;
                            }
                            trie_मानम् = trieValue2;  // may or may not help
                        }
                    } अन्यथा {
                        trie_मानम् = trieValue2;
                        value = maybe_Filter_मानम्(trieValue2, trie->null_मानम्, null_मानम्,
                                                 filter, context);
                        यदि (p_मानम् != शून्यसूचकः) { *p_मानम् = value; }
                        have_मानम् = सत्य;
                    }
                    यावत् ((++c & दत्त_आवरणम्) != 0) {
                        trieValue2 = get_मानम्(trie->दत्त, मानम्_विस्तारः, ++di);
                        यदि (trieValue2 != trie_मानम्) {
                            यदि (filter == शून्यसूचकः ||
                                    maybe_Filter_मानम्(trieValue2, trie->null_मानम्, null_मानम्,
                                                     filter, context) != value) {
                                प्रत्यागम c - 1;
                            }
                            trie_मानम् = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } यावत् (++i3 < i3_खण्डम्_दीर्घता);
    } यावत् (c < trie->उच्चः_Start);
    U_ASSERT(have_मानम्);
    int32_t di = trie->दत्त_दीर्घता - UCPTRIE_उच्चः_मानम्_NEG_दत्त_विस्थापनम्;
    uint32_t उच्चः_मानम् = get_मानम्(trie->दत्त, मानम्_विस्तारः, di);
    यदि (maybe_Filter_मानम्(उच्चः_मानम्, trie->null_मानम्, null_मानम्,
                         filter, context) != value) {
        प्रत्यागम c - 1;
    } अन्यथा {
        प्रत्यागम MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_आन्तरिकम्_Get_Range(UCPTrieGetRange *getRange,
                         ध्रुव शून्य *trie, UChar32 start,
                         UCPMapRangeOption option, uint32_t surrogate_मानम्,
                         UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    यदि (option == UCPMAP_RANGE_प्रसामान्यम्) {
        प्रत्यागम getRange(trie, start, filter, context, p_मानम्);
    }
    uint32_t value;
    यदि (p_मानम् == शून्यसूचकः) {
        // We need to examine the range value even if the caller does not want it.
        p_मानम् = &value;
    }
    UChar32 surr_अन्तः = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 अन्तः = getRange(trie, start, filter, context, p_मानम्);
    यदि (अन्तः < 0xd7ff || start > surr_अन्तः) {
        प्रत्यागम अन्तः;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    यदि (*p_मानम् == surrogate_मानम्) {
        यदि (अन्तः >= surr_अन्तः) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            प्रत्यागम अन्तः;
        }
    } अन्यथा {
        यदि (start <= 0xd7ff) {
            प्रत्यागम 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_मानम् = surrogate_मानम्;
        यदि (अन्तः > surr_अन्तः) {
            प्रत्यागम surr_अन्तः;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_अन्तः + 1, filter, context, &value2);
    यदि (value2 == surrogate_मानम्) {
        प्रत्यागम end2;
    }
    प्रत्यागम surr_अन्तः;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(ध्रुव UCPTrie *trie, UChar32 start,
                 UCPMapRangeOption option, uint32_t surrogate_मानम्,
                 UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    प्रत्यागम ucptrie_आन्तरिकम्_Get_Range(getRange, trie, start,
                                    option, surrogate_मानम्,
                                    filter, context, p_मानम्);
}

U_CAPI int32_t U_EXPORT2
ucptrie_गन्तव्यम्_द्विमान(ध्रुव UCPTrie *trie,
                 शून्य *दत्त, int32_t capacity,
                 UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_मानम्_विस्तारः मानम्_विस्तारः = (UCPTrie_मानम्_विस्तारः)trie->मानम्_विस्तारः;
    यदि (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            मानम्_विस्तारः < UCPTRIE_मानम्_द्वयङ्काः_16 || UCPTRIE_मानम्_द्वयङ्काः_8 < मानम्_विस्तारः ||
            capacity < 0 ||
            (capacity > 0 && (दत्त == शून्यसूचकः || (U_POINTER_आवरणम्_LSB(दत्त, 3) != 0)))) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    int32_t length = (int32_t)परिमाण(UCPTrie_शीर्षकम्) + trie->सूचकांक_दीर्घता * 2;
    विकल्प (मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        length += trie->दत्त_दीर्घता * 2;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        length += trie->दत्त_दीर्घता * 4;
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        length += trie->दत्त_दीर्घता;
        भङ्ग;
    स्वभाव:
        // unreachable
        भङ्ग;
    }
    यदि (capacity < length) {
        *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम length;
    }

    वर्ण *अष्टकानि = (वर्ण *)दत्त;
    UCPTrie_शीर्षकम् *शीर्षकम् = (UCPTrie_शीर्षकम् *)अष्टकानि;
    शीर्षकम्->signature = UCPTRIE_SIG;  // "Tri3"
    शीर्षकम्->options = (uint16_t)(
        ((trie->दत्त_दीर्घता & 0xf0000) >> 4) |
        ((trie->दत्त_Null_विस्थापनम् & 0xf0000) >> 8) |
        (trie->type << 6) |
        मानम्_विस्तारः);
    शीर्षकम्->सूचकांक_दीर्घता = (uint16_t)trie->सूचकांक_दीर्घता;
    शीर्षकम्->दत्त_दीर्घता = (uint16_t)trie->दत्त_दीर्घता;
    शीर्षकम्->index3_Null_विस्थापनम् = trie->index3_Null_विस्थापनम्;
    शीर्षकम्->दत्त_Null_विस्थापनम् = (uint16_t)trie->दत्त_Null_विस्थापनम्;
    शीर्षकम्->shifted_उच्चः_Start = trie->उच्चः_Start >> UCPTRIE_सरणम्_2;
    अष्टकानि += परिमाण(UCPTrie_शीर्षकम्);

    uprv_memcpy(अष्टकानि, trie->सूचकांक, trie->सूचकांक_दीर्घता * 2);
    अष्टकानि += trie->सूचकांक_दीर्घता * 2;

    विकल्प (मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        uprv_memcpy(अष्टकानि, trie->दत्त.ptr16, trie->दत्त_दीर्घता * 2);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        uprv_memcpy(अष्टकानि, trie->दत्त.ptr32, trie->दत्त_दीर्घता * 4);
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        uprv_memcpy(अष्टकानि, trie->दत्त.ptr8, trie->दत्त_दीर्घता);
        भङ्ग;
    स्वभाव:
        // unreachable
        भङ्ग;
    }
    प्रत्यागम length;
}

नामाकाश {

#यदि_परिभाषितम् UCPTRIE_DEBUG
दीर्घ गणनम्_Null(ध्रुव UCPTrie *trie) {
    uint32_t null_मानम्=trie->null_मानम्;
    int32_t length=trie->दत्त_दीर्घता;
    दीर्घ count=0;
    विकल्प (trie->मानम्_विस्तारः) {
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_16:
        प्रति(int32_t i=0; i<length; ++i) {
            यदि(trie->दत्त.ptr16[i]==null_मानम्) { ++count; }
        }
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_32:
        प्रति(int32_t i=0; i<length; ++i) {
            यदि(trie->दत्त.ptr32[i]==null_मानम्) { ++count; }
        }
        भङ्ग;
    पक्ष UCPTRIE_मानम्_द्वयङ्काः_8:
        प्रति(int32_t i=0; i<length; ++i) {
            यदि(trie->दत्त.ptr8[i]==null_मानम्) { ++count; }
        }
        भङ्ग;
    स्वभाव:
        // unreachable
        भङ्ग;
    }
    प्रत्यागम count;
}

U_CFUNC शून्य
ucptrie_printLengths(ध्रुव UCPTrie *trie, ध्रुव वर्ण *which) {
    दीर्घ सूचकांक_दीर्घता=trie->सूचकांक_दीर्घता;
    दीर्घ दत्त_दीर्घता=(दीर्घ)trie->दत्त_दीर्घता;
    दीर्घ योग_दीर्घता=(दीर्घ)परिमाण(UCPTrie_शीर्षकम्)+सूचकांक_दीर्घता*2+
            दत्त_दीर्घता*(trie->मानम्_विस्तारः==UCPTRIE_मानम्_द्वयङ्काः_16 ? 2 :
                        trie->मानम्_विस्तारः==UCPTRIE_मानम्_द्वयङ्काः_32 ? 4 : 1);
    प्रारूप_लिख("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, सूचकांक_दीर्घता, दत्त_दीर्घता, गणनम्_Null(trie), योग_दीर्घता);
}
#यदि_अन्तः

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(ध्रुव UCPMap *map, UChar32 c) {
    प्रत्यागम ucptrie_get(reinterpret_cast<ध्रुव UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(ध्रुव UCPMap *map, UChar32 start,
                UCPMapRangeOption option, uint32_t surrogate_मानम्,
                UCPMap_मानम्_Filter *filter, ध्रुव शून्य *context, uint32_t *p_मानम्) {
    प्रत्यागम ucptrie_getRange(reinterpret_cast<ध्रुव UCPTrie *>(map), start,
                            option, surrogate_मानम्,
                            filter, context, p_मानम्);
}
