// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucptrie.cpp (modified from utrie2.cpp)
// created: 2017dec29 Markus W. Scherer

// #define UCPTRIE_DEBUG
#నిర్వచితమైతే UCPTRIE_DEBUG
#   include <stdio.h>
#అయితే_ముగింపు

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucptrie_impl.h"

U_CAPI UCPTrie* U_EXPORT2
ucptrie_open_From_ద్విమాన(UCPTrieType type,
                       UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు,
                       స్థిరరాశి శూన్య* దత్తాంశం U_LIFETIME_BOUND,
                       int32_t length,
                       int32_t* p_Actual_నిడివి,
                       UError_సంకేతం* p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (length <= 0 || (U_సూచిక_ముసుగు_LSB(దత్తాంశం, 3) != 0) ||
            type < UCPTRIE_TYPE_ANY || UCPTRIE_TYPE_SMALL < type ||
            విలువ_వెడల్పు < UCPTRIE_విలువ_బిట్లు_ANY || UCPTRIE_విలువ_బిట్లు_8 < విలువ_వెడల్పు) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    // Enough data for a trie header?
    ఒకవేళ (length < (int32_t)పరిమాణం(UCPTrie_శీర్షిక)) {
        *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    // Check the signature.
    స్థిరరాశి UCPTrie_శీర్షిక *శీర్షిక = (స్థిరరాశి UCPTrie_శీర్షిక *)దత్తాంశం;
    ఒకవేళ (శీర్షిక->signature != UCPTRIE_SIG) {
        *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    int32_t options = శీర్షిక->options;
    int32_t typeInt = (options >> 6) & 3;
    int32_t విలువ_వెడల్పు_Int = options & UCPTRIE_OPTIONS_విలువ_బిట్లు_ముసుగు;
    ఒకవేళ (typeInt > UCPTRIE_TYPE_SMALL || విలువ_వెడల్పు_Int > UCPTRIE_విలువ_బిట్లు_8 ||
            (options & UCPTRIE_OPTIONS_RESERVED_ముసుగు) != 0) {
        *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    UCPTrieType actualType = (UCPTrieType)typeInt;
    UCPTrie_విలువ_వెడల్పు actual_విలువ_వెడల్పు = (UCPTrie_విలువ_వెడల్పు)విలువ_వెడల్పు_Int;
    ఒకవేళ (type < 0) {
        type = actualType;
    }
    ఒకవేళ (విలువ_వెడల్పు < 0) {
        విలువ_వెడల్పు = actual_విలువ_వెడల్పు;
    }
    ఒకవేళ (type != actualType || విలువ_వెడల్పు != actual_విలువ_వెడల్పు) {
        *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    // Get the length values and offsets.
    UCPTrie తాత్కాలిక_Trie;
    uprv_memset(&తాత్కాలిక_Trie, 0, పరిమాణం(తాత్కాలిక_Trie));
    తాత్కాలిక_Trie.సూచీ_నిడివి = శీర్షిక->సూచీ_నిడివి;
    తాత్కాలిక_Trie.దత్తాంశం_నిడివి =
        ((options & UCPTRIE_OPTIONS_దత్తాంశం_నిడివి_ముసుగు) << 4) | శీర్షిక->దత్తాంశం_నిడివి;
    తాత్కాలిక_Trie.index3_Null_స్థానభ్రంశం = శీర్షిక->index3_Null_స్థానభ్రంశం;
    తాత్కాలిక_Trie.దత్తాంశం_Null_స్థానభ్రంశం =
        ((options & UCPTRIE_OPTIONS_దత్తాంశం_NULL_స్థానభ్రంశం_ముసుగు) << 8) | శీర్షిక->దత్తాంశం_Null_స్థానభ్రంశం;

    తాత్కాలిక_Trie.ఎగువ_ప్రారంభం = శీర్షిక->shifted_ఎగువ_ప్రారంభం << UCPTRIE_జరుపు_2;
    తాత్కాలిక_Trie.shifted12_ఎగువ_ప్రారంభం = (తాత్కాలిక_Trie.ఎగువ_ప్రారంభం + 0xfff) >> 12;
    తాత్కాలిక_Trie.type = type;
    తాత్కాలిక_Trie.విలువ_వెడల్పు = విలువ_వెడల్పు;

    // Calculate the actual length.
    int32_t actual_నిడివి = (int32_t)పరిమాణం(UCPTrie_శీర్షిక) + తాత్కాలిక_Trie.సూచీ_నిడివి * 2;
    ఒకవేళ (విలువ_వెడల్పు == UCPTRIE_విలువ_బిట్లు_16) {
        actual_నిడివి += తాత్కాలిక_Trie.దత్తాంశం_నిడివి * 2;
    } లేకపోతే ఒకవేళ (విలువ_వెడల్పు == UCPTRIE_విలువ_బిట్లు_32) {
        actual_నిడివి += తాత్కాలిక_Trie.దత్తాంశం_నిడివి * 4;
    } లేకపోతే {
        actual_నిడివి += తాత్కాలిక_Trie.దత్తాంశం_నిడివి;
    }
    ఒకవేళ (length < actual_నిడివి) {
        *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;  // Not enough bytes.
        తిరిగి శూన్యసూచిక;
    }

    // Allocate the trie.
    UCPTrie *trie = (UCPTrie *)uprv_malloc(పరిమాణం(UCPTrie));
    ఒకవేళ (trie == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(trie, &తాత్కాలిక_Trie, పరిమాణం(తాత్కాలిక_Trie));
#నిర్వచితమైతే UCPTRIE_DEBUG
    trie->name = "fromSerialized";
#అయితే_ముగింపు

    // Set the pointers to its index and data arrays.
    స్థిరరాశి uint16_t *p16 = (స్థిరరాశి uint16_t *)(శీర్షిక + 1);
    trie->సూచీ = p16;
    p16 += trie->సూచీ_నిడివి;

    // Get the data.
    int32_t null_విలువ_స్థానభ్రంశం = trie->దత్తాంశం_Null_స్థానభ్రంశం;
    ఒకవేళ (null_విలువ_స్థానభ్రంశం >= trie->దత్తాంశం_నిడివి) {
        null_విలువ_స్థానభ్రంశం = trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం;
    }
    ఎంపిక (విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        trie->దత్తాంశం.ptr16 = p16;
        trie->null_విలువ = trie->దత్తాంశం.ptr16[null_విలువ_స్థానభ్రంశం];
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        trie->దత్తాంశం.ptr32 = (స్థిరరాశి uint32_t *)p16;
        trie->null_విలువ = trie->దత్తాంశం.ptr32[null_విలువ_స్థానభ్రంశం];
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        trie->దత్తాంశం.ptr8 = (స్థిరరాశి uint8_t *)p16;
        trie->null_విలువ = trie->దత్తాంశం.ptr8[null_విలువ_స్థానభ్రంశం];
        ఆపు;
    అప్రమేయ:
        // Unreachable because valueWidth was checked above.
        *p_Error_సంకేతం = U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (p_Actual_నిడివి != శూన్యసూచిక) {
        *p_Actual_నిడివి = actual_నిడివి;
    }
    తిరిగి trie;
}

U_CAPI శూన్య U_EXPORT2
ucptrie_close(UCPTrie *trie) {
    uprv_free(trie);
}

U_CAPI UCPTrieType U_EXPORT2
ucptrie_getType(స్థిరరాశి UCPTrie *trie) {
    తిరిగి (UCPTrieType)trie->type;
}

U_CAPI UCPTrie_విలువ_వెడల్పు U_EXPORT2
ucptrie_get_విలువ_వెడల్పు(స్థిరరాశి UCPTrie *trie) {
    తిరిగి (UCPTrie_విలువ_వెడల్పు)trie->విలువ_వెడల్పు;
}

U_CAPI int32_t U_EXPORT2
ucptrie_అంతర్గత_Small_సూచీ(స్థిరరాశి UCPTrie *trie, UChar32 c) {
    int32_t i1 = c >> UCPTRIE_జరుపు_1;
    ఒకవేళ (trie->type == UCPTRIE_TYPE_FAST) {
        U_ASSERT(0xffff < c && c < trie->ఎగువ_ప్రారంభం);
        i1 += UCPTRIE_BMP_సూచీ_నిడివి - UCPTRIE_OMITTED_BMP_సూచీ_1_నిడివి;
    } లేకపోతే {
        U_ASSERT((uint32_t)c < (uint32_t)trie->ఎగువ_ప్రారంభం && trie->ఎగువ_ప్రారంభం > UCPTRIE_SMALL_హద్దు);
        i1 += UCPTRIE_SMALL_సూచీ_నిడివి;
    }
    int32_t i3_ఖండం = trie->సూచీ[
        (int32_t)trie->సూచీ[i1] + ((c >> UCPTRIE_జరుపు_2) & UCPTRIE_సూచీ_2_ముసుగు)];
    int32_t i3 = (c >> UCPTRIE_జరుపు_3) & UCPTRIE_సూచీ_3_ముసుగు;
    int32_t దత్తాంశం_ఖండం;
    ఒకవేళ ((i3_ఖండం & 0x8000) == 0) {
        // 16-bit indexes
        దత్తాంశం_ఖండం = trie->సూచీ[i3_ఖండం + i3];
    } లేకపోతే {
        // 18-bit indexes stored in groups of 9 entries per 8 indexes.
        i3_ఖండం = (i3_ఖండం & 0x7fff) + (i3 & ~7) + (i3 >> 3);
        i3 &= 7;
        దత్తాంశం_ఖండం = ((int32_t)trie->సూచీ[i3_ఖండం++] << (2 + (2 * i3))) & 0x30000;
        దత్తాంశం_ఖండం |= trie->సూచీ[i3_ఖండం + i3];
    }
    తిరిగి దత్తాంశం_ఖండం + (c & UCPTRIE_SMALL_దత్తాంశం_ముసుగు);
}

U_CAPI int32_t U_EXPORT2
ucptrie_అంతర్గత_Small_U8_సూచీ(స్థిరరాశి UCPTrie *trie, int32_t lt1, uint8_t t2, uint8_t t3) {
    UChar32 c = (lt1 << 12) | (t2 << 6) | t3;
    ఒకవేళ (c >= trie->ఎగువ_ప్రారంభం) {
        // Possible because the UTF-8 macro compares with shifted12HighStart which may be higher.
        తిరిగి trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం;
    }
    తిరిగి ucptrie_అంతర్గత_Small_సూచీ(trie, c);
}

U_CAPI int32_t U_EXPORT2
ucptrie_అంతర్గత_U8_Prev_సూచీ(స్థిరరాశి UCPTrie *trie, UChar32 c,
                            స్థిరరాశి uint8_t *ప్రారంభం, స్థిరరాశి uint8_t *src) {
    int32_t i, length;
    // Support 64-bit pointers by avoiding cast of arbitrary difference.
    ఒకవేళ ((src - ప్రారంభం) <= 7) {
        i = length = (int32_t)(src - ప్రారంభం);
    } లేకపోతే {
        i = length = 7;
        ప్రారంభం = src - 7;
    }
    c = utf8_prev_Char_సురక్షిత_Body(ప్రారంభం, 0, &i, c, -1);
    i = length - i;  // Number of bytes read backward from src.
    int32_t idx = UCPTRIE_CP_సూచీ(trie, 0xffff, c);
    తిరిగి (idx << 3) | i;
}

నామాకాశ {

అంతఃవరుస uint32_t get_విలువ(UCPTrie_దత్తాంశం దత్తాంశం, UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు, int32_t దత్తాంశం_సూచీ) {
    ఎంపిక (విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        తిరిగి దత్తాంశం.ptr16[దత్తాంశం_సూచీ];
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        తిరిగి దత్తాంశం.ptr32[దత్తాంశం_సూచీ];
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        తిరిగి దత్తాంశం.ptr8[దత్తాంశం_సూచీ];
    అప్రమేయ:
        // Unreachable if the trie is properly initialized.
        తిరిగి 0xffffffff;
    }
}

}  // namespace

U_CAPI uint32_t U_EXPORT2
ucptrie_get(స్థిరరాశి UCPTrie *trie, UChar32 c) {
    int32_t దత్తాంశం_సూచీ;
    ఒకవేళ ((uint32_t)c <= 0x7f) {
        // linear ASCII
        దత్తాంశం_సూచీ = c;
    } లేకపోతే {
        UChar32 fastMax = trie->type == UCPTRIE_TYPE_FAST ? 0xffff : UCPTRIE_SMALL_MAX;
        దత్తాంశం_సూచీ = UCPTRIE_CP_సూచీ(trie, fastMax, c);
    }
    తిరిగి get_విలువ(trie->దత్తాంశం, (UCPTrie_విలువ_వెడల్పు)trie->విలువ_వెడల్పు, దత్తాంశం_సూచీ);
}

నామాకాశ {

constexpr int32_t MAX_UNICODE = 0x10ffff;

అంతఃవరుస uint32_t maybe_Filter_విలువ(uint32_t value, uint32_t trie_Null_విలువ, uint32_t null_విలువ,
                                 UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context) {
    ఒకవేళ (value == trie_Null_విలువ) {
        value = null_విలువ;
    } లేకపోతే ఒకవేళ (filter != శూన్యసూచిక) {
        value = filter(context, value);
    }
    తిరిగి value;
}

UChar32 getRange(స్థిరరాశి శూన్య *t, UChar32 ప్రారంభం,
                 UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context, uint32_t *p_విలువ) {
    ఒకవేళ (static_cast<uint32_t>(ప్రారంభం) > MAX_UNICODE) {
        తిరిగి U_SENTINEL;
    }
    స్థిరరాశి UCPTrie *trie = reinterpret_cast<స్థిరరాశి UCPTrie *>(t);
    UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు = static_cast<UCPTrie_విలువ_వెడల్పు>(trie->విలువ_వెడల్పు);
    ఒకవేళ (ప్రారంభం >= trie->ఎగువ_ప్రారంభం) {
        ఒకవేళ (p_విలువ != శూన్యసూచిక) {
            int32_t di = trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం;
            uint32_t value = get_విలువ(trie->దత్తాంశం, విలువ_వెడల్పు, di);
            ఒకవేళ (filter != శూన్యసూచిక) { value = filter(context, value); }
            *p_విలువ = value;
        }
        తిరిగి MAX_UNICODE;
    }

    uint32_t null_విలువ = trie->null_విలువ;
    ఒకవేళ (filter != శూన్యసూచిక) { null_విలువ = filter(context, null_విలువ); }
    స్థిరరాశి uint16_t *సూచీ = trie->సూచీ;

    int32_t prev_I3_ఖండం = -1;
    int32_t prev_ఖండం = -1;
    UChar32 c = ప్రారంభం;
    uint32_t trie_విలువ, value = null_విలువ;
    తార్కిక have_విలువ = అబద్ధం;
    చేయి {
        int32_t i3_ఖండం;
        int32_t i3;
        int32_t i3_ఖండం_నిడివి;
        int32_t దత్తాంశం_ఖండం_నిడివి;
        ఒకవేళ (c <= 0xffff && (trie->type == UCPTRIE_TYPE_FAST || c <= UCPTRIE_SMALL_MAX)) {
            i3_ఖండం = 0;
            i3 = c >> UCPTRIE_FAST_జరుపు;
            i3_ఖండం_నిడివి = trie->type == UCPTRIE_TYPE_FAST ?
                UCPTRIE_BMP_సూచీ_నిడివి : UCPTRIE_SMALL_సూచీ_నిడివి;
            దత్తాంశం_ఖండం_నిడివి = UCPTRIE_FAST_దత్తాంశం_ఖండం_నిడివి;
        } లేకపోతే {
            // Use the multi-stage index.
            int32_t i1 = c >> UCPTRIE_జరుపు_1;
            ఒకవేళ (trie->type == UCPTRIE_TYPE_FAST) {
                U_ASSERT(0xffff < c && c < trie->ఎగువ_ప్రారంభం);
                i1 += UCPTRIE_BMP_సూచీ_నిడివి - UCPTRIE_OMITTED_BMP_సూచీ_1_నిడివి;
            } లేకపోతే {
                U_ASSERT(c < trie->ఎగువ_ప్రారంభం && trie->ఎగువ_ప్రారంభం > UCPTRIE_SMALL_హద్దు);
                i1 += UCPTRIE_SMALL_సూచీ_నిడివి;
            }
            i3_ఖండం = trie->సూచీ[
                static_cast<int32_t>(trie->సూచీ[i1]) + ((c >> UCPTRIE_జరుపు_2) & UCPTRIE_సూచీ_2_ముసుగు)];
            ఒకవేళ (i3_ఖండం == prev_I3_ఖండం && (c - ప్రారంభం) >= UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు) {
                // The index-3 block is the same as the previous one, and filled with value.
                U_ASSERT((c & (UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు - 1)) == 0);
                c += UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు;
                కొనసాగు;
            }
            prev_I3_ఖండం = i3_ఖండం;
            ఒకవేళ (i3_ఖండం == trie->index3_Null_స్థానభ్రంశం) {
                // This is the index-3 null block.
                ఒకవేళ (have_విలువ) {
                    ఒకవేళ (null_విలువ != value) {
                        తిరిగి c - 1;
                    }
                } లేకపోతే {
                    trie_విలువ = trie->null_విలువ;
                    value = null_విలువ;
                    ఒకవేళ (p_విలువ != శూన్యసూచిక) { *p_విలువ = null_విలువ; }
                    have_విలువ = నిజం;
                }
                prev_ఖండం = trie->దత్తాంశం_Null_స్థానభ్రంశం;
                c = (c + UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు) & ~(UCPTRIE_CP_చొప్పున_సూచీ_2_నమోదు - 1);
                కొనసాగు;
            }
            i3 = (c >> UCPTRIE_జరుపు_3) & UCPTRIE_సూచీ_3_ముసుగు;
            i3_ఖండం_నిడివి = UCPTRIE_సూచీ_3_ఖండం_నిడివి;
            దత్తాంశం_ఖండం_నిడివి = UCPTRIE_SMALL_దత్తాంశం_ఖండం_నిడివి;
        }
        // Enumerate data blocks for one index-3 block.
        చేయి {
            int32_t ఖండం;
            ఒకవేళ ((i3_ఖండం & 0x8000) == 0) {
                ఖండం = సూచీ[i3_ఖండం + i3];
            } లేకపోతే {
                // 18-bit indexes stored in groups of 9 entries per 8 indexes.
                int32_t సమూహం = (i3_ఖండం & 0x7fff) + (i3 & ~7) + (i3 >> 3);
                int32_t gi = i3 & 7;
                ఖండం = (static_cast<int32_t>(సూచీ[సమూహం++]) << (2 + (2 * gi))) & 0x30000;
                ఖండం |= సూచీ[సమూహం + gi];
            }
            ఒకవేళ (ఖండం == prev_ఖండం && (c - ప్రారంభం) >= దత్తాంశం_ఖండం_నిడివి) {
                // The block is the same as the previous one, and filled with value.
                U_ASSERT((c & (దత్తాంశం_ఖండం_నిడివి - 1)) == 0);
                c += దత్తాంశం_ఖండం_నిడివి;
            } లేకపోతే {
                int32_t దత్తాంశం_ముసుగు = దత్తాంశం_ఖండం_నిడివి - 1;
                prev_ఖండం = ఖండం;
                ఒకవేళ (ఖండం == trie->దత్తాంశం_Null_స్థానభ్రంశం) {
                    // This is the data null block.
                    ఒకవేళ (have_విలువ) {
                        ఒకవేళ (null_విలువ != value) {
                            తిరిగి c - 1;
                        }
                    } లేకపోతే {
                        trie_విలువ = trie->null_విలువ;
                        value = null_విలువ;
                        ఒకవేళ (p_విలువ != శూన్యసూచిక) { *p_విలువ = null_విలువ; }
                        have_విలువ = నిజం;
                    }
                    c = (c + దత్తాంశం_ఖండం_నిడివి) & ~దత్తాంశం_ముసుగు;
                } లేకపోతే {
                    int32_t di = ఖండం + (c & దత్తాంశం_ముసుగు);
                    uint32_t trieValue2 = get_విలువ(trie->దత్తాంశం, విలువ_వెడల్పు, di);
                    ఒకవేళ (have_విలువ) {
                        ఒకవేళ (trieValue2 != trie_విలువ) {
                            ఒకవేళ (filter == శూన్యసూచిక ||
                                    maybe_Filter_విలువ(trieValue2, trie->null_విలువ, null_విలువ,
                                                     filter, context) != value) {
                                తిరిగి c - 1;
                            }
                            trie_విలువ = trieValue2;  // may or may not help
                        }
                    } లేకపోతే {
                        trie_విలువ = trieValue2;
                        value = maybe_Filter_విలువ(trieValue2, trie->null_విలువ, null_విలువ,
                                                 filter, context);
                        ఒకవేళ (p_విలువ != శూన్యసూచిక) { *p_విలువ = value; }
                        have_విలువ = నిజం;
                    }
                    అయితే ((++c & దత్తాంశం_ముసుగు) != 0) {
                        trieValue2 = get_విలువ(trie->దత్తాంశం, విలువ_వెడల్పు, ++di);
                        ఒకవేళ (trieValue2 != trie_విలువ) {
                            ఒకవేళ (filter == శూన్యసూచిక ||
                                    maybe_Filter_విలువ(trieValue2, trie->null_విలువ, null_విలువ,
                                                     filter, context) != value) {
                                తిరిగి c - 1;
                            }
                            trie_విలువ = trieValue2;  // may or may not help
                        }
                    }
                }
            }
        } అయితే (++i3 < i3_ఖండం_నిడివి);
    } అయితే (c < trie->ఎగువ_ప్రారంభం);
    U_ASSERT(have_విలువ);
    int32_t di = trie->దత్తాంశం_నిడివి - UCPTRIE_ఎగువ_విలువ_NEG_దత్తాంశం_స్థానభ్రంశం;
    uint32_t ఎగువ_విలువ = get_విలువ(trie->దత్తాంశం, విలువ_వెడల్పు, di);
    ఒకవేళ (maybe_Filter_విలువ(ఎగువ_విలువ, trie->null_విలువ, null_విలువ,
                         filter, context) != value) {
        తిరిగి c - 1;
    } లేకపోతే {
        తిరిగి MAX_UNICODE;
    }
}

}  // namespace

U_CFUNC UChar32
ucptrie_అంతర్గత_Get_Range(UCPTrieGetRange *getRange,
                         స్థిరరాశి శూన్య *trie, UChar32 ప్రారంభం,
                         UCPMapRangeOption option, uint32_t surrogate_విలువ,
                         UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context, uint32_t *p_విలువ) {
    ఒకవేళ (option == UCPMAP_RANGE_సాధారణ) {
        తిరిగి getRange(trie, ప్రారంభం, filter, context, p_విలువ);
    }
    uint32_t value;
    ఒకవేళ (p_విలువ == శూన్యసూచిక) {
        // We need to examine the range value even if the caller does not want it.
        p_విలువ = &value;
    }
    UChar32 surr_చివర = option == UCPMAP_RANGE_FIXED_ALL_SURROGATES ? 0xdfff : 0xdbff;
    UChar32 చివర = getRange(trie, ప్రారంభం, filter, context, p_విలువ);
    ఒకవేళ (చివర < 0xd7ff || ప్రారంభం > surr_చివర) {
        తిరిగి చివర;
    }
    // The range overlaps with surrogates, or ends just before the first one.
    ఒకవేళ (*p_విలువ == surrogate_విలువ) {
        ఒకవేళ (చివర >= surr_చివర) {
            // Surrogates followed by a non-surrogateValue range,
            // or surrogates are part of a larger surrogateValue range.
            తిరిగి చివర;
        }
    } లేకపోతే {
        ఒకవేళ (ప్రారంభం <= 0xd7ff) {
            తిరిగి 0xd7ff;  // Non-surrogateValue range ends before surrogateValue surrogates.
        }
        // Start is a surrogate with a non-surrogateValue code *unit* value.
        // Return a surrogateValue code *point* range.
        *p_విలువ = surrogate_విలువ;
        ఒకవేళ (చివర > surr_చివర) {
            తిరిగి surr_చివర;  // Surrogate range ends before non-surrogateValue rest of range.
        }
    }
    // See if the surrogateValue surrogate range can be merged with
    // an immediately following range.
    uint32_t value2;
    UChar32 end2 = getRange(trie, surr_చివర + 1, filter, context, &value2);
    ఒకవేళ (value2 == surrogate_విలువ) {
        తిరిగి end2;
    }
    తిరిగి surr_చివర;
}

U_CAPI UChar32 U_EXPORT2
ucptrie_getRange(స్థిరరాశి UCPTrie *trie, UChar32 ప్రారంభం,
                 UCPMapRangeOption option, uint32_t surrogate_విలువ,
                 UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context, uint32_t *p_విలువ) {
    తిరిగి ucptrie_అంతర్గత_Get_Range(getRange, trie, ప్రారంభం,
                                    option, surrogate_విలువ,
                                    filter, context, p_విలువ);
}

U_CAPI int32_t U_EXPORT2
ucptrie_గమ్యం_ద్విమాన(స్థిరరాశి UCPTrie *trie,
                 శూన్య *దత్తాంశం, int32_t capacity,
                 UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    UCPTrieType type = (UCPTrieType)trie->type;
    UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు = (UCPTrie_విలువ_వెడల్పు)trie->విలువ_వెడల్పు;
    ఒకవేళ (type < UCPTRIE_TYPE_FAST || UCPTRIE_TYPE_SMALL < type ||
            విలువ_వెడల్పు < UCPTRIE_విలువ_బిట్లు_16 || UCPTRIE_విలువ_బిట్లు_8 < విలువ_వెడల్పు ||
            capacity < 0 ||
            (capacity > 0 && (దత్తాంశం == శూన్యసూచిక || (U_సూచిక_ముసుగు_LSB(దత్తాంశం, 3) != 0)))) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    int32_t length = (int32_t)పరిమాణం(UCPTrie_శీర్షిక) + trie->సూచీ_నిడివి * 2;
    ఎంపిక (విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        length += trie->దత్తాంశం_నిడివి * 2;
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        length += trie->దత్తాంశం_నిడివి * 4;
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        length += trie->దత్తాంశం_నిడివి;
        ఆపు;
    అప్రమేయ:
        // unreachable
        ఆపు;
    }
    ఒకవేళ (capacity < length) {
        *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి length;
    }

    అక్షరం *బైట్లు = (అక్షరం *)దత్తాంశం;
    UCPTrie_శీర్షిక *శీర్షిక = (UCPTrie_శీర్షిక *)బైట్లు;
    శీర్షిక->signature = UCPTRIE_SIG;  // "Tri3"
    శీర్షిక->options = (uint16_t)(
        ((trie->దత్తాంశం_నిడివి & 0xf0000) >> 4) |
        ((trie->దత్తాంశం_Null_స్థానభ్రంశం & 0xf0000) >> 8) |
        (trie->type << 6) |
        విలువ_వెడల్పు);
    శీర్షిక->సూచీ_నిడివి = (uint16_t)trie->సూచీ_నిడివి;
    శీర్షిక->దత్తాంశం_నిడివి = (uint16_t)trie->దత్తాంశం_నిడివి;
    శీర్షిక->index3_Null_స్థానభ్రంశం = trie->index3_Null_స్థానభ్రంశం;
    శీర్షిక->దత్తాంశం_Null_స్థానభ్రంశం = (uint16_t)trie->దత్తాంశం_Null_స్థానభ్రంశం;
    శీర్షిక->shifted_ఎగువ_ప్రారంభం = trie->ఎగువ_ప్రారంభం >> UCPTRIE_జరుపు_2;
    బైట్లు += పరిమాణం(UCPTrie_శీర్షిక);

    uprv_memcpy(బైట్లు, trie->సూచీ, trie->సూచీ_నిడివి * 2);
    బైట్లు += trie->సూచీ_నిడివి * 2;

    ఎంపిక (విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        uprv_memcpy(బైట్లు, trie->దత్తాంశం.ptr16, trie->దత్తాంశం_నిడివి * 2);
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        uprv_memcpy(బైట్లు, trie->దత్తాంశం.ptr32, trie->దత్తాంశం_నిడివి * 4);
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        uprv_memcpy(బైట్లు, trie->దత్తాంశం.ptr8, trie->దత్తాంశం_నిడివి);
        ఆపు;
    అప్రమేయ:
        // unreachable
        ఆపు;
    }
    తిరిగి length;
}

నామాకాశ {

#నిర్వచితమైతే UCPTRIE_DEBUG
దీర్ఘ లెక్క_Null(స్థిరరాశి UCPTrie *trie) {
    uint32_t null_విలువ=trie->null_విలువ;
    int32_t length=trie->దత్తాంశం_నిడివి;
    దీర్ఘ count=0;
    ఎంపిక (trie->విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        కొరకు(int32_t i=0; i<length; ++i) {
            ఒకవేళ(trie->దత్తాంశం.ptr16[i]==null_విలువ) { ++count; }
        }
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        కొరకు(int32_t i=0; i<length; ++i) {
            ఒకవేళ(trie->దత్తాంశం.ptr32[i]==null_విలువ) { ++count; }
        }
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        కొరకు(int32_t i=0; i<length; ++i) {
            ఒకవేళ(trie->దత్తాంశం.ptr8[i]==null_విలువ) { ++count; }
        }
        ఆపు;
    అప్రమేయ:
        // unreachable
        ఆపు;
    }
    తిరిగి count;
}

U_CFUNC శూన్య
ucptrie_printLengths(స్థిరరాశి UCPTrie *trie, స్థిరరాశి అక్షరం *which) {
    దీర్ఘ సూచీ_నిడివి=trie->సూచీ_నిడివి;
    దీర్ఘ దత్తాంశం_నిడివి=(దీర్ఘ)trie->దత్తాంశం_నిడివి;
    దీర్ఘ మొత్తం_నిడివి=(దీర్ఘ)పరిమాణం(UCPTrie_శీర్షిక)+సూచీ_నిడివి*2+
            దత్తాంశం_నిడివి*(trie->విలువ_వెడల్పు==UCPTRIE_విలువ_బిట్లు_16 ? 2 :
                        trie->విలువ_వెడల్పు==UCPTRIE_విలువ_బిట్లు_32 ? 4 : 1);
    రాయి("**UCPTrieLengths(%s %s)** index:%6ld  data:%6ld  countNull:%6ld  serialized:%6ld\n",
           which, trie->name, సూచీ_నిడివి, దత్తాంశం_నిడివి, లెక్క_Null(trie), మొత్తం_నిడివి);
}
#అయితే_ముగింపు

}  // namespace

// UCPMap ----
// Initially, this is the same as UCPTrie. This may well change.

U_CAPI uint32_t U_EXPORT2
ucpmap_get(స్థిరరాశి UCPMap *map, UChar32 c) {
    తిరిగి ucptrie_get(reinterpret_cast<స్థిరరాశి UCPTrie *>(map), c);
}

U_CAPI UChar32 U_EXPORT2
ucpmap_getRange(స్థిరరాశి UCPMap *map, UChar32 ప్రారంభం,
                UCPMapRangeOption option, uint32_t surrogate_విలువ,
                UCPMap_విలువ_Filter *filter, స్థిరరాశి శూన్య *context, uint32_t *p_విలువ) {
    తిరిగి ucptrie_getRange(reinterpret_cast<స్థిరరాశి UCPTrie *>(map), ప్రారంభం,
                            option, surrogate_విలువ,
                            filter, context, p_విలువ);
}
